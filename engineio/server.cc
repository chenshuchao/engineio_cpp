#include "engineio/server.h"

#include <fstream>
#include <muduo/base/Logging.h>

#include "engineio/base/util.h"
#include "engineio/base/json_util.h"
#include "engineio/transports/websocket.h"

using namespace std;
using namespace woody;
using namespace engineio;

EngineIOServer::EngineIOServer(int port, const string& name)
    : woody::WebsocketServer(port, name),
      cookie_("io"),
      pingInterval_(25000),
      pingTimeout_(60000) {
  WebsocketServer::SetHandlerCloseCallback(
      boost::bind(&EngineIOServer::OnHandlerClose, this, _1));
}

void EngineIOServer::OnRequest(const WebsocketHandlerPtr& handler, 
                               const HTTPRequest& req) {
  LOG_DEBUG << "Handling http request, method: " << req.GetMethod()
            << ", url: " << req.GetUrl();
  if (!VerifyRequest(handler, req)) return;
  string sid;
  req.GetGETParams("sid", sid);
  string transport;
  req.GetGETParams("transport", transport);

  if (sid.empty()) {
    Handshake(transport, req, handler);
  } else {
      if(req.IsUpgrade()) {
        LOG_DEBUG << "Http request upgrade.";
        string sid;
        req.GetGETParams("sid", sid);
        EngineIOSocketPtr socket = sockets_[sid];
        if (socket->Upgrade(handler, req, transport)) {
          string name = socket->GetTransport()->GetHandler()->GetName();
          sockets_.erase(sid);
          LOG_DEBUG << "EngineIOServer::OnRequest - "
                    << "upgrade, handler name: " << name;
          ws_sockets_[name]= socket;
        }
        return;
      }
    EngineIOSocketPtr socket = sockets_[sid];
    socket->GetTransport()->OnRequest(req);
  }
}

void EngineIOServer::OnWebsocketTextMessage(const WebsocketHandlerPtr& handler,
    const TextMessage& message) {
  map<string, EngineIOSocketPtr>::const_iterator it = 
      ws_sockets_.find(handler->GetName());
  if (it != ws_sockets_.end()) {
    LOG_ERROR << "EngineIOServer::OnWebsocketTextMessage - "
              << "error: EngineIOSocket not found.";
    handler->HandleError();
    return;
  }
  EngineIOSocketPtr socket = it->second;
  socket->GetTransport()->OnData(message.GetData());
}

void EngineIOServer::Handshake(const string& transport_name,
                               const HTTPRequest& req,
                               const WebsocketHandlerPtr& handler) {
  // TODO
  string sid =  GenerateBase64ID();
  LOG_DEBUG << "EngineIOServer::Handshake - "
            << "sid : " << sid;
  BaseTransportPtr tran(transports_.GetFactory(transport_name)->Create(handler, req));
  string b64;
  if (req.GetGETParams("b64", b64)) {
    tran->SetSupportBinary(false);
  } else {
    tran->SetSupportBinary(true);
  }

  EngineIOSocketPtr socket(new EngineIOSocket(sid, tran));
  socket->SetCloseCallback(
      boost::bind(&EngineIOServer::OnSocketClose, this, _1));
  socket->SetMessagePacketCallback(
      boost::bind(&EngineIOServer::OnMessage, this, _1, _2));
  socket->SetPingPacketCallback(
      boost::bind(&EngineIOServer::OnPingMessage, this, _1, _2));
  sockets_[sid] = socket;

  tran->OnRequest(req);
  HTTPResponse resp;
  string body;
  JsonCodec codec;
  vector<string> upgrades;
  tran->GetAllUpgrades(upgrades);
  codec.Add("sid", sid)
       .Add("upgrades", upgrades)
       .Add("pingInterval", pingInterval_)
       .Add("pingTimeout", pingTimeout_);
  socket->SendOpenPacket(codec.Stringify());
}

bool EngineIOServer::VerifyRequest(const WebsocketHandlerPtr& handler,
                                   const woody::HTTPRequest& req) {
  string transport;
  if (!(req.GetGETParams("transport", transport) &&
       transports_.IsValid(transport))) {
    LOG_ERROR << "Unknown transport : " << transport;
    HandleRequestError(handler, req, kUnknownTransport);
    return false;
  }

  string sid;
  req.GetGETParams("sid", sid);
  if (sid.empty()) {
    if ("GET" != req.GetMethod()) {
      HandleRequestError(handler, req, kBadHandshakeMethod);
      return false;
    }
  } else {
    if (sockets_.find(sid) == sockets_.end()) {
      HandleRequestError(handler, req, kUnknownSid);
      return false;
    }
    if (!req.IsUpgrade() &&
        sockets_[sid]->GetTransport()->GetName() != transport) {
      HandleRequestError(handler, req, kBadRequest);
      return false;
    }
  }
  return true;
}

void EngineIOServer::HandleRequestError(const WebsocketHandlerPtr& handler,
                                        const HTTPRequest& req, 
                                        ErrorCode code) {
  HTTPResponse resp;
  resp.SetStatus(400, "Bad Request");
  resp.AddHeader("Content-Type", "application/json");
  string origin;
  if (req.GetHeader("origin", origin)) {
    resp.AddHeader("Access-Control-Allow-Credentials", "true")
        .AddHeader("Access-Control-Allow-Origin", origin);
  } else {
    resp.AddHeader("Access-Control-Allow-Origin", "*");
  }
 
  JsonCodec codec;
  codec.Add("code", code);
  const char* message = kErrorMessage[code];
  codec.Add("message", message);

  resp.AddBody(codec.Stringify());
  handler->SendResponse(resp);
}

void EngineIOServer::OnHandlerClose(const WebsocketHandlerPtr& handler) {
  LOG_DEBUG << "EngineIOServer::OnHandlerClose - "
            << handler->GetName();
  map<string, EngineIOSocketPtr>::const_iterator it =
      ws_sockets_.find(handler->GetName());
  if (it != ws_sockets_.end()) {
    it->second->OnClose();
  }
}

void EngineIOServer::OnSocketClose(const EngineIOSocketPtr& socket) {
  LOG_DEBUG << "EngineIOServer::OnSocketClose - "
            << socket->GetName();
  sockets_.erase(socket->GetSid());
  // for websocket
  ws_sockets_.erase(socket->GetTransport()->GetHandler()->GetName());
}


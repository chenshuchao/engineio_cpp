#include "engineio/server.h"

#include <fstream>
#include <muduo/base/Logging.h>

#include "engineio/base/util.h"
#include "engineio/base/rapidjson/document.h"
#include "engineio/base/rapidjson/prettywriter.h"

using namespace std;
using namespace woody;
using namespace engineio;



EngineIOServer::EngineIOServer(int port,
                               const std::string& name)
    : woody::WebsocketServer(port, name),
      cookie_("io") {
  transports_["polling"] = PollingTransportFactoryPtr(
      new PollingTransportFactory());
  transports_["websocket"] = WebsocketTransportFactoryPtr(
      new WebsocketTransportFactory());
}

void EngineIOServer::OnRequest(const WebsocketHandlerPtr& handler, 
                               const HTTPRequest& req) {
  // temp
  // temp end
  if (!VerifyRequest(req, handler)) return;

  LOG_INFO << "WebsocketHandler HandleRequest";
  if(req.IsUpgrade()) {
    LOG_INFO << "WebsocketHandler upgrade";
    handler->HandleUpgradeRequest(req);
    return;
  }

  LOG_DEBUG << "req methond: "<< req.GetMethod();
  string sid;
  req.GetGETParams("sid", sid);
  string transport;
  req.GetGETParams("transport", transport);
 
  if (sid.empty()) {
    Handshake(transport, req, handler);
  } else {
    EngineIOSocketPtr socket = sockets_[sid];
    socket->GetTransport()->OnRequest(req);
  }
}

void EngineIOServer::Handshake(const string& transport_name,
                               const HTTPRequest& req,
                               const WebsocketHandlerPtr& handler) {
  // TODO
  string sid =  GenerateBase64ID();
  BaseTransportPtr tran(transports_[transport_name]->Create(handler));
  EngineIOSocketPtr socket(new EngineIOSocket(tran));
  sockets_[sid] = socket;
  tran->OnRequest(req);

  HTTPResponse resp;
  resp.SetStatus(200, "OK");
  resp.AddHeader("Set-Cookie", cookie_ + "=" + sid);
  handler->SendResponse(resp);
}

// TODO ungly function
bool EngineIOServer::VerifyRequest(const woody::HTTPRequest& req,
                                   const WebsocketHandlerPtr& handler) {
  string transport;
  if (!(req.GetGETParams("transport", transport) &&
       transports_.find(transport) != transports_.end())) {
    HandleRequestError(req, kUnknownTransport, handler);
    return false;
  }

  string sid;
  req.GetGETParams("sid", sid);
  if (sid.empty()) {
    if ("GET" != req.GetMethod()) {
      HandleRequestError(req, kBadHandshakeMethod, handler);
      return false;
    }
  } else {
    if (sockets_.find(sid) == sockets_.end()) {
      HandleRequestError(req, kUnknownSid, handler);
      return false;
    }
    if (!req.IsUpgrade() &&
        sockets_[sid]->GetTransport()->GetName() != transport) {
      HandleRequestError(req, kBadRequest, handler);
      return false;
    }
  }
  return true;
}

void EngineIOServer::HandleRequestError(const HTTPRequest& req, 
                                        ErrorCode code,
                                        const WebsocketHandlerPtr& handler) {
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
  
  // TODO wrap rapidjson 
  rapidjson::Document d;
  d.Parse("{}");
  rapidjson::Value v_code;
  v_code.SetInt(code);
  d.AddMember("code", v_code, d.GetAllocator());
  rapidjson::Value v_message;
  const char* message = kErrorMessage[code];
  v_message.SetString(message,
                      static_cast<rapidjson::SizeType>(strlen(message)),
                      d.GetAllocator());
  d.AddMember("message", v_message, d.GetAllocator());
  rapidjson::StringBuffer sb;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
  d.Accept(writer);

  resp.AddBody(sb.GetString());
  handler->SendResponse(resp);
}

void EngineIOServer::OnWebsocketTextMessage(const WebsocketHandlerPtr& handler,
    const TextMessage& message) {
  std::map<WebsocketHandlerPtr, EngineIOSocketPtr>::const_iterator it = 
    ws_sockets_.find(handler);
  if (it->first) {
    LOG_ERROR << "EngineIOServer::OnWebsocketTextMessage - "
              << "error: EngineIOSocket not found.";
    handler->HandleError();
    return;
  }
  EngineIOSocketPtr socket = it->second;
  socket->GetTransport()->OnData(message.GetData());
}


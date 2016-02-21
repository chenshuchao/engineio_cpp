#include "engineio/transports/websocket.h"

#include <woody/base/string_util.h>

using namespace std;
using namespace woody;
using namespace engineio;

WebsocketTransport::WebsocketTransport(const HTTPHandlerPtr& handler,
                                       const HTTPRequest& req,
                                       HTTPResponse& resp)
    : BaseTransport("websocket"),
      handler_(new WebsocketHandler(convert_to_std(handler->GetConn()->name()))),
      writable_(false),
      upgraded_(false) {
}

void WebsocketTransport::HandleRequest(const HTTPHandlerPtr& handler,
                                       const HTTPRequest& req,
                                       HTTPResponse& resp) {
  // Websocket transport do not handle http request.
  if (!upgraded_ && req.IsUpgrade() &&
      handler_->HandleUpgrade(handler, req, resp)) {
    handler_->SetTextMessageCallback(
      boost::bind(&WebsocketTransport::OnTextMessage, this, _1, _2));
    return;
  }

  resp.SetStatus(400, "Bad Request");
  resp.End();
}

void WebsocketTransport::SendPackets(vector<Packet>& packets) {
  for(vector<Packet>::iterator it = packets.begin();
      it != packets.end();
      it ++) {
    string data;
    Parser::EncodePacket(*it, BaseTransport::IsSupportBinary(), data);
    woody::TextMessage message;
    message.Append(data);
    handler_->SendTextMessage(message);
  }
}

void WebsocketTransport::OnError() {
}

void WebsocketTransport::OnData(const string& data) {
  Packet packet;
  Parser::DecodePacket(data, packet);
  OnPacket(packet);
}

void WebsocketTransport::OnTextMessage(const WebsocketHandlerPtr& handler,
                                       const TextMessage& message) {
  OnData(message.GetData());
}

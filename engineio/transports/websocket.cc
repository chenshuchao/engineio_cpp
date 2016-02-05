#include "engineio/transports/websocket.h"

using namespace std;
using namespace woody;
using namespace engineio;

WebsocketTransport::WebsocketTransport(const WebsocketHandlerPtr& handler)
    : BaseTransport("websocket", handler) {
}
void WebsocketTransport::OnData(const string& data) {
  BaseTransport::OnData(data);
}

void WebsocketTransport::SendPacket(const EngineIOPacket& packet) {
  string data;
  EngineIOParser::EncodePacket(packet, data);
  woody::TextMessage message;
  message.Append(data);
  SendWebsocketMessage(message);
}

void WebsocketTransport::OnError() {
}


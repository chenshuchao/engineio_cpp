#include "engineio/transport.h"

using namespace std;
using namespace woody;
using namespace engineio;

WebsocketTransport::WebsocketTransport(const WebsocketHandlerPtr& handler)
    : BaseTransport("Websocket", handler) {
}

void BaseTransport::OnData(const string& data) {
  EngineIOPacket packet;
  parser_.DecodePacket(data, packet);
  OnPacket(packet);
}

void WebsocketTransport::OnData(const string& data) {
  BaseTransport::OnData(data);
}

void WebsocketTransport::SendPacket(const EngineIOPacket& packet) {
  string data;
  EncodePacket(packet, data);
  woody::TextMessage message;
  message.Append(data);
  SendWebsocketMessage(message);
}

void WebsocketTransport::OnError() {
}


void PollingTransport::SendPacket(const EngineIOPacket& packet) {
  
}

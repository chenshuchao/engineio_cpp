#include "engineio/socket.h"

using namespace std;
using namespace woody;
using namespace engineio;

void EngineIOSocket::OnPacket(const EngineIOPacket& packet) {
  int packet_type = packet.GetType();
  switch (packet_type) {
    case EngineIOPacket::kPacketPing: {
      OnPingPacket(packet);
      break;
    }
    case EngineIOPacket::kPacketMessage: {
      OnMessagePacket(packet);
      break;
    } 
  }
}

void EngineIOSocket::OnPingPacket(const EngineIOPacket& packet) {
  // TODO PONG? Is ping carrying data?
  ping_packet_callback_(shared_from_this(), packet.GetData());
}

void EngineIOSocket::OnMessagePacket(const EngineIOPacket& packet) {
  message_packet_callback_(shared_from_this(), packet.GetData());
}

void EngineIOSocket::SendPacket(int packet_type, const std::string& data) {
  EngineIOPacket packet;
  if (packet.SetType(packet_type)) {
    // error
    return;
  }
  packet.SetData(data);
  transport_->SendPacket(packet);
}



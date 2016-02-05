#include "engineio/socket.h"
#include <muduo/base/Logging.h>

using namespace std;
using namespace woody;
using namespace engineio;

EngineIOSocket::EngineIOSocket(const string sid, const BaseTransportPtr& tran)
    : sid_(sid),
      transport_(tran),
      upgraded_(false){
  SetTransport(tran);
}

void EngineIOSocket::SetTransport(const BaseTransportPtr& tran) {
  transport_ = tran;
  transport_->SetPacketCompleteCallback(
      boost::bind(&EngineIOSocket::OnPacket, this, _1));
  // transport_->SetCloseCallback()
}

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
  CreateAndSendPacket(EngineIOPacket::kPacketPong, "");
  ping_packet_callback_(shared_from_this(), packet.GetData());
}

void EngineIOSocket::OnMessagePacket(const EngineIOPacket& packet) {
  message_packet_callback_(shared_from_this(), packet.GetData());
}

void EngineIOSocket::CreateAndSendPacket(int packet_type, const string& data) {
  LOG_DEBUG << "EngineIOSocket::SendPacket, packet_type: " << packet_type;
  EngineIOPacket packet;
  // TODO unnecessary ?
  if (!packet.SetType(packet_type)) {
    LOG_ERROR << "EngineIOSocket::SendPacket - "
              << " Unknown packet type : " << packet_type;
    return;
  }
  packet.SetData(data);
  transport_->SendPacket(packet);
}

bool EngineIOSocket::Upgrade(const WebsocketHandlerPtr& handler,
                             const HTTPRequest& req,
                             std::string transport_name) {
  if (upgraded_) return false;
  //TODO
  BaseTransportFactoryPtr factory = transports_.GetFactory(transport_name);
  BaseTransportPtr tran(factory->Create(handler, req));
  SetTransport(tran);
  return transport_->HandleUpgrade(req);
}

void EngineIOSocket::OnClose() {
  close_callback_(shared_from_this());
}

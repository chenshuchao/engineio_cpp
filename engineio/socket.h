#ifndef ENGINEIO_SOCKET_H
#define ENGINEIO_SOCKET_H

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "engineio/transport.h"
#include "engineio/transports/transport_factory.h"

namespace engineio {
class EngineIOSocket : public boost::enable_shared_from_this<EngineIOSocket> {
 public:
  typedef boost::shared_ptr<EngineIOSocket> EngineIOSocketPtr;
  typedef boost::function<void (const EngineIOSocketPtr&)> CloseCallback;
  typedef boost::function<void (const EngineIOSocketPtr&, const std::string&)>
      PingPacketCallback;
  typedef boost::function<void (const EngineIOSocketPtr&, const std::string&)>
      MessagePacketCallback;

  EngineIOSocket(const std::string sid, const BaseTransportPtr& tran);

  std::string GetName() const { return name_; }
  std::string GetSid() const { return sid_; }
  BaseTransportPtr GetTransport() const { return transport_; }
  
  void SetTransport(const BaseTransportPtr& tran);

  void SetCloseCallback(const CloseCallback& cb) {
    close_callback_ = cb;
  }
  void SetMessagePacketCallback(const MessagePacketCallback& cb) {
    message_packet_callback_ = cb;
  }
  void SetPingPacketCallback(const PingPacketCallback& cb) {
    ping_packet_callback_ = cb;
  }
  // Called when packet was send from transport
  void OnPacket(const EngineIOPacket& packet);
  // Handle ping packet
  void OnPingPacket(const EngineIOPacket& packet);
  // Handle message packet
  void OnMessagePacket(const EngineIOPacket& packet);

  // TODO
  void SendOpenPacket(const std::string& data) {
    CreateAndSendPacket(EngineIOPacket::kPacketOpen, data);
  }
  void SendMessage(const std::string& data) {
    CreateAndSendPacket(EngineIOPacket::kPacketMessage, data);
  }
  void CreateAndSendPacket(int packet_type, const std::string& data);

  bool Upgrade(const woody::WebsocketHandlerPtr& handler,
               const woody::HTTPRequest& req,
               std::string transport_name);

  void OnClose();

  void ForceClose() {
    transport_->ForceClose();
  }

 private:
  std::string name_;
  std::string sid_;
  BaseTransportPtr transport_;
  CloseCallback close_callback_;
  PingPacketCallback ping_packet_callback_;
  MessagePacketCallback message_packet_callback_;
  bool upgraded_;
  EngineIOTransports transports_;
};
typedef boost::shared_ptr<EngineIOSocket> EngineIOSocketPtr;
}

#endif

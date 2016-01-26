#ifndef ENGINEIO_SOCKET_H
#define ENGINEIO_SOCKET_H

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "engineio/transport.h"

namespace engineio {
class EngineIOSocket : public boost::enable_shared_from_this<EngineIOSocket> {
 public:
  typedef boost::shared_ptr<EngineIOSocket> EngineIOSocketPtr;
  typedef boost::function<void (const EngineIOSocketPtr, const std::string&)>
      PingPacketCallback;
  typedef boost::function<void (const EngineIOSocketPtr, const std::string&)>
      MessagePacketCallback;

  EngineIOSocket(const BaseTransportPtr& tran)
      : transport_(tran) {
  }
  BaseTransportPtr GetTransport() const {
    return transport_;
  }
  
  void OnPacket(const EngineIOPacket& packet);
  void OnPingPacket(const EngineIOPacket& packet);
  void OnMessagePacket(const EngineIOPacket& packet);

  void Send(const std::string& data) {
    SendPacket(EngineIOPacket::kPacketMessage, data);
  }
  void SendPacket(int packet_type, const std::string& data);

 private:
  BaseTransportPtr transport_;
  PingPacketCallback ping_packet_callback_;
  MessagePacketCallback message_packet_callback_;
};
typedef boost::shared_ptr<EngineIOSocket> EngineIOSocketPtr;
}

#endif

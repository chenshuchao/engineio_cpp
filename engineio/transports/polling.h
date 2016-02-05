#ifndef ENGINEIO_TRANSPORTS_POLLING_H
#define ENGINEIO_TRANSPORTS_POLLING_H

#include "engineio/transport.h"

namespace engineio {

class PollingTransport : public BaseTransport {
 public:
  PollingTransport(const woody::WebsocketHandlerPtr& handler)
      : BaseTransport("polling", handler) {
  }
  virtual ~PollingTransport() { }
  void GetAllUpgrades(std::vector<std::string>& vec) {
    vec.push_back("websocket");
  }
  void SendPacket(const EngineIOPacket& packet);
  virtual void DoWrite(const std::string& data) = 0;
};

}
#endif

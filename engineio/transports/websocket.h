#ifndef ENGINEIO_TRANSPORTS_WEBSOCKET_H
#define ENGINEIO_TRANSPORTS_WEBSOCKET_H

#include "engineio/transport.h"

namespace engineio {
class WebsocketTransport : public BaseTransport {
 public:
  WebsocketTransport(const woody::WebsocketHandlerPtr& handler);
  virtual ~WebsocketTransport() { }
  virtual void OnData(const std::string& data);
  virtual void OnError();
  virtual void SendPacket(const EngineIOPacket& packet);
  virtual void GetAllUpgrades(std::vector<std::string>& vec) { }
  virtual bool HandleUpgrade(const woody::HTTPRequest& req) {
    return HandleUpgradeRequest(req);
  }
 private:
};
}

#endif

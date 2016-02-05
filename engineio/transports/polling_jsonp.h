#ifndef ENGINEIO_TRANSPORTS_POLLINGJSONP_H
#define ENGINEIO_TRANSPORTS_POLLINGJSONP_H

namespace engineio {

class PollingJsonpTransport : public PollingTransport {
 public:
  PollingJsonpTransport(const woody::WebsocketHandlerPtr& handler)
      : PollingTransport(handler) {
  }
  virtual ~PollingJsonpTransport() { }
  void DoWrite(const std::string& data) { }

};
}
#endif

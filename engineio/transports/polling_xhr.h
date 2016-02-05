#ifndef ENGINEIO_TRANSPORTS_POLLINGXHR_H
#define ENGINEIO_TRANSPORTS_POLLINGXHR_H

#include "engineio/transports/polling.h"

namespace engineio {
class PollingXhrTransport : public PollingTransport {
 public:
  PollingXhrTransport(const woody::WebsocketHandlerPtr& handler)
      : PollingTransport(handler) {
  }
  virtual ~PollingXhrTransport() { }
  virtual void DoWrite(const std::string& data);
};

/*
class PollingXhrTransportFactory : public BaseTransportFactory {
 public:
  PollingXhrTransport* Create(const woody::WebsocketHandlerPtr& handler) {
    return new PollingXhrTransport(handler);
  }
};
typedef boost::shared_ptr<PollingXhrTransportFactory> PollingXhrTransportFactoryPtr;
*/
}

#endif

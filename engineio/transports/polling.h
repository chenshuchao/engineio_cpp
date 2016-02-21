#ifndef ENGINEIO_TRANSPORTS_POLLING_H
#define ENGINEIO_TRANSPORTS_POLLING_H

#include "engineio/transport.h"

namespace engineio {

class PollingTransport : public BaseTransport {
 public:
  PollingTransport(const woody::HTTPHandlerPtr& handler,
                   const woody::HTTPRequest& req,
                   woody::HTTPResponse& resp)
      : BaseTransport("polling"),
        resp_(resp) {
  }
  virtual ~PollingTransport() { }

  virtual void GetAllUpgrades(std::vector<std::string>& vec) {
    vec.push_back("websocket");
  }

  virtual void HandleRequest(const woody::HTTPHandlerPtr& handler,
                             const woody::HTTPRequest& req,
                             woody::HTTPResponse& resp);

  virtual void SendPackets(std::vector<Packet>& packets);

  virtual void DoWrite(const std::string& data) = 0;

  virtual bool IsWritable() const { return writable_; }

  virtual void ForceClose() {
  }

  virtual void OnClose() {
  }

  woody::HTTPResponse GetResponse() const {
    return resp_;
  }
 private:
  void HandlePollRequest(const woody::HTTPRequest& req,
                         woody::HTTPResponse& resp);
  void HandleDataRequest(const woody::HTTPRequest& req,
                         woody::HTTPResponse& resp);

  virtual void OnData(const std::string& data);

  woody::HTTPResponse resp_;
  bool writable_;
};

}
#endif

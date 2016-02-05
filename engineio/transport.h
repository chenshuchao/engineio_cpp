#ifndef ENGINEIO_TRANSPORT_H
#define ENGINEIO_TRANSPORT_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <woody/http/http_request.h>
#include <woody/websocket/websocket_handler.h>
#include "engineio/parser.h"

namespace engineio {
class BaseTransport {
 public:
  typedef boost::function<void (const EngineIOPacket&)>
      PacketCompleteCallback;
  BaseTransport(const std::string& name,
                const woody::WebsocketHandlerPtr& handler)
      : name_(name),
        is_support_binary_(false),
        handler_(handler) {
  }
  std::string GetName() const { return name_; }

  const woody::WebsocketHandlerPtr& GetHandler() const { return handler_; }

  // Handle http request
  void OnRequest(const woody::HTTPRequest& req) {}

  // Handle data from websocket
  virtual void OnData(const std::string& data);

  // Called when data was decoded to packet
  void OnPacket(const EngineIOPacket& packet) {
    packet_complete_callback_(packet);
  }
  /*
  virtual void OnError();
  virtual void OnClose();
  */
  void SetPacketCompleteCallback(const PacketCompleteCallback& cb) {
    packet_complete_callback_ = cb;
  }

  // Send engineio packet to transport
  virtual void SendPacket(const EngineIOPacket& packet) = 0;

  virtual void GetAllUpgrades(std::vector<std::string>& vec) = 0;

  void SendResponse(woody::HTTPResponse& resp) {
    handler_->SendResponse(resp);
  }

  void SendWebsocketMessage(const woody::TextMessage message) {
    handler_->SendTextMessage(message);
  }

  bool IsSupportBinary() const { return is_support_binary_; }
  void SetSupportBinary(bool b) { is_support_binary_ = b; }

  // transport which support upgrade should override this function
  virtual bool HandleUpgrade(const woody::HTTPRequest& req) {
    return false;
  }
  bool HandleUpgradeRequest(const woody::HTTPRequest& req) {
    return handler_->HandleUpgradeRequest(req);
  }
  
  void OnClose() {
  }
  void ForceClose() {
    handler_->ForceClose();
  }

 private:
  std::string name_;
  bool is_support_binary_;
  const woody::WebsocketHandlerPtr handler_;
  PacketCompleteCallback packet_complete_callback_;
};
typedef boost::shared_ptr<BaseTransport> BaseTransportPtr;

class BaseTransportFactory {
 public:
  virtual BaseTransport* Create(const woody::WebsocketHandlerPtr& handler,
          const woody::HTTPRequest& req) = 0;
 private:
};
typedef boost::shared_ptr<BaseTransportFactory> BaseTransportFactoryPtr;
}

#endif

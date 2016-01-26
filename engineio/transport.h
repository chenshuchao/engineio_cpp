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
        handler_(handler) {
  }
  void OnRequest(const woody::HTTPRequest& req) {}
  virtual void OnData(const std::string& data);
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
  std::string GetName() const { return name_; }

  virtual void SendPacket(const EngineIOPacket& packet) = 0;
  virtual void EncodePacket(const EngineIOPacket& packet,
                            std::string& data) {
    parser_.EncodePacket(packet, data);
  }

  void SendResponse(woody::HTTPResponse& resp) {
    handler_->SendResponse(resp);
  }
  void SendWebsocketMessage(const woody::TextMessage message) {
    handler_->SendTextMessage(message);
  }

 private:
  const std::string name_;
  EngineIOParser parser_;
  woody::WebsocketHandlerPtr handler_;
  PacketCompleteCallback packet_complete_callback_;
};
typedef boost::shared_ptr<BaseTransport> BaseTransportPtr;

class BaseTransportFactory {
 public:
  virtual BaseTransport* Create(const woody::WebsocketHandlerPtr& handler) = 0;
 private:
};
typedef boost::shared_ptr<BaseTransportFactory> BaseTransportFactoryPtr;

class WebsocketTransport : public BaseTransport {
 public:
  WebsocketTransport(const woody::WebsocketHandlerPtr& handler);
  virtual void OnData(const std::string& data);
  virtual void OnError();

  virtual void SendPacket(const EngineIOPacket& packet);
};
class WebsocketTransportFactory : public BaseTransportFactory {
 public:
  WebsocketTransport* Create(const woody::WebsocketHandlerPtr& handler) {
    return new WebsocketTransport(handler);
  }
};
typedef boost::shared_ptr<WebsocketTransportFactory>
    WebsocketTransportFactoryPtr;

class PollingTransport : public BaseTransport {
 public:
  PollingTransport(const woody::WebsocketHandlerPtr& handler)
      : BaseTransport("Websocket", handler) {
  }
  void SendPacket(const EngineIOPacket& packet);
};
class PollingTransportFactory : public BaseTransportFactory {
 public:
  PollingTransport* Create(const woody::WebsocketHandlerPtr& handler) {
    return new PollingTransport(handler);
  }
};
typedef boost::shared_ptr<PollingTransportFactory> PollingTransportFactoryPtr;

}

#endif

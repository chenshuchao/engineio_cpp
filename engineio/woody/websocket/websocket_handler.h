#ifndef WOODY_WEBSOCKET_WEBSOCKETHANDLER_H
#define WOODY_WEBSOCKET_WEBSOCKETHANDLER_H

#include <vector>

#include <boost/enable_shared_from_this.hpp>

#include "woody/websocket/websocket_codec.h"
#include "woody/http/http_handler.h"

namespace woody {
static const int kHTTP = 0;
static const int kWebsocket = 1;
//static const int kVersions[] = {9, 10, 11, 12, 13}
static const int kVersion = 13; // only support 13
static const std::string kWebsocketGUID("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

class WebsocketHandler : public BaseHandler,
                         public boost::enable_shared_from_this<WebsocketHandler> {
 public:
  typedef boost::shared_ptr<WebsocketHandler> WebsocketHandlerPtr;
  typedef boost::function<void (const WebsocketHandlerPtr&, const HTTPRequest&)>
      RequestCompleteCallback;
  typedef boost::function<void (const WebsocketHandlerPtr&, const TextMessage&)> TextMessageCallback;
  typedef boost::function<void (const WebsocketHandlerPtr&, const BinaryMessage&)> BinaryMessageCallback;
  typedef boost::function<void (const WebsocketHandlerPtr&, const CloseMessage&)> CloseMessageCallback;
  typedef boost::function<void (const WebsocketHandlerPtr&, const PingMessage&)> PingMessageCallback;
  typedef boost::function<void (const WebsocketHandlerPtr&, const PongMessage&)> PongMessageCallback;
  typedef boost::function<void (const WebsocketHandlerPtr&)> ErrorCallback;

  
  WebsocketHandler(const std::string& name,
                   const muduo::net::TcpConnectionPtr& conn);
  virtual ~WebsocketHandler() { }
 
  int GetProtocol() { return protocol_; }
  void SetProtocol(int x) { protocol_ = x; }

  void AddSubProtocol(std::string protocol)
  { sub_protocols_.push_back(protocol); }
  
  virtual void OnData(muduo::net::Buffer* buf);

  virtual void HandleUpgradeRequest(const HTTPRequest& req);

  void SendResponse(HTTPResponse& resp) {
    http_handler_.SendResponse(resp);
  }

  bool SendTextMessage(const TextMessage& message);
  bool SendBinaryMessage(const BinaryMessage& message);
  bool SendCloseMessage(const CloseMessage& message);
  bool SendPingMessage(const PingMessage& message);
  // TODO private?
  bool SendPongMessage(const PongMessage& message);

 
  virtual void HandleError();
 
  virtual void OnTextMessage(const TextMessage& message);

  virtual void OnBinaryMessage(const BinaryMessage& message);

  virtual void OnCloseMessage(const CloseMessage& message);

  virtual void OnPingMessage(const PingMessage& message);

  virtual void OnPongMessage(const PongMessage& message);

  void OnRequestComplete(const HTTPRequest& req);

  void SetRequestCompleteCallback(const RequestCompleteCallback& cb) {
    request_complete_callback_ = cb;
  }
  void SetTextMessageCallback(const TextMessageCallback& cb) {
    text_message_callback_ = cb;
  }
  void SetBinaryMessageCallback(const BinaryMessageCallback& cb) {
    binary_message_callback_ = cb;
  }
  void SetCloseMessageCallback(const CloseMessageCallback& cb) {
    close_message_callback_ = cb;
  }
  void SetPingMessageCallback(const PingMessageCallback& cb) {
    ping_message_callback_ = cb;
  }
  void SetPongMessageCallback(const PongMessageCallback& cb) {
    pong_message_callback_ = cb;
  }
  void SetErrorCallback(const ErrorCallback& cb) {
    error_callback_ = cb;
  }

 private:
  bool SendWebsocketMessage(const WebsocketMessage::MessageType type,
                            const std::string& data);
  // TODO enum
  int protocol_;
  std::vector<std::string> sub_protocols_;
  WebsocketCodec ws_codec_;
  HTTPHandler http_handler_;
  RequestCompleteCallback request_complete_callback_;
  TextMessageCallback text_message_callback_;
  BinaryMessageCallback binary_message_callback_;
  CloseMessageCallback close_message_callback_;
  PingMessageCallback ping_message_callback_;
  PongMessageCallback pong_message_callback_;
  ErrorCallback error_callback_;
};
typedef boost::shared_ptr<WebsocketHandler> WebsocketHandlerPtr;
}

#endif

#ifndef WOODY_WEBSOCKET_WEBSOCKETSERVER_H
#define WOODY_WEBSOCKET_WEBSOCKETSERVER_H

#include <boost/noncopyable.hpp>
#include <muduo/net/EventLoop.h>

#include "woody/tcp_server.h"
#include "woody/http/http_handler.h"
#include "woody/websocket/websocket_handler.h"

namespace woody {

class WebsocketServer {
 public:
  WebsocketServer(int port, const std::string& name);
  virtual ~WebsocketServer() {}

  void Start();
  void Stop();
  void Bind();

  virtual void OnRequest(const WebsocketHandlerPtr& handler, 
                             const HTTPRequest& req) = 0;
  virtual void OnWebsocketTextMessage(const WebsocketHandlerPtr& handler,
                             const TextMessage&) = 0;
  virtual void OnBinaryMessage(const WebsocketHandlerPtr& handler,
                               const BinaryMessage&) {}
  virtual void OnCloseMessage(const WebsocketHandlerPtr& handler,
                               const CloseMessage&) {}
  virtual void OnPingMessage(const WebsocketHandlerPtr& handler,
                               const PingMessage&) {}
  virtual void OnPongMessage(const WebsocketHandlerPtr& handler,
                               const PongMessage&) {}
 private:
  //
  void OnCreateOrDestroyConnection(const muduo::net::TcpConnectionPtr& conn);
  void OnConnection(const muduo::net::TcpConnectionPtr& conn);
  void OnDisconnection(const muduo::net::TcpConnectionPtr& conn);
  void OnData(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp);
  //
  void OnHandlerError(const WebsocketHandlerPtr& handler);

  void CreateHandler(const muduo::net::TcpConnectionPtr& conn);
  void RemoveHandler(const WebsocketHandlerPtr& handler);
  WebsocketHandlerPtr FindHandler(const muduo::net::TcpConnectionPtr& conn);

  std::string name_;
  muduo::net::EventLoop loop_;
  muduo::net::TcpServer tcp_server_;
  std::map<std::string, WebsocketHandlerPtr> handler_map_;
};
}

#endif
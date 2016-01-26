#ifndef ENGINEIO_SERVER_H
#define ENGINEIO_SERVER_H

#include <map>

#include <woody/websocket/websocket_server.h>
#include "engineio/socket.h"

namespace engineio {
static const char* kErrorMessage [] = {
  "Transport unknown",
  "Session ID unknown",
  "Bad handshake method",
  "Bad request"
};

class EngineIOServer : public woody::WebsocketServer {
 enum ErrorCode {
   kUnknownTransport = 0,
   kUnknownSid = 1,
   kBadHandshakeMethod = 2,
   kBadRequest = 3,
 };
 public:
  EngineIOServer(int port, const std::string& name);

  virtual ~EngineIOServer() { }

  virtual void OnRequest(const woody::WebsocketHandlerPtr& handler, 
                         const woody::HTTPRequest& req);
  
  virtual void OnWebsocketTextMessage(
      const woody::WebsocketHandlerPtr& handler, const woody::TextMessage&);
 
  // TODO how to differ function name for On_Messgae from transport and socket;
  virtual void OnPingMessage(const EngineIOSocketPtr&, const std::string& data) { }
  virtual void OnMessage(const EngineIOSocketPtr&, const std::string& data) = 0;
  
 private:
  bool VerifyRequest(const woody::HTTPRequest& req,
                     const woody::WebsocketHandlerPtr& handler);

  void Handshake(const std::string& transport_name,
                 const woody::HTTPRequest& req,
                 const woody::WebsocketHandlerPtr& handler);

  void HandleRequestError(const woody::HTTPRequest& req,
                          ErrorCode code,
                          const woody::WebsocketHandlerPtr& handler);

  std::string cookie_;

  // {sid : socket} for http request to find its sockets
  std::map<std::string, EngineIOSocketPtr> sockets_;
  // {handlerPtr : socket} only for websocket
  std::map<woody::WebsocketHandlerPtr, EngineIOSocketPtr> ws_sockets_;
  std::map<std::string, BaseTransportFactoryPtr> transports_;
};
}

#endif

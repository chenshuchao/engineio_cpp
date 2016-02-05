#ifndef ENGINEIO_SERVER_H
#define ENGINEIO_SERVER_H

#include <map>

#include <woody/websocket/websocket_server.h>
#include "engineio/socket.h"
#include "engineio/transports/transport_factory.h"

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
  // Implement the base class virtual function
  // Called When http request come
  virtual void OnRequest(const woody::WebsocketHandlerPtr& handler, 
                         const woody::HTTPRequest& req);
  // Implement the base class virtual function
  // Called When Websocket text message come
  virtual void OnWebsocketTextMessage(
      const woody::WebsocketHandlerPtr& handler, const woody::TextMessage&);
 
  // TODO how to differ function name for On_Messgae from transport and socket;
  // Called when ping message was parsed from transport
  virtual void OnPingMessage(const EngineIOSocketPtr&, const std::string& data) { }
  // Called when text message was parsed from transport
  virtual void OnMessage(const EngineIOSocketPtr&, const std::string& data) = 0;

 private:
  bool VerifyRequest(const woody::WebsocketHandlerPtr& handler,
                     const woody::HTTPRequest& req);
  // Handshake a new client
  // If successfully, a new EngineIOSocket will be newed;
  void Handshake(const std::string& transport_name,
                 const woody::HTTPRequest& req,
                 const woody::WebsocketHandlerPtr& handler);
  // Send error message to client
  void HandleRequestError(const woody::WebsocketHandlerPtr& handler,
                          const woody::HTTPRequest& req,
                          ErrorCode code);

  void OnHandlerClose(const woody::WebsocketHandlerPtr& handler);

  void OnSocketClose(const EngineIOSocketPtr& socket);

  std::string cookie_;  // prefix of cookie
  int pingInterval_;   // ms
  int pingTimeout_;
  // {sid : socket}
  // for http request to find its sockets
  std::map<std::string, EngineIOSocketPtr> sockets_;
  // {handlerPtr : socket}
  // for websocket
  std::map<std::string, EngineIOSocketPtr> ws_sockets_;
  EngineIOTransports transports_;
};
}

#endif

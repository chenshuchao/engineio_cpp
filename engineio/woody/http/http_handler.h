#ifndef WOODY_HTTP_HTTPHANDLER_H
#define WOODY_HTTP_HTTPHANDLER_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <muduo/net/TcpConnection.h>

#include "woody/base_handler.h"
#include "woody/http/http_codec.h"

namespace woody {
class HTTPHandler : public BaseHandler,
                    public boost::enable_shared_from_this<HTTPHandler> {
 public:
  typedef boost::shared_ptr<HTTPHandler> HTTPHandlerPtr;
  typedef boost::function<void (const HTTPRequest&)> RequestCompleteCallback;
  typedef boost::function<void (const HTTPHandlerPtr&)> OnCloseCallback;
  typedef boost::function<void (const HTTPHandlerPtr&)> ForceCloseCallback;
  
  HTTPHandler(const std::string& name,
              const muduo::net::TcpConnectionPtr&);
  virtual ~HTTPHandler() { }

  virtual void OnData(muduo::net::Buffer* buf);
  virtual void OnMessageBegin(HTTPCodec::StreamID id);
  virtual void OnMessageComplete(HTTPCodec::StreamID id, HTTPRequest& request);
  
  void FormatResponse(HTTPResponse& resp); 
  void SendResponse(HTTPResponse& response);
  void HandleError() {
    ForceClose();
  }

  virtual void OnClose();
  virtual void ForceClose();

  void SetRequestCompleteCallback(const RequestCompleteCallback& cb) {
    request_complete_callback_ = cb;
  }
  void SetOnCloseCallback(const OnCloseCallback& cb) {
    on_close_callback_ = cb;
  }
  void SetForceCloseCallback(const ForceCloseCallback& cb) {
    force_close_callback_ = cb;
  }

 private:
  HTTPCodec codec_;
  RequestCompleteCallback request_complete_callback_;
  OnCloseCallback on_close_callback_;
  ForceCloseCallback force_close_callback_;
};
typedef boost::shared_ptr<HTTPHandler> HTTPHandlerPtr;
}

#endif

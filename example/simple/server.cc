#include <fstream>
#include <string>

#include <muduo/base/Logging.h>
#include <engineio/server.h>

using namespace std;
using namespace woody;
using namespace engineio;

bool IsStringEndWith(string s, string p) {
  return s.find(p) == (s.size() - p.size());
}

class SimpleServer : public EngineIOServer {
 public:
  SimpleServer(int port, const std::string& name)
      : EngineIOServer(port, name),
        root_("/home/shuchao/Documents/github/engineio_cpp/example/simple") {
  }
  virtual ~SimpleServer() { }

  virtual void OnRequest(const WebsocketHandlerPtr& handler,
                         const HTTPRequest& req) {
    string url = req.GetUrl();
    HTTPResponse resp;
    if (url == "/chat.html" || url.find("/static/") == 0) {
      string abs_url = root_ + "/" + url;
      ifstream ifs(abs_url.c_str());
      string content((std::istreambuf_iterator<char>(ifs)),
                          (std::istreambuf_iterator<char>()));
      string content_type;
      if (IsStringEndWith(url, ".js")) {
        content_type = "text/javascript";
      } else if (IsStringEndWith(url, ".css")) {
        content_type = "text/css";
      } else if (IsStringEndWith(url, ".swf")) {
        content_type = "application/x-shockwave-flash";
      } else {
        content_type = "text/html";
      }
      resp.SetStatus(200, "OK")
          .AddHeader("Content-Type", content_type)
          .AddBody(content);
      handler->SendResponse(resp);
      return;
    }

    if (url.find("/engine.io") == 0) {
      EngineIOServer::OnRequest(handler, req);
      return;
    }
    
    resp.SetStatus(404, "Not Found");
    handler->SendResponse(resp);
  }
  virtual void OnMessage(const EngineIOSocketPtr& socket,
                          const std::string& data ) {
    socket->SendMessage(data);
  }
  private:
   string root_;
};

int main() {
  muduo::Logger::setLogLevel(muduo::Logger::DEBUG);
  SimpleServer server(5011, "SimpleServer");
  server.Start();
}


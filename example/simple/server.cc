#include <fstream>
#include <string>

#include <muduo/base/Logging.h>
#include <woody/http/http_server.h>
#include <engineio/server.h>

using namespace std;
using namespace woody;

bool IsStringEndWith(string s, string p) {
  return s.find(p) == (s.size() - p.size());
}

class FileApp : public HTTPApplication {
 public:
  FileApp() : root_("/home/shuchao/Documents/github/engineio_cpp/example/simple") {
  }
  virtual ~FileApp() { }

  virtual void HandleRequest(const HTTPHandlerPtr& handler,
                             const HTTPRequest& req,
                             HTTPResponse& resp) {
    string url = req.GetUrl();
    if (url == "/chat.html" || url.find("/static/") == 0) {
      string abs_url = root_ + "/" + url;
      ifstream ifs(abs_url.c_str());
      string content((std::istreambuf_iterator<char>(ifs)),
                          (std::istreambuf_iterator<char>()));
      string content_type;
      if (IsStringEndWith(url, ".js")) {
        content_type = "application/javascript";
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
      resp.End();
      return;
    }

    resp.SetStatus(404, "Not Found");
    resp.End();
  }

  private:
   string root_;
};

void HandleMessage(const engineio::SocketPtr& socket, const string& data) {
  socket->SendMessage(data + " from server.");
}

int main() {
  muduo::Logger::setLogLevel(muduo::Logger::DEBUG);

  engineio::Server eio_server("EngineIOServer");
  eio_server.SetMessageCallback(boost::bind(&HandleMessage, _1, _2));

  FileApp file_app;

  HTTPServer http_server(5011, "SimpleServer");
  http_server.Handle("/engine.io", &eio_server);
  http_server.Handle("/", &file_app);
  http_server.Start();
}


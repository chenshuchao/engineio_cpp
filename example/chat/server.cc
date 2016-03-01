#define LOG_LEVEL DEBUG
#include <fstream>
#include <string>

#include <bytree/logging.hpp>
#include <bytree/string_util.hpp>
#include <woody/http/http_server.h>
#include <engineio/server.h>

using namespace std;
using namespace bytree;
using namespace woody;

class FileApp : public HTTPApplication {
 public:
  FileApp(const string& root) : root_(root) {
  }
  virtual ~FileApp() { }

  virtual void HandleRequest(const HTTPHandlerPtr& handler,
                             const HTTPRequest& req,
                             HTTPResponse& resp) {
    string url = req.GetUrl();
    if (url == "/index.html" || url.find("/static/") == 0) {
      string abs_url = root_ + url;
      ifstream ifs(abs_url.c_str());
      string content((std::istreambuf_iterator<char>(ifs)),
                          (std::istreambuf_iterator<char>()));
      string content_type;
      if (EndsWith(url, ".js")) {
        content_type = "application/javascript";
      } else if (EndsWith(url, ".css")) {
        content_type = "text/css";
      } else if (EndsWith(url, ".swf")) {
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
  socket->SendMessage("From server: " + data);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    LOG(ERROR) << "Program needs more params.";
    return 1;
  }

  string root_path(argv[1]);
  FileApp file_app(root_path);

  engineio::Server eio_server("EngineIOServer");
  eio_server.SetMessageCallback(boost::bind(&HandleMessage, _1, _2));

  HTTPServer http_server(5011, "SimpleServer");
  http_server.Handle("/engine.io", &eio_server);
  http_server.Handle("/", &file_app);
  http_server.Start();
}


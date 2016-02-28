# engineio_cpp

[Engine.IO](https://github.com/socketio/engine.io) C++ 版本

##示例

```
// Handle text message from client.
void HandleMessage(const engineio::SocketPtr& socket, 
                   const string& data) {
  socket->SendMessage("From server: " + data);
}

int main() {
  engineio::Server eio_server("EngineIOServer");
  eio_server.SetMessageCallback(boost::bind(&HandleMessage, _1, _2));

  int port = 5011;
  HTTPServer http_server(port, "SimpleServer");
  http_server.Handle("/engine.io", &eio_server);
  //...
  http_server.Start();
}
```


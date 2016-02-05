#include "engineio/transports/polling_xhr.h"

using namespace std;
using namespace woody;
using namespace engineio;

void PollingXhrTransport::DoWrite(const string& data) {
  HTTPResponse resp;
  resp.SetStatus(200, "OK")
      .AddHeader("Content-Type", "text/plain; charset=UTF-8")
      .AddBody(data);

  SendResponse(resp);
}


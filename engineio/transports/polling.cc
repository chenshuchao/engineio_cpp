#include "engineio/transports/polling.h"

using namespace std;
using namespace woody;
using namespace engineio;

void PollingTransport::SendPackets(vector<Packet>& packets) {
  string data;
  Parser::EncodePayload(packets, BaseTransport::IsSupportBinary(), data);
  DoWrite(data);
}

void PollingTransport::HandleRequest(const HTTPHandlerPtr& handler,
                                     const HTTPRequest& req,
                                     HTTPResponse& resp) {
  if ("GET" == req.GetMethod()) {
    HandlePollRequest(req, resp);
  } else if ("POST" == req.GetMethod()) {
    HandleDataRequest(req, resp);
  } else {
    resp.SetStatus(500, "Internal Server Error");
    resp.End();
  }
}

void PollingTransport::OnData(const std::string& data) {
}

void PollingTransport::HandlePollRequest(const HTTPRequest& req,
                                         HTTPResponse& resp) {
  resp_ = resp;
  TriggerFlush();
}

void PollingTransport::HandleDataRequest(const HTTPRequest& req,
                                         HTTPResponse& resp) {
}


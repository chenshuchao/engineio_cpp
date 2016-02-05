#include "engineio/transports/polling.h"

using namespace std;
using namespace woody;
using namespace engineio;

void PollingTransport::SendPacket(const EngineIOPacket& packet) {
  string data;
  EngineIOParser::EncodePayload(packet, data);
  DoWrite(data);
}


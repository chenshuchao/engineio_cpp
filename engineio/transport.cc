#include "engineio/transport.h"

using namespace std;
using namespace woody;
using namespace engineio;

void BaseTransport::OnData(const string& data) {
  EngineIOPacket packet;
  EngineIOParser::DecodePacket(data, packet);
  OnPacket(packet);
}


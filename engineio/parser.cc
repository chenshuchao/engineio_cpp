#include "engineio/parser.h"
#include "woody/base/string_util.h"

using namespace std;
using namespace engineio;
using namespace woody;

void EngineIOParser::DecodePacket(const string& data, EngineIOPacket& packet) {
  if (!data.size()) return;

  if (data[0] == 'b') {
    DecodeBase64Packet(data.substr(1), packet);
    return;
  }

  int type = data[0] - '0';
  // TODO  UTF-8
  if (!EngineIOPacket::IsValidPacketType(type)) {
    // error
    return;
  }
  packet.SetType(type);
  if (data.size() > 1) {
    packet.SetData(data.substr(1));
  }
  return;
}

void EngineIOParser::EncodePacket(const EngineIOPacket& packet, string& data) {
  int type = packet.GetType();
  data.append(int_to_string(type));
  data.append(packet.GetData()); 
}


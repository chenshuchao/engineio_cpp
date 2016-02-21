#include "engineio/parser.h"
#include "woody/base/string_util.h"

using namespace std;
using namespace engineio;
using namespace woody;

void Parser::DecodePacket(const string& data, Packet& packet) {
  if (!data.size()) return;

  if (data[0] == 'b') {
    DecodeBase64Packet(data.substr(1), packet);
    return;
  }

  int type = data[0] - '0';
  // TODO  UTF-8
  if (!Packet::IsValidPacketType(type)) {
    // error
    return;
  }
  packet.SetType(type);
  if (data.size() > 1) {
    packet.SetData(data.substr(1));
  }
  return;
}

void Parser::EncodePacket(const Packet& packet, bool support_binary, string& result) {
  int type = packet.GetType();
  result.append(int_to_string(type));
  result.append(packet.GetData()); 
}

void Parser::EncodePayload(vector<Packet>& packets, bool support_binary, string& result) {
  if (support_binary) {
    EncodePayloadAsBinary(packets, result);
    return;
  }
  for (vector<Packet>::iterator it = packets.begin();
       it != packets.end();
       it ++) {
    string r;
    EncodePacket(*it, false, r);
    result += int_to_string(r.size()) + ":" + r;
  }
}

void Parser::EncodePayloadAsBinary(vector<Packet>& packets, string& result) {
  for (vector<Packet>::iterator it = packets.begin();
       it != packets.end();
       it ++) {
    string d;
    EncodePacket(*it, true, d);
    string encoding_len = int_to_string(d.size());
    int buf_size = encoding_len.size() + 2;
    string buf(buf_size, '0');
    buf[0] = 0;
    for (int i = 0; i < encoding_len.size(); i ++) {
      buf[i+1] = encoding_len[i] - '0';
    }
    buf[buf_size - 1] = 255;
    result = buf + d;
  }
}


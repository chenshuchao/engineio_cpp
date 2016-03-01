#include "engineio/parser.h"

#include <bytree/logging.hpp>
#include <bytree/string_util.hpp>
#include <bytree/ssl_util.hpp>

using namespace std;
using namespace bytree;
using namespace engineio;

bool Parser::DecodePacket(const string& data, Packet& packet) {
  if (!data.size()) return true;

  if (data[0] == 'b') {
    return DecodeBase64Packet(data.substr(1), packet);
  }

  int type = data[0] - '0';
  // TODO  UTF-8
  if (!Packet::IsValidPacketType(type)) {
    // error
    return false;
  }
  packet.SetType(type);
  if (data.size() > 1) {
    packet.SetBody(data.substr(1));
  }
  return true;
}

// TODO
bool Parser::DecodeBase64Packet(const string& data, Packet& packet) {
  if (0 == data.size()) return false;
  packet.SetType(data[0]-'0');
  string decoded = Base64Decode(data.substr(1));
  packet.SetBody(decoded);
  return true;
}

bool Parser::DecodePayload(const string& data, vector<Packet>& packets) {
  if (!(data[0] == 'b' || data[0] > '0')) {
    return DecodePayloadAsBinary(data, packets);
  }
  int i = 0, j = 0;
  int size = data.size();
  string len_str;
  while(i < size) {
    if (data[i] != ':') {
      i ++;
    } else {
      len_str = string(data, j, i-j);
      if (len_str.empty()) {
        LOG(ERROR) << "Parser::DecodePayload - Packet length can't be empty.";
        return false;
      }
      unsigned int len = StringToInt(len_str);
      string msg(data, i+1, len);
      if (msg.size() != len) {
        LOG(ERROR) << "Parser::DecodePayload - Packet length conflict.";
        return false;
      }
      if (!msg.empty()) {
        Packet packet;
        if (!DecodePacket(msg, packet)) {
          LOG(ERROR) << "Parser::DecodePayload - DecodePacket Error.";
          return false;
        }
        packets.push_back(packet);
      }
      i += len + 1;
      j = i;
    }
  }
  if (i != j) {
    LOG(ERROR) << "Parser::DecodePayload - Data is incomplete.";
    return false;
  }
  return true;
}

bool Parser::DecodePayloadAsBinary(const string& data, vector<Packet>& packets) {
  string s;
  int i = 0, j = 0;
  int size = data.size();
  string len_str;
  while(i < size) {
    if (data[i] == 255) {
      len_str = string(data, j+1, i-j-1);
      unsigned int len = StringToInt(len_str);
      string single(data, i+1, len);
      if (single.size() != len) {
        return false;
      }
      // 310 = char length of Number.MAX_VALUE
      if (s.size() > 310) return false;

      Packet packet;
      DecodePacket(single, packet);
      packets.push_back(packet);
      j = i + 1;
    } 
    i ++;
  }
  if (i != j) {
    return false;
  }
  return true;
}

void Parser::EncodePacket(const Packet& packet, bool support_binary, string& result) {
  int type = packet.GetType();
  result.append(IntToString(type));
  result.append(packet.GetBody()); 
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
    result += IntToString(r.size()) + ":" + r;
  }
}

void Parser::EncodePayloadAsBinary(vector<Packet>& packets, string& result) {
  for (vector<Packet>::iterator it = packets.begin();
       it != packets.end();
       it ++) {
    string d;
    EncodePacket(*it, true, d);
    string encoding_len = IntToString(d.size());
    int buf_size = encoding_len.size() + 2;
    string buf(buf_size, '0');
    buf[0] = 0;
    for (unsigned int i = 0; i < encoding_len.size(); i ++) {
      buf[i+1] = encoding_len[i] - '0';
    }
    buf[buf_size - 1] = 255;
    result = buf + d;
  }
}


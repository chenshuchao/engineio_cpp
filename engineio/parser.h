#ifndef ENGINEIO_PARSER_H
#define ENGINEIO_PARSER_H

#include <string>
#include <vector>
#include "engineio/packet.h"

namespace engineio {
class Parser {
 public:
  static void DecodePacket(const std::string& data, Packet& packet);
  static void DecodeBase64Packet(const std::string& data, Packet& packet) { }

  static void EncodePacket(const Packet& packet, bool is_bianry, std::string& data);

  static void EncodePayload(std::vector<Packet>& packets, bool is_binary, std::string& data);
  static void EncodePayloadAsBinary(std::vector<Packet>& packets, std::string& data);

private:
};
}

#endif

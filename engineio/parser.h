#ifndef ENGINEIO_PARSER_H
#define ENGINEIO_PARSER_H

#include <string>
#include "engineio/packet.h"

namespace engineio {
class EngineIOParser {
 public:
  static void DecodePacket(const std::string& data, EngineIOPacket& packet);
  static void DecodeBase64Packet(const std::string& data, EngineIOPacket& packet) { }
  static void EncodePacket(const EngineIOPacket& packet, std::string& data);
  static void EncodePayload(const EngineIOPacket& packet, std::string& data);

private:
};
}

#endif

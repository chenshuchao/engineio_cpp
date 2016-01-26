#ifndef ENGINEIO_PARSER_H
#define ENGINEIO_PARSER_H

#include <string>
#include "engineio/packet.h"

namespace engineio {
class EngineIOParser {
 public:
  void DecodePacket(const std::string& data, EngineIOPacket& packet);
  void DecodeBase64Packet(const std::string& data, EngineIOPacket& packet) { }

  void EncodePacket(const EngineIOPacket& packet, std::string& data);

private:
};
}

#endif

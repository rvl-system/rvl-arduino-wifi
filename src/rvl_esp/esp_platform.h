/*
Copyright (c) Bryan Hughes <bryan@nebri.us>

This file is part of Raver Lights ESP.

Raver Lights ESP is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Raver Lights ESP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Raver Lights ESP.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RVL_ESP_ESP_PLATFORM_H_
#define RVL_ESP_ESP_PLATFORM_H_

#include <RVLLogging.h>
#include <RVLMessaging.h>

namespace ESPPlatform {

class ESPLogging : public RVLLoggingInterface {
 public:
  ESPLogging(uint16_t baudrate);
  void print(const char *s);
  void println();
  void println(const char *s);
};

class ESPPlatform : public RVLPlatformInterface {
 public:
  uint32_t getLocalTime();
  uint16_t getDeviceId();
};

class ESPTransport : public RVLTransportInterface {
 public:
  ESPTransport(uint16_t port);
  void beginWrite();
  void write8(uint8_t data);
  void write16(uint16_t data);
  void write32(uint32_t data);
  void write(uint8_t* data, uint16_t length);
  void endWrite();

  uint16_t parsePacket();
  uint8_t read8();
  uint16_t read16();
  uint32_t read32();
  void read(uint8_t* buffer, uint16_t length);
};

}  // namespace ESPPlatform

#endif  // RVL_ESP_ESP_PLATFORM_H_

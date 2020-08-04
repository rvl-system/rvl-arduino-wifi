/*
Copyright (c) Bryan Hughes <bryan@nebri.us>

This file is part of RVL Arduino Wifi.

RVL Arduino Wifi is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RVL Arduino Wifi is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RVL Arduino Wifi.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RVL_WIFI_H_
#define RVL_WIFI_H_

#include <WiFiUdp.h>
#include <rvl.h>

namespace RVLWifi {

class System : public rvl::System {
 public:
  System(const char* newssid, const char* newpassword, uint16_t newport);
  void loop();

  void beginWrite(uint8_t destination);
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

  uint16_t getDeviceId();

  uint32_t localClock();
  void print(const char* str);
  void println(const char* str);
};

void setConnectedState(bool connectedState);

}  // namespace RVLWifi

#endif  // RVL_WIFI_H_

/*
Copyright (c) Bryan Hughes <bryan@nebri.us>

This file is part of RVL WiFi.

RVL WiFi is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RVL WiFi is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RVL WiFi.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RVL_WIFI_H_
#define RVL_WIFI_H_

#include <WiFiUdp.h>
#include <rvl.hpp>

namespace RVLWifi {

class System : public rvl::System {
public:
  System(const char* newssid, const char* newpassword, uint16_t newport);
  void loop() override;

  void beginWrite(uint8_t destination) override;
  void write8(uint8_t data) override;
  void write16(uint16_t data) override;
  void write32(uint32_t data) override;
  void write(uint8_t* data, uint16_t length) override;
  void endWrite() override;

  uint16_t parsePacket() override;
  uint8_t read8() override;
  uint16_t read16() override;
  uint32_t read32() override;
  void read(uint8_t* buffer, uint16_t length) override;
  void endRead() override;

  uint16_t getDeviceId() override;

  uint32_t localClock() override;
  void print(const char* str) override;
  void println(const char* str) override;
};

void setConnectedState(bool connectedState);

} // namespace RVLWifi

#endif // RVL_WIFI_H_

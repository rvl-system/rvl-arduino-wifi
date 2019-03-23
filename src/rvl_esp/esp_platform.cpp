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

#include <string.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "./rvl_esp/esp_platform.h"

namespace ESPPlatform {

uint16_t port = 0;

// Logging implementation

ESPLogging::ESPLogging(uint16_t baudrate) {
  Serial.begin(baudrate);
}

void ESPLogging::print(const char *s) {
  Serial.print(s);
}

void ESPLogging::println() {
  Serial.println();
}

void ESPLogging::println(const char *s) {
  Serial.println(s);
}

// Platform implementation

uint32_t ESPPlatform::getLocalTime() {
  return millis();
}

uint16_t ESPPlatform::getDeviceId() {
  return WiFi.localIP()[3];
}

// Transport implementation

ESPTransport::ESPTransport(uint16_t newPort) {
  port = newPort;
}

void ESPTransport::beginWrite() {
  auto ip = WiFi.localIP();
  ip[3] = 255;
  udp.beginPacket(ip, port);
}

void ESPTransport::write8(uint8_t data) {
  udp.write(data);
}

void ESPTransport::write16(uint16_t data) {
  udp.write(data >> 8);
  udp.write(data & 0xFF);
}

void ESPTransport::write32(uint32_t data) {
  udp.write(data >> 24);
  udp.write(data >> 16 & 0xFF);
  udp.write(data >> 8 & 0xFF);
  udp.write(data & 0xFF);
}

void ESPTransport::write(uint8_t* data, uint16_t length) {
  udp.write(data, length);
}

void ESPTransport::endWrite() {
  udp.endPacket();
}

uint16_t ESPTransport::parsePacket() {
  return udp.parsePacket();
}

uint8_t ESPTransport::read8() {
  return udp.read();
}

uint16_t ESPTransport::read16() {
  uint16_t val = 0;
  val |= udp.read() << 8;
  val |= udp.read();
  return val;
}

uint32_t ESPTransport::read32() {
  uint32_t val = 0;
  val |= udp.read() << 24;
  val |= udp.read() << 16;
  val |= udp.read() << 8;
  val |= udp.read();
  return val;
}

void ESPTransport::read(uint8_t* buffer, uint16_t length) {
  udp.read(buffer, length);
}

}  // namespace ESPPlatform

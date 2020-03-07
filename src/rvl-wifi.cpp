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

#include <string.h>
#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#include <WiFiUdp.h>
#endif
#include "./rvl-wifi.h"

namespace RVLWifi {

#define STATE_DISCONNECTED 0
#define STATE_CONNECTING 1
#define STATE_CONNECTED 2

uint8_t state = STATE_DISCONNECTED;

WiFiUDP udp;

const char* ssid;
const char* password;
uint16_t port;
bool connected = false;
void (*connectionStateChangeCallback)(bool connected) = NULL;
void (*controlledStateChangeCallback)(bool connected) = NULL;
void (*deviceModeChangeCallback)(rvl::DeviceMode mode) = NULL;

Transport::Transport(const char* newssid, const char* newpassword, uint16_t newport) {
  ssid = newssid;
  password = newpassword;
  port = newport;
#ifdef ESP8266
  WiFi.setSleepMode(WIFI_NONE_SLEEP);   // Helps keep LEDs from flickering
#endif
}

void Transport::loop() {
  switch (state) {
    case STATE_DISCONNECTED:
      rvl::info("Connecting to %s", ssid);
      WiFi.begin(ssid, password);
      state = STATE_CONNECTING;
      this->setConnectedState(false);
      // Fall through here instead of breaking
    case STATE_CONNECTING:
      if (WiFi.status() == WL_CONNECTED) {
        rvl::info("Connected to WiFi with address %d.%d.%d.%d",
          WiFi.localIP()[0],
          WiFi.localIP()[1],
          WiFi.localIP()[2],
          WiFi.localIP()[3]);
        udp.begin(port);
        state = STATE_CONNECTED;
        this->setConnectedState(true);
        if (connectionStateChangeCallback != NULL) {
          connectionStateChangeCallback(true);
        }
      }
      break;
    case STATE_CONNECTED:
      if (WiFi.status() != WL_CONNECTED) {
        rvl::info("Disconnected from WiFi, retrying");
        state = STATE_DISCONNECTED;
        this->setConnectedState(false);
        if (connectionStateChangeCallback != NULL) {
          connectionStateChangeCallback(false);
        }
        break;
      }
      break;
  }
}

// Destination: 1 byte = 0-239: individual device, 240-254: multicast, 255: broadcast
void Transport::beginWrite(uint8_t destination) {
  // We don't have real multicast, so we fall back to broadcast
  if (destination >= 240) {
    IPAddress ip(255, 255, 255, 255);
    udp.beginPacket(ip, port);
  } else {
    IPAddress ip(WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], destination);
    udp.beginPacket(ip, port);
  }
}

void Transport::write8(uint8_t data) {
  udp.write(data);
}

void Transport::write16(uint16_t data) {
  udp.write(data >> 8);
  udp.write(data & 0xFF);
}

void Transport::write32(uint32_t data) {
  udp.write(data >> 24);
  udp.write(data >> 16 & 0xFF);
  udp.write(data >> 8 & 0xFF);
  udp.write(data & 0xFF);
}

void Transport::write(uint8_t* data, uint16_t length) {
  udp.write(data, length);
}

void Transport::endWrite() {
  udp.endPacket();
}

uint16_t Transport::parsePacket() {
  return udp.parsePacket();
}

uint8_t Transport::read8() {
  return udp.read();
}

uint16_t Transport::read16() {
  uint16_t val = 0;
  val |= udp.read() << 8;
  val |= udp.read();
  return val;
}

uint32_t Transport::read32() {
  uint32_t val = 0;
  val |= udp.read() << 24;
  val |= udp.read() << 16;
  val |= udp.read() << 8;
  val |= udp.read();
  return val;
}

void Transport::read(uint8_t* buffer, uint16_t length) {
  udp.read(buffer, length);
}

uint16_t Transport::getDeviceId() {
  return WiFi.localIP()[3];
}

}  // namespace RVLWifi

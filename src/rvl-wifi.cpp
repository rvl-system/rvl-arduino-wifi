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

#include <Arduino.h>
#include <string.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include "./rvl-wifi.hpp"

#include <rvl/config.hpp>

namespace RVLWifi {

#define STATE_DISCONNECTED 0
#define STATE_CONNECTING 1
#define STATE_CONNECTED 2

// Everything both protocols share, implemented once. It overrides the pure
// virtuals of whichever rvl::System interface it is given, since their read and
// write halves have the same signatures. Unlike the ESP32 transport this one is
// polled, so there is no queue and no network task to hand packets across
template <class Base> class UdpEndpoint : public Base {
public:
  UdpEndpoint(uint16_t port, const char* name) : port(port), name(name) {
  }

  bool open() {
    return udp.begin(port) != 0;
  }

  void close() {
    udp.stop();
    packetArrivalClock = UINT32_MAX;
  }

  void write8(uint8_t data) override {
    writeByte(data);
  }

  void write16(uint16_t data) override {
    writeByte(data >> 8);
    writeByte(data & 0xFF);
  }

  void write32(uint32_t data) override {
    writeByte(data >> 24);
    writeByte(data >> 16 & 0xFF);
    writeByte(data >> 8 & 0xFF);
    writeByte(data & 0xFF);
  }

  void write(uint8_t* data, uint16_t length) override {
    size_t written = udp.write(data, length);
    if (written != length) {
      rvl::error("Error sending buffer. Expected result of %d, but wrote %d",
          length, written);
    }
  }

  void endWrite() override {
    if (udp.endPacket() == 0) {
      rvl::error("Could not send %s packet", name);
    }
  }

  uint16_t parsePacket() override {
    uint16_t size = udp.parsePacket();
    if (size > 0) {
      // For a polling transport, parse time is the best arrival estimate we
      // have
      packetArrivalClock = millis();
    }
    return size;
  }

  uint8_t read8() override {
    return udp.read();
  }

  uint16_t read16() override {
    uint16_t val = 0;
    val |= udp.read() << 8;
    val |= udp.read();
    return val;
  }

  uint32_t read32() override {
    uint32_t val = 0;
    val |= udp.read() << 24;
    val |= udp.read() << 16;
    val |= udp.read() << 8;
    val |= udp.read();
    return val;
  }

  void read(uint8_t* buffer, uint16_t length) override {
    udp.read(buffer, length);
  }

  void endRead() override {
    udp.flush();
    packetArrivalClock = UINT32_MAX;
  }

  uint32_t packetArrivalTime() override {
    return packetArrivalClock;
  }

protected:
  // Every send is a broadcast. RVL never addresses a node, and the receiving
  // side filters on the header
  void beginPacket() {
    if (udp.beginPacket(IPAddress(255, 255, 255, 255), port) == 0) {
      rvl::error("Error beginning %s packet", name);
    }
  }

private:
  void writeByte(uint8_t data) {
    uint8_t result = udp.write(data);
    if (result != 1) {
      rvl::error(
          "Error sending byte. Expected result of 1, but got %d", result);
    }
  }

  uint16_t port;
  const char* name;
  WiFiUDP udp;
  uint32_t packetArrivalClock = UINT32_MAX;
};

class AnimationEndpoint : public UdpEndpoint<rvl::System::Animation> {
public:
  using UdpEndpoint::UdpEndpoint;

  void beginChannelWrite() override {
    beginPacket();
  }
};

class InfrastructureEndpoint
    : public UdpEndpoint<rvl::System::Infrastructure> {
public:
  using UdpEndpoint::UdpEndpoint;

  void beginBroadcastWrite() override {
    beginPacket();
  }

  // The coordinator is the access point, so a broadcast reaches it with the
  // same MAC-layer retries a unicast would get
  void beginCoordinatorWrite() override {
    beginPacket();
  }
};

AnimationEndpoint animationEndpoint(RVLA_PORT, "Animation");
InfrastructureEndpoint infrastructureEndpoint(RVLI_PORT, "Infrastructure");

uint8_t state = STATE_DISCONNECTED;
bool socketErrorLogged = false;

const char* ssid;
const char* password;

System::System(const char* newssid, const char* newpassword) {
  ssid = newssid;
  password = newpassword;
#ifdef ESP8266
  WiFi.setSleepMode(WIFI_NONE_SLEEP); // Helps keep LEDs from flickering
#endif
}

void System::loop() {
  switch (state) {
  case STATE_DISCONNECTED:
    rvl::info("Connecting to %s", ssid);
    WiFi.begin(ssid, password);
    state = STATE_CONNECTING;
    socketErrorLogged = false;
    rvl::setLinkUpState(false);
    rvl::setDeviceId(UNASSIGNED_DEVICE_ID);
    // Fall through here instead of breaking
  case STATE_CONNECTING:
    if (WiFi.status() != WL_CONNECTED) {
      break;
    }
    // Stay here and retry next loop if either socket fails to open. Reporting
    // the link up without both would leave the node unable to talk
    if (!animationEndpoint.open() || !infrastructureEndpoint.open()) {
      animationEndpoint.close();
      infrastructureEndpoint.close();
      if (!socketErrorLogged) {
        socketErrorLogged = true;
        rvl::error("Could not open the RVL sockets, retrying");
      }
      break;
    }
    rvl::info("Connected to WiFi with address %d.%d.%d.%d", WiFi.localIP()[0],
        WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
    state = STATE_CONNECTED;
    rvl::setDeviceId(WiFi.localIP()[3]);
    rvl::setLinkUpState(true);
    break;
  case STATE_CONNECTED:
    if (WiFi.status() != WL_CONNECTED) {
      rvl::info("Disconnected from WiFi, retrying");
      state = STATE_DISCONNECTED;
      animationEndpoint.close();
      infrastructureEndpoint.close();
      rvl::setLinkUpState(false);
      rvl::setDeviceId(UNASSIGNED_DEVICE_ID);
    }
    break;
  }
}

rvl::System::Animation& System::animation() {
  return animationEndpoint;
}

rvl::System::Infrastructure& System::infrastructure() {
  return infrastructureEndpoint;
}

bool System::isLinkUp() {
  return WiFi.status() == WL_CONNECTED;
}

uint32_t System::localClock() {
  return millis();
}

uint32_t System::random() {
#ifdef ESP8266
  return ESP.random();
#else
  return esp_random();
#endif
}

void System::print(const char* str) {
  Serial.print(str);
}

void System::println(const char* str) {
  Serial.println(str);
}

} // namespace RVLWifi

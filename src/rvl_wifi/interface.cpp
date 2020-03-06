/*
Copyright (c) Bryan Hughes <bryan@nebri.us>

This file is part of RVL Arduino.

RVL Arduino is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RVL Arduino is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RVL Arduino.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <rvl.h>
#include "./rvl_wifi/interface.h"
#include "./rvl_wifi/platform.h"

namespace RVLWifiInterface {

#define STATE_DISCONNECTED 0
#define STATE_CONNECTING 1
#define STATE_CONNECTED 2

uint8_t state = STATE_DISCONNECTED;

WiFiUDP udp;

RVLWifiPlatform::Transport* transport;

const char* ssid;
const char* password;
uint16_t port;
void (*connectionStateChangeCallback)(bool connected) = NULL;
void (*controlledStateChangeCallback)(bool connected) = NULL;
void (*deviceModeChangeCallback)(RVLDeviceMode mode) = NULL;

void initNetwork(const char* newssid, const char* newpassword, uint16_t newport) {
  ssid = newssid;
  password = newpassword;
  port = newport;

#ifdef ESP8266
  WiFi.setSleepMode(WIFI_NONE_SLEEP);   // Helps keep LEDs from flickering
#endif

  transport = new RVLWifiPlatform::Transport(&udp, newport);
  RVLMessagingInit(transport);
}

void loop() {
  switch (state) {
    case STATE_DISCONNECTED:
      rvl::info("Connecting to %s", ssid);
      WiFi.begin(ssid, password);
      state = STATE_CONNECTING;
      RVLWifiPlatform::setConnectedState(false);
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
        RVLWifiPlatform::setConnectedState(true);
        if (connectionStateChangeCallback != NULL) {
          connectionStateChangeCallback(true);
        }
      }
      break;
    case STATE_CONNECTED:
      if (WiFi.status() != WL_CONNECTED) {
        rvl::info("Disconnected from WiFi, retrying");
        state = STATE_DISCONNECTED;
        RVLWifiPlatform::setConnectedState(false);
        if (connectionStateChangeCallback != NULL) {
          connectionStateChangeCallback(false);
        }
        break;
      }
      break;
  }
  RVLMessagingLoop();
}

}  // namespace RVLWifiInterface

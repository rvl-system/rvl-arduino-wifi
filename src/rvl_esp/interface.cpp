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

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <RVLMessaging.h>
#include <RVLLogging.h>
#include "./RVL-ESP.h"
#include "./rvl_esp/esp_platform.h"

namespace Interface {

#define STATE_DISCONNECTED 0
#define STATE_CONNECTING 1
#define STATE_CONNECTED 2

uint8_t state = STATE_DISCONNECTED;

WiFiUDP udp;

ESPPlatform::ESPLogging* loggingInterface;
ESPPlatform::ESPPlatform* platform;
ESPPlatform::ESPTransport* transport;
RVLLogging* logger;

const char* ssid;
const char* password;
uint16_t port;
void (*connectionStateChangeCallback)(bool connected) = NULL;

void init(const char* newssid, const char* newpassword, uint16_t newport, uint16_t logBaudrate, RVLLogLevel logLevel) {
  ssid = newssid;
  password = newpassword;
  port = newport;

  WiFi.setSleepMode(WIFI_NONE_SLEEP);   // Helps keep LEDs from flickering

  loggingInterface = new ESPPlatform::ESPLogging(logBaudrate);
  platform = new ESPPlatform::ESPPlatform();
  transport = new ESPPlatform::ESPTransport(&udp, newport);
  logger = new RVLLogging(loggingInterface, static_cast<RVLLogLevel>(logLevel));

  RVLMessagingInit(platform, transport, logger);
}

void loop() {
  switch (state) {
    case STATE_DISCONNECTED:
      logger->info("Connecting to %s", ssid);
      WiFi.begin(ssid, password);
      state = STATE_CONNECTING;
      // Fall through here instead of breaking
    case STATE_CONNECTING:
      if (WiFi.status() == WL_CONNECTED) {
        logger->info("Connected to WiFi with address %d.%d.%d.%d",
          WiFi.localIP()[0],
          WiFi.localIP()[1],
          WiFi.localIP()[2],
          WiFi.localIP()[3]);
        udp.begin(port);
        state = STATE_CONNECTED;
        if (connectionStateChangeCallback != NULL) {
          connectionStateChangeCallback(true);
        }
      }
      break;
    case STATE_CONNECTED:
      if (WiFi.status() != WL_CONNECTED) {
        logger->info("Disconnected from WiFi, retrying");
        state = STATE_DISCONNECTED;
        if (connectionStateChangeCallback != NULL) {
          connectionStateChangeCallback(false);
        }
        break;
      }
      RVLMessagingLoop();
      break;
  }
}

void setMode(RVLDeviceMode mode) {
  platform->setDeviceMode(RVLDeviceMode::Controller);
}

void setWaveSettings(RVLWaveSettings* settings) {
  platform->setWaveSettings(settings);
}

uint32_t getAnimationClock() {
  return platform->getAnimationClock();
}

RVLLogging* getLogger() {
  return logger;
}

void onWaveSettingsUpdated(void (*callback)(RVLWaveSettings* settings)) {
  platform->onWaveSettingsUpdated(callback);
}

void onConnectionStateChanged(void (*callback)(bool connected)) {
  connectionStateChangeCallback = callback;
}

}  // namespace Interface

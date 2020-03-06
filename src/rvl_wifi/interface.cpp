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

RVLWifiPlatform::Platform* platform;
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

  platform = new RVLWifiPlatform::Platform();
  transport = new RVLWifiPlatform::Transport(&udp, newport);
  RVLMessagingInit(platform, transport);
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

RVLDeviceMode getMode() {
  return rvl::getDeviceMode();
}

void setMode(RVLDeviceMode mode) {
  rvl::setDeviceMode(mode);
  if (deviceModeChangeCallback != NULL) {
    deviceModeChangeCallback(mode);
  }
}

RVLWaveSettings* getWaveSettings() {
  return rvl::getWaveSettings();
}

void setWaveSettings(RVLWaveSettings* settings) {
  rvl::setWaveSettings(settings);
}

uint32_t getAnimationClock() {
  return rvl::getAnimationClock();
}

uint8_t getChannel() {
  return rvl::getChannel();
}

void setChannel(uint8_t channel) {
  rvl::setChannel(channel);
}

bool getPowerState() {
  return rvl::getPowerState();
}

void setPowerState(bool powerState) {
  rvl::setPowerState(powerState);
}

uint8_t getBrightness() {
  return rvl::getBrightness();
}

void setBrightness(uint8_t brightness) {
  rvl::setBrightness(brightness);
}

bool getSynchronizationState() {
  return rvl::getSynchronizationState();
}

void onWaveSettingsUpdated(void (*callback)(RVLWaveSettings* settings)) {
  platform->setOnWaveSettingsUpdatedCallback(callback);
}

void onConnectionStateChanged(void (*callback)(bool connected)) {
  connectionStateChangeCallback = callback;
}

void onControlledStateChanged(void (*callback)(bool controlled)) {
  controlledStateChangeCallback = callback;
}

void onModeChanged(void (*callback)(RVLDeviceMode mode)) {
  deviceModeChangeCallback = callback;
}

void onPowerStateChanged(void (*callback)(bool powerState)) {
  platform->setOnPowerStateUpdatedCallback(callback);
}

void onBrightnessChanged(void (*callback)(uint8_t brightness)) {
  platform->setOnBrightnessUpdatedCallback(callback);
}

void onSynchronizationStateChage(void (*callback)(bool synchronized)) {
  platform->setOnSynchronizationStateUpdatedCallback(callback);
}

}  // namespace RVLWifiInterface

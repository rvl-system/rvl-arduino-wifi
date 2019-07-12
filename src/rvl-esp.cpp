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

#include "./rvl-esp.h"
#include "./rvl_esp/interface.h"

void RVLESPInitNetwork(const char* ssid, const char* password, uint16_t port) {
  Interface::initNetwork(ssid, password, port);
}

RVLLogging* RVLESPInitLogging(RVLLogLevel logLevel) {
  return Interface::initLogging(logLevel);
}

void RVLESPLoop() {
  Interface::loop();
}

RVLDeviceMode RVLESPGetMode() {
  return Interface::getMode();
}

void RVLESPSetMode(RVLDeviceMode mode) {
  Interface::setMode(mode);
}

RVLWaveSettings* RVLESPGetWaveSettings() {
  return Interface::getWaveSettings();
}

void RVLESPSetWaveSettings(RVLWaveSettings* settings) {
  Interface::setWaveSettings(settings);
}

uint32_t RVLESPGetAnimationClock() {
  return Interface::getAnimationClock();
}

uint8_t RVLESPGetChannel() {
  return Interface::getChannel();
}

void RVLESPSetChannel(uint8_t channel) {
  Interface::setChannel(channel);
}

bool RVLESPGetPowerState() {
  return Interface::getPowerState();
}

void RVLESPSetPowerState(bool powerState) {
  Interface::setPowerState(powerState);
}

uint8_t RVLESPGetBrightness() {
  return Interface::getBrightness();
}

void RVLESPSetBrightness(uint8_t brightness) {
  Interface::setBrightness(brightness);
}

void RVLESPOnWaveSettingsUpdate(void (*callback)(RVLWaveSettings* settings)) {
  Interface::onWaveSettingsUpdated(callback);
}

void RVLESPOnConnectionStateChanged(void (*callback)(bool connected)) {
  Interface::onConnectionStateChanged(callback);
}

void RVLESPOnModeChanged(void (*callback)(RVLDeviceMode mode)) {
  Interface::onModeChanged(callback);
}

void RVLESPOnPowerStateChanged(void (*callback)(bool powerState)) {
  Interface::onPowerStateChanged(callback);
}

void RVLESPOnBrightnessChanged(void (*callback)(uint8_t brightness)) {
  Interface::onBrightnessChanged(callback);
}

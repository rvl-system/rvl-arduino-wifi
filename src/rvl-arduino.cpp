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

#include "./rvl-arduino.h"
#include "./rvl_arduino/interface.h"

void RVLInitNetwork(const char* ssid, const char* password, uint16_t port) {
  Interface::initNetwork(ssid, password, port);
}

RVLLogging* RVLInitLogging(RVLLogLevel logLevel) {
  return Interface::initLogging(logLevel);
}

void RVLLoop() {
  Interface::loop();
}

RVLDeviceMode RVLGetMode() {
  return Interface::getMode();
}

void RVLSetMode(RVLDeviceMode mode) {
  Interface::setMode(mode);
}

RVLWaveSettings* RVLGetWaveSettings() {
  return Interface::getWaveSettings();
}

void RVLSetWaveSettings(RVLWaveSettings* settings) {
  Interface::setWaveSettings(settings);
}

uint32_t RVLGetAnimationClock() {
  return Interface::getAnimationClock();
}

uint8_t RVLGetChannel() {
  return Interface::getChannel();
}

void RVLSetChannel(uint8_t channel) {
  Interface::setChannel(channel);
}

bool RVLGetPowerState() {
  return Interface::getPowerState();
}

void RVLSetPowerState(bool powerState) {
  Interface::setPowerState(powerState);
}

uint8_t RVLGetBrightness() {
  return Interface::getBrightness();
}

void RVLSetBrightness(uint8_t brightness) {
  Interface::setBrightness(brightness);
}

void RVLOnWaveSettingsUpdate(void (*callback)(RVLWaveSettings* settings)) {
  Interface::onWaveSettingsUpdated(callback);
}

void RVLOnConnectionStateChanged(void (*callback)(bool connected)) {
  Interface::onConnectionStateChanged(callback);
}

void RVLOnModeChanged(void (*callback)(RVLDeviceMode mode)) {
  Interface::onModeChanged(callback);
}

void RVLOnPowerStateChanged(void (*callback)(bool powerState)) {
  Interface::onPowerStateChanged(callback);
}

void RVLOnBrightnessChanged(void (*callback)(uint8_t brightness)) {
  Interface::onBrightnessChanged(callback);
}

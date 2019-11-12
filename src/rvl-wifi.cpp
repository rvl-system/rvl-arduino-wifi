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

#include "./rvl-wifi.h"
#include "./rvl_wifi/interface.h"

void RVLInitNetwork(const char* ssid, const char* password, uint16_t port) {
  RVLWifiInterface::initNetwork(ssid, password, port);
}

RVLLogging* RVLInitLogging(RVLLogLevel logLevel) {
  return RVLWifiInterface::initLogging(logLevel);
}

void RVLLoop() {
  RVLWifiInterface::loop();
}

RVLDeviceMode RVLGetMode() {
  return RVLWifiInterface::getMode();
}

void RVLSetMode(RVLDeviceMode mode) {
  RVLWifiInterface::setMode(mode);
}

RVLWaveSettings* RVLGetWaveSettings() {
  return RVLWifiInterface::getWaveSettings();
}

void RVLSetWaveSettings(RVLWaveSettings* settings) {
  RVLWifiInterface::setWaveSettings(settings);
}

uint32_t RVLGetAnimationClock() {
  return RVLWifiInterface::getAnimationClock();
}

uint8_t RVLGetChannel() {
  return RVLWifiInterface::getChannel();
}

void RVLSetChannel(uint8_t channel) {
  RVLWifiInterface::setChannel(channel);
}

bool RVLGetPowerState() {
  return RVLWifiInterface::getPowerState();
}

void RVLSetPowerState(bool powerState) {
  RVLWifiInterface::setPowerState(powerState);
}

uint8_t RVLGetBrightness() {
  return RVLWifiInterface::getBrightness();
}

void RVLSetBrightness(uint8_t brightness) {
  RVLWifiInterface::setBrightness(brightness);
}

bool RVLGetSynchronizationState() {
  return RVLWifiInterface::getSynchronizationState();
}

void RVLOnWaveSettingsUpdate(void (*callback)(RVLWaveSettings* settings)) {
  RVLWifiInterface::onWaveSettingsUpdated(callback);
}

void RVLOnConnectionStateChanged(void (*callback)(bool connected)) {
  RVLWifiInterface::onConnectionStateChanged(callback);
}

void RVLOnModeChanged(void (*callback)(RVLDeviceMode mode)) {
  RVLWifiInterface::onModeChanged(callback);
}

void RVLOnPowerStateChanged(void (*callback)(bool powerState)) {
  RVLWifiInterface::onPowerStateChanged(callback);
}

void RVLOnBrightnessChanged(void (*callback)(uint8_t brightness)) {
  RVLWifiInterface::onBrightnessChanged(callback);
}

void RVLOnSynchronizationStateChage(void (*callback)(bool synchronized)) {
  RVLWifiInterface::onSynchronizationStateChage(callback);
}

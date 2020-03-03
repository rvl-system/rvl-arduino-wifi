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
#endif
#include "./rvl_wifi/platform.h"

namespace RVLWifiPlatform {

// Platform implementation

uint32_t Platform::getLocalTime() {
  return millis();
}

uint16_t Platform::getDeviceId() {
  return WiFi.localIP()[3];
}

bool Platform::isNetworkAvailable() {
  return WiFi.status() == WL_CONNECTED;
}

void Platform::onWaveSettingsUpdated() {
  RVLPlatformInterface::onWaveSettingsUpdated();
  if (this->waveSettingsUpdatedCallback != NULL) {
    this->waveSettingsUpdatedCallback(this->getWaveSettings());
  }
}

void Platform::setOnWaveSettingsUpdatedCallback(void (*callback)(RVLWaveSettings* settings)) {
  this->waveSettingsUpdatedCallback = callback;
}

void Platform::onPowerStateUpdated() {
  RVLPlatformInterface::onPowerStateUpdated();
  if (this->powerStateUpdatedCallback != NULL) {
    this->powerStateUpdatedCallback(this->getPowerState());
  }
}

void Platform::setOnPowerStateUpdatedCallback(void (*callback)(bool powerState)) {
  this->powerStateUpdatedCallback = callback;
}

void Platform::onBrightnessUpdated() {
  RVLPlatformInterface::onBrightnessUpdated();
  if (this->brightnessUpdatedCallback) {
    this->brightnessUpdatedCallback(this->getBrightness());
  }
}

void Platform::setOnBrightnessUpdatedCallback(void (*callback)(uint8_t brightness)) {
  this->brightnessUpdatedCallback = callback;
}

void Platform::onSynchronizationStateUpdated() {
  RVLPlatformInterface::onSynchronizationStateUpdated();
  if (this->synchronizationStateUpdatedCallback) {
    this->synchronizationStateUpdatedCallback(this->getSynchronizationState());
  }
}

void Platform::setOnSynchronizationStateUpdatedCallback(void (*callback)(bool synchronizationState)) {
  this->synchronizationStateUpdatedCallback = callback;
}

// Transport implementation

Transport::Transport(WiFiUDP* udp, uint16_t port) {
  this->udp = udp;
  this->port = port;
}

// Destination: 1 byte = 0-239: individual device, 240-254: multicast, 255: broadcast
void Transport::beginWrite(uint8_t destination) {
  // We don't have real multicast, so we fall back to broadcast
  if (destination >= 240) {
    IPAddress ip(255, 255, 255, 255);
    this->udp->beginPacket(ip, this->port);
  } else {
    IPAddress ip(WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], destination);
    this->udp->beginPacket(ip, this->port);
  }
}

void Transport::write8(uint8_t data) {
  this->udp->write(data);
}

void Transport::write16(uint16_t data) {
  this->udp->write(data >> 8);
  this->udp->write(data & 0xFF);
}

void Transport::write32(uint32_t data) {
  this->udp->write(data >> 24);
  this->udp->write(data >> 16 & 0xFF);
  this->udp->write(data >> 8 & 0xFF);
  this->udp->write(data & 0xFF);
}

void Transport::write(uint8_t* data, uint16_t length) {
  this->udp->write(data, length);
}

void Transport::endWrite() {
  this->udp->endPacket();
}

uint16_t Transport::parsePacket() {
  return this->udp->parsePacket();
}

uint8_t Transport::read8() {
  return this->udp->read();
}

uint16_t Transport::read16() {
  uint16_t val = 0;
  val |= this->udp->read() << 8;
  val |= this->udp->read();
  return val;
}

uint32_t Transport::read32() {
  uint32_t val = 0;
  val |= this->udp->read() << 24;
  val |= this->udp->read() << 16;
  val |= this->udp->read() << 8;
  val |= this->udp->read();
  return val;
}

void Transport::read(uint8_t* buffer, uint16_t length) {
  this->udp->read(buffer, length);
}

bool connected = false;
void setConnectedState(bool connectedState) {
  connected = connectedState;
}

bool Transport::isConnected() {
  return connected;
}

}  // namespace RVLWifiPlatform

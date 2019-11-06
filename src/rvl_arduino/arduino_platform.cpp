/*
Copyright (c) Bryan Hughes <bryan@nebri.us>

This file is part of Raver Lights Arduino.

Raver Lights Arduino is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Raver Lights Arduino is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Raver Lights Arduino.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include "./rvl_arduino/arduino_platform.h"

namespace ArduinoPlatform {

// Logging implementation

void ArduinoLogging::print(const char *s) {
  Serial.print(s);
}

void ArduinoLogging::println() {
  Serial.println();
}

void ArduinoLogging::println(const char *s) {
  Serial.println(s);
}

// Platform implementation

uint32_t ArduinoPlatform::getLocalTime() {
  return millis();
}

uint16_t ArduinoPlatform::getDeviceId() {
  return WiFi.localIP()[3];
}

bool ArduinoPlatform::isNetworkAvailable() {
  return WiFi.status() == WL_CONNECTED;
}

void ArduinoPlatform::onWaveSettingsUpdated() {
  RVLPlatformInterface::onWaveSettingsUpdated();
  if (this->waveSettingsUpdatedCallback != NULL) {
    this->waveSettingsUpdatedCallback(this->getWaveSettings());
  }
}

void ArduinoPlatform::setOnWaveSettingsUpdatedCallback(void (*callback)(RVLWaveSettings* settings)) {
  this->waveSettingsUpdatedCallback = callback;
}

void ArduinoPlatform::onPowerStateUpdated() {
  RVLPlatformInterface::onPowerStateUpdated();
  if (this->powerStateUpdatedCallback != NULL) {
    this->powerStateUpdatedCallback(this->getPowerState());
  }
}

void ArduinoPlatform::setOnPowerStateUpdatedCallback(void (*callback)(bool powerState)) {
  this->powerStateUpdatedCallback = callback;
}

void ArduinoPlatform::onBrightnessUpdated() {
  RVLPlatformInterface::onBrightnessUpdated();
  if (this->brightnessUpdatedCallback) {
    this->brightnessUpdatedCallback(this->getBrightness());
  }
}

void ArduinoPlatform::setOnBrightnessUpdatedCallback(void (*callback)(uint8_t brightness)) {
  this->brightnessUpdatedCallback = callback;
}

void ArduinoPlatform::onSynchronizationStateUpdated() {
  RVLPlatformInterface::onSynchronizationStateUpdated();
  if (this->synchronizationStateUpdatedCallback) {
    this->synchronizationStateUpdatedCallback(this->getSynchronizationState());
  }
}

void ArduinoPlatform::setOnSynchronizationStateUpdatedCallback(void (*callback)(bool synchronizationState)) {
  this->synchronizationStateUpdatedCallback = callback;
}

// Transport implementation

ArduinoTransport::ArduinoTransport(WiFiUDP* udp, uint16_t port) {
  this->udp = udp;
  this->port = port;
}

void ArduinoTransport::beginWrite() {
  IPAddress ip(255, 255, 255, 255);
  this->udp->beginPacket(ip, this->port);
}

void ArduinoTransport::write8(uint8_t data) {
  this->udp->write(data);
}

void ArduinoTransport::write16(uint16_t data) {
  this->udp->write(data >> 8);
  this->udp->write(data & 0xFF);
}

void ArduinoTransport::write32(uint32_t data) {
  this->udp->write(data >> 24);
  this->udp->write(data >> 16 & 0xFF);
  this->udp->write(data >> 8 & 0xFF);
  this->udp->write(data & 0xFF);
}

void ArduinoTransport::write(uint8_t* data, uint16_t length) {
  this->udp->write(data, length);
}

void ArduinoTransport::endWrite() {
  this->udp->endPacket();
}

uint16_t ArduinoTransport::parsePacket() {
  return this->udp->parsePacket();
}

uint8_t ArduinoTransport::read8() {
  return this->udp->read();
}

uint16_t ArduinoTransport::read16() {
  uint16_t val = 0;
  val |= this->udp->read() << 8;
  val |= this->udp->read();
  return val;
}

uint32_t ArduinoTransport::read32() {
  uint32_t val = 0;
  val |= this->udp->read() << 24;
  val |= this->udp->read() << 16;
  val |= this->udp->read() << 8;
  val |= this->udp->read();
  return val;
}

void ArduinoTransport::read(uint8_t* buffer, uint16_t length) {
  this->udp->read(buffer, length);
}

}  // namespace ArduinoPlatform

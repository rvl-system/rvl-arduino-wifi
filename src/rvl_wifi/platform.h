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

#ifndef RVL_WIFI_PLATFORM_H_
#define RVL_WIFI_PLATFORM_H_

#include <WiFiUdp.h>
#include <rvl.h>

namespace RVLWifiPlatform {

class Platform : public RVLPlatformInterface {
 private:
  void (*waveSettingsUpdatedCallback)(RVLWaveSettings* settings) = NULL;
  void (*powerStateUpdatedCallback)(bool powerState) = NULL;
  void (*brightnessUpdatedCallback)(uint8_t brightness) = NULL;
  void (*synchronizationStateUpdatedCallback)(bool synchronizationState) = NULL;

 protected:
  void onWaveSettingsUpdated();
  void onPowerStateUpdated();
  void onBrightnessUpdated();
  void onSynchronizationStateUpdated();

 public:
  uint16_t getDeviceId();
  bool isNetworkAvailable();

  void setOnWaveSettingsUpdatedCallback(void (*callback)(RVLWaveSettings* settings));
  void setOnPowerStateUpdatedCallback(void (*callback)(bool powerState));
  void setOnBrightnessUpdatedCallback(void (*callback)(uint8_t brightness));
  void setOnSynchronizationStateUpdatedCallback(void (*callback)(bool synchronizationState));
};

class Transport : public RVLTransportInterface {
 private:
  WiFiUDP* udp;
  uint16_t port;

 public:
  Transport(WiFiUDP* udp, uint16_t port);
  void beginWrite(uint8_t destination);
  void write8(uint8_t data);
  void write16(uint16_t data);
  void write32(uint32_t data);
  void write(uint8_t* data, uint16_t length);
  void endWrite();

  uint16_t parsePacket();
  uint8_t read8();
  uint16_t read16();
  uint32_t read32();
  void read(uint8_t* buffer, uint16_t length);

  bool isConnected();
};

void setConnectedState(bool connectedState);

}  // namespace RVLWifiPlatform

#endif  // RVL_WIFI_PLATFORM_H_

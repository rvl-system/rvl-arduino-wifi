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

#ifndef RVL_WIFI_INTERFACE_H_
#define RVL_WIFI_INTERFACE_H_

#include <stdint.h>
#include <rvl.h>

namespace RVLWifiInterface {

void initNetwork(const char* ssid, const char* password, uint16_t port);
void loop();

RVLDeviceMode getMode();
void setMode(RVLDeviceMode mode);

RVLWaveSettings* getWaveSettings();
void setWaveSettings(RVLWaveSettings* settings);

uint32_t getAnimationClock();

uint8_t getChannel();
void setChannel(uint8_t channel);

bool getPowerState();
void setPowerState(bool powerState);

uint8_t getBrightness();
void setBrightness(uint8_t brightness);

bool getSynchronizationState();

void onConnectionStateChanged(void (*callback)(bool connected));
void onControlledStateChanged(void (*callback)(bool controlled));
void onModeChanged(void (*callback)(RVLDeviceMode mode));
void onPowerStateChanged(void (*callback)(bool powerState));
void onBrightnessChanged(void (*callback)(uint8_t brightness));
void onSynchronizationStateChage(void (*callback)(bool synchronized));

}  // namespace RVLWifiInterface

#endif  // RVL_WIFI_INTERFACE_H_

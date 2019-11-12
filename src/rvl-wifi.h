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

#ifndef RVL_WIFI_H_
#define RVL_WIFI_H_

#include <rvl.h>
#include <stdint.h>

void RVLInitNetwork(const char* ssid, const char* password, uint16_t port);
RVLLogging* RVLInitLogging(RVLLogLevel logLevel);
void RVLLoop();

RVLDeviceMode RVLGetMode();
void RVLSetMode(RVLDeviceMode mode);

RVLWaveSettings* RVLGetWaveSettings();
void RVLSetWaveSettings(RVLWaveSettings* settings);

uint32_t RVLGetAnimationClock();

uint8_t RVLGetChannel();
void RVLSetChannel(uint8_t channel);

bool RVLGetPowerState();
void RVLSetPowerState(bool powerState);

bool RVLGetSynchronizationState();

uint8_t RVLGetBrightness();
void RVLSetBrightness(uint8_t brightness);

void RVLOnWaveSettingsUpdate(void (*callback)(RVLWaveSettings* settings));
void RVLOnConnectionStateChanged(void (*callback)(bool connected));
void RVLOnModeChanged(void (*callback)(RVLDeviceMode mode));
void RVLOnPowerStateChanged(void (*callback)(bool powerState));
void RVLOnBrightnessChanged(void (*callback)(uint8_t brightness));
void RVLOnSynchronizationStateChage(void (*callback)(bool synchronized));

#endif  // RVL_WIFI_H_

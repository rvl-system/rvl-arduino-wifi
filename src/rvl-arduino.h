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

#ifndef RVL_ARDUINO_H_
#define RVL_ARDUINO_H_

#include "./rvl/rvl.h"
#include <stdint.h>

void RVLESPInitNetwork(const char* ssid, const char* password, uint16_t port);
RVLLogging* RVLESPInitLogging(RVLLogLevel logLevel);
void RVLESPLoop();

RVLDeviceMode RVLESPGetMode();
void RVLESPSetMode(RVLDeviceMode mode);

RVLWaveSettings* RVLESPGetWaveSettings();
void RVLESPSetWaveSettings(RVLWaveSettings* settings);

uint32_t RVLESPGetAnimationClock();

uint8_t RVLESPGetChannel();
void RVLESPSetChannel(uint8_t channel);

bool RVLESPGetPowerState();
void RVLESPSetPowerState(bool powerState);

uint8_t RVLESPGetBrightness();
void RVLESPSetBrightness(uint8_t brightness);

void RVLESPOnWaveSettingsUpdate(void (*callback)(RVLWaveSettings* settings));
void RVLESPOnConnectionStateChanged(void (*callback)(bool connected));
void RVLESPOnModeChanged(void (*callback)(RVLDeviceMode mode));
void RVLESPOnPowerStateChanged(void (*callback)(bool powerState));
void RVLESPOnBrightnessChanged(void (*callback)(uint8_t brightness));

#endif  // RVL_ARDUINO_H_

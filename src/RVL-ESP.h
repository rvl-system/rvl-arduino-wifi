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

#ifndef RVL_ESP_H_
#define RVL_ESP_H_

#include <RVLMessaging.h>
#include <RVLLogging.h>
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

void RVLESPOnWaveSettingsUpdate(void (*callback)(RVLWaveSettings* settings));
void RVLESPOnConnectionStateChanged(void (*callback)(bool connected));
void RVLESPOnModeChanged(void (*callback)(RVLDeviceMode mode));

#endif  // RVL_ESP_H_

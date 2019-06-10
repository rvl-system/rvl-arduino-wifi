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

#ifndef RVL_ESP_INTERFACE_H_
#define RVL_ESP_INTERFACE_H_

#include <RVLMessaging.h>
#include <RVLLogging.h>
#include <stdint.h>

namespace Interface {

void initNetwork(const char* ssid, const char* password, uint16_t port);
RVLLogging* initLogging(RVLLogLevel logLevel);
void loop();

RVLDeviceMode getMode();
void setMode(RVLDeviceMode mode);

RVLWaveSettings* getWaveSettings();
void setWaveSettings(RVLWaveSettings* settings);

uint32_t getAnimationClock();

uint8_t getChannel();
void setChannel(uint8_t channel);

void onWaveSettingsUpdated(void (*callback)(RVLWaveSettings* settings));
void onConnectionStateChanged(void (*callback)(bool connected));
void onModeChanged(void (*callback)(RVLDeviceMode mode));

}  // namespace Interface

#endif  // RVL_ESP_INTERFACE_H_

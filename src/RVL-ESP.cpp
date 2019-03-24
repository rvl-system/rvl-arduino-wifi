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

#include "./RVL-ESP.h"
#include "./rvl_esp/interface.h"

void RVLESPInit(const char* newssid, const char* newpassword, uint16_t newport, uint16_t logBaudrate, RVLLogLevel logLevel) {
  Interface::init(newssid, newpassword, newport, logBaudrate, logLevel);
}

void RVLESPLoop() {
  Interface::loop();
}

void RVLESPSetMode(RVLDeviceMode mode) {
  Interface::setMode(mode);
}

void RVLESPSetWaveSettings(RVLWaveSettings* settings) {
  Interface::setWaveSettings(settings);
}

uint32_t RVLESPGetAnimationClock() {
  return Interface::getAnimationClock();
}

RVLLogging* RVLSESPGetLogger() {
  return Interface::getLogger();
}

void RVLESPOnWaveSettingsUpdate(void (*callback)(RVLWaveSettings* settings)) {
  Interface::onWaveSettingsUpdated(callback);
}

void RVLESPOnConnectionStateChanged(void (*callback)(bool connected)) {
  Interface::onConnectionStateChanged(callback);
}

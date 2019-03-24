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

#include <Arduino.h>
#include "./RVL-ESP.h"

RVLLogging* logger;

void onSettingsUpdate(RVLWaveSettings* newSettings) {
  logger->info("Wave settings updated");
}

void onConnectionStateChanged(bool connected) {
  if (connected) {
    logger->info("Connected to WiFi");
  } else {
    logger->info("Disconnected to WiFi");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  RVLESPInitNetwork("RaverLights", "4&7EWf7p3h222;$+wg9]&4dN,dGh4^@}", 4978);
  logger = RVLESPInitLogging(RVLLogLevel::Debug);

  RVLDeviceMode mode = RVLDeviceMode::Receiver;
  RVLESPSetMode(mode);

  RVLESPOnWaveSettingsUpdate(onSettingsUpdate);
  RVLESPOnConnectionStateChanged(onConnectionStateChanged);

  logger->info("Initialized");

  if (mode == RVLDeviceMode::Controller) {
    RVLWaveSettings newSettings;
    newSettings.waves[0].h.b = 190;
    newSettings.waves[0].s.b = 255;
    newSettings.waves[0].v.b = 255;
    newSettings.waves[0].a.a = 255;
    newSettings.waves[0].a.w_t = 2;
    newSettings.waves[1].h.b = 80;
    newSettings.waves[1].s.b = 255;
    newSettings.waves[1].v.b = 255;
    newSettings.waves[1].a.b = 255;
    RVLESPSetWaveSettings(&newSettings);
  }
}

void loop() {
  RVLESPLoop();
}

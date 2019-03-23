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

#include "../../src/RVL-ESP.h"

void setup() {
  RVLESPinit(
    "network-name",
    "pass-to-network",
    4978,
    112500,
    RVLLogLevel::debug
  );
  RVLESPSetMode(RVLDeviceMode::controller);
}

void loop() {
  RVLESPLoop();
}

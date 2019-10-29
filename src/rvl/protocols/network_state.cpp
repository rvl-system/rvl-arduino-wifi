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

#include <Arduino.h>
#include <string.h>
#include "./rvl/protocols/network_state.h"
#include "./rvl/platform.h"

namespace NetworkState {

#define NUM_NODES 255
#define NODE_EXPIRATION_DURATION 10000

uint32_t nodeTimestamps[NUM_NODES];

void init() {
  memset(nodeTimestamps, 0, sizeof(uint32_t) * NUM_NODES);
}

void loop() {
  uint32_t expirationTime = millis() - NODE_EXPIRATION_DURATION;
  for (uint8_t i = 0; i < NUM_NODES; i++) {
    if (nodeTimestamps[i] > 0 && nodeTimestamps[i] < expirationTime) {
      Platform::logging->debug("Node %d expired from network map", i);
      nodeTimestamps[i] = 0;
    }
  }
}

void refreshNode(uint8_t node) {
  nodeTimestamps[node] = millis();
}

bool isNodeActive(uint8_t node) {
  return nodeTimestamps[node] > 0;
}

}  // namespace NetworkState

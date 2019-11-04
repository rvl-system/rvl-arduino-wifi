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

uint8_t controllerNode;
uint32_t controllerNodeLastRefreshed;

void init() {
  memset(nodeTimestamps, 0, sizeof(uint32_t) * NUM_NODES);
}

void loop() {
  uint32_t expirationTime = millis() - NODE_EXPIRATION_DURATION;
  for (uint8_t i = 0; i < NUM_NODES; i++) {
    if (nodeTimestamps[i] > 0 && nodeTimestamps[i] < expirationTime) {
      Platform::logging->debug("Node %d expired from the network map", i);
      nodeTimestamps[i] = 0;
    }
  }
}

void refreshNode(uint8_t node) {
  if (!isNodeActive(node)) {
      Platform::logging->debug("Adding node %d to the network map", node);
  }
  nodeTimestamps[node] = millis();
}

uint8_t getNumNodes() {
  uint8_t numNodes = 0;
  for (uint8_t i = 0; i < NUM_NODES; i++) {
    if (isNodeActive(i)) {
      numNodes++;
    }
  }
  return numNodes;
}

uint8_t getNextNode(uint8_t node) {
  for (uint8_t i = node + 1; i < NUM_NODES; i++) {
    if (isNodeActive(i)) {
      return i;
    }
  }
  for (uint8_t i = 0; i < node; i++) {
    if (isNodeActive(i)) {
      return i;
    }
  }
  return 255;
}

bool isNodeActive(uint8_t node) {
  return nodeTimestamps[node] > 0;
}

bool isNodeController(uint8_t node) {
  // First, we check if we're in controller mode, in which case we always ignore
  // remote control
  if (Platform::platform->getDeviceMode() != RVLDeviceMode::Controller) {
    return node == Platform::platform->getDeviceId();
  }
  return node == controllerNode;
}

void refreshControllerNode(uint8_t node) {
  // Check if this is the same controller node we've seen before, or not. If not,
  // there *may* two active controllers at the same time, but it's also possible
  // the old one is no longer a controller
  if (controllerNode != node) {
    // Check if the old controller hasn't broadcast in a while, meaning it's
    // likely offline or no longer in controller mode and can be replaced with
    // this new controller
    if (millis() - controllerNodeLastRefreshed > NODE_EXPIRATION_DURATION) {
      controllerNode = node;
      controllerNodeLastRefreshed = millis();
    }
  } else {
    controllerNodeLastRefreshed = millis();
  }
}

}  // namespace NetworkState

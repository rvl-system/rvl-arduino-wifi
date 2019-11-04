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

#include <stdint.h>
#include <algorithm>
#include "./rvl/rvl.h"
#include "./rvl/config.h"
#include "./rvl/platform.h"
#include "./rvl/protocols/protocol.h"
#include "./rvl/protocols/network_state.h"
#include "./rvl/protocols/clock_sync/clock_sync.h"

namespace ProtocolClockSync {

#define SYNC_PACKET_TYPE 1
#define FOLLOW_UP_PACKET_TYPE 2
#define DELAY_REQUEST_PACKET_TYPE 3
#define DELAY_RESPONSE_PACKET_TYPE 4

#define MAX_OFFSET_THRESHOLD 1000

#define SYNC_ITERATION_MODULO 500
#define SYNC_ITERATION_MODULO_MAX 1400

#define SYNC_TIMEOUT 500

uint8_t syncId = 0;
uint8_t currentSyncNode = 255;
uint32_t syncTimeout = 0;

uint32_t t1;
uint32_t t2;
uint32_t t3;
uint32_t t4;

// The clock syncing algorithm is based on the Precision Time Protocol
// Note that this is *not* spec compliant with IEEE 1588, but does use some of
// it's concepts as laid out at https://en.wikipedia.org/wiki/Precision_Time_Protocol

/*
Note: the order of packets defined here are listed in algorithmic order

Parent packet:
Type: 1 byte = 1: Sync, 2: Follow Up, 3: Delay Request 4: Delay Response
ID: 1 byte = the id of the synchronization set

Sync packet (broadcast) controller->receiver:
no body, receiver stores T2 as soon as this packet is received

Follow up packet (unicast) controller->receiver:
T1: 4 bytes = the timestamp of the first broadcast (right after it's sent)

Delay Request packet (unicast) receiver->controller:
no body, receiver stores T3 right before this packet is sent

Delay Response packet (unicast) controller->receiver:
T4: 4 bytes = the timestamp of the last broadcast (right before it's sent)
*/

void synchronizeNextNode() {
  // Make sure we are in our alloted network usage time, and if not, skip for now
  uint32_t localTime = Platform::platform->getLocalTime();
  uint32_t syncTime = localTime % CLIENT_SYNC_INTERVAL;
  if (syncTime < SYNC_ITERATION_MODULO || syncTime > SYNC_ITERATION_MODULO_MAX) {
    return;
  }

  // Check if there is a current sync in progress, and if so skip
  if (syncTimeout > 0) {
    // Check if the sync has timed out and we need to move to the next one,
    // or let this sync continue doing its thing
    if (Platform::platform->getLocalTime() > syncTimeout) {
      Platform::logging->debug("Clock synchronization with node %d has timed out", currentSyncNode);
      syncTimeout = 0;
    } else {
      return;
    }
  }

  // First we check if the next node has value 255 (broadcast) which means there
  // are no other known nodes in this system, and we can stop here
  uint8_t nextSyncNode = NetworkState::getNextNode(currentSyncNode);
  if (nextSyncNode == 255) {
    return;
  }

  // If we got here, then this ia valid next sync node
  currentSyncNode = nextSyncNode;
  Platform::logging->debug("Starting clock synchronization with node %d", currentSyncNode);

  // Start the timeout. If the two nodes don't finish exchanging information
  // within this time, we skip and go to the next one.
  syncTimeout = Platform::platform->getLocalTime() + SYNC_TIMEOUT;

  // Send the Sync packet
  Platform::transport->beginWrite();
  Protocol::sendHeader(PACKET_TYPE_CLOCK_SYNC, currentSyncNode);
  Platform::transport->write8(SYNC_PACKET_TYPE);  // reserved
  Platform::transport->write8(syncId++);
  Platform::transport->endWrite();

  // Send the follow up packet of the time when we *finished* sending the sync packet
  // This gives a more accurate clock offset calculation
  uint32_t clock = Platform::platform->getAnimationClock();
  Platform::transport->beginWrite();
  Protocol::sendHeader(PACKET_TYPE_CLOCK_SYNC, currentSyncNode);
  Platform::transport->write8(FOLLOW_UP_PACKET_TYPE);  // reserved
  Platform::transport->write8(syncId);
  Platform::transport->write32(clock);
  Platform::transport->endWrite();
}

void init() {
}

void loop() {
  if (Platform::platform->getDeviceMode() != RVLDeviceMode::Controller) {
    return;
  }
  synchronizeNextNode();
}

void parsePacket(uint8_t source) {
  uint8_t packetType = Platform::transport->read8();
  uint8_t id = Platform::transport->read8();

  switch (packetType) {
    case SYNC_PACKET_TYPE: {
      t2 = Platform::platform->getAnimationClock();
      break;
    }

    case FOLLOW_UP_PACKET_TYPE: {
      t1 = Platform::transport->read32();
      t3 = Platform::platform->getAnimationClock();
      Platform::transport->beginWrite();
      Protocol::sendHeader(PACKET_TYPE_CLOCK_SYNC, source);
      Platform::transport->write8(DELAY_REQUEST_PACKET_TYPE);
      Platform::transport->write8(id);
      Platform::transport->endWrite();
      break;
    }

    case DELAY_REQUEST_PACKET_TYPE: {
      uint32_t clock = Platform::platform->getAnimationClock();
      Platform::transport->beginWrite();
      Protocol::sendHeader(PACKET_TYPE_CLOCK_SYNC, source);
      Platform::transport->write8(DELAY_RESPONSE_PACKET_TYPE);
      Platform::transport->write8(id);
      Platform::transport->write32(clock);
      Platform::transport->endWrite();

      // Now that we finished the clock sync process, we can synchronize the next node
      synchronizeNextNode();
      break;
    }

    case DELAY_RESPONSE_PACKET_TYPE: {
      t4 = Platform::transport->read32();
      int32_t meanPathDelay = ((static_cast<int32_t>(t2) - static_cast<int32_t>(t1)) +
        (static_cast<int32_t>(t4) - static_cast<int32_t>(t3))) / 2;
      Platform::platform->setAnimationClock(t4 + meanPathDelay);
      Platform::logging->debug("Round trip clock sync took %dms with latency %dms", t4 + meanPathDelay - t1, meanPathDelay);
      break;
    }

    default: {
      Platform::logging->debug("Received unknown clock sync subpacket type %d", packetType);
      break;
    }
  }
}

}  // namespace ProtocolClockSync

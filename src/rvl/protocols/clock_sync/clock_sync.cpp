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
bool hasSyncedThisLoop = false;

uint8_t syncId = 0;
uint8_t currentSyncNode = 255;

uint32_t t1;
uint32_t t2;
uint32_t t3;
uint32_t t4;

/*
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

void synchronizeNode(uint8_t node) {
  // Send the Sync packet
  Platform::logging->debug("Sending Clock Sync sync packet");
  Platform::transport->beginWrite();
  Protocol::sendHeader(PACKET_TYPE_CLOCK_SYNC, node);
  Platform::transport->write8(SYNC_PACKET_TYPE);  // reserved
  Platform::transport->write8(syncId++);
  Platform::transport->endWrite();

  uint32_t clock = Platform::platform->getAnimationClock();

  // Send the follow up packet of the time when we *finished* sending the sync packet
  Platform::transport->beginWrite();
  Protocol::sendHeader(PACKET_TYPE_CLOCK_SYNC, node);
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
  if (Platform::platform->getLocalTime() % CLIENT_SYNC_INTERVAL < SYNC_ITERATION_MODULO) {
    hasSyncedThisLoop = false;
    return;
  }
  if (hasSyncedThisLoop) {
    return;
  }
  hasSyncedThisLoop = true;
  uint8_t nextNode = NetworkState::getNextNode(255);
  Platform::logging->debug("Next node: %d", nextNode);
  if (nextNode != 255) {
    Platform::logging->debug("Starting clock synchronization");
    synchronizeNode(nextNode);
  }
}

void parsePacket(uint8_t source) {
  uint8_t packetType = Platform::transport->read8();
  uint8_t id = Platform::transport->read8();

  switch (packetType) {
    case SYNC_PACKET_TYPE: {
      Platform::logging->debug("Parsing Clock Sync sync packet");
      t2 = Platform::platform->getAnimationClock();
      break;
    }

    case FOLLOW_UP_PACKET_TYPE: {
      Platform::logging->debug("Parsing Clock Sync follow up packet");
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
      Platform::logging->debug("Parsing Clock Sync delay request packet");
      Platform::transport->beginWrite();
      Protocol::sendHeader(PACKET_TYPE_CLOCK_SYNC, source);
      Platform::transport->write8(DELAY_RESPONSE_PACKET_TYPE);
      Platform::transport->write8(id);
      Platform::transport->write32(clock);
      Platform::transport->endWrite();

      // Get the next active node in the network. If this node has a lower address
      // than the previous one, then that means we've wrapped around the list and can stop
      uint8_t nextNode = NetworkState::getNextNode(source);
      if (nextNode < source) {
        synchronizeNode(nextNode);
      }
      break;
    }

    case DELAY_RESPONSE_PACKET_TYPE: {
      Platform::logging->debug("Parsing Clock Sync delay response packet with parameters t1=%d t2=%d t3=%d t4=%d", t1, t2, t3, t4);
      t4 = Platform::transport->read32();
      int32_t meanPathDelay = ((static_cast<int32_t>(t2) - static_cast<int32_t>(t1)) +
        (static_cast<int32_t>(t4) - static_cast<int32_t>(t3))) / 2;
      Platform::logging->debug("meanPathDelay=%d, t4=%d", meanPathDelay, t4);
      Platform::platform->setAnimationClock(t4 + meanPathDelay);
      break;
    }

    default: {
      Platform::logging->debug("Received unknown clock sync subpacket type %d", packetType);
      break;
    }
  }
}

}  // namespace ProtocolClockSync

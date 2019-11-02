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
#include "./rvl/rvl.h"
#include "./rvl/config.h"
#include "./rvl/platform.h"
#include "./rvl/protocols/protocol.h"
#include "./rvl/protocols/clock_sync/clock_sync.h"

namespace ProtocolClockSync {

#define REFERENCE_BROADCAST_PACKET_TYPE 1
#define OBSERVATION_TIME_PACKET_TYPE 2

#define MAX_OFFSET_THRESHOLD 1000

uint16_t currentReferenceID = 0;
uint32_t currentReferenceBroadcastTimes[240];

uint8_t referenceIdCounter = 0;
uint32_t nextSyncTime = 0;

/*
Parent packet:
Type: 1 byte = 1: reference broadcast, 2: observed time, 3: select new reference controller
ID: 2 bytes = Upper byte is device ID (to make it unique), lower byte is a counter
Clock: 4 bytes = Normally ignored, but is used by receivers that had not previously
  been synchronized to immediately synchronize in a loose manner. The receiver will
  necessarily be off by fair amount of latency, but it's better than nothing.
  Note: this can also be used to roughly measure latency, although only after the system
  is considered to be "well synchronized." It's _possible_ that "well synchronized" could
  be defined as all type 2 packet's clocks being within some margin of error
Reserved: 1 byte
*/

void init() {
  nextSyncTime = Platform::platform->getLocalTime();
}

void loop() {
  if (Platform::platform->getDeviceMode() != RVLDeviceMode::Controller) {
    return;
  }
  if (Platform::platform->getLocalTime() < nextSyncTime) {
    return;
  }
  nextSyncTime = Platform::platform->getLocalTime() + CLIENT_SYNC_INTERVAL;
  currentReferenceID = (Platform::platform->getDeviceId() << 8) + referenceIdCounter;
  Platform::logging->debug("Sending reference broadcast");
  Platform::transport->beginWrite();
  Protocol::sendBroadcastHeader(PACKET_TYPE_CLOCK_SYNC);
  Platform::transport->write8(REFERENCE_BROADCAST_PACKET_TYPE);  // type
  Platform::transport->write16(currentReferenceID);  // id
  Platform::transport->write32(Platform::platform->getAnimationClock());  // clock
  Platform::transport->write8(0);  // reserver
  Platform::transport->endWrite();
  referenceIdCounter++;
}

void parsePacket(uint8_t source) {
  Platform::logging->debug("Parsing Clock Sync packet");
  uint8_t packetType = Platform::transport->read8();
  uint16_t id = Platform::transport->read16();
  uint32_t commandTime = Platform::transport->read32();
  Platform::transport->read8(); // Reserved

  switch (packetType) {
    case REFERENCE_BROADCAST_PACKET_TYPE: {
      uint32_t observedTime = Platform::platform->getAnimationClock();
      if (observedTime < commandTime - MAX_OFFSET_THRESHOLD || observedTime > commandTime + MAX_OFFSET_THRESHOLD) {
        Platform::logging->debug(
          "Local time %d is outside the threshold of reference broadcast time %d. Replacing local time with reference time",
          observedTime, commandTime);
        Platform::platform->setClockOffset(
          static_cast<int32_t>(commandTime) -
          static_cast<int32_t>(Platform::platform->getLocalTime()));
        break;
      }
      currentReferenceID = id;
      Platform::logging->debug("Responding to reference broadcast id=%d with local time % d", id, observedTime);
      Platform::transport->beginWrite();
      Protocol::sendBroadcastHeader(PACKET_TYPE_CLOCK_SYNC);
      Platform::transport->write8(OBSERVATION_TIME_PACKET_TYPE);  // type
      Platform::transport->write16(id);  // id
      Platform::transport->write32(observedTime);  // clock
      Platform::transport->write8(0);  // reserver
      Platform::transport->endWrite();
      break;
    }

    case OBSERVATION_TIME_PACKET_TYPE: {
      // Check if we received an observation packet from an old or missed reference
      // broadcast, and if so ignore it since we likely don't have enough info to act on it
      Platform::logging->debug("observation packet ids: %d, %d", id, currentReferenceID);
      if (id != currentReferenceID) {
        break;
      }
      Platform::logging->debug("Received observation packet from %d with observed time %d", source, commandTime);
      // TODO: figure out when we've collected enough observations to calculate the offset, then do it
      break;
    }

    default:
      Platform::logging->debug("Received unknown clock sync subpacket type %d", packetType);
      break;
  }
}

}  // namespace ProtocolClockSync

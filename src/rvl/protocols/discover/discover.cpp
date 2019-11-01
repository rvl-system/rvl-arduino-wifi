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

#include <limits.h>
#include <stdint.h>
#include "./rvl/rvl.h"
#include "./rvl/protocols/protocol.h"
#include "./rvl/protocols/discover/discover.h"
#include "./rvl/protocols/network_state.h"
#include "./rvl/platform.h"
#include "./rvl/config.h"

namespace ProtocolDiscover {

uint32_t nextSyncTime = INT_MAX;

/*
Reserved: 4 bytes
*/

void init() {
  nextSyncTime = Platform::platform->getLocalTime() + CLIENT_SYNC_INTERVAL / 4;
}

void loop() {
  if (Platform::platform->getLocalTime() < nextSyncTime) {
    return;
  }
  nextSyncTime = Platform::platform->getLocalTime() + CLIENT_SYNC_INTERVAL;
  sync();
}

void sync() {
  Platform::logging->debug("Broadcasting discover packet");
  Platform::transport->beginWrite();
  Protocol::sendBroadcastHeader(PACKET_TYPE_DISCOVER);
  Platform::transport->write32(0);  // reserved
  Platform::transport->endWrite();
}

void parsePacket(uint8_t source) {
  Platform::logging->debug("Parsing Discover packet");
  Platform::transport->read32();  // reserved
  NetworkState::refreshNode(source);
}

}  // namespace ProtocolDiscover

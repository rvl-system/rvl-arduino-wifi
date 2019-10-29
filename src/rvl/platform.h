/*
Copyright (c) 2018 Bryan Hughes <bryan@nebri.us>

This file is part of Raver Lights.

Raver Lights is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Raver Lights is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Raver Lights.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RVL_PLATFORM_H_
#define RVL_PLATFORM_H_

#include "./rvl/rvl.h"

namespace Platform {

extern RVLTransportInterface* transport;
extern RVLLogging* logging;
extern RVLPlatformInterface* platform;

void init(RVLPlatformInterface* newPlatform, RVLTransportInterface* newTransport, RVLLogging* newLogging);

}  // namespace Platform

#endif  // RVL_PLATFORM_H_
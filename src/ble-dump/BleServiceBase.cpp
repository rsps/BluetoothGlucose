/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#include <chrono>
#include <thread>
#include <BleServiceBase.h>
#include <exceptions.h>

namespace rsp {

BleServiceBase::BleServiceBase(const TrustedDevice &arDevice, uuid::Identifiers aServiceUuid)
    : mDevice(arDevice),
      mService(mDevice.GetServiceById(aServiceUuid))
{
}

SimpleBLE::Characteristic BleServiceBase::findCharacteristicByUuid(const std::string &arUUID)
{
    for (auto &ch : mService.characteristics()) {
        if (ch.uuid() == arUUID) {
            return ch;
        }
    }
    THROW_WITH_BACKTRACE1(ECharacteristicNotFound, uuid::ToName(uuid::FromString(arUUID)));
}

void BleServiceBase::delay(std::uint32_t aMilliseconds, volatile const bool *apAbort)
{
    for (int i = 0; i < aMilliseconds; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (apAbort && *apAbort) {
            break;
        }
    }
}

} // namespace rsp

/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#include "BleServiceBase.h"
#include "exceptions.h"

namespace rsp {

BleServiceBase::BleServiceBase(const SimpleBLE::Service &arService)
    : mService(arService)
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

} // namespace rsp


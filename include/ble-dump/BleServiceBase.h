/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef BLUETOOTHGLUCOSE_BLESERVICEBASE_H
#define BLUETOOTHGLUCOSE_BLESERVICEBASE_H

#include <logging/LogChannel.h>
#include "UUID.h"
#include <simpleble/SimpleBLE.h>
#include "TrustedDevice.h"

namespace rsp {

class BleServiceBase
{
public:
    explicit BleServiceBase(const TrustedDevice &arDevice, uuid::Identifiers aServiceUuid);
    virtual ~BleServiceBase() = default;

    [[nodiscard]] SimpleBLE::Service& GetService() { return mService; }

    [[nodiscard]] uuid::Identifiers GetId() const { return mId; }
    [[nodiscard]] bool operator==(uuid::Identifiers aId) const { return mId == aId; }

protected:
    uuid::Identifiers mId = uuid::Identifiers::None;
    TrustedDevice mDevice;
    SimpleBLE::Service mService;

    SimpleBLE::Characteristic findCharacteristicByUuid(const std::string &arUUID);
    static void delay(std::uint32_t aMilliseconds, const bool volatile *apAbort = nullptr);
};

template<class T>
class BleService : public BleServiceBase, public rsp::logging::NamedLogger<T>
{
public:
    explicit BleService(const TrustedDevice &arDevice, uuid::Identifiers aServiceUuid) : BleServiceBase(arDevice, aServiceUuid) {}
};


} // namespace rsp

#endif //BLUETOOTHGLUCOSE_BLESERVICEBASE_H

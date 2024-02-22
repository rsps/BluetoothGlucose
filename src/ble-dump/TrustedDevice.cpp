/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <TrustedDevice.h>
#include <exceptions.h>
#include <UUID.h>
#include <magic_enum.hpp>

namespace rsp {

TrustedDevice::TrustedDevice(const SimpleBLE::Peripheral &arDevice)
    : mDevice(arDevice)
{
    mLogger.Info() << "Attempting to connect with " << mDevice.address() << std::endl;
    mDevice.connect();
    if (mDevice.is_connected()) {
        if (mDevice.initialized()) {
            return;
        }
    }
    mLogger.Error() << "Failed to connect to " << " [" << mDevice.address() << "]" << std::endl;
    THROW_WITH_BACKTRACE(EDeviceNotPaired);
}

TrustedDevice::~TrustedDevice()
{
    if (mDevice.is_connected()) {
        mDevice.disconnect();
    }
}

bool TrustedDevice::HasServiceWithId(uuid::Identifiers aId)
try {
    auto service = GetServiceById(uuid::Identifiers::GlucoseService);
    return true;
}
catch (const EServiceNotFound&) {
    return false;
}

SimpleBLE::Service TrustedDevice::GetServiceById(uuid::Identifiers aId)
{
    for (auto &service : mDevice.services()) {
        if (uuid::FromString(service.uuid()) == aId) {
            return service;
        }
    }
    THROW_WITH_BACKTRACE1(EServiceNotFound, std::string(magic_enum::enum_name(aId)));
}

std::ostream& operator<<(std::ostream &o, TrustedDevice &arDevice)
{
    o << arDevice.GetPeripheral();
    return o;
}

} // rsp

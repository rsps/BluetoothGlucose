/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef BLUETOOTHGLUCOSE_BLE_DUMP_TRUSTEDDEVICE_H
#define BLUETOOTHGLUCOSE_BLE_DUMP_TRUSTEDDEVICE_H

#include <simpleble/SimpleBLE.h>
#include <vector>
#include "UUID.h"
#include "GlucoseServiceProfile.h"
#include <logging/LogChannel.h>

namespace rsp {

class TrustedDevice : public rsp::logging::NamedLogger<TrustedDevice>
{
public:
    explicit TrustedDevice(std::shared_ptr<SimpleBluez::Device> &arDevice);
    ~TrustedDevice() override;

    void PrintServices();

    [[nodiscard]] bool HasGlucoseService();
    GlucoseServiceProfile GetGlucoseService();

protected:
    std::shared_ptr<SimpleBluez::Device> mpDevice;
    std::vector<UUID> mServiceList{};

    void makeServiceList();
    UUID& findServiceById(UUID::Identifiers aId);
};

} // rsp

#endif //BLUETOOTHGLUCOSE_BLE_DUMP_TRUSTEDDEVICE_H

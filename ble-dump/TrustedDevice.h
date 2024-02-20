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

#include <ostream>
#include <logging/LogChannel.h>
#include "UUID.h"
#include <vector>

namespace rsp {

class TrustedDevice : public rsp::logging::NamedLogger<TrustedDevice>
{
public:
    explicit TrustedDevice(const SimpleBLE::Peripheral &arDevice);
    ~TrustedDevice() override;

    [[nodiscard]] bool HasServiceWithId(uuid::Identifiers aId);
    [[nodiscard]] SimpleBLE::Service GetServiceById(uuid::Identifiers aId);

    SimpleBLE::Peripheral& GetPeripheral()  { return mDevice; }

protected:
    SimpleBLE::Peripheral mDevice;
};

std::ostream& operator<<(std::ostream &o, TrustedDevice &arDevice);

} // rsp

#endif //BLUETOOTHGLUCOSE_BLE_DUMP_TRUSTEDDEVICE_H

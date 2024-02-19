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

#include <vector>
#include "UUID.h"
#include <logging/LogChannel.h>

namespace rsp {

class TrustedDevice : public rsp::logging::NamedLogger<TrustedDevice>
{
public:
    explicit TrustedDevice(SimpleBLE::Peripheral aDevice);
    ~TrustedDevice() override;

    void PrintServices();

    [[nodiscard]] bool HasServiceWithId(uuid::Identifiers aId);
    [[nodiscard]] SimpleBLE::Service GetServiceById(uuid::Identifiers aId);

    SimpleBLE::Peripheral& GetPeripheral() { return mDevice; }

protected:
    SimpleBLE::Peripheral mDevice;
};

std::ostream& operator<<(std::ostream &o, SimpleBLE::Peripheral &arDevice);


} // rsp

#endif //BLUETOOTHGLUCOSE_BLE_DUMP_TRUSTEDDEVICE_H

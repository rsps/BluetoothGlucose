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

#include "simplebluez/Bluez.h"

namespace rsp {

struct GlucoseRecord {

};

class TrustedDevice
{
public:
    explicit TrustedDevice(std::shared_ptr<SimpleBluez::Device> &arDevice);
    ~TrustedDevice();

    void PrintInfo();
    std::vector<GlucoseRecord> GetRecords();

protected:
    std::shared_ptr<SimpleBluez::Device> mpDevice;

    static void debug(const std::string &arTitle, const SimpleBluez::ByteArray &new_value);

};

} // rsp

#endif //BLUETOOTHGLUCOSE_BLE_DUMP_TRUSTEDDEVICE_H

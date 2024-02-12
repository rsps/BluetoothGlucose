/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef BLUETOOTHGLUCOSE_BLE_DUMP_EXCEPTIONS_H
#define BLUETOOTHGLUCOSE_BLE_DUMP_EXCEPTIONS_H

#include <exceptions/CoreException.h>

namespace rsp {

class ENoAdapter : public exceptions::ETerminate
{
public:
    explicit ENoAdapter() : ETerminate(-2) {}
};

class ENoDevice : public exceptions::ETerminate
{
public:
    explicit ENoDevice() : ETerminate(-3) {}
};

class EDeviceNotFound : public exceptions::ETerminate
{
public:
    explicit EDeviceNotFound() : ETerminate(-4) {}
};

class EDeviceNotPaired : public exceptions::ETerminate
{
public:
    explicit EDeviceNotPaired() : ETerminate(-5) {}
};


} // namespace rsp

#endif //BLUETOOTHGLUCOSE_BLE_DUMP_EXCEPTIONS_H

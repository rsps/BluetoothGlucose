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

class ENoAdapter : public exceptions::ApplicationException
{
public:
    explicit ENoAdapter() : ApplicationException("Missing adapter option.") {}
};

class ENoDevice : public exceptions::ApplicationException
{
public:
    explicit ENoDevice() : ApplicationException("Missing device option.") {}
};

class EDeviceNotFound : public exceptions::ApplicationException
{
public:
    explicit EDeviceNotFound() : ApplicationException("No devices found.") {}
};

class EDeviceNotPaired : public exceptions::ApplicationException
{
public:
    explicit EDeviceNotPaired() : ApplicationException("Could not pair with device.") {}
};

class EGlucoseArgument : public exceptions::ApplicationException
{
public:
    explicit EGlucoseArgument() : ApplicationException("Invalid glucose measurement received") {}
};

class EServiceNotFound : public exceptions::ApplicationException
{
public:
    explicit EServiceNotFound(const std::string &arServiceName) : ApplicationException("Service not found: " + arServiceName) {}
};

} // namespace rsp

#endif //BLUETOOTHGLUCOSE_BLE_DUMP_EXCEPTIONS_H

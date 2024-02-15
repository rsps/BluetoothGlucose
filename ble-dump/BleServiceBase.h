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

#include <chrono>
#include <thread>
#include <logging/LogChannel.h>

namespace rsp {

class BleServiceBase
{
public:
    static void Delay(int aMilliseconds, const bool volatile *apAbort = nullptr)
    {
        for (int i = 0; i < aMilliseconds; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            if (apAbort && *apAbort) {
                break;
            }
        }
    }
};

template<class T>
class BleService : public BleServiceBase, public rsp::logging::NamedLogger<T>
{
};


} // namespace rsp

#endif //BLUETOOTHGLUCOSE_BLESERVICEBASE_H

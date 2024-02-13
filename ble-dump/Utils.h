/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef BLUETOOTHGLUCOSE_BLE_DUMP_DELAY_H
#define BLUETOOTHGLUCOSE_BLE_DUMP_DELAY_H

#include <thread>
namespace rsp {

class Utils
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

} // namespace rsp

#endif //BLUETOOTHGLUCOSE_BLE_DUMP_DELAY_H

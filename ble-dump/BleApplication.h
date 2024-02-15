/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef BLUETOOTHGLUCOSE_BLE_DUMP_BLEAPPLICATION_H
#define BLUETOOTHGLUCOSE_BLE_DUMP_BLEAPPLICATION_H

#include <thread>
#include <application/ApplicationBase.h>
#include <utils/Thread.h>
#include "simplebluez/Bluez.h"

namespace rsp {

/**
 * \brief Bluetooth Low Energy application to retrieve glucose data from BGM.
 *
 * \see GATT Specification Supplement (https://www.bluetooth.com/specifications/specs/gatt-specification-supplement-5/)
 * \see Glucose Service (https://www.bluetooth.com/specifications/gls-1-0-1/)
 * \see Glucose Profile (https://www.bluetooth.com/specifications/glp-1-0-1/)
 */
class BleApplication: public rsp::application::ApplicationBase
{
public:
    BleApplication(int argc, const char **argv);
    ~BleApplication() override;

    static BleApplication& Get() { return ApplicationBase::Get<BleApplication>(); }

protected:
    SimpleBluez::Bluez mBluez;
    utils::Thread mBluezThread;
    std::vector<std::shared_ptr<SimpleBluez::Device>> mPeripherals;
    std::string mDeviceMAC{};
    bool mVerbose = false;

    void beforeExecute() override;
    void afterExecute() override;
    void showHelp() override;
    void showVersion() override;
    void handleOptions() override;
    void execute() override;
    void bluezThreadExecute();

    std::shared_ptr<SimpleBluez::Adapter> getAdapter();
    void scan(std::shared_ptr<SimpleBluez::Adapter> &arAdapter, bool aVerbose);
    std::shared_ptr<SimpleBluez::Device> getDevice(std::shared_ptr<SimpleBluez::Adapter> &arAdapter);
};

} // rsp

#endif //BLUETOOTHGLUCOSE_BLE_DUMP_BLEAPPLICATION_H

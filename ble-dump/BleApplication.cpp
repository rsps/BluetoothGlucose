/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <chrono>
#include "BleApplication.h"
#include <application/Console.h>
#include <exceptions/SignalHandler.h>
#include <utils/Function.h>
#include <simplebluez/Exceptions.h>
#include "exceptions.h"
#include "TrustedDevice.h"
#include "Utils.h"
#include <utils/StrUtils.h>

using namespace rsp::application;
using namespace rsp::exceptions;
using namespace rsp::utils;

namespace rsp {

BleApplication::BleApplication(int argc, const char **argv)
    : ApplicationBase(argc, argv, "ble-dump"),
      mBluezThread("Bluez")
{
    SignalHandler::Register(Signals::Terminate, [this]() { Terminate(0); });

    mBluez.init();

    mBluezThread.SetExecute(Method(this, &BleApplication::bluezThreadExecute));

}

BleApplication::~BleApplication()
{
    mBluezThread.Stop();
    SignalHandler::Unregister(Signals::Terminate);
}

void BleApplication::beforeExecute()
{
    ApplicationBase::beforeExecute();

    if (mCmd.GetCommands().empty()) {
        showHelp();
        THROW_WITH_BACKTRACE1(ETerminate, cResultSuccess);
    }
}

void BleApplication::afterExecute()
{
    ApplicationBase::afterExecute();
}

void BleApplication::showHelp()
{
    Console::Info() << ""
       "Usage: ble-bump <options> <command>\n"
       "  Options:\n"
       "    --adapter=<adapter name>        Name of the BlueTooth adapter to use. Defaults to first.\n"
       "    --device=<device address>       Address of BlueTooth device to connect to.\n"
       "    -h                              Same as --help.\n"
       "    --help                          Show this help information.\n"
       "    --log=<filename|syslog>         Log output to file.\n"
       "    --loglevel=<[error|info|debug]> Set the log level for file logging,\n"
       "                                    default level is info.\n"
       "    --version                       Show version.\n"
       "    -v                              Increase verbosity level to Info.\n"
       "    -vv                             Increase verbosity level to Debug.\n"
       "\n"
       "Commands:\n"
       "    devices                         List found BlueTooth devices\n"
       "    dump                            Dump records from device\n"
    << std::endl;

    auto adapters = mBluez.get_adapters();
    if (adapters.empty()) {
        Console::Info() << "No Bluetooth adapters found." << std::endl;
    }
    else {
        Console::Info() << "The following adapters were found:" << std::endl;
        for (int i = 0; i < adapters.size(); i++) {
            Console::Info() << "[" << i << "] " << adapters[i]->identifier() << " [" << adapters[i]->address() << "]" << std::endl;
        }
    }
}

void BleApplication::showVersion()
{
    ApplicationBase::showVersion();
}

void BleApplication::handleOptions()
{
    ApplicationBase::handleOptions();
}

void BleApplication::execute()
{
    mBluezThread.Start();
    auto adapter = getAdapter();
    scan(adapter);

    if (mCmd.GetCommands()[0] == "devices") {
        listDevices();
    }
    else if (mCmd.GetCommands()[0] == "dump") {
        auto device = getDevice(adapter);
        TrustedDevice td(device);
        td.PrintInfo();
        auto recs = td.GetRecords();

        Console::Info() << "Records received: " << recs.size() << std::endl;

        Utils::Delay(1000);
    }
    Terminate(cResultSuccess);
}

void BleApplication::bluezThreadExecute()
{
    mBluez.run_async();
    std::this_thread::sleep_for(std::chrono::microseconds(100));
}

std::shared_ptr<SimpleBluez::Adapter> BleApplication::getAdapter()
{
    std::string option_value;
    auto adapters = mBluez.get_adapters();
    if (mCmd.GetOptionValue("--adapter", option_value)) {
        option_value.erase(option_value.begin()); // remove '='
        auto f = [&](const std::shared_ptr<SimpleBluez::Adapter>& arAdapter) {
            return ((arAdapter->address() == option_value) || (arAdapter->identifier() == option_value));
        };
        auto it = std::find_if(adapters.begin(), adapters.end(), f);
        if (it != adapters.end()) {
            return *it;
        }
    }
    else if (!adapters.empty()) {
        return adapters[0];
    }

    showHelp();
    THROW_WITH_BACKTRACE(ENoAdapter);
}

void BleApplication::scan(std::shared_ptr<SimpleBluez::Adapter> &arAdapter)
{
    Console::Info() << "Scanning for Bluetooth devices...";
    bool found_contour = false;
    SimpleBluez::Adapter::DiscoveryFilter filter;
    filter.Transport = SimpleBluez::Adapter::DiscoveryFilter::TransportType::LE;
    arAdapter->discovery_filter(filter);

    arAdapter->set_on_device_updated([&](const std::shared_ptr<SimpleBluez::Device> &arDevice) {
        if (std::find(mPeripherals.begin(), mPeripherals.end(), arDevice) == mPeripherals.end()) {
            Console::Info() << "\nFound: " << arDevice->name() << "[" << arDevice->address() << "]" << std::endl;
            if (StrUtils::Contains(arDevice->name(), "Contour")) {
                found_contour = true;
            }
            mPeripherals.push_back(arDevice);
        }
    });

    arAdapter->discovery_start();
    for (int i = 0 ; i < 60 ; ++i) {
        Utils::Delay(1000);
        Console::Info() << ".";
        if (found_contour) {
            break;
        }
    }
    Console::Info() << std::endl;
    arAdapter->discovery_stop();
}

std::shared_ptr<SimpleBluez::Device> BleApplication::getDevice(std::shared_ptr<SimpleBluez::Adapter> &arAdapter)
{
    std::string option_value;
    if (mCmd.GetOptionValue("--device", option_value)) {
        option_value.erase(option_value.begin()); // remove '='
        auto f = [&](const std::shared_ptr<SimpleBluez::Device>& arDevice) {
            return ((arDevice->address() == option_value) || (arDevice->name() == option_value));
        };
        auto it = std::find_if(mPeripherals.begin(), mPeripherals.end(), f);
        if (it != mPeripherals.end()) {
            return *it;
        }
        THROW_WITH_BACKTRACE(EDeviceNotFound);
    }
    THROW_WITH_BACKTRACE(ENoDevice);
}

void BleApplication::listDevices()
{
    if (mPeripherals.empty()) {
        Console::Info() << "No Bluetooth devices found." << std::endl;
    }
    else {
        Console::Info() << "The following devices were found:" << std::endl;
        for (size_t i = 0; i < mPeripherals.size(); i++) {
            Console::Info() << "[" << i << "] " << mPeripherals[i]->name() << " [" << mPeripherals[i]->address() << "]" << std::endl;
        }
    }
}

} // rsp

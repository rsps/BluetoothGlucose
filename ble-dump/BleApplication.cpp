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
#include "exceptions.h"
#include "TrustedDevice.h"
#include "GlucoseServiceProfile.h"
#include <utils/StrUtils.h>
#include "BleServiceBase.h"
#include <simplebluez/Bluez.h>

using namespace rsp::exceptions;
using namespace rsp::utils;

namespace rsp {

BleApplication::BleApplication(int argc, const char **argv)
    : ApplicationBase(argc, argv, "ble-dump")
{
    SignalHandler::Register(Signals::Terminate, [this]() { Terminate(0); });
}

BleApplication::~BleApplication()
{
    SignalHandler::Unregister(Signals::Terminate);
}

void BleApplication::beforeExecute()
{
    ApplicationBase::beforeExecute();

    if (!SimpleBLE::Adapter::bluetooth_enabled()) {
        mLogger.Error() << "Bluetooth is not enabled";
        THROW_WITH_BACKTRACE(ENoBlueTooth);
    }

    if (mCmd.GetCommands().empty()) {
        showHelp();
        THROW_WITH_BACKTRACE1(ETerminate, cResultSuccess);
    }

    if (mCmd.GetOptionValue("--device", mDeviceMAC)) {
        std::string_view removals("= ");
        while (removals.find(mDeviceMAC[0]) != std::string_view::npos) {
            mDeviceMAC.erase(mDeviceMAC.begin()); // remove first character
        }
        StrUtils::ToUpper(mDeviceMAC);
    }
}

void BleApplication::afterExecute()
{
    ApplicationBase::afterExecute();
}

void BleApplication::showHelp()
{
    using namespace rsp::application;
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
       "    dump                            Dump records from the device in CSV format\n"
       "    attributes                      List attributes for the device\n"
       "    info                            Show general device information\n"
       << std::endl;

    auto adapters = SimpleBLE::Adapter::get_adapters();
    if (adapters.empty()) {
        Console::Info() << "No Bluetooth adapters found." << std::endl;
    }
    else {
        Console::Info() << "The following adapters were found:" << std::endl;
        for (int i = 0; i < adapters.size(); i++) {
            Console::Info() << "[" << i << "] " << adapters[i].identifier() << " [" << adapters[i].address() << "]" << std::endl;
        }
    }
    Console::Info() << std::endl;
}

void BleApplication::showVersion()
{
    using namespace rsp::application;
    Console::Info() << "Version 0.0.1";
    ApplicationBase::showVersion();
    Console::Info() << "SimpleBLE version: " << SimpleBLE::get_simpleble_version();;
}

void BleApplication::handleOptions()
{
    ApplicationBase::handleOptions();
}

void BleApplication::execute()
{
    auto adapter = getAdapter();
    auto cmd = mCmd.GetCommands()[0];
    if (cmd == "devices") {
        if (!(mCmd.HasOption("-v") || mCmd.HasOption("-vv") || mCmd.HasOption("-vvv"))) {
            mLogWriter->SetAcceptLogLevel(logging::LogLevel::Info);
        }
        scan(adapter);
        if (!(mCmd.HasOption("-v") || mCmd.HasOption("-vv") || mCmd.HasOption("-vvv"))) {
            mLogWriter->SetAcceptLogLevel(logging::LogLevel::Notice);
        }
    }
    else if (cmd == "dump") {
        auto device = getDevice(adapter);
        GlucoseServiceProfile gls(device);
        gls.ReadAllMeasurements();
        auto &recs = gls.GetMeasurements();
        std::cout << recs << std::endl;
    }
    else if (cmd == "attributes") {
        auto device = getDevice(adapter);
        device.PrintServices();
    }
    else if (cmd == "info") {
        auto device = getDevice(adapter);
//        auto dis =td.GetDeviceInformationService();
//        dis.PrintInfo();
    }
    Terminate(cResultSuccess);
}

SimpleBLE::Adapter BleApplication::getAdapter()
{
    std::string option_value;
    auto adapters = SimpleBLE::Adapter::get_adapters();
    if (mCmd.GetOptionValue("--adapter", option_value)) {
        option_value.erase(option_value.begin()); // remove '='
        auto f = [&](SimpleBLE::Adapter &arAdapter) {
            return ((arAdapter.address() == option_value) || (arAdapter.identifier() == option_value));
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

void BleApplication::scan(SimpleBLE::Adapter &arAdapter)
{
    bool found_device = false;

    SimpleBluez::Adapter::DiscoveryFilter filter;
    filter.Transport = SimpleBluez::Adapter::DiscoveryFilter::TransportType::LE;
    static_cast<SimpleBluez::Adapter*>(arAdapter.underlying())->discovery_filter(filter);


    arAdapter.set_callback_on_scan_found([&](SimpleBLE::Peripheral aPeripheral) {
        mLogger.Info() << "Found device: " << aPeripheral.identifier()
            << " [" << aPeripheral.address() << "] "
            << aPeripheral.rssi() << " dBm";
        if (!mDeviceMAC.empty() && (aPeripheral.address() == mDeviceMAC)) {
            found_device = true;
        }
    });
    arAdapter.set_callback_on_scan_updated([&](SimpleBLE::Peripheral aPeripheral) {
        mLogger.Info() << "Updated device: " << aPeripheral.identifier()
            << " [" << aPeripheral.address() << "] "
            << aPeripheral.rssi() << " dBm";
    });
    arAdapter.set_callback_on_scan_start([&]() {
        mLogger.Info() << "Scanning for Bluetooth devices...";
    });
    arAdapter.set_callback_on_scan_stop([&]() {
        mLogger.Info() << "Scan stopped.";
    });

    arAdapter.scan_start();
    for (int i = 0 ; i < 60 ; ++i) {
        BleServiceBase::Delay(500);
        if (found_device) {
            break;
        }
        else {
            mLogger.Info() << ".";
        }
    }
    arAdapter.scan_stop();
    mLogger.Info() << "Scan complete.";

    mPeripherals = arAdapter.scan_get_results();
}

TrustedDevice BleApplication::getDevice(SimpleBLE::Adapter &arAdapter)
{
    if (mDeviceMAC.empty()) {
        THROW_WITH_BACKTRACE(ENoDevice);
    }
    scan(arAdapter);
    auto f = [&](SimpleBLE::Peripheral &arDevice) {
        return (arDevice.address() == mDeviceMAC);
    };
    auto it = std::find_if(mPeripherals.begin(), mPeripherals.end(), f);
    if (it != mPeripherals.end()) {
        return TrustedDevice(*it);
    }
    THROW_WITH_BACKTRACE(EDeviceNotFound);
}

} // rsp

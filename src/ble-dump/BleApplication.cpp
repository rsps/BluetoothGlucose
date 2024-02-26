/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#include <TrustedDevice.h>
#include <application/Console.h>
#include <BleApplication.h>
#include <CurrentTimeServiceProfile.h>
#include <DeviceInformationServiceProfile.h>
#include <exceptions/SignalHandler.h>
#include <exceptions.h>
#include <GlucoseServiceProfile.h>
#include <fstream>
#include <Scanner.h>
#include <utils/CsvEncoder.h>
#include <utils/Function.h>
#include <utils/StrUtils.h>
#include <version.h>

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
            mDeviceMAC.erase(mDeviceMAC.begin()); // remove first character: '='
        }
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
       "    --filename=<filename|auto>      Name of file to store device records into. Defaults to auto.\n"
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
       "    attributes                      List attributes for the device\n"
       "    clear                           Clear all records on the device\n"
       "    devices                         List found BlueTooth devices\n"
       "    dump                            Dump records from the device in CSV format\n"
       "    info                            Show general device information\n"
       "    sync-time                       Synchronize the device time with this host\n"
       "    time                            Show the current time in the device\n"
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
    rsp::application::Console::Info()
        << GetAppName() << " v0.1.0\n"
        << "RSP Core Library v" << get_library_version() << "\n"
        << "SimpleBLE v" << SimpleBLE::get_simpleble_version() << std::endl;
}

void BleApplication::handleOptions()
{
    ApplicationBase::handleOptions();
}

void BleApplication::execute()
{
    auto cmd = mCmd.GetCommands()[0];
    if (cmd == "devices") {
        devicesCommand();
    }
    else if (cmd == "dump") {
        dumpCommand();
    }
    else if (cmd == "clear") {
        clearCommand();
    }
    else if (cmd == "attributes") {
        attributesCommand();
    }
    else if (cmd == "info") {
        infoCommand();
    }
    else if (cmd == "time") {
        timeCommand();
    }
    else if (cmd == "sync-time") {
        syncTimeCommand();
    }
    else {
        showHelp();
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

TrustedDevice BleApplication::getDevice(SimpleBLE::Adapter &arAdapter)
{
    if (mDeviceMAC.empty()) {
        THROW_WITH_BACKTRACE(ENoDevice);
    }

    Scanner s(arAdapter, mDeviceMAC);
    if (s.RunUntilFound(30000)) {
        return TrustedDevice(s.GetResult().front());
    }

    THROW_WITH_BACKTRACE(EDeviceNotFound);
}

void BleApplication::devicesCommand()
{
    auto adapter = getAdapter();
    Scanner s(adapter, mDeviceMAC);
    s.RunFor(30000);
}

void BleApplication::dumpCommand()
{
    auto adapter = getAdapter();
    auto device = getDevice(adapter);
    GlucoseServiceProfile gls(device);
    mLogger.Notice() << "Reading measurement records from " << device.GetPeripheral().identifier() << " [" << device.GetPeripheral().address() << "]";
    auto &recs = gls.ReadAllMeasurements();
    DynamicData dd;
    dd << recs;
    CsvEncoder csv(true, ';');
    std::string file_name = getFileName(device);
    mLogger.Notice() << "Writing " << recs.size() << " records to " << file_name;
    std::ofstream file;
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    file.open(file_name, std::ios::out | std::ios::trunc);

    csv.SetValueFormatter([](std::string &arResult, const DynamicData &arValue) -> bool {
        if (arValue.AsString() == "HbA1c") {
            arResult = arValue.AsString();
            return true;
        }
        else if (arValue.GetType() == Variant::Types::Float) {
            arResult = StrUtils::ToString(arValue.AsFloat(), 1, true);
            return true;
        }
        arResult = arValue.AsString();
        return false;
    }).Encode(file, dd);

    file.close();
}

void BleApplication::clearCommand()
{
    using namespace rsp::application;
    auto adapter = getAdapter();
    auto device = getDevice(adapter);
    GlucoseServiceProfile gls(device);
    auto count = gls.GetMeasurementsCount();
    mLogger.Warning() << "Deleting " << count << " measurement records from " << device.GetPeripheral().address();
    gls.ClearAllMeasurements();
}

void BleApplication::attributesCommand()
{
    auto adapter = getAdapter();
    auto device = getDevice(adapter);
    mLogger.Notice() << device;
}

void BleApplication::infoCommand()
{
    auto adapter = getAdapter();
    auto device = getDevice(adapter);
    DeviceInformationServiceProfile dis(device);
    mLogger.Notice() << dis;
}

void BleApplication::timeCommand()
{
    auto adapter = getAdapter();
    auto device = getDevice(adapter);
    CurrentTimeServiceProfile cts(device);
    mLogger.Notice() << cts;
}

void BleApplication::syncTimeCommand()
{
    auto adapter = getAdapter();
    auto device = getDevice(adapter);
    CurrentTimeServiceProfile cts(device);
    cts.SetTime(DateTime::Now());
    mLogger.Notice() << cts;
}

std::string BleApplication::getFileName(TrustedDevice &arDevice)
{
    std::string filename = "auto";
    if (mCmd.GetOptionValue("--filename", filename)) {
        std::string_view removals("= ");
        while (removals.find(filename) != std::string_view::npos) {
            filename.erase(filename.begin()); // remove first character: '='
        }
    }
    if(filename == "auto") {
        filename = arDevice.GetPeripheral().identifier() + "-" + DateTime().ToString("%Y%m%d%H%M%S") + ".csv";
    }
    return filename;
}

} // rsp

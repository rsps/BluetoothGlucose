/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include "BleApplication.h"
#include <application/Console.h>
#include <chrono>
#include <exceptions/SignalHandler.h>
#include <utils/Function.h>

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
       "Usage: ble-bump [<options>] <device>\n"
       "  Options:\n"
       "    --adapter=<adapter name>        Name of the Bluetooth adapter to use. Defaults to first.\n"
       "    -h                              Same as --help.\n"
       "    --help                          Show this help information.\n"
       "    --log=<filename|syslog>         Log output to file.\n"
       "    --loglevel=<[error|info|debug]> Set the log level for file logging,\n"
       "                                    default level is info.\n"
       "    --version                       Show version.\n"
       "    -v                              Increase verbosity level to Info.\n"
       "    -vv                             Increase verbosity level to Debug.\n"
    << std::endl;
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
    ApplicationBase::execute();
}

void BleApplication::bluezThreadExecute()
{
    mBluez.run_async();
    std::this_thread::sleep_for(std::chrono::microseconds(100));
}

} // rsp
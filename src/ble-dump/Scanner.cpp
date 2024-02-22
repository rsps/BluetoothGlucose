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
#include <thread>
#include <Scanner.h>
#ifdef __linux__
#include <simplebluez/Bluez.h>
#endif

namespace rsp {

Scanner::Scanner(const SimpleBLE::Adapter &arAdapter, std::vector<std::string> aAddressList)
    : mAdapter(arAdapter),
      mAcceptFilter(std::move(aAddressList))
{
}

Scanner::Scanner(const SimpleBLE::Adapter &arAdapter, const std::string &arAddress)
    :  Scanner(arAdapter)
{
    if (!arAddress.empty()) {
        mAcceptFilter.push_back(arAddress);
    }
}

const std::vector<SimpleBLE::Peripheral>& Scanner::RunFor(std::uint32_t aMilliseconds)
{
    execute(aMilliseconds, false);
    return mScanResult;
}

bool Scanner::RunUntilFound(std::uint32_t aTimeoutMilliseconds)
{
    execute(aTimeoutMilliseconds, true);
    return !mScanResult.empty();
}

void Scanner::execute(std::uint32_t aMilliseconds, bool aStopWhenFound)
{
#ifdef __linux__
    // It seems like default discovery filter is not set to Auto on Linux.
    // Found no way to set the discovery filter in the SimpleBLE API, so this is a SimpleBluez solution only.
    SimpleBluez::Adapter::DiscoveryFilter filter;
//    filter.Transport = SimpleBluez::Adapter::DiscoveryFilter::TransportType::LE;
    filter.Transport = SimpleBluez::Adapter::DiscoveryFilter::TransportType::AUTO;
    static_cast<SimpleBluez::Adapter*>(mAdapter.underlying())->discovery_filter(filter);
#endif

    mAdapter.set_callback_on_scan_found([this](SimpleBLE::Peripheral aPeripheral) {
        if (!addressAccepted(aPeripheral.address())) {
            return;
        }
        mLogger.Notice() << "Found device: " << aPeripheral.identifier()
                       << " [" << aPeripheral.address() << "] "
                       << aPeripheral.rssi() << " dBm";
        mScanResult.push_back(aPeripheral);
        mFoundDevice = true;
    });
//    mAdapter.set_callback_on_scan_updated([this](SimpleBLE::Peripheral aPeripheral) {
//        mLogger.Info() << "Updated device: " << aPeripheral.identifier()
//            << " [" << aPeripheral.address() << "] "
//            << aPeripheral.rssi() << " dBm";
//    });
    mAdapter.set_callback_on_scan_start([this]() {
        mLogger.Notice() << "Scanning for Bluetooth devices...";
    });
    mAdapter.set_callback_on_scan_stop([this]() {
        mLogger.Notice() << "Scan complete.";
    });

    mScanResult.clear();
    mFoundDevice = false;
    mAdapter.scan_start();
    aMilliseconds = (aMilliseconds / 100) + 1;
    for (int i = 0 ; i < aMilliseconds ; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (aStopWhenFound && mFoundDevice) {
            break;
        }
    }
    mAdapter.scan_stop();
}

bool Scanner::addressAccepted(const std::string &arAddress) const
{
    if (mAcceptFilter.empty()) {
        return true;
    }
    return (std::find_if(mAcceptFilter.begin(), mAcceptFilter.end(), [&](const std::string &arAddr) {
        return arAddr == arAddress;
    }) != mAcceptFilter.end());
}

} // rsp

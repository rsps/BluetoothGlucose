/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include "TrustedDevice.h"
#include <application/Console.h>
#include <simplebluez/Exceptions.h>
#include "exceptions.h"
#include "UUID.h"
#include "BleServiceBase.h"
#include <magic_enum.hpp>

using namespace rsp::application;

namespace rsp {

TrustedDevice::TrustedDevice(std::shared_ptr<SimpleBluez::Device> &arDevice)
    : mpDevice(arDevice)
{
    Console::Error() << "Attempting to connect with " << arDevice->address() << std::endl;
    for (int attempt = 0; attempt < 3; attempt++) {
        try {
            BleServiceBase::Delay(1000);
            arDevice->connect();
            if (arDevice->connected()) {
                if (arDevice->services_resolved()) {
                    makeServiceList();
                    return;
                }
            }
        } catch (SimpleDBus::Exception::SendFailed& e) {
            Console::Debug() << "Debug: " << e.what() << std::endl;
        } catch (const std::exception &e) {
            Console::Error() << "Error: " << e.what() << std::endl;
        }
    }
    Console::Error() << "Failed to connect to " << arDevice->name() << " [" << arDevice->address() << "]" << std::endl;
    THROW_WITH_BACKTRACE(EDeviceNotPaired);
}

TrustedDevice::~TrustedDevice()
{
    mpDevice->disconnect();
}

void TrustedDevice::PrintServices()
{
    Console::Info() << "Services on " << mpDevice->address() << ":" << std::endl;

    for (auto& uuid : mServiceList) {
        Console::Info() << uuid << std::endl;
    }
}

bool TrustedDevice::HasGlucoseService()
try {
    findServiceById(UUID::Identifiers::GlucoseService);
    return true;
}
catch (const EServiceNotFound&) {
    return false;
}

GlucoseServiceProfile TrustedDevice::GetGlucoseService()
{
    auto &service = findServiceById(UUID::Identifiers::GlucoseService);
    return GlucoseServiceProfile(mpDevice, service);
}

void TrustedDevice::makeServiceList()
{
    for (auto &service : mpDevice->services()) {
        auto &s = mServiceList.emplace_back(UUID::Types::Service, service->uuid(), service->path());
        for (auto &characteristic : service->characteristics()) {
            auto &c = mServiceList.emplace_back(UUID::Types::Characteristic, characteristic->uuid(), characteristic->path(), &s);
            for (auto &descriptor : characteristic->descriptors()) {
                mServiceList.emplace_back(UUID::Types::Descriptor, descriptor->uuid(), descriptor->path(), &c);
            }
        }
    }
}

UUID& TrustedDevice::findServiceById(UUID::Identifiers aId)
{
    auto it = std::find_if(mServiceList.begin(), mServiceList.end(), [aId](UUID& aUuid) {
        return (aUuid == aId);
    });
    if (it == mServiceList.end()) {
        THROW_WITH_BACKTRACE1(EServiceNotFound, std::string(magic_enum::enum_name(aId)));
    }
    return *it;
}

} // rsp

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
#include "Utils.h"
#include "exceptions.h"
#include "UUID.h"
#include <iomanip>

using namespace rsp::application;

namespace rsp {

TrustedDevice::TrustedDevice(std::shared_ptr<SimpleBluez::Device> &arDevice)
    : mpDevice(arDevice)
{
    Console::Info() << "Attempting to connect with " << arDevice->address() << std::endl;
    for (int attempt = 0; attempt < 3; attempt++) {
        try {
            Utils::Delay(1000);
            arDevice->connect();
            if (arDevice->connected()) {
                if (arDevice->services_resolved()) {
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

void TrustedDevice::PrintInfo()
{
    Console::Info() << "Services on " << mpDevice->address() << ":" << std::endl;

    auto o = Console::Info();
    for (auto &service : mpDevice->services()) {
        o << UUID(UUID::Types::Service, service->uuid(), service->path()) << std::endl;
        for (auto &characteristic : service->characteristics()) {
            o << UUID(UUID::Types::Characteristic, characteristic->uuid(), characteristic->path()) << std::endl;
            for (auto &descriptor : characteristic->descriptors()) {
                o << UUID(UUID::Types::Descriptor, descriptor->uuid(), descriptor->path()) << std::endl;
            }
        }
    }
    o << std::endl;
}

std::vector<GlucoseRecord> TrustedDevice::GetRecords()
{
    std::vector<GlucoseRecord> result;
    std::string root_uuid;
    for (auto &service : mpDevice->services()) {
        auto uuid = UUID(UUID::Types::Service, service->uuid(), service->path());
        if (uuid.GetId() == UUID::Identifiers::GlucoseService) {
            root_uuid = service->uuid().substr(8);
            break;
        }
    }
    auto service = mpDevice->get_service(ToString(UUID::Identifiers::GlucoseService) + root_uuid);

    auto gm = service->get_characteristic(ToString(UUID::Identifiers::GlucoseMeasurement) + root_uuid);
    gm->set_on_value_changed([&](const SimpleBluez::ByteArray &new_value) {
        debug("Measurement: ", new_value);
        result.emplace_back();
    });
    gm->start_notify();

    auto gmc = service->get_characteristic(ToString(UUID::Identifiers::GlucoseMeasurementContext) + root_uuid);
    gmc->set_on_value_changed([&](const SimpleBluez::ByteArray &new_value) {
        debug("Context: ", new_value);
    });
    gmc->start_notify();

    auto gmr = service->get_characteristic(ToString(UUID::Identifiers::RecordAccessControlPoint) + root_uuid);
    gmr->set_on_value_changed([&](const SimpleBluez::ByteArray &new_value) {
        debug("Record: ", new_value);
    });
    gmc->start_notify();
    gmc->write_request("write 0x01 0x01");

    Utils::Delay(20000);

    return result;
}

void TrustedDevice::debug(const std::string &arTitle, const SimpleBluez::ByteArray &new_value)
{
    auto o = Console::Debug();
    o << arTitle << ": ";
    for (auto &byte : new_value) {
        o << std::setfill ('0') << std::setw(2) << std::hex << uint32_t(uint8_t(byte)) << " ";
    }
    o << std::endl;
}

} // rsp
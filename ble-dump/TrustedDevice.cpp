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
    Console::Info() << "Preparing to get records." << std::endl;
    Utils::Delay(2000);

    std::string root_uuid;
    for (auto &service : mpDevice->services()) {
        auto uuid = UUID(UUID::Types::Service, service->uuid(), service->path());
        if (uuid.GetId() == UUID::Identifiers::GlucoseService) {
            root_uuid = service->uuid().substr(8);
            break;
        }
    }
    auto service = mpDevice->get_service(ToString(UUID::Identifiers::GlucoseService) + root_uuid);

    Console::Info() << "Using glucose service: " << service->uuid() << std::endl;

    std::vector<GlucoseRecord> result;
    auto gm = service->get_characteristic(ToString(UUID::Identifiers::GlucoseMeasurement) + root_uuid);
    Console::Info() << "Listening on glucose measurement: " << gm->uuid() << std::endl;
    gm->set_on_value_changed([&](const SimpleBluez::ByteArray &arValue) {
        debug("Measurement: ", arValue);
        decodeMeasurement(result.emplace_back(), arValue);
    });
    gm->start_notify();

    auto gmc = service->get_characteristic(ToString(UUID::Identifiers::GlucoseMeasurementContext) + root_uuid);
    Console::Info() << "Listening on glucose measurement context: " << gmc->uuid() << std::endl;
    gmc->set_on_value_changed([&](const SimpleBluez::ByteArray &arValue) {
        debug("Context: ", arValue);
    });
    gmc->start_notify();

    uint16_t record_count = 0;
    bool all_done = false;
    auto gmr = service->get_characteristic(ToString(UUID::Identifiers::RecordAccessControlPoint) + root_uuid);
    Console::Info() << "Listening on record access control point: " << gmr->uuid() << std::endl;
    gmr->set_on_value_changed([&](const SimpleBluez::ByteArray &arValue) {
        debug("Record: ", arValue);
        if ((arValue[0] == '\5') && (arValue.size() >= 4)) {
            auto value = uint16_t(arValue[3]) << 8;
            value += uint16_t(arValue[2]);
            record_count = value;
        }
        else if ((arValue.size() == 4) && (arValue == "\x06\x00\x01\x01")) {
            all_done = true;
        }
    });
    gmr->start_notify();

    Console::Info() << "Requesting record count" << std::endl;
    gmr->write_command("\x04\x01");
    Utils::Delay(1000);

    Console::Info() << "Requesting all records" << std::endl;
    gmr->write_command("\x01\x01");

    Utils::Delay(20000, &all_done);

    gmr->stop_notify();
    gmc->stop_notify();
    gm->stop_notify();

    return result;
}

void TrustedDevice::debug(const std::string &arTitle, const SimpleBluez::ByteArray &arValue)
{
    auto o = Console::Info();
    o << arTitle << ": ";
    for (auto &byte : arValue) {
        o << std::setfill ('0') << std::setw(2) << std::hex << uint32_t(uint8_t(byte)) << " ";
    }
    o << std::endl;
}

void TrustedDevice::decodeMeasurement(GlucoseRecord &arRecord, const SimpleBluez::ByteArray &arValue)
{

}

} // rsp

/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#include <cmath>
#include "GlucoseServiceProfile.h"
#include "exceptions.h"
#include "AttributeStream.h"
#include <magic_enum.hpp>

using namespace rsp::utils;

namespace rsp {

GlucoseServiceProfile::GlucoseMeasurement::GlucoseMeasurement(AttributeStream &s)
{
    if (s.GetArray().size() < 10) {
        THROW_WITH_BACKTRACE(EGlucoseArgument);
    }

    uint8_t flags = s.Uint8();
    mUnit = (flags & Flags::GlucoseInMMol) ? Units::mmol_L : Units::mg_dL;
    mSequenceNo = s.Uint16();
    mCaptureTime = s.DateTime();
    if (flags & Flags::TimeOffsetPresent) {
        auto offset = int16_t(s.Uint16());
        mCaptureTime += std::chrono::minutes(offset);
    }
    if (flags & Flags::GlucoseConcentrationPresent) {
        mGlucoseConcentration = s.MedFloat16() * 1000.0f;
        uint8_t type_location = s.Uint8();
        mType = Type(type_location & 0x0F);
        mLocation = Location((type_location >> 4) & 0x0F);
    }
    if (flags & Flags::SensorStatusPresent) {
        mSensorStatus = SensorStatus(s.Uint16());
    }
}

std::ostream& operator<<(std::ostream &o, const GlucoseServiceProfile::GlucoseMeasurement &arGM)
{
    o   << arGM.mSequenceNo << ", "
        << arGM.mCaptureTime.ToRFC3339() << ", "
        << arGM.mGlucoseConcentration << ", "
        << ((arGM.mUnit == GlucoseServiceProfile::Units::mg_dL) ? "mg/dl" : "mmol/L") << ", "
        << magic_enum::enum_name(arGM.mType) << ", "
        << magic_enum::enum_name(arGM.mLocation) << ", "
        << "0x" << std::setfill('0') << std::setw(2) << std::hex << uint32_t(arGM.mSensorStatus) << std::dec << ", "
        << int(arGM.mHasContext);
    return o;
}

std::ostream& operator<<(std::ostream &o, const std::vector<GlucoseServiceProfile::GlucoseMeasurement> &arList)
{
    o << "SequenceNo, CaptureTime, GlucoseConcentration, Unit, Type, Location, SensorStatus, Context\r\n";
    for (auto &row : arList) {
        o << row << "\r\n";
    }
    return o;
}

GlucoseServiceProfile::GlucoseServiceProfile(std::shared_ptr<SimpleBluez::Device> apDevice, UUID &arService)
    : mpDevice(std::move(apDevice)),
      mrService(arService)
{
    std::string root_uuid = mrService.GetUUID().substr(8);
    auto service = mpDevice->get_service(mrService.GetUUID());

    mGlucoseMeasurement = service->get_characteristic(ToString(UUID::Identifiers::GlucoseMeasurement) + root_uuid);
    mLogger.Debug() << "Listening on glucose measurement: " << mGlucoseMeasurement->uuid();
    mGlucoseMeasurement->set_on_value_changed([&](const SimpleBluez::ByteArray &arValue) {
        measurementHandler(AttributeStream(arValue));
    });

    mGlucoseMeasurementContext = service->get_characteristic(ToString(UUID::Identifiers::GlucoseMeasurementContext) + root_uuid);
    mLogger.Debug() << "Listening on glucose measurement context: " << mGlucoseMeasurementContext->uuid();
    mGlucoseMeasurementContext->set_on_value_changed([&](const SimpleBluez::ByteArray &arValue) {
        measurementContextHandler(AttributeStream(arValue));
    });

    mRacp = service->get_characteristic(ToString(UUID::Identifiers::RecordAccessControlPoint) + root_uuid);
    mLogger.Debug() << "Listening on record access control point: " << mRacp->uuid();
    mRacp->set_on_value_changed([&](const SimpleBluez::ByteArray &arValue) {
        racpHandler(AttributeStream(arValue));
    });

    mGlucoseMeasurement->start_notify();
    mGlucoseMeasurementContext->start_notify();
    mRacp->start_notify();
}

GlucoseServiceProfile::~GlucoseServiceProfile()
{
    mRacp->stop_notify();
    mGlucoseMeasurementContext->stop_notify();
    mGlucoseMeasurement->stop_notify();
}

size_t GlucoseServiceProfile::GetMeasurementsCount()
{
    mLogger.Info() << "Requesting record count";
    mRecordCount = 0;
    sendCommand(0x0401, 2000);
    return mRecordCount;
}

GlucoseServiceProfile& GlucoseServiceProfile::ReadAllMeasurements()
{
    mLogger.Info() << "Requesting all records";
    sendCommand(0x0101, 20000);
    return *this;
}

GlucoseServiceProfile& GlucoseServiceProfile::ClearAllMeasurements()
{
    mLogger.Info() << "Deleting all records";
    sendCommand(0x0201, 20000);
    return *this;
}

void GlucoseServiceProfile::sendCommand(std::uint16_t aCommand, int aTimeoutMs)
{
    mCommandDone = false;
    AttributeStream command(2);
    command.Uint16(aCommand);
    mRacp->write_command(command.GetArray());
    Delay(aTimeoutMs, &mCommandDone);
}

void GlucoseServiceProfile::racpHandler(AttributeStream aStream)
{
    bool error = false;
    uint16_t opcode;
    if (aStream.GetArray().size() != 4) {
        error = true;
    }
    else {
        opcode = aStream.Uint16();
        switch (opcode) {
            case 0x0004:
                mRecordCount = aStream.Uint16();
                break;
            case 0x0006:
                if (aStream.Uint16() == 0x0101) {
                    break;
                }
            default:
                error = true;
                break;
        }
    }
    if (error) {
        mLogger.Error() << "Unexpected result from RACP (" << opcode << ")";
        mLogger.Info() << "Record: " << aStream;
    }
    mCommandDone = true;
}

void GlucoseServiceProfile::measurementHandler(AttributeStream aStream)
{
    mLogger.Info() << "Measurement: " << aStream;
    mMeasurements.emplace_back(aStream);
}

void GlucoseServiceProfile::measurementContextHandler(AttributeStream aStream)
{
    mLogger.Info() << "Context: " << aStream;
    /*auto flags =*/ aStream.Uint8();
    auto seq_no = aStream.Uint16();
    for (auto &mes : mMeasurements) {
        if (mes.mSequenceNo == seq_no) {
            mes.mHasContext = true;
            break;
        }
    }
}

} // namespace rsp

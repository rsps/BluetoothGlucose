/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#include <cctype>
#include <GlucoseServiceProfile.h>
#include <exceptions.h>
#include <AttributeStream.h>
#include <magic_enum.hpp>

using namespace rsp::utils;

namespace rsp {

static std::string tr(std::string_view aString)
{
    if (aString == "NotAvailable") {
        return "N/A";
    }
    std::string result;
    result.reserve(aString.size() + 5);
    bool first = true;
    for (char chr : aString) {
        if (std::isupper(chr) && !first) {
            result += " ";
        }
        result += chr;
        first = false;
    }
    return result;
}

GlucoseServiceProfile::GlucoseMeasurement::GlucoseMeasurement(AttributeStream &s)
{
    if (s.GetArray().size() < 10) {
        THROW_WITH_BACKTRACE(EGlucoseArgument);
    }

    uint8_t flags = s.Uint8();
    mUnit = (flags & Flags::GlucoseInMMol) ? GlucoseUnits::mmol_L : GlucoseUnits::mg_dL;
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

void GlucoseServiceProfile::GlucoseMeasurementContext::Populate(uint8_t flags, AttributeStream &s)
{
    if (s.GetArray().size() < 3) {
        THROW_WITH_BACKTRACE(EGlucoseArgument);
    }

    mMedicationUnit = (flags & Flags::MedicationUnitsOfMilligrams) ? MedicationUnits::MassKilogram : MedicationUnits::VolumeLitre;
    if (flags & Flags::ExtendedPresent) {
        s.Uint8(); // All reserved. Simply discard.
    }
    if (flags & Flags::CarbohydratesPresent) {
        mCarbohydrateID = CarbohydrateIDs(s.Uint8());
        mCarbohydrate = s.MedFloat16();
    }
    if (flags & Flags::MealPresent) {
        mMeal = Meals(s.Uint8());
    }
    if (flags & Flags::TesterHealthPresent) {
        mTester = Testers(s.Uint8() & 0x0F);
        mHealth = Healths((s.Uint8() >> 4) & 0x0F);
    }
    if (flags & Flags::ExercisePresent) {
        mExerciseDurationSeconds = s.Uint16();
        mExerciseIntensity = s.Uint8();
    }
    if (flags & Flags::MedicationPresent) {
        mMedicationID = MedicationIDs(s.Uint8());
        mMedication = s.MedFloat16();
    }
    if (flags & Flags::HbA1cPresent) {
        mHbA1c = s.MedFloat16();
    }
}


std::ostream& operator<<(std::ostream &o, const GlucoseServiceProfile::GlucoseMeasurement &arGM)
{
    o << arGM.mSequenceNo << ","
      << arGM.mCaptureTime.ToISO8601UTC() << ","
      << arGM.mGlucoseConcentration << ","
      << ((arGM.mUnit == GlucoseServiceProfile::GlucoseUnits::mg_dL) ? "mg/dl" : "mmol/L") << ","
        << tr(magic_enum::enum_name(arGM.mType)) << ","
        << tr(magic_enum::enum_name(arGM.mLocation)) << ","
        << "0x" << std::setfill('0') << std::setw(2) << std::hex << uint32_t(arGM.mSensorStatus) << std::dec << ","
        << arGM.mContext;
    return o;
}

std::ostream& operator<<(std::ostream &o, const GlucoseServiceProfile::GlucoseMeasurementContext &arGMC)
{
    o   << tr(magic_enum::enum_name(arGMC.mCarbohydrateID)) << ","
        << arGMC.mCarbohydrate << ","
        << tr(magic_enum::enum_name(arGMC.mMeal)) << ","
        << tr(magic_enum::enum_name(arGMC.mTester)) << ","
        << tr(magic_enum::enum_name(arGMC.mHealth)) << ","
        << arGMC.mExerciseDurationSeconds << ","
        << int(arGMC.mExerciseIntensity) << "%,"
        << tr(magic_enum::enum_name(arGMC.mMedicationID)) << ","
        << arGMC.mMedication << ","
        << ((arGMC.mMedicationUnit == GlucoseServiceProfile::MedicationUnits::MassKilogram) ? "mg" : "ml") << ","
        << arGMC.mHbA1c;
    return o;
}

std::ostream& operator<<(std::ostream &o, const std::vector<GlucoseServiceProfile::GlucoseMeasurement> &arList)
{
    o << "Sequence No,Capture Time,Glucose Concentration,Unit,Type,Location,Sensor Status,Carbohydrate ID,Carbohydrate,Meal,Tester,Health,Exercise Duration,Exercise Intensity,Medication ID,Medication,Medication Unit,HbA1c\r\n";
    for (auto &row : arList) {
        o << row << "\r\n";
    }
    return o;
}

GlucoseServiceProfile::GlucoseServiceProfile(const TrustedDevice &arDevice)
    : BleService<GlucoseServiceProfile>(arDevice, uuid::Identifiers::GlucoseService)
{
    std::string root_uuid = mService.uuid().substr(8);

    mGlucoseMeasurement = ToString(uuid::Identifiers::GlucoseMeasurement) + root_uuid;
    mLogger.Debug() << "Listening on glucose measurement: " << mGlucoseMeasurement;
    mDevice.GetPeripheral().notify(mService.uuid(), mGlucoseMeasurement, [&](const SimpleBLE::ByteArray &arValue) {
        measurementHandler(AttributeStream(arValue));
    });

    mGlucoseMeasurementContext = ToString(uuid::Identifiers::GlucoseMeasurementContext) + root_uuid;
    mLogger.Debug() << "Listening on glucose measurement context: " << mGlucoseMeasurementContext;
    mDevice.GetPeripheral().notify(mService.uuid(), mGlucoseMeasurementContext, [&](const SimpleBLE::ByteArray &arValue) {
        measurementContextHandler(AttributeStream(arValue));
    });

    mRACP = ToString(uuid::Identifiers::RecordAccessControlPoint) + root_uuid;
    mLogger.Debug() << "Listening on record access control point: " << mRACP;
    mDevice.GetPeripheral().notify(mService.uuid(), mRACP, [&](const SimpleBLE::ByteArray &arValue) {
        racpHandler(AttributeStream(arValue));
    });
}

GlucoseServiceProfile::~GlucoseServiceProfile()
{
    mDevice.GetPeripheral().unsubscribe(mService.uuid(), mRACP);
    mDevice.GetPeripheral().unsubscribe(mService.uuid(), mGlucoseMeasurementContext);
    mDevice.GetPeripheral().unsubscribe(mService.uuid(), mGlucoseMeasurement);
}

size_t GlucoseServiceProfile::GetMeasurementsCount()
{
    mLogger.Info() << "Requesting record count";
    mRecordCount = 0;
    sendCommand(0x0401, 2000);
    return mRecordCount;
}

const std::vector<GlucoseServiceProfile::GlucoseMeasurement>& GlucoseServiceProfile::ReadAllMeasurements()
{
    mLogger.Info() << "Requesting all records";
    sendCommand(0x0101, 20000);
    return mMeasurements;
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
    mDevice.GetPeripheral().write_command(mService.uuid(), mRACP, command.GetArray());
    delay(aTimeoutMs, &mCommandDone);
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
    auto flags = aStream.Uint8();
    auto seq_no = aStream.Uint16();
    for (auto &mes : mMeasurements) {
        if (mes.mSequenceNo == seq_no) {
            mes.mContext.Populate(flags, aStream);
            break;
        }
    }
}

} // namespace rsp

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
#include <utils/Rounding.h>

using namespace rsp::utils;

template <>
struct magic_enum::customize::enum_range<rsp::GlucoseServiceProfile::SensorStatus> {
    static constexpr bool is_flags = true;
};

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

    mFlags = Flags(s.Uint8());
    mUnit = (mFlags & Flags::GlucoseInMMol) ? GlucoseUnits::mmol_L : GlucoseUnits::mg_dL;
    mSequenceNo = s.Uint16();
    mCaptureTime = s.DateTime();
    if (mFlags & Flags::TimeOffsetPresent) {
        auto offset = int16_t(s.Uint16());
        mCaptureTime += std::chrono::minutes(offset);
    }
    if (mFlags & Flags::GlucoseConcentrationPresent) {
        mGlucoseConcentration = s.MedFloat16() * 1000.0f;
        uint8_t type_location = s.Uint8();
        mType = Type(type_location & 0x0F);
        mLocation = Location((type_location >> 4) & 0x0F);
    }
    if (mFlags & Flags::SensorStatusPresent) {
        mSensorStatus = SensorStatus(s.Uint16());
    }
}

void GlucoseServiceProfile::GlucoseMeasurementContext::Populate(Flags flags, AttributeStream &s)
{
    if (s.GetArray().size() < 3) {
        THROW_WITH_BACKTRACE(EGlucoseArgument);
    }
    mFlags = flags;
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


utils::DynamicData& operator<<(utils::DynamicData &o, const GlucoseServiceProfile::GlucoseMeasurement &arGM)
{
    o
        .Add("SequenceNo", arGM.mSequenceNo)
        .Add("CaptureTime", arGM.mCaptureTime.ToISO8601UTC())
        .Add("GlucoseConcentration", utils::DynamicData())
        .Add("Unit", utils::DynamicData())
        .Add("Type", utils::DynamicData())
        .Add("Location", utils::DynamicData())
        .Add("SensorStatus", utils::DynamicData());

    if (arGM.mFlags & GlucoseServiceProfile::GlucoseMeasurement::Flags::GlucoseConcentrationPresent) {
        o["GlucoseConcentration"] = arGM.mGlucoseConcentration;
        o["Unit"] = ((arGM.mUnit == GlucoseServiceProfile::GlucoseUnits::mg_dL) ? "mg/dl" : "mmol/L");
        o["Type"] = std::string(magic_enum::enum_name(arGM.mType));
        o["Location"] = std::string(magic_enum::enum_name(arGM.mLocation));
    }
    if (arGM.mFlags & GlucoseServiceProfile::GlucoseMeasurement::Flags::SensorStatusPresent) {
        o["SensorStatus"] = magic_enum::enum_flags_name(arGM.mSensorStatus);
    }
    o << arGM.mContext;

    return o;
}

utils::DynamicData& operator<<(utils::DynamicData &o, const GlucoseServiceProfile::GlucoseMeasurementContext &arGMC)
{
    if (arGMC.mFlags & GlucoseServiceProfile::GlucoseMeasurementContext::Flags::CarbohydratesPresent) {
        o.Add("Carbohydrate ID", std::string(magic_enum::enum_name(arGMC.mCarbohydrateID)));
        o.Add("Carbohydrate", arGMC.mCarbohydrate);
    }
    else {
        o.Add("Carbohydrate ID", utils::DynamicData());
        o.Add("Carbohydrate", utils::DynamicData());
    }
    if (arGMC.mFlags & GlucoseServiceProfile::GlucoseMeasurementContext::Flags::MealPresent) {
        o.Add("Meal", std::string(magic_enum::enum_name(arGMC.mMeal)));
    }
    else {
        o.Add("Meal", utils::DynamicData());
    }
    if (arGMC.mFlags & GlucoseServiceProfile::GlucoseMeasurementContext::Flags::TesterHealthPresent) {
        o.Add("Tester", std::string(magic_enum::enum_name(arGMC.mTester)));
        o.Add("Health", std::string(magic_enum::enum_name(arGMC.mHealth)));
    }
    else {
        o.Add("Tester", utils::DynamicData());
        o.Add("Health", utils::DynamicData());
    }
    if (arGMC.mFlags & GlucoseServiceProfile::GlucoseMeasurementContext::Flags::ExercisePresent) {
        o.Add("Exercise Duration", arGMC.mExerciseDurationSeconds);
        o.Add("Exercise Intensity", int(arGMC.mExerciseIntensity));
    }
    else {
        o.Add("Exercise Duration", utils::DynamicData());
        o.Add("Exercise Intensity", utils::DynamicData());
    }
    if (arGMC.mFlags & GlucoseServiceProfile::GlucoseMeasurementContext::Flags::MedicationPresent) {
        o.Add("Medication ID", std::string(magic_enum::enum_name(arGMC.mMedicationID)));
        o.Add("Medication", arGMC.mMedication);
    }
    else {
        o.Add("Medication ID", utils::DynamicData());
        o.Add("Medication", utils::DynamicData());
    }
    o.Add("Medication Unit", ((arGMC.mMedicationUnit == GlucoseServiceProfile::MedicationUnits::MassKilogram) ? "mg" : "ml"));
    if (arGMC.mFlags & GlucoseServiceProfile::GlucoseMeasurementContext::Flags::HbA1cPresent) {
        o.Add("HbA1c", arGMC.mHbA1c);
    }
    else {
        o.Add("HbA1c", utils::DynamicData());
    }

    return o;
}

utils::DynamicData& operator<<(utils::DynamicData &o, const std::vector<GlucoseServiceProfile::GlucoseMeasurement> &arList)
{
    for (auto &row : arList) {
        DynamicData dd_row;
        dd_row << row;
        o.Add(dd_row);
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
    auto flags = GlucoseMeasurementContext::Flags(aStream.Uint8());
    auto seq_no = aStream.Uint16();
    for (auto &mes : mMeasurements) {
        if (mes.mSequenceNo == seq_no) {
            mes.mContext.Populate(flags, aStream);
            break;
        }
    }
}

} // namespace rsp

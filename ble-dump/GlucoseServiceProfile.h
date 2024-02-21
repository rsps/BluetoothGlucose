/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef GLUCOSE_SERVICE_PROFILE_H
#define GLUCOSE_SERVICE_PROFILE_H

#include <utils/DateTime.h>
#include <vector>
#include <memory>
#include "UUID.h"
#include "BleServiceBase.h"
#include "AttributeStream.h"

namespace rsp {

class GlucoseServiceProfile : public BleService<GlucoseServiceProfile>
{
public:
    enum Flags {
        TimeOffsetPresent           = 0x01,
        GlucoseConcentrationPresent = 0x02,
        GlucoseInMMol               = 0x04,
        SensorStatusPresent         = 0x08
    };
    enum class Units {
        mg_dL,
        mmol_L
    };
    enum class Type {
        Reserved,
        CapillaryWholeBlood,
        CapillaryPlasma,
        VenousWholeBlood,
        VenousPlasma,
        ArterialWholeBlood,
        ArterialPlasma,
        UndeterminedWholeBlood,
        UndeterminedPlasma,
        InterstitialFluid,
        ControlSolution,
        ReservedForFutureUse1 = 0x0B,
        ReservedForFutureUse2 = 0x0C,
        ReservedForFutureUse3 = 0x0D,
        ReservedForFutureUse4 = 0x0E,
        ReservedForFutureUse5 = 0x0F
    };
    enum class Location {
        Reserved,
        Finger,
        AlternateSiteTest,
        Earlobe,
        ControlSolution,
        NotAvailable = 0xF
    };
    enum SensorStatus {
        None = 0,
        BatteryLow              = 0x0001,
        SensorMalfunction       = 0x0002,
        SampleSizeInsufficient  = 0x0004,
        StripInsertionError     = 0x0008,
        StripTypeIncorrect      = 0x0010,
        SensorResultToHigh      = 0x0020,
        SensorResultToLow       = 0x0040,
        SensorTemperatureToHigh = 0x0080,
        SensorTemperatureToLow  = 0x0100,
        SensorReadInterrupted   = 0x0200,
        GeneralDeviceFault      = 0x0400,
        TimeFault               = 0x0800,
        Reserved                = 0xF000
    };

    struct GlucoseMeasurement {
        uint16_t mSequenceNo = 0;
        rsp::utils::DateTime mCaptureTime{};
        Units mUnit = Units::mg_dL;
        float mGlucoseConcentration = 0.0;
        Type mType = Type::Reserved;
        Location mLocation = Location::Reserved;
        SensorStatus mSensorStatus = SensorStatus::None;
        bool mHasContext = false;

        GlucoseMeasurement() = default;
        /**
         * \brief Constructor that takes binary GlucoseMeasurement data
         * \param arValue Byte array with data
         * \Reference Section 3.107 in GATT Specification Supplement (https://www.bluetooth.com/specifications/specs/gatt-specification-supplement-5/)
         */
        explicit GlucoseMeasurement(AttributeStream &s);
    };

    explicit GlucoseServiceProfile(const TrustedDevice &arDevice);
    ~GlucoseServiceProfile() override;

    size_t GetMeasurementsCount();
    const std::vector<GlucoseMeasurement>& ReadAllMeasurements();
    GlucoseServiceProfile& ClearAllMeasurements();

    [[nodiscard]] const std::vector<GlucoseMeasurement>& GetMeasurements() const { return mMeasurements; }

protected:
    std::string mRACP{};
    std::string mGlucoseMeasurement{};
    std::string mGlucoseMeasurementContext{};
    std::vector<GlucoseMeasurement> mMeasurements{};
    std::uint16_t mRecordCount = 0;
    bool mCommandDone = false;

    void sendCommand(std::uint16_t aCommand, int aTimeoutMs);
    void racpHandler(AttributeStream aStream);
    void measurementHandler(AttributeStream aStream);
    void measurementContextHandler(AttributeStream aStream);
};

std::ostream& operator<<(std::ostream &o, const GlucoseServiceProfile::GlucoseMeasurement &arGM);
std::ostream& operator<<(std::ostream &o, const std::vector<GlucoseServiceProfile::GlucoseMeasurement> &arList);

} // namespace rsp

#endif //GLUCOSE_SERVICE_PROFILE_H

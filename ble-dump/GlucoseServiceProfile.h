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

#include "simplebluez/Bluez.h"
#include <logging/LogChannel.h>
#include <utils/DateTime.h>
#include <vector>
#include <memory>

namespace rsp {


class GlucoseServiceProfile : public rsp::logging::NamedLogger<GlucoseServiceProfile>
{
public:
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
        ArterialPlasma,
        UndeterminedWholeBlood,
        UndeterminedPlasma,
        InterstitialFluid,
        ControlSolution
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

    };

    struct GlucoseMeasurement {
        uint16_t mSequenceNo = 0;
        rsp::utils::DateTime mCaptureTime{};
        Units mUnit = Units::mg_dL;
        float mGlucoseConcentration = 0.0;
        Type mType = Type::Reserved;
        Location mLocation = Location::Reserved;
        SensorStatus mSensorStatus = SensorStatus::None;

        GlucoseMeasurement() = default;
        /**
         * \brief Constructor that takes binary GlucoseMeasurement data
         * \param arValue Byte array with data
         * \Reference Section 3.107 in GATT Specification Supplement (https://www.bluetooth.com/specifications/specs/gatt-specification-supplement-5/)
         */
        explicit GlucoseMeasurement(const SimpleBluez::ByteArray &arValue);
    };

    explicit GlucoseServiceProfile(std::shared_ptr<SimpleBluez::Device> apDevice);

    GlucoseServiceProfile& ReadAllMeasurements();
    GlucoseServiceProfile& ClearAllMeasurements();

    [[nodiscard]] const std::vector<GlucoseMeasurement>& GetMeasurements() const { return mMeasurements; }

protected:
    std::shared_ptr<SimpleBluez::Device> mpDevice;
    std::vector<GlucoseMeasurement> mMeasurements{};
};

} // namespace rsp

#endif //GLUCOSE_SERVICE_PROFILE_H

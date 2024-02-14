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
#include <limits>
#include "GlucoseServiceProfile.h"
#include "exceptions.h"
#include "AttributeStream.h"

using namespace rsp::utils;

namespace rsp {

GlucoseServiceProfile::GlucoseMeasurement::GlucoseMeasurement(const SimpleBluez::ByteArray &arValue)
{
    if (arValue.size() < 10) {
        THROW_WITH_BACKTRACE(EGlucoseArgument);
    }
    AttributeStream s(arValue);


    auto it = arValue.begin();
    uint8_t flags = *it++;
    mUnit = (flags & 0x04) ? Units::mmol_L : Units::mg_dL;
    mSequenceNo = uint16_t(*it++) + (uint16_t(*it++) << 8);
    uint16_t year = uint16_t(*it++) + (uint16_t(*it++) << 8);
    mCaptureTime = DateTime(year, *it++, *it++, *it++, *it++, *it++);
    if (flags & 0x01) {
        int16_t offset = int16_t(uint16_t(*it++) + (uint16_t(*it++) << 8));
        mCaptureTime += std::chrono::minutes(offset);
    }
    if (flags & 0x02) {
        mGlucoseConcentration = SFloat(it).GetFloat();
        uint8_t type_location = *it++;
        mType = Type((type_location & 0xF0) >> 4);
        mLocation = Location(type_location & 0x0F);
    }
    if (flags & 0x04) {
        mSensorStatus = SensorStatus::None;

    }
}

GlucoseServiceProfile::GlucoseServiceProfile(std::shared_ptr<SimpleBluez::Device> apDevice)
    : mpDevice(std::move(apDevice))
{
}

GlucoseServiceProfile& GlucoseServiceProfile::ReadAllMeasurements()
{
    return *this;
}

GlucoseServiceProfile& GlucoseServiceProfile::ClearAllMeasurements()
{
    return *this;
}

} // namespace rsp

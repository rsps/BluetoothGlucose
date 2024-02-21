/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef CURRENT_TIME_SERVICE_PROFILE_H
#define CURRENT_TIME_SERVICE_PROFILE_H

#include "BleServiceBase.h"
#include <ostream>
#include <utils/DateTime.h>

namespace rsp {

class CurrentTimeServiceProfile : public BleService<CurrentTimeServiceProfile>
{
public:
    enum class AdjustReason {
        ManualTimeUpdate,
        ExternalReferenceTimeUpdate,
        ChangeOfTimeZone,
        ChangeOfDST,
        Reserved
    };

    explicit CurrentTimeServiceProfile(const TrustedDevice &arDevice);

    utils::DateTime GetTime();
    CurrentTimeServiceProfile& SetTime(const utils::DateTime &arDT);

    [[nodiscard]] AdjustReason GetReason() const { return mAdjustReason; }

protected:
    std::string mRootUuid{};
    AdjustReason mAdjustReason = AdjustReason::Reserved;
};
std::ostream& operator<<(std::ostream &o, CurrentTimeServiceProfile &arService);

} // rsp

#endif //CURRENT_TIME_SERVICE_PROFILE_H

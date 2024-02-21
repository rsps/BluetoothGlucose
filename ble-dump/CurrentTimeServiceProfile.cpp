/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#include "AttributeStream.h"
#include "CurrentTimeServiceProfile.h"
#include <magic_enum.hpp>

template <>
struct magic_enum::customize::enum_range<rsp::CurrentTimeServiceProfile::AdjustReason> {
    static constexpr bool is_flags = true;
};

namespace rsp {

std::ostream& operator<<(std::ostream &o, CurrentTimeServiceProfile &arService)
{
    o << "Current Time: " << arService.GetTime() << ", " << magic_enum::enum_flags_name(arService.GetReason());
    return o;
}

CurrentTimeServiceProfile::CurrentTimeServiceProfile(const TrustedDevice &arDevice)
    : BleService<CurrentTimeServiceProfile>(arDevice, uuid::Identifiers::CurrentTimeService)
{
    mRootUuid = mService.uuid().substr(8);
}

utils::DateTime CurrentTimeServiceProfile::GetTime()
{
    AttributeStream s(mDevice.GetPeripheral().read(mService.uuid(), ToString(uuid::Identifiers::CurrentTime) + mRootUuid));
    auto result = s.DateTime(true, true);
    mAdjustReason = AdjustReason(s.Uint8());
    return result;
}

CurrentTimeServiceProfile& CurrentTimeServiceProfile::SetTime(const utils::DateTime &arDT)
{
    AttributeStream s(10);
    s.DateTime(arDT, true, true);
    s.Uint8(uint8_t(AdjustReason::ManualTimeUpdate));
    mDevice.GetPeripheral().write(mService.uuid(), ToString(uuid::Identifiers::CurrentTime) + mRootUuid, ToString(uuid::Identifiers::ClientCharacteristicConfiguration) + mRootUuid, s.GetArray());
    return *this;
}

} // rsp

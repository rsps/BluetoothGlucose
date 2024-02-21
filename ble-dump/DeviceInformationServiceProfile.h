/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#ifndef DEVICE_INFORMATION_SERVICE_PROFILE_H
#define DEVICE_INFORMATION_SERVICE_PROFILE_H

#include "AttributeStream.h"
#include "BleServiceBase.h"
#include <ostream>

namespace rsp {

class DeviceInformationServiceProfile : public BleService<DeviceInformationServiceProfile>
{
public:
    enum class VendorIdSource {
        Reserved,
        BluetoothSIG,
        USBImplementersForum
    };

    struct PnPID {
        VendorIdSource mSource = VendorIdSource::Reserved;
        uint16_t mVendorId = 0;
        uint16_t mProductId = 0;
        uint16_t mProductVersion = 0;

        PnPID() = default;
        explicit PnPID(AttributeStream aStream);
    };

    explicit DeviceInformationServiceProfile(const TrustedDevice &arDevice);

protected:
    friend std::ostream& operator<<(std::ostream &o, const DeviceInformationServiceProfile &arDeviceInformation);

    std::string mRootUuid{};
    uint64_t mSystemId{};
    std::string mModelNumber{};
    std::string mSerialNumber{};
    std::string mFirmwareRevision{};
    std::string mSoftwareRevision{};
    std::string mManufacturerName{};
    AttributeStream mRegulatoryCertDataList;
    PnPID mPnPID{};

    AttributeStream read(uuid::Identifiers aIdentifier);
};
std::ostream& operator<<(std::ostream &o, const DeviceInformationServiceProfile &arDeviceInformation);
std::ostream& operator<<(std::ostream &o, const DeviceInformationServiceProfile::PnPID &arPnpID);

} // rsp

#endif //DEVICE_INFORMATION_SERVICE_PROFILE_H

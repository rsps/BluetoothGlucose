/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#include "DeviceInformationServiceProfile.h"
#include <iomanip>
#include <magic_enum.hpp>

namespace rsp {

std::ostream& operator<<(std::ostream &o, const DeviceInformationServiceProfile &arDeviceInformation)
{
    o
        << "System Id: 0x" << std::hex << arDeviceInformation.mSystemId << std::dec << "\n"
        << "Model Number: " << arDeviceInformation.mModelNumber << "\n"
        << "Serial Number: " << arDeviceInformation.mSerialNumber << "\n"
        << "Firmware Revision: " << arDeviceInformation.mFirmwareRevision << "\n"
        << "Software Revision: " << arDeviceInformation.mSoftwareRevision << "\n"
        << "Manufacturer Name: " << arDeviceInformation.mManufacturerName << "\n"
        << "Regulatory Cert. Data List: " << arDeviceInformation.mRegulatoryCertDataList << "\n"
        << arDeviceInformation.mPnPID;
    return o;
}

std::ostream& operator<<(std::ostream &o, const DeviceInformationServiceProfile::PnPID &arPnpID)
{
    o
        << "Pnp ID - Vendor ID Source: " << magic_enum::enum_name(arPnpID.mSource) << "\n"
        << "Pnp ID - Vendor ID: " << arPnpID.mVendorId << "\n"
        << "Pnp ID - Product ID: " << arPnpID.mProductId << "\n"
        << "Pnp ID - Product Version: " << arPnpID.mProductVersion << "\n";
    return o;
}

DeviceInformationServiceProfile::PnPID::PnPID(AttributeStream aStream)
{
    mSource = VendorIdSource(aStream.Uint8());
    mVendorId = aStream.Uint16();
    mProductId = aStream.Uint16();
    mProductVersion = aStream.Uint16();
}

DeviceInformationServiceProfile::DeviceInformationServiceProfile(const rsp::TrustedDevice &arDevice)
    : BleService<DeviceInformationServiceProfile>(arDevice, uuid::Identifiers::DeviceInformationService)
{
    mRootUuid = mService.uuid().substr(8);

    mSystemId = read(uuid::Identifiers::SystemID).Uint64();
    mModelNumber = read(uuid::Identifiers::ModelNumberString).String();
    mSerialNumber = read(uuid::Identifiers::SerialNumberString).String();
    mFirmwareRevision = read(uuid::Identifiers::FirmwareRevisionString).String();
    mSoftwareRevision = read(uuid::Identifiers::SoftwareRevisionString).String();
    mManufacturerName = read(uuid::Identifiers::ManufacturerNameString).String();
    mRegulatoryCertDataList = read(uuid::Identifiers::IEEE_11073_20601_RegulatoryCertDataList);
    mPnPID = PnPID(read(uuid::Identifiers::PnPID));
}

AttributeStream DeviceInformationServiceProfile::read(uuid::Identifiers aIdentifier)
{
    AttributeStream stream(mDevice.GetPeripheral().read(mService.uuid(), ToString(aIdentifier) + mRootUuid));

    mLogger.Info() << "Read from " << ToName(aIdentifier) << ": " << stream;

    return stream;
}

} // rsp

/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef BLUETOOTHGLUCOSE_BLE_DUMP_UUID_H
#define BLUETOOTHGLUCOSE_BLE_DUMP_UUID_H

#include <string>
#include <simpleble/SimpleBLE.h>

namespace rsp {

namespace uuid {

// Service and Characteristic identifiers:
enum class Identifiers : uint32_t
{
    None = 0,
    GenericAttributeProfileService = 0x1801,
    CurrentTimeService = 0x1805,
    GlucoseService = 0x1808,
    DeviceInformationService = 0x180a,

    ClientCharacteristicConfiguration = 0x2902,
    GlucoseMeasurement = 0x2a18,
    GlucoseMeasurementContext = 0x2a34,
    GlucoseFeature = 0x2a51,
    RecordAccessControlPoint = 0x2a52,

    SystemID = 0x2a23,
    ModelNumberString = 0x2a24,
    SerialNumberString = 0x2a25,
    FirmwareRevisionString = 0x2a26,
    SoftwareRevisionString = 0x2a28,
    ManufacturerNameString = 0x2a29,
    IEEE_11073_20601_RegulatoryCertDataList = 0x2a2a,
    PnPID = 0x2a50,

    CurrentTime = 0x2a2b
};

Identifiers FromString(const std::string &arUUID);

std::string ToString(Identifiers aIdentifier);

std::string ToName(Identifiers aIdentifier);

} // namespace uuid

std::ostream& operator<<(std::ostream &o, rsp::uuid::Identifiers aIdentifier);
std::ostream& operator<<(std::ostream &o, SimpleBLE::Service &arService);
std::ostream& operator<<(std::ostream &o, SimpleBLE::Characteristic &arCharacteristic);
std::ostream& operator<<(std::ostream &o, SimpleBLE::Descriptor &arDescriptor);

} // namespace rsp

#endif //BLUETOOTHGLUCOSE_BLE_DUMP_UUID_H

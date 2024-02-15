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

#include <ostream>
#include <string_view>
#include "simplebluez/Bluez.h"

namespace rsp {

class UUID
{
public:
    // Service and Characteristic identifiers:
    enum class Identifiers : uint32_t {
        None = 0,
        GenericAttributeProfileService  = 0x1801,
        CurrentTimeService              = 0x1805,
        GlucoseService                  = 0x1808,
        DeviceInformationService        = 0x180a,

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

    enum class Types {
        Service,
        Characteristic,
        Descriptor
    };

    UUID(Types aType, std::string aUUID, std::string aPath, UUID* apParent = nullptr);

    [[nodiscard]] const std::string& GetUUID() const { return mUUID; }
    [[nodiscard]] const std::string& GetPath() const { return mPath; }
    [[nodiscard]] const std::string& GetName() const { return mName; }
    [[nodiscard]] Identifiers GetId() const { return mId; }
    [[nodiscard]] Types GetType() const { return mType; }
    [[nodiscard]] UUID& GetService();

    [[nodiscard]] bool operator==(Identifiers aId) const { return mId == aId; }

protected:
    std::string mUUID;
    std::string mName;
    std::string mPath;
    UUID *mpParent;
    Identifiers mId = Identifiers::None;
    Types mType;

    static std::string toName(Identifiers aId);
};

UUID::Identifiers FromString(const std::string &arUUID);
std::string ToString(UUID::Identifiers aIdentifier);

std::ostream& operator<<(std::ostream &o, const UUID &arUuid);

} // rsp

#endif //BLUETOOTHGLUCOSE_BLE_DUMP_UUID_H

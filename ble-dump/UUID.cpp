/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include "UUID.h"
#include <iomanip>
#include <map>
#include <sstream>

namespace rsp {


UUID::Identifiers FromString(const std::string &arUUID)
{
    uint32_t id = std::stoul(arUUID, nullptr, 16);
    return static_cast<UUID::Identifiers>(id);
}

std::string ToString(UUID::Identifiers aIdentifier)
{
    std::stringstream stream;
    stream << std::setfill ('0') << std::setw(8) << std::hex << uint32_t(aIdentifier);
    return stream.str();
}

UUID::UUID(Types aType, std::string aUUID, std::string aPath, UUID* apParent)
    : mType(aType),
      mUUID(std::move(aUUID)),
      mPath(std::move(aPath)),
      mpParent(apParent)
{
    mId = FromString(mUUID);
    mName = toName(mId);
}

std::string UUID::toName(UUID::Identifiers aId)
{
    const struct {
        Identifiers id;
        const char *name;
    } cMap[] {
        { Identifiers::GenericAttributeProfileService, "Generic Attribute Profile" },
        { Identifiers::CurrentTimeService, "Current Time" },
        { Identifiers::GlucoseService, "Glucose" },
        { Identifiers::DeviceInformationService, "Device Information" },
        { Identifiers::ClientCharacteristicConfiguration, "Client Characteristic Configuration" },
        { Identifiers::GlucoseMeasurement, "Glucose Measurement" },
        { Identifiers::GlucoseMeasurementContext, "Glucose Measurement Context" },
        { Identifiers::GlucoseFeature, "Glucose Feature" },
        { Identifiers::RecordAccessControlPoint, "Record Access Control Point" },
        { Identifiers::SystemID, "System ID" },
        { Identifiers::ModelNumberString, "Model Number String" },
        { Identifiers::SerialNumberString, "Serial Number String" },
        { Identifiers::FirmwareRevisionString, "Firmware Revision String" },
        { Identifiers::SoftwareRevisionString, "Software Revision String" },
        { Identifiers::ManufacturerNameString, "Manufacturer Name String" },
        { Identifiers::IEEE_11073_20601_RegulatoryCertDataList, "IEEE 11073-20601 Regulatory Cert. Data List" },
        { Identifiers::PnPID, "PnP ID" },
        { Identifiers::CurrentTime, "Current Time" }
    };

    for (auto &el : cMap) {
        if (el.id == aId) {
            return el.name;
        }
    }
    return "Vendor specific";
}

UUID& UUID::GetService()
{
    UUID* result = this;
    while (result->mType != Types::Service) {
        result = result->mpParent;
    }
    return *result;
}

std::ostream& operator<<(std::ostream &o, const UUID &arUuid)
{
    switch(arUuid.GetType()) {
        case UUID::Types::Service:
            o << "Service: ";
            break;
        case UUID::Types::Characteristic:
            o << "  Characteristic: ";
            break;
        case UUID::Types::Descriptor:
            o << "    Descriptor: ";
            break;
    }
    o << arUuid.GetName() << ", " << arUuid.GetUUID() << ", " << arUuid.GetPath();
    return o;
}

} // rsp

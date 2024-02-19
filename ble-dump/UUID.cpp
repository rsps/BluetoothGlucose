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
#include <sstream>

namespace rsp::uuid {


Identifiers FromString(const std::string &arUUID)
{
    uint32_t id = std::stoul(arUUID, nullptr, 16);
    return static_cast<Identifiers>(id);
}

std::string ToString(Identifiers aIdentifier)
{
    std::stringstream stream;
    stream << std::setfill ('0') << std::setw(8) << std::hex << uint32_t(aIdentifier);
    return stream.str();
}

std::string ToName(Identifiers aId)
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

} // rsp::uuid

namespace rsp {
std::ostream &operator<<(std::ostream &o, rsp::uuid::Identifiers aIdentifier)
{
    o << rsp::uuid::ToName(aIdentifier);
    return o;
}

std::ostream &operator<<(std::ostream &o, SimpleBLE::Service &arService)
{
    using namespace rsp::uuid;
    o << ToName(FromString(arService.uuid())) << ", " << arService.uuid();
    return o;
}

std::ostream &operator<<(std::ostream &o, SimpleBLE::Characteristic &arCharacteristic)
{
    using namespace rsp::uuid;
    o << ToName(FromString(arCharacteristic.uuid())) << ", " << arCharacteristic.uuid();
    return o;
}

std::ostream &operator<<(std::ostream &o, SimpleBLE::Descriptor &arDescriptor)
{
    using namespace rsp::uuid;
    o << ToName(FromString(arDescriptor.uuid())) << ", " << arDescriptor.uuid();
    return o;
}

} // namespace rsp
//
// Created by steffen on 14-02-24.
//

#ifndef ATTRIBUTE_STREAM_H
#define ATTRIBUTE_STREAM_H

#include <simpleble/SimpleBLE.h>
#include <utils/DateTime.h>

namespace rsp {

class AttributeStream
{
public:
    AttributeStream() = default;
    explicit AttributeStream(size_t aSize);
    AttributeStream(SimpleBLE::ByteArray aBytes); // NOLINT

    AttributeStream& Uint8(uint8_t aValue);
    AttributeStream& Uint16(uint16_t aValue);
    AttributeStream& Uint32(uint32_t aValue);
    AttributeStream& Uint64(uint64_t aValue);
    AttributeStream& MedFloat16(float aValue);
    AttributeStream& MedFloat32(float aValue);
    AttributeStream& DateTime(const utils::DateTime &arDt, bool aIncludeDayOfWeek = false, bool aIncludeFractions = false);
    AttributeStream& String(const std::string &arString);

    uint8_t Uint8();
    uint16_t Uint16();
    uint32_t Uint32();
    uint64_t Uint64();
    float MedFloat16();
    float MedFloat32();
    rsp::utils::DateTime DateTime(bool aIncludeDayOfWeek = false, bool aIncludeFractions = false);
    std::string String();

    [[nodiscard]] const SimpleBLE::ByteArray& GetArray() const { return mByteArray; }

protected:
    SimpleBLE::ByteArray mByteArray;
    SimpleBLE::ByteArray::iterator mIt;

    static float makeFloat(int aExponent, int aMantissa);
    static void splitFloat(float aValue, int &arExponent, int &arMantissa);
};

std::ostream& operator<<(std::ostream &o, const AttributeStream &arBA);

} // rsp

#endif //ATTRIBUTE_STREAM_H

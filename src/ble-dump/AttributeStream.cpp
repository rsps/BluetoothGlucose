//
// Created by steffen on 14-02-24.
//

#include <cmath>
#include <AttributeStream.h>

namespace rsp {

AttributeStream::AttributeStream(size_t aSize)
        : mByteArray(aSize, '\0'),
          mIt(mByteArray.begin())
{
}

AttributeStream::AttributeStream(SimpleBLE::ByteArray aBytes)
        : mByteArray(std::move(aBytes)),
          mIt(mByteArray.begin())
{
}

uint8_t AttributeStream::Uint8()
{
    return *mIt++;
}

AttributeStream &AttributeStream::Uint8(uint8_t aValue)
{
    *mIt = char(aValue);
    mIt++;
    return *this;
}

AttributeStream& AttributeStream::Uint16(uint16_t aValue)
{
    Uint8(uint8_t(aValue & 0x00FF));
    Uint8(uint8_t(aValue >> 8));
    return *this;
}

AttributeStream &AttributeStream::Uint32(uint32_t aValue)
{
    Uint16(uint16_t(aValue & 0x0000FFFF));
    Uint16(uint16_t(aValue >> 16));
    return *this;
}

AttributeStream &AttributeStream::MedFloat16(float aValue)
{
    int exponent;
    int mantissa;
    splitFloat(aValue, exponent, mantissa);
    Uint16(uint16_t((exponent & 0x0F) << 12) + uint16_t(mantissa & 0x0FFF));
    return *this;
}

AttributeStream &AttributeStream::MedFloat32(float aValue)
{
    int exponent;
    int mantissa;
    splitFloat(aValue, exponent, mantissa);
    Uint16(uint32_t((exponent & 0xFF) << 24) + uint32_t(mantissa & 0x00FFFFFF));
    return *this;
}

uint16_t AttributeStream::Uint16()
{
    return uint16_t(Uint8()) + (uint16_t(Uint8()) << 8);
}

uint32_t AttributeStream::Uint32()
{
    return uint32_t(Uint16()) + (uint32_t(Uint16()) << 16);
}

float AttributeStream::MedFloat16()
{
    uint16_t value = Uint16();
    switch (value) {
        case 0x07FF:
        case 0x0800: // Not at this resolution
        case 0x0801: // Reserved
            return std::nanf("");
        case 0x07FE: // +INFINITY
            return std::numeric_limits<float>::infinity();
        case 0x0802: // -INFINITY
            return -std::numeric_limits<float>::infinity();
        default:
            break;
    }
    int exp = int(value & 0xF000) >> 12;
    int mantissa = int(value & 0x0FFF);
    if (exp >= 0x0008) {
        exp = -((0x000F + 1) - exp);
    }
    if (mantissa >= 0x0800) {
        mantissa = -((0x0FFF + 1) - mantissa);
    }
    return makeFloat(exp, mantissa);
}

float AttributeStream::MedFloat32()
{
    uint32_t value = Uint32();
    switch (value) {
        case 0x007FFFFF:
        case 0x00800000: // Not at this resolution
        case 0x00800001: // Reserved
            return std::nanf("");
        case 0x007FFFFE: // +INFINITY
            return std::numeric_limits<float>::infinity();
        case 0x00800002: // -INFINITY
            return -std::numeric_limits<float>::infinity();
        default:
            break;
    }
    int exp = int(value & 0xFF000000) >> 24;
    int mantissa = int(value & 0x00FFFFFF);
    if (exp >= 0x80) {
        exp = -((0xFF + 1) - exp);
    }
    if (mantissa >= 0x00800000) {
        mantissa = -((0x00FFFFFF + 1) - mantissa);
    }
    return makeFloat(exp, mantissa);
}

float AttributeStream::makeFloat(int aExponent, int aMantissa)
{
    double magnitude = std::pow(10.0f, aExponent);
    return float(aMantissa * magnitude);
}

void AttributeStream::splitFloat(float aValue, int &arExponent, int &arMantissa)
{
    uint32_t ui = reinterpret_cast<uint32_t&>(aValue);
    // TODO: std::frexp() std::ldexp()
}

AttributeStream &AttributeStream::DateTime(const utils::DateTime &arDt, bool aIncludeDayOfWeek, bool aIncludeFractions)
{
    using namespace std::chrono;
    std::tm tm = arDt;
    Uint16(tm.tm_year);
    Uint8(tm.tm_mon);
    Uint8(tm.tm_mday);
    Uint8(tm.tm_hour);
    Uint8(tm.tm_min);
    Uint8(tm.tm_sec);
    if (aIncludeDayOfWeek) {
        Uint8((tm.tm_wday == 0) ? 7 : tm.tm_wday); // tm_wday starts on sunday, ISO 8601 goes 1-7 from Monday
    }
    if (aIncludeFractions) {
        time_point<system_clock, milliseconds> msd = time_point_cast<milliseconds>(system_clock::time_point(arDt));
        long msecs = msd.time_since_epoch().count() % 1000;
        Uint8((msecs * 256) / 1000);
    }
    return *this;
}

rsp::utils::DateTime AttributeStream::DateTime(bool aIncludeDayOfWeek, bool aIncludeFractions)
{
    auto y = Uint16();
    auto m = Uint8();
    auto d = Uint8();
    auto h = Uint8();
    auto i = Uint8();
    auto s = Uint8();
    if (aIncludeDayOfWeek) {
        Uint8(); // Discard day of week
    }
    int msec = 0;
    if (aIncludeFractions) {
        auto f = long(Uint8());
        msec = int((f * 1000) / 256);
    }
    return {y, m,d, h, i, s, msec};
}

AttributeStream &AttributeStream::String(const std::string &arString)
{
    mByteArray = arString;
    return *this;
}

std::string AttributeStream::String()
{
    return mByteArray;
}

AttributeStream& AttributeStream::Uint64(uint64_t aValue)
{
    Uint32(uint32_t(aValue & 0x00000000FFFFFFFF));
    Uint32(uint32_t(aValue >> 32));
    return *this;
}

uint64_t AttributeStream::Uint64()
{
    return uint64_t(Uint32()) + (uint64_t(Uint32()) << 32);
}

std::ostream& operator<<(std::ostream &o, const AttributeStream &arBA)
{
    for (auto &byte: arBA.GetArray()) {
        o << std::setfill('0') << std::setw(2) << std::hex << uint32_t(uint8_t(byte)) << " ";
    }
    return o;
}

} // rsp

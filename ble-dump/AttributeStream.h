//
// Created by steffen on 14-02-24.
//

#ifndef ATTRIBUTE_STREAM_H
#define ATTRIBUTE_STREAM_H

#include "simplebluez/Bluez.h"

namespace rsp {

class AttributeStream
{
public:
    AttributeStream& Uint8(uint8_t aValue);
    AttributeStream& Uint16(uint16_t aValue);
    AttributeStream& Uint32(uint32_t aValue);
    AttributeStream& MedFloat16(float aValue);
    AttributeStream& MedFloat32(float aValue);

    uint8_t Uint8();
    uint16_t Uint16();
    uint32_t Uint32();
    float MedFloat16();
    float MedFloat32();

    AttributeStream(size_t aSize);
    AttributeStream(SimpleBluez::ByteArray aBytes);

protected:
    SimpleBluez::ByteArray mByteArray;
    SimpleBluez::ByteArray::iterator mIt;

    static float makeFloat(int aExponent, int aMantissa);
    static void splitFloat(float aValue, int &arExponent, int &arMantissa);
};

} // rsp

#endif //ATTRIBUTE_STREAM_H

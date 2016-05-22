#include "gtest/gtest.h"
#include "dwg/io.h"

/*                                                          */
/*               ReadBITSHORT() tests packet.               */
/*                                                          */

TEST(bitshort, bitshort_full_offset0)
{
    size_t bitOffsetFromStart = 0;
    char buffer[3];
    // 00110000 11000011 11 = 00 bitcode, 4035 value.
    buffer[0] = 0b00110000;
    buffer[1] = 0b11000011;
    buffer[2] = 0b11000000;
    short a = ReadBITSHORT ( buffer, bitOffsetFromStart );
    ASSERT_EQ (4035, a);
}

TEST(bitshort, bitshort_full_offset1)
{
    size_t bitOffsetFromStart = 1;
    char buffer[3];
    // 00110000 11000011 11
    buffer[0] = 0b00011000;
    buffer[1] = 0b01100001;
    buffer[2] = 0b11100000;
    short a = ReadBITSHORT ( buffer, bitOffsetFromStart );
    ASSERT_EQ (4035, a);
}

TEST(bitshort, bitshort_unsigned_char_offset0)
{
    size_t bitOffsetFromStart = 0;
    char buffer[3];
    // 01100010 10
    buffer[0] = 0b01100010;
    buffer[1] = 0b10110000;
    buffer[2] = 0b11000000;
    short a = ReadBITSHORT ( buffer, bitOffsetFromStart );
    ASSERT_EQ (138, a);
}


TEST(bitshort, bitshort_0_offset0)
{
    size_t bitOffsetFromStart = 0;
    char buffer[3];
    buffer[0] = 0b10000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;
    short a = ReadBITSHORT ( buffer, bitOffsetFromStart );
    ASSERT_EQ (0, a);
}

TEST(bitshort, bitshort_256_offset0)
{
    size_t bitOffsetFromStart = 0;
    char buffer[3];
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;
    short a = ReadBITSHORT ( buffer, bitOffsetFromStart );
    ASSERT_EQ (256, a);
}

/*                                                          */
/*               Read3B() tests packet.                     */
/*                                                          */

TEST(triplebits, triplebits_offset0)
{
    size_t bitOffsetFromStart = 0;
    char buffer[3];
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;
    char a = Read3B ( buffer, bitOffsetFromStart );
    ASSERT_EQ (6, a);
}

TEST(triplebits, triplebits_offset1)
{
    size_t bitOffsetFromStart = 1;
    char buffer[3];
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;
    char a = Read3B ( buffer, bitOffsetFromStart );
    ASSERT_EQ (4, a);
}

TEST(triplebits, triplebits_offset2)
{
    size_t bitOffsetFromStart = 2;
    char buffer[3];
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;
    char a = Read3B ( buffer, bitOffsetFromStart );
    ASSERT_EQ (0, a);
}

TEST(triplebits, triplebits_offset3)
{
    size_t bitOffsetFromStart = 3;
    char buffer[3];
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;
    char a = Read3B ( buffer, bitOffsetFromStart );
    ASSERT_EQ (0, a);
}

TEST(triplebits, triplebits_offset6)
{
    size_t bitOffsetFromStart = 6;
    char buffer[3];
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;
    char a = Read3B ( buffer, bitOffsetFromStart );
    ASSERT_EQ (3, a);
}

TEST(triplebits, triplebits_offset7)
{
    size_t bitOffsetFromStart = 7;
    char buffer[3];
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;
    char a = Read3B ( buffer, bitOffsetFromStart );
    ASSERT_EQ (6, a);
}


/*                                                          */
/*               ReadRAWSHORT() tests packet.               */
/*                                                          */

//11111000 11100011 = -1821
TEST(rawshort, rawshort_offset0)
{
    size_t bitOffsetFromStart = 0;
    char buffer[3];
    buffer[0] = 0b11100011;
    buffer[1] = 0b11111000;
    buffer[2] = 0b00000001;
    short a = ReadRAWSHORT ( buffer, bitOffsetFromStart );
    ASSERT_EQ (-1821, a);
}

TEST(rawshort, rawshort_offset6)
{
    size_t bitOffsetFromStart = 6;
    char buffer[3];
    // 10 11110101 00100000 = 18621
    buffer[0] = 0b00000010;
    buffer[1] = 0b11110101;
    buffer[2] = 0b00100000;
    short a = ReadRAWSHORT ( buffer, bitOffsetFromStart );
    ASSERT_EQ (18621, a);
}

TEST(rawshort, rawshort_offset7)
{
    size_t bitOffsetFromStart = 7;
    char buffer[3];
    // 1 10110001 0111000 = - 18216
    buffer[0] = 0b00000001;
    buffer[1] = 0b10110001;
    buffer[2] = 0b01110001;
    short a = ReadRAWSHORT ( buffer, bitOffsetFromStart );
    ASSERT_EQ (-18216, a);
}

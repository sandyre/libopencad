#include "gtest/gtest.h"
#include "internal/io/cadbitstreamreader.hpp"

/*                                                          */
/*               ReadBITSHORT() tests packet.               */
/*                                                          */

TEST(bitshort, bitshort_full_offset0)
{
	std::vector<unsigned char> buffer(3);
    buffer[0] = 0b00110000;
    buffer[1] = 0b11000011;
    buffer[2] = 0b11000000;

	libopencad::CADBitStreamReader reader(buffer, 0);
    short a = reader.ReadBitShort();
    ASSERT_EQ (4035, a);
}

TEST(bitshort, bitshort_full_offset1)
{
	std::vector<unsigned char> buffer(3);
    // 00110000 11000011 11
    buffer[0] = 0b00011000;
    buffer[1] = 0b01100001;
    buffer[2] = 0b11100000;

	libopencad::CADBitStreamReader reader(buffer, 1);
    short a = reader.ReadBitShort();
    ASSERT_EQ (4035, a);
}

TEST(bitshort, bitshort_unsigned_char_offset0)
{
	std::vector<unsigned char> buffer(3);
    // 01100010 10
    buffer[0] = 0b01100010;
    buffer[1] = 0b10110000;
    buffer[2] = 0b11000000;

	libopencad::CADBitStreamReader reader(buffer, 0);
    short a = reader.ReadBitShort();
    ASSERT_EQ (138, a);
}


TEST(bitshort, bitshort_0_offset0)
{
	std::vector<unsigned char> buffer(3);
    buffer[0] = 0b10000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;

	libopencad::CADBitStreamReader reader(buffer, 0);
    short a = reader.ReadBitShort();
    ASSERT_EQ (0, a);
}

TEST(bitshort, bitshort_256_offset0)
{
	std::vector<unsigned char> buffer(3);
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;

	libopencad::CADBitStreamReader reader(buffer, 0);
    short a = reader.ReadBitShort();
    ASSERT_EQ (256, a);
}

/*                                                          */
/*               Read3B() tests packet.                     */
/*                                                          */

TEST(triplebits, triplebits_offset0)
{
	std::vector<unsigned char> buffer(3);
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;

	libopencad::CADBitStreamReader reader(buffer, 0);
    char a = reader.Read3Bits();
    ASSERT_EQ (6, a);
}

TEST(triplebits, triplebits_offset1)
{
	std::vector<unsigned char> buffer(3);
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;

	libopencad::CADBitStreamReader reader(buffer, 1);
    char a = reader.Read3Bits();
    ASSERT_EQ (4, a);
}

TEST(triplebits, triplebits_offset2)
{
	std::vector<unsigned char> buffer(3);
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;

	libopencad::CADBitStreamReader reader(buffer, 2);
    char a = reader.Read3Bits();
    ASSERT_EQ (0, a);
}

TEST(triplebits, triplebits_offset3)
{
	std::vector<unsigned char> buffer(3);
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;

	libopencad::CADBitStreamReader reader(buffer, 3);
    char a = reader.Read3Bits();
    ASSERT_EQ (0, a);
}

TEST(triplebits, triplebits_offset6)
{
	std::vector<unsigned char> buffer(3);
    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;

	libopencad::CADBitStreamReader reader(buffer, 6);
    char a = reader.Read3Bits();
    ASSERT_EQ (3, a);
}

TEST(triplebits, triplebits_offset7)
{
	std::vector<unsigned char> buffer(3);

    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;

	libopencad::CADBitStreamReader reader(buffer, 7);
    char a = reader.Read3Bits();
    ASSERT_EQ (6, a);
}


/*                                                          */
/*               ReadRAWSHORT() tests packet.               */
/*                                                          */

TEST(rawshort, rawshort_offset0)
{
	std::vector<unsigned char> buffer(3);
	//11111000 11100011 = -1821
    buffer[0] = 0b11100011;
    buffer[1] = 0b11111000;
    buffer[2] = 0b00000001;

	libopencad::CADBitStreamReader reader(buffer);
    short a = reader.ReadRawShort();
    ASSERT_EQ (-1821, a);
}

TEST(rawshort, rawshort_offset6)
{
	std::vector<unsigned char> buffer(3);
    // 10 11110101 00100000 = 18621
    buffer[0] = 0b00000010;
    buffer[1] = 0b11110101;
    buffer[2] = 0b00100000;

	libopencad::CADBitStreamReader reader(buffer, 6);
    short a = reader.ReadRawShort();
    ASSERT_EQ (18621, a);
}

TEST(rawshort, rawshort_offset7)
{
	std::vector<unsigned char> buffer(3);
    // 1 10110001 0111000 = - 18216
    buffer[0] = 0b00000001;
    buffer[1] = 0b10110001;
    buffer[2] = 0b01110001;

	libopencad::CADBitStreamReader reader(buffer, 7);
    short a = reader.ReadRawShort();
    ASSERT_EQ (-18216, a);
}

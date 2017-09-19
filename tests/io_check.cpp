#include "gtest/gtest.h"
#include "internal/io/cadbitstreamreader.hpp"


TEST(singlebit, all)
{
    std::vector<unsigned char> buffer(3);
    buffer[0] = 0b00101011;
    buffer[1] = 0b10001111;
    buffer[2] = 0b01101100;

    libopencad::CADBitStreamReader reader(buffer);

    ASSERT_EQ(0, reader.ReadBit());
    ASSERT_EQ(0, reader.ReadBit());
    ASSERT_EQ(1, reader.ReadBit());
    ASSERT_EQ(0, reader.ReadBit());
    ASSERT_EQ(1, reader.ReadBit());
    ASSERT_EQ(0, reader.ReadBit());
    ASSERT_EQ(1, reader.ReadBit());
    ASSERT_EQ(1, reader.ReadBit());
}


TEST(doublebits, all)
{
    std::vector<unsigned char> buffer(3);
    buffer[0] = 0b00101011;
    buffer[1] = 0b10001111;
    buffer[2] = 0b01101100;

    libopencad::CADBitStreamReader reader(buffer);

    ASSERT_EQ(0, reader.Read2Bits());
    ASSERT_EQ(2, reader.Read2Bits());
    ASSERT_EQ(2, reader.Read2Bits());
    ASSERT_EQ(3, reader.Read2Bits());
    ASSERT_EQ(2, reader.Read2Bits());
    ASSERT_EQ(0, reader.Read2Bits());
    ASSERT_EQ(3, reader.Read2Bits());
    ASSERT_EQ(3, reader.Read2Bits());
}


TEST(triplebits, all)
{
    std::vector<unsigned char> buffer(3);
    buffer[0] = 0b00101011;
    buffer[1] = 0b10001111;
    buffer[2] = 0b01101100;

    libopencad::CADBitStreamReader reader(buffer);

    ASSERT_EQ(1, reader.Read3Bits());
    ASSERT_EQ(2, reader.Read3Bits());
    ASSERT_EQ(7, reader.Read3Bits());
    ASSERT_EQ(0, reader.Read3Bits());
    ASSERT_EQ(7, reader.Read3Bits());
    ASSERT_EQ(5, reader.Read3Bits());
    ASSERT_EQ(5, reader.Read3Bits());
    ASSERT_EQ(4, reader.Read3Bits());
}


TEST(quadrobits, all)
{
    std::vector<unsigned char> buffer(3);
    buffer[0] = 0b00101011;
    buffer[1] = 0b10001111;
    buffer[2] = 0b01101100;

    libopencad::CADBitStreamReader reader(buffer);

    ASSERT_EQ(2, reader.Read4Bits());
    ASSERT_EQ(11, reader.Read4Bits());
    ASSERT_EQ(8, reader.Read4Bits());
    ASSERT_EQ(15, reader.Read4Bits());
    ASSERT_EQ(6, reader.Read4Bits());
    ASSERT_EQ(12, reader.Read4Bits());
}


TEST(bitshort, all)
{
    std::vector<unsigned char> buffer(3);
    buffer[0] = 0b00110000;
    buffer[1] = 0b11000011;
    buffer[2] = 0b11000000;

    libopencad::CADBitStreamReader reader(buffer);
    ASSERT_EQ(4035, reader.ReadBitShort());

    // 00110000 11000011 11
    buffer[0] = 0b00011000;
    buffer[1] = 0b01100001;
    buffer[2] = 0b11100000;

    reader = libopencad::CADBitStreamReader(buffer, 1);
    ASSERT_EQ(4035, reader.ReadBitShort());

    // 01100010 10
    buffer[0] = 0b01100010;
    buffer[1] = 0b10110000;
    buffer[2] = 0b11000000;

    reader = libopencad::CADBitStreamReader(buffer);
    ASSERT_EQ(138, reader.ReadBitShort());

    buffer[0] = 0b10000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;
    reader = libopencad::CADBitStreamReader(buffer);
    ASSERT_EQ(0, reader.ReadBitShort());

    buffer[0] = 0b11000001;
    buffer[1] = 0b10000000;
    buffer[2] = 0b00000001;
    reader = libopencad::CADBitStreamReader(buffer);
    ASSERT_EQ(256, reader.ReadBitShort());
}


TEST(bitlong, all)
{
    // stream: 00000000 010000000 10000000 00000000 01001000 0111110 
    // contains: 256, 0, 15, 0
    std::vector<unsigned char> buffer(6);
    buffer[0] = 0b00000000;
    buffer[1] = 0b01000000;
    buffer[2] = 0b01000000;
    buffer[3] = 0b00000000;
    buffer[4] = 0b00100100;
    buffer[5] = 0b00111110;

    libopencad::CADBitStreamReader reader(buffer);
    ASSERT_EQ(257, reader.ReadBitLong());
    ASSERT_EQ(0, reader.ReadBitLong());
    ASSERT_EQ(15, reader.ReadBitLong());
    ASSERT_EQ(0, reader.ReadBitLong());
}


TEST(bitdouble, all)
{
    // stream: 00000000 00000000 00000000 00000000 00000000 00000000 00000110 00010000 00011011
    // contains: 6, 1.0, 0.0, 0.0 (not used)
    std::vector<unsigned char> buffer(9);
    buffer[0] = 0b00000000;
    buffer[1] = 0b00000000;
    buffer[2] = 0b00000000;
    buffer[3] = 0b00000000;
    buffer[4] = 0b00000000;
    buffer[5] = 0b00000000;
    buffer[6] = 0b00000110;
    buffer[7] = 0b00010000;
    buffer[8] = 0b00011011;

    libopencad::CADBitStreamReader reader(buffer);
    ASSERT_NEAR(6.0, reader.ReadBitDouble(), 0.0001);
    ASSERT_NEAR(1.0, reader.ReadBitDouble(), 0.0001);
    ASSERT_NEAR(0.0, reader.ReadBitDouble(), 0.0001);
    ASSERT_NEAR(0.0, reader.ReadBitDouble(), 0.0001);
    
}


TEST(rawshort, all)
{
    // stream: 00000000 11111011 11111010 11000111 00001001 11111011
    // contains: 251, -1337, 2555
    std::vector<unsigned char> buffer(6);
    buffer[0] = 0b11111011;
    buffer[1] = 0b00000000;
    buffer[2] = 0b11000111;
    buffer[3] = 0b11111010;
    buffer[4] = 0b11111011;
    buffer[5] = 0b00001001;

    libopencad::CADBitStreamReader reader(buffer);
    ASSERT_EQ(251, reader.ReadRawShort());
    ASSERT_EQ(-1337, reader.ReadRawShort());
    ASSERT_EQ(2555, reader.ReadRawShort());
}


TEST(rawdouble, all)
{
    // stream: 00000000 00000000 00000000 00000000 00000000 00000000 11110000 00111111
    // contains: 1
    std::vector<unsigned char> buffer(8);
    buffer[0] = 0b00000000;
    buffer[1] = 0b00000000;
    buffer[2] = 0b00000000;
    buffer[3] = 0b00000000;
    buffer[4] = 0b00000000;
    buffer[5] = 0b00000000;
    buffer[6] = 0b11110000;
    buffer[7] = 0b00111111;

    libopencad::CADBitStreamReader reader(buffer);
    ASSERT_NEAR(1.0, reader.ReadRawDouble(), 0.0001);
}

#include "gtest/gtest.h"
#include "internal/io/cadbitstreamreader.hpp"

#define TESTS_ITERATIONS 10000

TEST(singlebit, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
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
}


TEST(doublebits, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
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
}


TEST(triplebits, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
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
}


TEST(quadrobits, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
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
}


TEST(bitshort, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
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
}


TEST(bitlong, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
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
}


TEST(bitdouble, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
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
}


TEST(rawshort, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
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
}


TEST(rawdouble, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
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
}


TEST(modularchar, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
    {
        // stream: 10000010 00100100 11101001 10010111 11100110 00110101 10000101 01001011
        // contains: 4610, 112823273, -1413
        std::vector<unsigned char> buffer(8);
        buffer[0] = 0b10000010;
        buffer[1] = 0b00100100;
        buffer[2] = 0b11101001;
        buffer[3] = 0b10010111;
        buffer[4] = 0b11100110;
        buffer[5] = 0b00110101;
        buffer[6] = 0b10000101;
        buffer[7] = 0b01001011;

        libopencad::CADBitStreamReader reader(buffer);
        ASSERT_EQ(4610, reader.ReadMChar());
        ASSERT_EQ(112823273, reader.ReadMChar());
        ASSERT_EQ(-1413, reader.ReadMChar());
    }
}


TEST(modularshort, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
    {
        // stream: 00110001 11110100 10001101 00000000
        // contains: 4650033
        std::vector<unsigned char> buffer(4);
        buffer[0] = 0b00110001;
        buffer[1] = 0b11110100;
        buffer[2] = 0b10001101;
        buffer[3] = 0b00000000;

        libopencad::CADBitStreamReader reader(buffer);
        ASSERT_EQ(4650033, reader.ReadMShort());
    }
}
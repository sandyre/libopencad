#include "gtest/gtest.h"
#include "internal/io/cadbitstreamreader.hpp"
#include "internal/io/Serialization.hpp"

#define TESTS_ITERATIONS 10000

using namespace libopencad::internal;

TEST(singlebit, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
    {
        const auto buff = std::make_shared<BitBuffer>(3);
        buff->at(0) = 0b00101011;
        buff->at(1) = 0b10001111;
        buff->at(2) = 0b01101100;

		const auto stream = std::make_shared<InputBitStream>(buff);

        ASSERT_EQ(0, BitN<1>::Deserialize(stream));
        ASSERT_EQ(0, BitN<1>::Deserialize(stream));
        ASSERT_EQ(1, BitN<1>::Deserialize(stream));
        ASSERT_EQ(0, BitN<1>::Deserialize(stream));
        ASSERT_EQ(1, BitN<1>::Deserialize(stream));
        ASSERT_EQ(0, BitN<1>::Deserialize(stream));
        ASSERT_EQ(1, BitN<1>::Deserialize(stream));
        ASSERT_EQ(1, BitN<1>::Deserialize(stream));
    }
}


TEST(doublebits, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
    {
        const auto buff = std::make_shared<BitBuffer>(3);
        buff->at(0) = 0b00101011;
        buff->at(1) = 0b10001111;
        buff->at(2) = 0b01101100;

		const auto stream = std::make_shared<InputBitStream>(buff);

        ASSERT_EQ(0, BitN<2>::Deserialize(stream));
        ASSERT_EQ(2, BitN<2>::Deserialize(stream));
        ASSERT_EQ(2, BitN<2>::Deserialize(stream));
        ASSERT_EQ(3, BitN<2>::Deserialize(stream));
        ASSERT_EQ(2, BitN<2>::Deserialize(stream));
        ASSERT_EQ(0, BitN<2>::Deserialize(stream));
        ASSERT_EQ(3, BitN<2>::Deserialize(stream));
        ASSERT_EQ(3, BitN<2>::Deserialize(stream));
    }
}


TEST(triplebits, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
    {
        const auto buff = std::make_shared<BitBuffer>(3);
        buff->at(0) = 0b00101011;
        buff->at(1) = 0b10001111;
        buff->at(2) = 0b01101100;

		const auto stream = std::make_shared<InputBitStream>(buff);

        ASSERT_EQ(1, BitN<3>::Deserialize(stream));
        ASSERT_EQ(2, BitN<3>::Deserialize(stream));
        ASSERT_EQ(7, BitN<3>::Deserialize(stream));
        ASSERT_EQ(0, BitN<3>::Deserialize(stream));
        ASSERT_EQ(7, BitN<3>::Deserialize(stream));
        ASSERT_EQ(5, BitN<3>::Deserialize(stream));
        ASSERT_EQ(5, BitN<3>::Deserialize(stream));
        ASSERT_EQ(4, BitN<3>::Deserialize(stream));
    }
}


TEST(quadrobits, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
    {
        const auto buff = std::make_shared<BitBuffer>(3);
        buff->at(0) = 0b00101011;
        buff->at(1) = 0b10001111;
        buff->at(2) = 0b01101100;

		const auto stream = std::make_shared<InputBitStream>(buff);

        ASSERT_EQ(2,  BitN<4>::Deserialize(stream));
        ASSERT_EQ(11, BitN<4>::Deserialize(stream));
        ASSERT_EQ(8,  BitN<4>::Deserialize(stream));
        ASSERT_EQ(15, BitN<4>::Deserialize(stream));
        ASSERT_EQ(6,  BitN<4>::Deserialize(stream));
        ASSERT_EQ(12, BitN<4>::Deserialize(stream));
	}
}


TEST(bitshort, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
    {
		const auto buff = std::make_shared<BitBuffer>(3);
        buff->at(0) = 0b00110000;
        buff->at(1) = 0b11000011;
        buff->at(2) = 0b11000000;

		auto stream = std::make_shared<InputBitStream>(buff);

        ASSERT_EQ(4035, Bit<int16_t>::Deserialize(stream));

        // 01100010 10
        buff->at(0) = 0b01100010;
        buff->at(1) = 0b10110000;
        buff->at(2) = 0b11000000;

		stream = std::make_shared<InputBitStream>(buff);

        ASSERT_EQ(138, Bit<int16_t>::Deserialize(stream));

        buff->at(0) = 0b10000001;
        buff->at(1) = 0b10000000;
        buff->at(2) = 0b00000001;

		stream = std::make_shared<InputBitStream>(buff);

	 	ASSERT_EQ(0, Bit<int16_t>::Deserialize(stream));

        buff->at(0) = 0b11000001;
        buff->at(1) = 0b10000000;
        buff->at(2) = 0b00000001;

		stream = std::make_shared<InputBitStream>(buff);

        ASSERT_EQ(256, Bit<int16_t>::Deserialize(stream));
    }
}


TEST(bitlong, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
    {
        // stream: 00000000 010000000 10000000 00000000 01001000 0111110
        // contains: 256, 0, 15, 0
        const auto buff = std::make_shared<BitBuffer>(6);
        buff->at(0) = 0b00000000;
        buff->at(1) = 0b01000000;
        buff->at(2) = 0b01000000;
        buff->at(3) = 0b00000000;
        buff->at(4) = 0b00100100;
        buff->at(5) = 0b00111110;

		const auto stream = std::make_shared<InputBitStream>(buff);

        ASSERT_EQ(257, Bit<int32_t>::Deserialize(stream));
        ASSERT_EQ(0,   Bit<int32_t>::Deserialize(stream));
        ASSERT_EQ(15,  Bit<int32_t>::Deserialize(stream));
        ASSERT_EQ(0,   Bit<int32_t>::Deserialize(stream));
    }
}


TEST(bitdouble, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
    {
        // stream: 00000000 00000000 00000000 00000000 00000000 00000000 00000110 00010000 00011011
        // contains: 6, 1.0, 0.0, 0.0 (not used)
        const auto buff = std::make_shared<BitBuffer>(9);
        buff->at(0) = 0b00000000;
        buff->at(1) = 0b00000000;
        buff->at(2) = 0b00000000;
        buff->at(3) = 0b00000000;
        buff->at(4) = 0b00000000;
        buff->at(5) = 0b00000000;
        buff->at(6) = 0b00000110;
        buff->at(7) = 0b00010000;
        buff->at(8) = 0b00011011;

		const auto stream = std::make_shared<InputBitStream>(buff);

        ASSERT_NEAR(6.0, Bit<double>::Deserialize(stream), 0.0001);
        ASSERT_NEAR(1.0, Bit<double>::Deserialize(stream), 0.0001);
        ASSERT_NEAR(0.0, Bit<double>::Deserialize(stream), 0.0001);
        ASSERT_NEAR(0.0, Bit<double>::Deserialize(stream), 0.0001);
    }
}


TEST(rawshort, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
    {
        // stream: 00000000 11111011 11111010 11000111 00001001 11111011
        // contains: 251, -1337, 2555
        const auto buff = std::make_shared<BitBuffer>(6);
        buff->at(0) = 0b11111011;
        buff->at(1) = 0b00000000;
        buff->at(2) = 0b11000111;
        buff->at(3) = 0b11111010;
        buff->at(4) = 0b11111011;
        buff->at(5) = 0b00001001;

		const auto stream = std::make_shared<InputBitStream>(buff);
        ASSERT_EQ(251,   Raw<int16_t>::Deserialize(stream));
        ASSERT_EQ(-1337, Raw<int16_t>::Deserialize(stream));
        ASSERT_EQ(2555,  Raw<int16_t>::Deserialize(stream));
    }
}


TEST(rawdouble, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
    {
        // stream: 00000000 00000000 00000000 00000000 00000000 00000000 11110000 00111111
        // contains: 1
        const auto buff = std::make_shared<BitBuffer>(8);
        buff->at(0) = 0b00000000;
        buff->at(1) = 0b00000000;
        buff->at(2) = 0b00000000;
        buff->at(3) = 0b00000000;
        buff->at(4) = 0b00000000;
        buff->at(5) = 0b00000000;
        buff->at(6) = 0b11110000;
        buff->at(7) = 0b00111111;

		const auto stream = std::make_shared<InputBitStream>(buff);

        ASSERT_NEAR(1.0, Raw<double>::Deserialize(stream), 0.0001);
    }
}

/*
TEST(modularchar, all)
{
    for (size_t idx = 0; idx < TESTS_ITERATIONS; ++idx)
    {
        // stream: 10000010 00100100 11101001 10010111 11100110 00110101 10000101 01001011
        // contains: 4610, 112823273, -1413
        std::vector<unsigned char> buff(8);
        buff->at(0) = 0b10000010;
        buff->at(1) = 0b00100100;
        buff->at(2) = 0b11101001;
        buff->at(3) = 0b10010111;
        buff->at(4) = 0b11100110;
        buff->at(5) = 0b00110101;
        buff->at(6) = 0b10000101;
        buff->at(7) = 0b01001011;

        libopencad::CADBitStreamReader reader(buff);
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
        std::vector<unsigned char> buff(4);
        buff->at(0) = 0b00110001;
        buff->at(1) = 0b11110100;
        buff->at(2) = 0b10001101;
        buff->at(3) = 0b00000000;

        libopencad::CADBitStreamReader reader(buff);
        ASSERT_EQ(4650033, reader.ReadMShort());
    }
}
*/

#include "gtest/gtest.h"
#include "internal/io/bitstream_reader.hpp"

TEST(raw_types, raw_char)
{
	using namespace libopencad::internal;

	std::vector<std::byte> input
	{
		std::byte{0b00000001},
		std::byte{0b00000010},
		std::byte{0b00000100},
		std::byte{0b00001000},
		std::byte{0b00010000},
		std::byte{0b00100000},
		std::byte{0b01000000},
		std::byte{0b10000000}
	};

	std::vector<int8_t> expected
	{
		1,
		2,
		4,
		8,
		16,
		32,
		64,
		-128
	};
	std::vector<RawChar> result(expected.size(), RawChar{});

	io::InputBitStream stream{input};
	io::BitStreamReader reader{stream};

	for (size_t idx = 0; idx < result.size(); ++idx)
	{
		reader >> result[idx];
		ASSERT_EQ(expected[idx], result[idx].Value);
	}
}

TEST(raw_types, raw_short)
{
	using namespace libopencad::internal;

	std::vector<std::byte> input
	{
		std::byte{0b00000001},
		std::byte{0b00000010},
		std::byte{0b00000100},
		std::byte{0b00001000},
		std::byte{0b00010000},
		std::byte{0b00100000},
		std::byte{0b01000000},
		std::byte{0b10000000}
	};

	std::vector<int16_t> expected
	{
		513,
		2052,
		8208,
		-32704
	};
	std::vector<RawShort> result(expected.size(), RawShort{});

	io::InputBitStream stream{input};
	io::BitStreamReader reader{stream};

	for (size_t idx = 0; idx < result.size(); ++idx)
	{
		reader >> result[idx];
		ASSERT_EQ(expected[idx], result[idx].Value);
	}
}

TEST(raw_types, raw_long)
{
	using namespace libopencad::internal;

	std::vector<std::byte> input
	{
		std::byte{0b00000001},
		std::byte{0b00000010},
		std::byte{0b00000100},
		std::byte{0b00001000},
		std::byte{0b00010000},
		std::byte{0b00100000},
		std::byte{0b01000000},
		std::byte{0b10000000}
	};

	std::vector<int32_t> expected
	{
		134480385,
		-2143281136
	};
	std::vector<RawLong> result(expected.size(), RawLong{});

	io::InputBitStream stream{input};
	io::BitStreamReader reader{stream};

	for (size_t idx = 0; idx < result.size(); ++idx)
	{
		reader >> result[idx];
		ASSERT_EQ(expected[idx], result[idx].Value);
	}
}

TEST(raw_types, raw_long_long)
{
	using namespace libopencad::internal;

	std::vector<std::byte> input
	{
		std::byte{0b00000001},
		std::byte{0b00000010},
		std::byte{0b00000100},
		std::byte{0b00001000},
		std::byte{0b00010000},
		std::byte{0b00100000},
		std::byte{0b01000000},
		std::byte{0b10000000}
	};

	std::vector<int64_t> expected
	{
		-9205322385119247871
	};
	std::vector<RawLongLong> result(expected.size(), RawLongLong{});

	io::InputBitStream stream{input};
	io::BitStreamReader reader{stream};

	for (size_t idx = 0; idx < result.size(); ++idx)
	{
		reader >> result[idx];
		ASSERT_EQ(expected[idx], result[idx].Value);
	}
}

TEST(raw_types, raw_double)
{
	using namespace libopencad::internal;

	std::vector<std::byte> input
	{
		std::byte{0b00000000},
		std::byte{0b00000000},
		std::byte{0b00000000},
		std::byte{0b00000000},
		std::byte{0b00000000},
		std::byte{0b00000000},
		std::byte{0b11110000},
		std::byte{0b00111111}
	};

	std::vector<double> expected
	{
		1.0
	};
	std::vector<RawDouble> result(expected.size(), RawDouble{});

	io::InputBitStream stream{input};
	io::BitStreamReader reader{stream};

	for (size_t idx = 0; idx < result.size(); ++idx)
	{
		reader >> result[idx];
		ASSERT_EQ(expected[idx], result[idx].Value);
	}
}

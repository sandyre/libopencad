#include "gtest/gtest.h"
#include "internal/io/bitstream_reader.hpp"

TEST(raw_types, raw_bits_2)
{
	using namespace libopencad::internal;

	std::vector<bool> input
	{
		0,0,0,0,0,0,0,1,
		0,0,0,0,0,0,1,0,
		0,0,0,0,0,1,0,0,
		0,0,0,0,1,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		1,0,0,0,0,0,0,0
	};

	std::vector<std::bitset<2>> expected
	{
		std::bitset<2>{0}, std::bitset<2>{0}, std::bitset<2>{0}, std::bitset<2>{1},
		std::bitset<2>{0}, std::bitset<2>{0}, std::bitset<2>{0}, std::bitset<2>{2},
		std::bitset<2>{0}, std::bitset<2>{0}, std::bitset<2>{1}, std::bitset<2>{0},
		std::bitset<2>{0}, std::bitset<2>{0}, std::bitset<2>{2}, std::bitset<2>{0},
		std::bitset<2>{0}, std::bitset<2>{1}, std::bitset<2>{0}, std::bitset<2>{0},
		std::bitset<2>{0}, std::bitset<2>{2}, std::bitset<2>{0}, std::bitset<2>{0},
		std::bitset<2>{1}, std::bitset<2>{0}, std::bitset<2>{0}, std::bitset<2>{0},
		std::bitset<2>{2}, std::bitset<2>{0}, std::bitset<2>{0}, std::bitset<2>{0}
	};
	std::vector<RawBits<2>> result(expected.size(), RawBits<2>{});

	io::InputBitStream stream{input};
	io::BitStreamReader reader{stream};

	for (size_t idx = 0; idx < result.size(); ++idx)
	{
		reader >> result[idx];
		ASSERT_EQ(expected[idx], result[idx].Value);
	}
}

TEST(raw_types, raw_bits_3)
{
	using namespace libopencad::internal;

	std::vector<bool> input
	{
		0,0,0,0,0,0,0,1,
		0,0,0,0,0,0,1,0,
		0,0,0,0,0,1,0,0,
		0,0,0,0,1,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		1,0,0,0,0,0,0,0
	};

	std::vector<std::bitset<3>> expected
	{
		std::bitset<3>{0}, std::bitset<3>{0}, std::bitset<3>{2}, std::bitset<3>{0},
		std::bitset<3>{1}, std::bitset<3>{0}, std::bitset<3>{0}, std::bitset<3>{4},
		std::bitset<3>{0}, std::bitset<3>{2}, std::bitset<3>{0}, std::bitset<3>{1},
		std::bitset<3>{0}, std::bitset<3>{0}, std::bitset<3>{4}, std::bitset<3>{0},
		std::bitset<3>{2}, std::bitset<3>{0}, std::bitset<3>{1}, std::bitset<3>{0},
		std::bitset<3>{0}
	};
	std::vector<RawBits<3>> result(expected.size(), RawBits<3>{});

	io::InputBitStream stream{input};
	io::BitStreamReader reader{stream};

	for (size_t idx = 0; idx < result.size(); ++idx)
	{
		reader >> result[idx];
		ASSERT_EQ(expected[idx], result[idx].Value);
	}
}

TEST(raw_types, raw_char)
{
	using namespace libopencad::internal;

	std::vector<bool> input
	{
		0,0,0,0,0,0,0,1,
		0,0,0,0,0,0,1,0,
		0,0,0,0,0,1,0,0,
		0,0,0,0,1,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		1,0,0,0,0,0,0,0
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

	std::vector<bool> input
	{
		0,0,0,0,0,0,0,1,
		0,0,0,0,0,0,1,0,
		0,0,0,0,0,1,0,0,
		0,0,0,0,1,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		1,0,0,0,0,0,0,0
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

	std::vector<bool> input
	{
		0,0,0,0,0,0,0,1,
		0,0,0,0,0,0,1,0,
		0,0,0,0,0,1,0,0,
		0,0,0,0,1,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		1,0,0,0,0,0,0,0
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

	std::vector<bool> input
	{
		0,0,0,0,0,0,0,1,
		0,0,0,0,0,0,1,0,
		0,0,0,0,0,1,0,0,
		0,0,0,0,1,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		1,0,0,0,0,0,0,0
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

	std::vector<bool> input
	{
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		1,1,1,1,0,0,0,0,
		0,0,1,1,1,1,1,1
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

TEST(bit_types, bit_short)
{
	using namespace libopencad::internal;

	std::vector<bool> input
	{
		0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		0,1,1,0,1,1,0,1,
		0,0,0,0,1,1,1,1,
		1,0
	};

	std::vector<int16_t> expected
	{
		257,
		0,
		256,
		15,
		0
	};
	std::vector<BitShort> result(expected.size(), BitShort{});

	io::InputBitStream stream{input};
	io::BitStreamReader reader{stream};

	for (size_t idx = 0; idx < result.size(); ++idx)
	{
		reader >> result[idx];
		ASSERT_EQ(expected[idx], result[idx].Value);
	}
}

TEST(bit_types, bit_long)
{
	using namespace libopencad::internal;

	std::vector<bool> input
	{
		0,0,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,1,0,0,1,0,0,
		0,0,1,1,1,1,1,0
	};

	std::vector<int32_t> expected
	{
		257,
		0,
		15,
		0
	};
	std::vector<BitLong> result(expected.size(), BitLong{});

	io::InputBitStream stream{input};
	io::BitStreamReader reader{stream};

	for (size_t idx = 0; idx < result.size(); ++idx)
	{
		reader >> result[idx];
		ASSERT_EQ(expected[idx], result[idx].Value);
	}
}

TEST(bit_types, bit_double)
{
	using namespace libopencad::internal;

	std::vector<bool> input
	{
		0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		1,1,1,1,0,0,0,0,
		0,0,1,1,1,1,1,1,
		1,0,
		0,1
	};

	std::vector<double> expected
	{
		1.0,
		0.0,
		1.0
	};
	std::vector<BitDouble> result(expected.size(), BitDouble{});

	io::InputBitStream stream{input};
	io::BitStreamReader reader{stream};

	for (size_t idx = 0; idx < result.size(); ++idx)
	{
		reader >> result[idx];
		ASSERT_EQ(expected[idx], result[idx].Value);
	}
}

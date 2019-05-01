#pragma once

#include <bitset>

namespace libopencad::internal {

template <class T>
struct BaseType
{
	using ValueT = T;
	ValueT	Value;
};

template <size_t Count>
struct RawBits : BaseType<std::bitset<Count> >
{ };

struct RawChar : BaseType<int8_t>
{ };

struct RawShort : BaseType<int16_t>
{ };

struct RawLong : BaseType<int32_t>
{ };

struct RawLongLong : BaseType<int64_t>
{ };

struct RawDouble : BaseType<double>
{ };

struct BitShort : BaseType<int16_t>
{
	static constexpr std::bitset<2> kNormal   {0};
	static constexpr std::bitset<2> kUChar    {1};
	static constexpr std::bitset<2> kValue0   {2};
	static constexpr std::bitset<2> kValue256 {3};
};

struct BitLong : BaseType<int32_t>
{
	static constexpr std::bitset<2> kNormal   {0};
	static constexpr std::bitset<2> kUChar    {1};
	static constexpr std::bitset<2> kValue0   {2};
	static constexpr std::bitset<2> kUnused   {3};
};

struct BitDouble : BaseType<double>
{
	static constexpr std::bitset<2> kNormal   {0};
	static constexpr std::bitset<2> kValue1   {1};
	static constexpr std::bitset<2> kValue0   {2};
	static constexpr std::bitset<2> kUnused   {3};
};

struct BitDoubleWD : BitDouble
{
	static constexpr std::bitset<2> kDefault   {0};
	static constexpr std::bitset<2> kPatched4  {1};
	static constexpr std::bitset<2> kPatched6  {2};
	static constexpr std::bitset<2> kFull      {3};

	ValueT	DefaultValue;
};

}

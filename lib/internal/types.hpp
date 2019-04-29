#pragma once

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
{ };

struct BitLong : BaseType<int32_t>
{ };

struct BitDouble : BaseType<double>
{ };

struct BitDoubleWD : BitDouble
{
	ValueT	DefaultValue;
};

}

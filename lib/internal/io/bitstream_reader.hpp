#pragma once

#include <internal/io/bitstream.hpp>
#include <internal/types.hpp>
#include <internal/utils.hpp>

#include <iostream>

namespace libopencad::internal::io {

class BitStreamReader
{
public:
	explicit BitStreamReader(const InputBitStream& stream)
		: _stream(stream)
	{ }

	template <class TypeT>
	BitStreamReader& operator>>(TypeT& value);

private:
	InputBitStream	_stream;
};

template <>
BitStreamReader& BitStreamReader::operator>>(RawBits<1>& value)
{
	value.Value = _stream.Read<1>();
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawBits<2>& value)
{
	value.Value = _stream.Read<2>();
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawBits<3>& value)
{
	value.Value = _stream.Read<3>();
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawBits<4>& value)
{
	value.Value = _stream.Read<4>();
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawChar& value)
{
	value.Value = static_cast<RawChar::ValueT>(_stream.Read<8>().to_ulong());
	return *this;
}

// TODO: code below assumes machine endianness is LE

template <>
BitStreamReader& BitStreamReader::operator>>(RawShort& value)
{
	const uint16_t raw_val = utils::swap_endianness(static_cast<uint16_t>(_stream.Read<16>().to_ulong()));
	value.Value = static_cast<RawShort::ValueT>(raw_val);
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawLong& value)
{
	const uint32_t raw_val = utils::swap_endianness(static_cast<uint32_t>(_stream.Read<32>().to_ulong()));
	value.Value = static_cast<RawLong::ValueT>(raw_val);
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawLongLong& value)
{
	const uint64_t raw_val = utils::swap_endianness(static_cast<uint64_t>(_stream.Read<64>().to_ullong()));
	value.Value = static_cast<RawLongLong::ValueT>(raw_val);
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawDouble& value)
{
	const uint64_t raw_val = utils::swap_endianness(_stream.Read<64>().to_ullong());
	std::memcpy(static_cast<void*>(&value.Value), static_cast<const void*>(&raw_val), sizeof(RawDouble::ValueT));
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(BitShort& value)
{
	RawBits<2> bitcode;
	*this >> bitcode;

	if (bitcode.Value == BitShort::kNormal)
	{
		RawShort raw;
		*this >> raw;
		value.Value = raw.Value;
	}
	else if (bitcode.Value == BitShort::kUChar)
	{
		RawChar raw;
		*this >> raw;
		value.Value = raw.Value;
	}
	else if (bitcode.Value == BitShort::kValue0)
	{
		value.Value = 0;
	}
	else if (bitcode.Value == BitShort::kValue256)
	{
		value.Value = 256;
	}
	else
		assert(false);

	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(BitLong& value)
{
	RawBits<2> bitcode;
	*this >> bitcode;

	if (bitcode.Value == BitLong::kNormal)
	{
		RawLong raw;
		*this >> raw;
		value.Value = raw.Value;
	}
	else if (bitcode.Value == BitShort::kUChar)
	{
		RawChar raw;
		*this >> raw;
		value.Value = raw.Value;
	}
	else if (bitcode.Value == BitShort::kValue0)
	{
		value.Value = 0;
	}
	else
		assert(false);

	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(BitDouble& value)
{
	RawBits<2> bitcode;
	*this >> bitcode;

	if (bitcode.Value == BitDouble::kNormal)
	{
		RawDouble raw;
		*this >> raw;
		value.Value = raw.Value;
	}
	else if (bitcode.Value == BitDouble::kValue1)
	{
		value.Value = 1.0;
	}
	else if (bitcode.Value == BitShort::kValue0)
	{
		value.Value = 0.0;
	}
	else
		assert(false);

	return *this;
}

}

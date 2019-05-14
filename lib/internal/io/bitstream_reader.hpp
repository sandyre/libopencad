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

namespace detail
{

	template <class RawType>
	void ReadRawType(InputBitStream& stream, RawType& value)
	{
		using ValueT = typename RawType::ValueT;
		union Storage
		{
			ValueT value;
			std::array<std::byte, sizeof(ValueT)> bytes;
		} s;

		s.bytes = {};

		s.bytes = stream.ReadBytes<sizeof(ValueT)>();
		utils::little_to_native(s.bytes.begin(), s.bytes.end());
		value.Value = s.value;
	}

}

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
BitStreamReader& BitStreamReader::operator>>(RawChar& value)
{
	value.Value = static_cast<RawChar::ValueT>(_stream.Read<8>().to_ulong());
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawShort& value)
{
	static_assert(sizeof(RawShort::ValueT) == 2);
	detail::ReadRawType(_stream, value);
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawLong& value)
{
	static_assert(sizeof(RawLong::ValueT) == 4);
	detail::ReadRawType(_stream, value);
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawLongLong& value)
{
	static_assert(sizeof(RawLongLong::ValueT) == 8);
	detail::ReadRawType(_stream, value);
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawDouble& value)
{
	static_assert(sizeof(RawDouble::ValueT) == 8);
	detail::ReadRawType(_stream, value);
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

template <>
BitStreamReader& BitStreamReader::operator>>(ModChar& value)
{
	using ValueT = typename ModChar::ValueT;
	union Storage
	{
		ValueT value;
		std::array<RawChar, sizeof(ValueT)> bytes;
	} s;

	s.bytes = {};

	bool negate = false;
	size_t mchar_size = 0;
	while (true)
	{
		*this >> s.bytes[mchar_size];

		auto& current_byte = s.bytes[mchar_size++].Value;
		if (!(current_byte & 0b10000000))
		{
			negate = static_cast<bool>(current_byte & 0b01000000);
			current_byte &= 0b10111111;
			break;
		}
		current_byte &= 0b01111111;
	}

	utils::little_to_big(s.bytes.begin(), s.bytes.end());

	s.bytes[3].Value |= ((s.bytes[2].Value & 0b00000001) << 7);
	s.bytes[2].Value >>= 1;
	s.bytes[2].Value |= ((s.bytes[1].Value & 0b00000011) << 6);
	s.bytes[1].Value >>= 2;
	s.bytes[1].Value |= ((s.bytes[0].Value & 0b00000111) << 5);
	s.bytes[0].Value >>= 3;

	utils::big_to_native(s.bytes.begin(), s.bytes.end());

	value.Value = negate ? -s.value : s.value;
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(ModShort& value)
{
	using ValueT = typename ModShort::ValueT;
	union Storage
	{
		ValueT value;
		std::array<RawChar, sizeof(ValueT)> bytes;
	} s;

	s.bytes = {};

	size_t mshort_size = 0;
	while (true)
	{
		*this >> s.bytes[mshort_size++];
		*this >> s.bytes[mshort_size];

		auto& current_byte = s.bytes[mshort_size++].Value;
		if (!(current_byte & 0b10000000))
			break;
		current_byte &= 0b01111111;
	}

	for (size_t idx = 0; idx < sizeof(ValueT); idx += 2)
	{
		utils::little_to_big(s.bytes.begin() + idx, s.bytes.begin() + idx + 2);
	}

	s.bytes[2].Value |= ((s.bytes[1].Value & 0b00000001) << 7);
	s.bytes[1].Value >>= 1;
	s.bytes[1].Value |= ((s.bytes[0].Value & 0b00000001) << 7);
	s.bytes[0].Value >>= 2;

	utils::big_to_native(s.bytes.begin(), s.bytes.end());

	value.Value = s.value;
	return *this;
}

}

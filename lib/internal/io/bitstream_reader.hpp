#pragma once

#include <internal/io/bitstream.hpp>
#include <internal/types.hpp>

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
BitStreamReader& BitStreamReader::operator>>(RawChar& value)
{
	auto raw = _stream.Read(8);
	value.Value = *reinterpret_cast<RawChar::ValueT*>(raw.data());
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawShort& value)
{
	auto raw = _stream.Read(16);
	value.Value = *reinterpret_cast<RawShort::ValueT*>(raw.data());
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawLong& value)
{
	auto raw = _stream.Read(32);
	value.Value = *reinterpret_cast<RawLong::ValueT*>(raw.data());
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawLongLong& value)
{
	auto raw = _stream.Read(64);
	value.Value = *reinterpret_cast<RawLongLong::ValueT*>(raw.data());
	return *this;
}

template <>
BitStreamReader& BitStreamReader::operator>>(RawDouble& value)
{
	auto raw = _stream.Read(64);
	value.Value = *reinterpret_cast<RawDouble::ValueT*>(raw.data());
	return *this;
}

}

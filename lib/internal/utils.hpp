#pragma once

namespace libopencad::internal::utils {

template <class T>
T swap_endianness(T value);

template <>
uint16_t swap_endianness(uint16_t value)
{
	return (value >> 8)
		| (value << 8);
}

template <>
uint32_t swap_endianness(uint32_t value)
{
	return (value >> 24)
		| ((value << 8) & 0x00FF0000)
		| ((value >> 8) & 0x0000FF00)
		| (value << 24);
}

template <>
uint64_t swap_endianness(uint64_t value)
{
	return (value >> 56)
		| ((value << 40) & 0x00FF000000000000)
		| ((value << 24) & 0x0000FF0000000000)
		| ((value << 8)  & 0x000000FF00000000)
		| ((value >> 8)  & 0x00000000FF000000)
		| ((value >> 24) & 0x0000000000FF0000)
		| ((value >> 40) & 0x000000000000FF00)
		| (value << 56);
}

}

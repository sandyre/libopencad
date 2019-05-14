#pragma once

#include <algorithm>

namespace libopencad::internal::utils {

template <typename Iterator>
void little_to_big(Iterator beg, Iterator end)
{
	std::reverse(beg, end);
}

template <typename Iterator>
void big_to_little(Iterator beg, Iterator end)
{
	std::reverse(beg, end);
}

template <typename Iterator>
void little_to_native(Iterator beg, Iterator end)
{
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)

	std::reverse(beg, end);

#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)

#endif
}

template <typename Iterator>
void native_to_little(Iterator beg, Iterator end)
{
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)

	std::reverse(beg, end);

#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)

#endif
}

template <typename Iterator>
void big_to_native(Iterator beg, Iterator end)
{
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)

#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)

	std::reverse(beg, end);

#endif
}

template <typename Iterator>
void native_to_big(Iterator beg, Iterator end)
{
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)

#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)

	std::reverse(beg, end);

#endif
}

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

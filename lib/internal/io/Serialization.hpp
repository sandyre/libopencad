/*******************************************************************************
 *  Project: libopencad
 *  Purpose: OpenSource CAD formats support library
 *  Author: Alexandr Borzykh, mush3d at gmail.com
 *  Language: C++
 *******************************************************************************
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2018 Alexandr Borzykh
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *******************************************************************************/
#ifndef LIBOPENCAD_INTERNAL_IO_SERIALIZATION_HPP
#define LIBOPENCAD_INTERNAL_IO_SERIALIZATION_HPP

#include "BitStream.hpp"

#include <cstdint>
#include <bitset>

namespace libopencad {
namespace internal
{

	template < typename Type >
	struct Raw
	{
		static Type Deserialize(const InputBitStreamPtr& stream)
		{ return *reinterpret_cast<Type*>(stream->Read<sizeof(Type) * 8>().data()); }
	};


	template < size_t Count >
	struct BitN
	{
		static uint8_t Deserialize(const InputBitStreamPtr& stream)
		{
			const uint8_t data = stream->Read<Count>().at(0);

			if (Count == 1)
				return data & 0b10000000 ? 1 : 0;
			else
				return data >> (8 - Count);
		}
	};


	template < typename T >
	struct Bit
	{ };


	template <>
	struct Bit<int16_t>
	{
		enum Bitcode
		{
			Normal       = 0x00,
			UnsignedChar = 0x01,
			Value0       = 0x02,
			Value256     = 0x03
		};

		static int16_t Deserialize(const InputBitStreamPtr& stream)
		{
			const uint8_t bitcode = BitN<2>::Deserialize(stream);

			switch (bitcode)
			{
			case Normal:		return Raw<int16_t>::Deserialize(stream);
			case UnsignedChar:	return Raw<uint8_t>::Deserialize(stream);
			case Value0:		return 0;
			case Value256:		return 256;
			}

			throw std::runtime_error("BitShort::Deserialize: undefined bitcode");
		}
	};


	template <>
	struct Bit<int32_t>
	{
		enum Bitcode
		{
			Normal       = 0x00,
			UnsignedChar = 0x01,
			Value0       = 0x02,
			Unused       = 0x03
		};

		static int32_t Deserialize(const InputBitStreamPtr& stream)
		{
			const uint8_t bitcode = BitN<2>::Deserialize(stream);

			switch (bitcode)
			{
			case Normal:		return Raw<int32_t>::Deserialize(stream);
			case UnsignedChar:	return Raw<uint8_t>::Deserialize(stream);
			case Unused:
			case Value0:		return 0;
			}

			throw std::runtime_error("BitShort::Deserialize: undefined bitcode");
		}
	};


	template <>
	struct Bit<double>
	{
		enum Bitcode
		{
			Normal = 0x00,
			Value1 = 0x01,
			Value0 = 0x02,
			Unused = 0x03,

			WDDefault  = 0x00,
			WDPatched4 = 0x01,
			WDPatched6 = 0x02,
			WDFull     = 0x03
		};

		static double Deserialize(const InputBitStreamPtr& stream, double defaultValue)
		{
			const uint8_t bitcode = BitN<2>::Deserialize(stream);
			double result = defaultValue;

			switch (bitcode)
			{
			case WDDefault:		return defaultValue;
			case WDFull:		return Raw<double>::Deserialize(stream);

			case WDPatched4:
			case WDPatched6:
				const size_t count = bitcode == WDPatched4 ? 4 : 6;

				for (size_t idx = 0; idx < count; ++idx)
					*(reinterpret_cast<char*>(&result) + idx) = Raw<uint8_t>::Deserialize(stream);

				return result;
			}

			throw std::runtime_error("BitDoubleWD::Deserialize: undefined bitcode");
		}

		static double Deserialize(const InputBitStreamPtr& stream)
		{
			const uint8_t bitcode = BitN<2>::Deserialize(stream);

			switch (bitcode)
			{
			case Normal:	return Raw<double>::Deserialize(stream);
			case Value1:	return 1.0;
			case Unused:
			case Value0:	return 0.0;
			}

			throw std::runtime_error("BitDouble::Deserialize: undefined bitcode");
		}
	};


	struct Tv
	{
		static std::string Deserialize(const InputBitStreamPtr& stream)
		{
			std::string result;

			for (size_t idx = 0; idx < Bit<int16_t>::Deserialize(stream); ++idx)
				result += Raw<int8_t>::Deserialize(stream);

			return result;
		}
	};

}}

#endif

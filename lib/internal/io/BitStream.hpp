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
#ifndef LIBOPENCAD_INTERNAL_IO_BITSTREAM_HPP
#define LIBOPENCAD_INTERNAL_IO_BITSTREAM_HPP

#include <cstdint>
#include <memory>
#include <vector>

namespace libopencad {
namespace internal
{

	using BitBuffer = std::vector<uint8_t>;
	using BitBufferPtr = std::shared_ptr<BitBuffer>;

	using ByteArray = std::vector<uint8_t>;
	using ByteArrayPtr = std::shared_ptr<ByteArray>;

	class InputBitStream
	{
	public:
		explicit InputBitStream(const BitBufferPtr& buffer)
			:	_currentOffset(),
				_buffer(buffer)
		{ }

		template < size_t Count >
		ByteArray Read()
		{
			const size_t byteOffset = _currentOffset / 8;
			const size_t bitOffset = _currentOffset % 8;

			const size_t resultSizeBits = bitOffset + Count;
			const size_t resultSizeBytes = (resultSizeBits / 8) + (resultSizeBits % 8 ? 1 : 0);

			if (_currentOffset >= _buffer->size() * 8)
				throw std::runtime_error("Buffer is empty, but read requested");

			ByteArray result(resultSizeBytes, 0);
			std::copy(_buffer->cbegin() + byteOffset, _buffer->cbegin() + byteOffset + resultSizeBytes, result.begin());

			switch (bitOffset)
			{
			case 0:
				break;

			default:
				if (result.size() == 1)
					result[0] <<= bitOffset;
				else
				{
					for (size_t idx = 0; idx < result.size() - 1; ++idx)
					{
						result[idx] <<= bitOffset;
						result[idx] |= (result[idx+1] >> (8 - bitOffset));
					}
				}
				break;
			}

			_currentOffset += Count;

			return result;
		}

	private:
		size_t			_currentOffset;
		BitBufferPtr	_buffer;
	};
	using InputBitStreamPtr = std::shared_ptr<InputBitStream>;

}}

#endif

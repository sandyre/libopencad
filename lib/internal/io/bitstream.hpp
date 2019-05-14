#pragma once

#include <array>
#include <bitset>
#include <climits>
#include <cstddef>

#include <iostream>

namespace libopencad::internal::io {

using BitBuffer = std::vector<bool>;

class InputBitStream
{
public:
	explicit InputBitStream(const BitBuffer& buffer)
		: _storage(buffer), _offset()
	{ }

	template <size_t Count>
	std::bitset<Count> Read()
	{
		assert((_offset + Count) <= _storage.size());

		std::bitset<Count> result;
		for (ssize_t i = Count-1; i >= 0; --i)
		{
			result[i] = _storage[_offset];
			_offset++;
		}

		return result;
	}

	template <size_t Count>
	std::array<std::byte, Count> ReadBytes()
	{
		assert((_offset + Count * CHAR_BIT) <= _storage.size());

		std::array<std::byte, Count> result;
		for (size_t i = 0; i < Count; ++i)
		{
			uint8_t byte = 0;
			for (ssize_t i = CHAR_BIT-1; i >= 0; --i)
			{
				byte |= (static_cast<uint8_t>(_storage[_offset++]) << i);
			}
			result[i] = std::byte{byte};
		}

		return result;
	}

private:
	BitBuffer	_storage;
	size_t		_offset;
};

}

#pragma once

#include <bitset>
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

		std::cout << "Requested: " << Count << " bits, got: " << result << " offset: " << _offset << " buffer length: " << _storage.size() << "\n";

		return result;
	}

private:
	BitBuffer	_storage;
	size_t		_offset;
};

}

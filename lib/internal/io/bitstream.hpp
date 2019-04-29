#pragma once

namespace libopencad::internal::io {

using BitBuffer = std::vector<std::byte>;

class InputBitStream
{
public:
	explicit InputBitStream(const BitBuffer& buffer, size_t offset = 0)
		: _storage(buffer), _offset(offset)
	{ }

	BitBuffer Read(size_t bit_count)
	{
		const size_t byte_offset = _offset / 8;
		const size_t bit_offset = _offset % 8;
		const size_t result_size_bits = bit_offset + bit_count;
		const size_t result_size_bytes = (result_size_bits / 8) + (result_size_bits % 8 ? 1 : 0);

		assert(_offset < (_storage.size() * 8));

		BitBuffer result(result_size_bytes, std::byte());
		std::copy(_storage.cbegin() + byte_offset, _storage.cbegin() + byte_offset + result_size_bytes, result.begin());

		switch (bit_offset)
		{
		case 0:
			break;
		default:
			if (result.size() == 1)
				result[0] <<= bit_offset;
			else
			{
				for (size_t idx = 0; idx < result.size() - 1; ++idx)
				{
					result[idx] <<= bit_offset;
					result[idx] |= (result[idx+1] >> (8 - bit_offset));
				}
			}
			break;
		}

		_offset += bit_count;
		return result;
	}

private:
	BitBuffer	_storage;
	size_t		_offset;
};

}

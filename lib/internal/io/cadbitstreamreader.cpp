#include "cadbitstreamreader.hpp"

#include <algorithm>
#include <cstring>


namespace libopencad
{

    CADBitStreamReader::CADBitStreamReader(const CADBitBuffer& buffer, size_t initialOffset)
        : _buffer(buffer),
          _offset(initialOffset)
    { }


    int32_t CADBitStreamReader::ReadRawLong()
    {
        int32_t result = 0;
        ByteArray longBytes = ReadBitsImpl(32);

        std::memcpy(&result, longBytes.data(), sizeof(result));

        return result;
    }


    int16_t CADBitStreamReader::ReadRawShort()
    {
        int16_t result = 0;
        ByteArray shortBytes = ReadBitsImpl(16);

        std::memcpy(&result, shortBytes.data(), sizeof(result));

        return result;
    }


    double CADBitStreamReader::ReadRawDouble()
    {
        double result = 0;
        ByteArray doubleBytes = ReadBitsImpl(64);

        std::memcpy(&result, doubleBytes.data(), sizeof(result));

        return result;
    }


    uint8_t CADBitStreamReader::ReadChar()
    {
        return ReadBitsImpl(8)[0];
    }


    std::string CADBitStreamReader::ReadTv()
    {
        int16_t stringLength = ReadBitShort();

        std::string result;

        for (int16_t idx = 0; idx < stringLength; ++idx)
            result += ReadChar();
    }


    bool CADBitStreamReader::ReadBit()
    {
        return (ReadBitsImpl(1)[0] >> 7) & binary(00000001);
    }


    uint8_t CADBitStreamReader::Read2Bits()
    {
        return (ReadBitsImpl(2)[0] >> 6) & binary(00000011);
    }


    uint8_t CADBitStreamReader::Read3Bits()
    {
        return (ReadBitsImpl(3)[0] >> 5) & binary(00000111);
    }


    int16_t CADBitStreamReader::ReadBitShort()
    {
        uint8_t bitcode = Read2Bits();

        size_t byteOffset = _offset / 8;
        size_t bitOffset = _offset % 8;

        switch (bitcode)
        {
        case BITSHORT_NORMAL:
        {
            ByteArray shortBytes = ReadBitsImpl(16);

            int16_t result = 0;
            std::memcpy(&result, shortBytes.data(), sizeof(result));

            return result;
        }

        case BITSHORT_UNSIGNED_CHAR:
        {
            uint8_t result = ReadBitsImpl(8)[0];
            return result;
        }

        case BITSHORT_ZERO_VALUE:
        {
            return 0;
        }

        case BITSHORT_256:
        {
            return 256;
        }
        }

        return -1;
    }


    CADHandle CADBitStreamReader::ReadHandle()
    {
        CADHandle result(Read4Bits());
        uint8_t counter = Read4Bits();

        for (uint8_t idx = 0; idx < counter; ++idx)
            result.addOffset(ReadChar());

        return result;
    }


    void CADBitStreamReader::SeekHandle()
    {
        Read4Bits();
        uint8_t counter = Read4Bits();
        _offset += counter * 8;
    }


    CADHandle CADBitStreamReader::ReadHandle8BitsLength()
    {
        CADHandle result;
        uint8_t counter = ReadChar();

        for (uint8_t idx = 0; idx < counter; ++idx)
            result.addOffset(ReadChar());

        return result;
    }


    int32_t CADBitStreamReader::ReadBitLong()
    {
        uint8_t bitcode = Read2Bits();

        size_t byteOffset = _offset / 8;
        size_t bitOffset = _offset % 8;

        switch (bitcode)
        {
        case BITLONG_NORMAL:
        {
            ByteArray shortBytes = ReadBitsImpl(32);

            int32_t result = 0;
            std::memcpy(&result, shortBytes.data(), sizeof(result));

            return result;
        }

        case BITLONG_UNSIGNED_CHAR:
        {
            uint8_t result = ReadBitsImpl(8)[0];
            return result;
        }

        case BITLONG_ZERO_VALUE:
        {
            return 0;
        }

        case BITLONG_NOT_USED:
        {
            return 0;
        }
        }

        return -1;
    }


    void CADBitStreamReader::SeekBitLong()
    {
        ReadBitLong();
    }


    void CADBitStreamReader::SeekBitShort()
    {
        ReadBitShort();
    }


    void CADBitStreamReader::SeekBits(size_t bitsCount)
    {
        _offset += bitsCount;
    }


    double CADBitStreamReader::ReadBitDouble()
    {
        uint8_t bitcode = Read2Bits();

        size_t byteOffset = _offset / 8;
        size_t bitOffset = _offset % 8;

        switch (bitcode)
        {
        case BITDOUBLE_NORMAL:
        {
            ByteArray shortBytes = ReadBitsImpl(64);

            double result = 0.0;
            std::memcpy(&result, shortBytes.data(), sizeof(result));

            return result;
        }

        case BITDOUBLE_ONE_VALUE:
        {
            return 1.0;
        }

        case BITDOUBLE_ZERO_VALUE:
        {
            return 0.0;
        }

        case BITDOUBLE_NOT_USED:
        {
            return 0.0;
        }
        }

        return -1.0;
    }


    double CADBitStreamReader::ReadBitDoubleWd(double defaultValue)
    {
        uint8_t bitcode = Read2Bits();

        size_t byteOffset = _offset / 8;
        size_t bitOffset = _offset % 8;

        switch (bitcode)
        {
        case BITDOUBLEWD_DEFAULT_VALUE:
        {
            return defaultValue;
        }

        case BITDOUBLEWD_4BYTES_PATCHED:
        {
            for (size_t idx = 0; idx < 4; ++idx)
                *(static_cast<char*>(&defaultValue) + idx) = ReadChar();

            return defaultValue;
        }

        case BITDOUBLEWD_6BYTES_PATCHED:
        {
            for (size_t idx = 0; idx < 6; ++idx)
                *(static_cast<char*>(&defaultValue) + idx) = ReadChar();

            return defaultValue;
        }

        case BITDOUBLE_NOT_USED:
        {
            double result = 0.0;
            ByteArray doubleBytes = ReadBitsImpl(64);

            std::memcpy(&result, doubleBytes.data(), sizeof(result));
        }
        }

        return -1.0;
    }


    int32_t CADBitStreamReader::ReadUMChar()
    {
        int64_t result = 0;
        size_t byteOffset = _offset / 8;
        size_t bitOffset = _offset % 8;

        ByteArray mcharBytes;
        for (size_t idx = 0; idx < 8; ++idx)
        {
            mcharBytes.push_back(ReadChar());

            if (!(mcharBytes.back() & binary(10000000)))
                break;
        }

        std::reverse(mcharBytes.begin(), mcharBytes.end());

        for (size_t idx = 0; idx < mcharBytes.size(); ++idx)
            mcharBytes[idx] &= binary(01111111);

        switch (mcharBytes.size())
        {
        case 1:
            break;

        case 2:
        {
            char tmp = mcharBytes[0] & binary(00000001);
            mcharBytes[0] = mcharBytes[0] >> 1;
            mcharBytes[1] |= ( tmp << 7 );
            break;
        }

        case 3:
        {
            unsigned char tmp1 = mcharBytes[0] & binary(00000011);
            unsigned char tmp2 = mcharBytes[1] & binary(00000001);
            mcharBytes[0] = mcharBytes[0] >> 2;
            mcharBytes[1] = mcharBytes[1] >> 1;
            mcharBytes[1] |= ( tmp1 << 6 );
            mcharBytes[2] |= ( tmp2 << 7 );
            break;
        }

        case 4:
        {
            unsigned char tmp1 = mcharBytes[0] & binary(00000111);
            unsigned char tmp2 = mcharBytes[1] & binary(00000011);
            unsigned char tmp3 = mcharBytes[2] & binary(00000001);
            mcharBytes[0] = mcharBytes[0] >> 3;
            mcharBytes[1] = mcharBytes[1] >> 2;
            mcharBytes[2] = mcharBytes[2] >> 1;
            mcharBytes[1] |= ( tmp1 << 5 );
            mcharBytes[2] |= ( tmp2 << 6 );
            mcharBytes[3] |= ( tmp3 << 7 );
            break;
        }
        }

        std::reverse(mcharBytes.begin(), mcharBytes.end());
        std::memcpy(&result, mcharBytes.data(), sizeof(result));

        return result;
    }


    ByteArray CADBitStreamReader::ReadBitsImpl(size_t bitsCount)
    {
        size_t byteOffset = _offset / 8;
        size_t bitOffset = _offset % 8;

        size_t resultSizeBits = bitOffset + bitsCount;
        size_t resultSizeBytes = (resultSizeBits / 8) + (resultSizeBits % 8 ? 1 : 0);

        ByteArray result(resultSizeBytes, 0);

        std::copy(_buffer.cbegin() + byteOffset, _buffer.cbegin() + byteOffset + resultSizeBytes, result.begin());

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

        _offset += bitsCount;

        return result;
    }

}

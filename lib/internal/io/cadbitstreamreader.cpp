#include "cadbitstreamreader.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>

namespace libopencad
{

    CADBitStreamReader::CADBitStreamReader(const CADBitBuffer& buffer, size_t initialOffset)
        : _buffer(buffer),
          _offset(initialOffset)
    { }


    bool CADBitStreamReader::Available() const
    {
        return _offset / 8 + 1 < _buffer.size();
    }


    size_t CADBitStreamReader::GetOffset() const
    {
        return _offset;
    }


    int32_t CADBitStreamReader::ReadRawLong()
    {
        return *reinterpret_cast<int32_t*>(ReadBitsImpl(32).data());
    }


    int16_t CADBitStreamReader::ReadRawShort()
    {
        return *reinterpret_cast<int16_t*>(ReadBitsImpl(16).data());
    }


    double CADBitStreamReader::ReadRawDouble()
    {
        return *reinterpret_cast<double*>(ReadBitsImpl(64).data());
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
        return ReadBitsImpl(1)[0] & binary(10000000);
    }


    uint8_t CADBitStreamReader::Read2Bits()
    {
        return (ReadBitsImpl(2)[0] >> 6) & binary(00000011);
    }


    uint8_t CADBitStreamReader::Read3Bits()
    {
        return (ReadBitsImpl(3)[0] >> 5) & binary(00000111);
    }


    uint8_t CADBitStreamReader::Read4Bits()
    {
        return (ReadBitsImpl(4)[0] >> 4) & binary(00001111);
    }


    int16_t CADBitStreamReader::ReadBitShort()
    {
        uint8_t bitcode = Read2Bits();

        size_t byteOffset = _offset / 8;
        size_t bitOffset = _offset % 8;

        switch (bitcode)
        {
        case BITSHORT_NORMAL:
            return *reinterpret_cast<int16_t*>(ReadBitsImpl(16).data());

        case BITSHORT_UNSIGNED_CHAR:
            return ReadBitsImpl(8)[0];

        case BITSHORT_ZERO_VALUE:
            return 0;

        case BITSHORT_256:
            return 256;
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
            return *reinterpret_cast<int32_t*>(ReadBitsImpl(32).data());

        case BITLONG_UNSIGNED_CHAR:
            return ReadBitsImpl(8)[0];

        case BITLONG_ZERO_VALUE:
        case BITLONG_NOT_USED:
            return 0;
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


    void CADBitStreamReader::SeekBitDouble()
    {
        ReadBitDouble();
    }


    void CADBitStreamReader::SeekTv()
    {
        ReadTv();
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
            return ReadRawDouble();

        case BITDOUBLE_ONE_VALUE:
            return 1.0;

        case BITDOUBLE_ZERO_VALUE:
        case BITDOUBLE_NOT_USED:
            return 0.0;
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
                *(reinterpret_cast<char*>(&defaultValue) + idx) = ReadChar();

            return defaultValue;
        }

        case BITDOUBLEWD_6BYTES_PATCHED:
        {
            for (size_t idx = 0; idx < 6; ++idx)
                *(reinterpret_cast<char*>(&defaultValue) + idx) = ReadChar();

            return defaultValue;
        }

        case BITDOUBLEWD_FULL_RD:
        {
            return ReadRawDouble();
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
        std::memcpy(&result, mcharBytes.data(), mcharBytes.size());

        return result;
    }


    uint32_t CADBitStreamReader::ReadMShort()
    {
        int32_t result = 0;
        size_t byteOffset = _offset / 8;
        size_t bitOffset = _offset % 8;

        ByteArray mshortBytes;
        mshortBytes.push_back(ReadChar());
        mshortBytes.push_back(ReadChar());

        if (mshortBytes[1] & binary(10000000))
        {
            mshortBytes.push_back(ReadChar());
            mshortBytes.push_back(ReadChar());
        }

        std::reverse(mshortBytes.begin(), mshortBytes.end());

        if (mshortBytes.size() == 2)
        {
            mshortBytes[0] &= binary(01111111);
        }
        else if (mshortBytes.size() == 4)
        {
            mshortBytes[0] &= binary(01111111);
            mshortBytes[2] &= binary(01111111);

            mshortBytes[2] |= (mshortBytes[1] << 7);
            mshortBytes[1] = (mshortBytes[1] >> 1);
            mshortBytes[1] |= (mshortBytes[0] << 7);
            mshortBytes[0] = (mshortBytes[0] >> 1);
        }

        std::reverse(mshortBytes.begin(), mshortBytes.end());
        std::memcpy(&result, mshortBytes.data(), mshortBytes.size());

        return result;
    }


    CADVector CADBitStreamReader::ReadVector()
    {
        return CADVector(ReadBitDouble(), ReadBitDouble(), ReadBitDouble());
    }


    CADVector CADBitStreamReader::ReadRawVector()
    {
        return CADVector(ReadRawDouble(), ReadRawDouble());
    }


    ByteArray CADBitStreamReader::ReadBitsImpl(size_t bitsCount)
    {
        size_t byteOffset = _offset / 8;
        size_t bitOffset = _offset % 8;

        size_t resultSizeBits = bitOffset + bitsCount;
        size_t resultSizeBytes = (resultSizeBits / 8) + (resultSizeBits % 8 ? 1 : 0);

        ByteArray result(resultSizeBytes, 0);

        if (byteOffset + resultSizeBytes >= _buffer.size())
            throw std::runtime_error("Buffer is empty, but read requested");

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

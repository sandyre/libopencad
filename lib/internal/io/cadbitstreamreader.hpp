/*******************************************************************************
 *  Project: libopencad
 *  Purpose: OpenSource CAD formats support library
 *  Author: Alexandr Borzykh, mush3d at gmail.com
 *  Language: C++
 *******************************************************************************
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2017 Alexandr Borzykh
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
#ifndef LIBOPENCAD_INTERNAL_IO_CADBITSTREAMREADER_HPP
#define LIBOPENCAD_INTERNAL_IO_CADBITSTREAMREADER_HPP

#include "../../cadobjects.h"
#include "../toolkit.hpp"

#include <cstdint>
#include <vector>

using ByteArray = std::vector<uint8_t>;
using CADBitBuffer = std::vector<uint8_t>;


namespace libopencad
{

    class CADBitStreamReader
    {
    public:
        explicit CADBitStreamReader(const CADBitBuffer& buffer, size_t initialOffset = 0);

        bool Available() const;
        size_t GetOffset() const;

        double  ReadRawDouble();
        int64_t ReadRawLongLong();
        int32_t ReadRawLong();
        int16_t ReadRawShort();
        uint8_t ReadChar();
        uint8_t Read4Bits();
        uint8_t Read3Bits();
        uint8_t Read2Bits();
        bool    ReadBit();

        double  ReadBitDoubleWd();
        double  ReadBitDouble();
        int32_t ReadBitLong();
        int16_t ReadBitShort();

        int32_t ReadMChar();
        int32_t ReadUMChar();
        uint16_t ReadMShort();
        
        std::string ReadTv();

        CADHandle ReadHandle();
        CADHandle ReadHandle8BitsLength();

        CADVector ReadVector();
        CADVector ReadRawVector();

        void SeekBitDouble();
        void SeekBitLong();
        void SeekBitShort();
        void SeekBits(size_t bitsCount);
        void SeekBit();

        void SeekTv();
        void SeekHandle();

    private:
        ByteArray ReadBitsImpl(size_t bitsCount);

    private:
        enum Bitcodes
        {
            BITSHORT_NORMAL            = 0,
            BITSHORT_UNSIGNED_CHAR     = 1,
            BITSHORT_ZERO_VALUE        = 2,
            BITSHORT_256               = 3,

            BITLONG_NORMAL             = 0,
            BITLONG_UNSIGNED_CHAR      = 1,
            BITLONG_ZERO_VALUE         = 2,
            BITLONG_NOT_USED           = 3,

            BITDOUBLE_NORMAL           = 0,
            BITDOUBLE_ONE_VALUE        = 1,
            BITDOUBLE_ZERO_VALUE       = 2,
            BITDOUBLE_NOT_USED         = 3,

            BITDOUBLEWD_DEFAULT_VALUE  = 0,
            BITDOUBLEWD_4BYTES_PATCHED = 1,
            BITDOUBLEWD_6BYTES_PATCHED = 2,
            BITDOUBLEWD_FULL_RD        = 3
        };

    private:
        CADBitBuffer    _buffer;
        size_t          _offset;
    };

}

#endif

/************************************************************************************
 *  Name: dwg_io.h
 *  Project: libOpenCAD OpenSource CAD formats support library
 *  Author: Alexandr Borzykh, mush3d at gmail.com
 *  Language: C++
 ************************************************************************************
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2016 Alexandr Borzykh
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
 ************************************************************************************/

#ifndef LIB_DWG_IO_H
#define LIB_DWG_IO_H

#include "dwg_data_structs.h"

#include "stdint.h"
#include <string>

namespace libopencad
{
namespace dwg
{
namespace io
{

// TODO: probably it would be better to have no dependencies on <algorithm>.
template<typename T, typename S>
void SwapEndianness ( T &object, S &&size )
{
    std::reverse (( char * ) & object, ( char * ) & object + size);
}

//int16_t     CalculateCRC8 ( uint16_t initial_val, const char * ptr, int32_t num );

int64_t     ReadRAWLONGLONG ( const char * input_array, size_t& bitOffsetFromStart );
int32_t     ReadRAWLONG ( const char * input_array, size_t& bitOffsetFromStart );
int16_t     ReadRAWSHORT ( const char * input_array, size_t& bitOffsetFromStart );
double      ReadRAWDOUBLE ( const char * input_array, size_t& bitOffsetFromStart );
uint8_t     Read2B ( const char * input_array, size_t& bitOffsetFromStart );
uint8_t     Read3B ( const char * input_array, size_t& bitOffsetFromStart );
uint8_t     Read4B ( const char * input_array, size_t& bitOffsetFromStart );
DWG_HANDLE  ReadHANDLE ( const char * input_array, size_t& bitOffsetFromStart );

bool        ReadBIT ( const char * input_array, size_t& bitOffsetFromStart );
uint8_t     ReadCHAR ( const char * input_array, size_t& bitOffsetFromStart );
int16_t     ReadBITSHORT ( const char * input_array, size_t& bitOffsetFromStart );
int32_t     ReadBITLONG ( const char * input_array, size_t& bitOffsetFromStart );
double      ReadBITDOUBLE ( const char * input_array, size_t& bitOffsetFromStart );
double      ReadBITDOUBLEWD ( const char * input_array, size_t& bitOffsetFromStart, double defaultvalue );
int64_t     ReadMCHAR ( const char * input_array, size_t& bitOffsetFromStart );
uint32_t    ReadMSHORT ( const char * input_array, size_t& bitOffsetFromStart );
std::string ReadTV ( const char * input_array, size_t& bitOffsetFromStart );

}
}
}

#endif //LIB_DWG_IO_H

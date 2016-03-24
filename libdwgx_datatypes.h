#ifndef __LIBGDWX_DATATYPES_H__
#define __LIBGDWX_DATATYPES_H__

#define BITSHORT_NORMAL         0x0
#define BITSHORT_UNSIGNED_CHAR  0x1
#define BITSHORT_ZERO_VALUE     0x2
#define BITSHORT_256            0x3

#define BITLONG_NORMAL          0x0
#define BITLONG_UNSIGNED_CHAR   0x1
#define BITLONG_ZERO_VALUE      0x2
#define BITLONG_NOT_USED        0x3

#define BITDOUBLE_NORMAL        0x0
#define BITDOUBLE_ONE_VALUE     0x1
#define BITDOUBLE_ZERO_VALUE    0x2
#define BITDOUBLE_NOT_USED      0x3

#include <fstream>
#include <vector>
#include <algorithm>

int64_t ReadModularChar ( std::ifstream * input )
{
    bool isNegative = false;
    char cBuf = 0;
    std::vector < char > buffer;
    int64_t result = 0;

    while ( true )
    {
        input->read ( &cBuf, 1 );
        printf ( "byte readed" );
        buffer.push_back ( cBuf );
        if( ! ( ( cBuf >> 7 ) & 0 ) )
            break;
    }

    // Change endianness.
    std::reverse( buffer.begin(), buffer.end() );

    // Check if its negative and drop this bit.
    if ( ( buffer[0] >> 6 ) & 1 )
    {
        isNegative = true;
        buffer[0] &= 0b10111111;
    }

    // First bit set to 0, its no longer used.
    for ( size_t i = 0; i < buffer.size(); ++i )
    {
        buffer[i] &= 0b01111111;
    }

    for ( size_t i = 0; i < buffer.size() - 1; ++i )
    {
        buffer[i] |= ( buffer[i + 1] & 0b00000001 );
        buffer[i + 1] = buffer[i + 1] >> 1;
    }

    if ( isNegative )
    {
        buffer[0] |= 0b10000000;
    }

    std::reverse( buffer.begin(), buffer.end() );
    memcpy ( &result, buffer.data(), buffer.size() );

    return result;
}

#endif

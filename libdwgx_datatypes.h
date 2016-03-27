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

#include "libdwgx.h"

#include <fstream>
#include <vector>
#include <algorithm>

bool ReadBIT ( const char * input_array, size_t& bitOffsetFromStart )
{
    size_t byteOffset      = bitOffsetFromStart / 8;
    size_t bitOffsetInByte = bitOffsetFromStart % 8;

    const char * pBoolByte = input_array + byteOffset;

    char result = ( pBoolByte[0] >> ( 7 - bitOffsetInByte ) ) & 0b00000001;

    ++bitOffsetFromStart;

    return result;
}

short ReadBITSHORT ( const char * input_array, size_t& bitOffsetFromStart )
{
    size_t byteOffset      = bitOffsetFromStart / 8;
    size_t bitOffsetInByte = bitOffsetFromStart % 8;

    const char * pShortFirstByte = input_array + byteOffset;
    char aShortBytes[3]; // maximum bytes a single short can take.
    memcpy ( aShortBytes, pShortFirstByte, 3 );

    char BITCODE = ( ( aShortBytes[0] >> ( 6 - bitOffsetInByte ) ) & 0b00000011 );

    switch( BITCODE )
    {
        case BITSHORT_NORMAL:
        {
            aShortBytes[0]  = ( aShortBytes[0] << ( bitOffsetInByte + 2 ) );
            aShortBytes[0] |= ( aShortBytes[1] >> ( 6 - bitOffsetInByte ) );
            aShortBytes[1]  = ( aShortBytes[1] << ( bitOffsetInByte + 2 ) );
            aShortBytes[1] |= ( aShortBytes[2] >> ( 6 - bitOffsetInByte ) );

            bitOffsetFromStart += 18;

            void * ptr = aShortBytes;
            short * result = static_cast< short *> ( ptr );

            return * result;
        }

        case BITSHORT_UNSIGNED_CHAR:
        {
            aShortBytes[0]  = ( aShortBytes[0] << ( bitOffsetInByte + 2 ) );
            aShortBytes[0] |= ( aShortBytes[1] >> ( 6 - bitOffsetInByte ) );

            bitOffsetFromStart += 10;

            return ( unsigned char ) aShortBytes[0];
        }

        case BITSHORT_ZERO_VALUE:
        {
            bitOffsetFromStart += 2;
            return ( short ) 0;
        }

        case BITSHORT_256:
        {
            bitOffsetFromStart += 2;
            return ( short ) 256;
        }
    }

    return -1;
}

char ReadCHAR ( const char * input_array, size_t& bitOffsetFromStart )
{
    unsigned char result = 0;
    size_t byteOffset      = bitOffsetFromStart / 8;
    size_t bitOffsetInByte = bitOffsetFromStart % 8;

    const char * pCharFirstByte = input_array + byteOffset;
    unsigned char aCharBytes[2]; // maximum bytes a single char can take.
    memcpy ( aCharBytes, pCharFirstByte, 2 );

    result  = ( aCharBytes[0] << bitOffsetInByte );
    result |= ( aCharBytes[1] >> ( 8 - bitOffsetInByte ) );

    bitOffsetFromStart += 8;

    return result;
}

std::string ReadTV ( const char * input_array, size_t& bitOffsetFromStart )
{
    short string_length = ReadBITSHORT ( input_array, bitOffsetFromStart );
    std::string result;

    for ( size_t i = 0; i < string_length; ++i )
    {
        result += ReadCHAR ( input_array, bitOffsetFromStart );
    }

    return result;
}

long long ReadMCHAR ( const char * input_array, size_t& bitOffsetFromStart )
{
    long long result = 0;
    bool   negative = false;
    size_t byteOffset      = bitOffsetFromStart / 8;
    size_t bitOffsetInByte = bitOffsetFromStart % 8;

    const char * pMCharFirstByte = input_array + byteOffset;

    size_t MCharBytesCount = 1;
    for ( size_t i = 0; i < 8; ++i )
    {
        if ( !( pMCharFirstByte[i] & 0b10000000 ) )
            break;
        ++MCharBytesCount;
    }

    char aMCharBytes[MCharBytesCount]; // 8 bytes is maximum.
    memcpy ( aMCharBytes, pMCharFirstByte, MCharBytesCount );

    std::reverse ( aMCharBytes, aMCharBytes + MCharBytesCount ); // LSB to MSB

    if ( aMCharBytes[MCharBytesCount - 1] & 0b01000000 )
    {
        aMCharBytes[MCharBytesCount - 1] &= 0b10111111;
        negative = true;
    }

    for ( size_t i = 0; i < MCharBytesCount; ++i )
    {
        aMCharBytes[i] &= 0b01111111;
    }

    for ( size_t i = 0; i < MCharBytesCount - 1; ++i )
    {
        aMCharBytes[i]    |= ( aMCharBytes[i + 1] & 0b00000001 );
        aMCharBytes[i + 1] = ( aMCharBytes[i + 1] >> 1 );
    }

    memcpy ( &result, aMCharBytes, MCharBytesCount );

    if ( negative ) result *= -1;

    bitOffsetFromStart += MCharBytesCount * 8;

    return result;
}

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

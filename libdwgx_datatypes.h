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

bool ReadBIT ( const char * input_array, size_t& bitOffsetFromStart );
short ReadSHORT ( const char * input_array, size_t& bitOffsetFromStart );
char ReadCHAR ( const char * input_array, size_t& bitOffsetFromStart );
double ReadBITDOUBLE ( const char * input_array, size_t& bitOffsetFromStart );
long long ReadMCHAR ( const char * input_array, size_t& bitOffsetFromStart );
std::string ReadTV ( const char * input_array, size_t& bitOffsetFromStart );

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

    // std::cout << "CONTENT: " << std::bitset<8>(aShortBytes[0]) << " "
    //                          << std::bitset<8>(aShortBytes[1]) << " "
    //                          << std::bitset<8>(aShortBytes[2]) << " "
    //                          << " BIT OFFSET: " << bitOffsetInByte << std::endl;

    char BITCODE = ( ( aShortBytes[0] >> ( 6 - bitOffsetInByte ) ) & 0b00000011 );

    switch( BITCODE )
    {
        case BITSHORT_NORMAL:
        {
            // std::cout << "[ NORMAL CASE ]" << std::endl;
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
            // std::cout << "[ UNSIGNED CASE ]" << std::endl;
            aShortBytes[0]  = ( aShortBytes[0] << ( bitOffsetInByte + 2 ) );
            aShortBytes[0] |= ( aShortBytes[1] >> ( 6 - bitOffsetInByte ) );

            bitOffsetFromStart += 10;

            return ( unsigned char ) aShortBytes[0];
        }

        case BITSHORT_ZERO_VALUE:
        {
            // std::cout << "[ ZERO CASE ]" << std::endl;
            bitOffsetFromStart += 2;
            return ( short ) 0;
        }

        case BITSHORT_256:
        {
            // std::cout << "[ 256 CASE ]" << std::endl;
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
    // std::cout << "STRING LENGTH: "<< string_length << std::endl;
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

    std::cout << std::bitset<8>(pMCharFirstByte[0]) << " "
              << std::bitset<8>(pMCharFirstByte[1]) << " "
              << std::bitset<8>(pMCharFirstByte[2]) << " "
              << std::bitset<8>(pMCharFirstByte[3]) << " "
              << std::bitset<8>(pMCharFirstByte[4]) << std::endl;

    size_t MCharBytesCount = 1;
    for ( size_t i = 0; i < 8; ++i )
    {
        if ( !( pMCharFirstByte[i] & 0b10000000 ) )
            break;
        ++MCharBytesCount;
    }

    std::cout << "BYTES COUNT: " << MCharBytesCount << std::endl;

    char aMCharBytes[MCharBytesCount]; // 8 bytes is maximum.
    memcpy ( aMCharBytes, pMCharFirstByte, MCharBytesCount );

    std::reverse ( aMCharBytes, aMCharBytes + MCharBytesCount ); // LSB to MSB

    if ( aMCharBytes[0] & 0b01000000 )
    {
        std::cout << "NEGATIVE MCHAR" << std::endl;
        aMCharBytes[0] &= 0b10111111;
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

    std::reverse ( aMCharBytes, aMCharBytes + MCharBytesCount ); // MSB to LSB

    memcpy ( &result, aMCharBytes, MCharBytesCount );

    if ( negative ) result *= -1;

    bitOffsetFromStart += MCharBytesCount * 8;

    return result;
}

double ReadBITDOUBLE ( const char * input_array, size_t& bitOffsetFromStart )
{
    size_t byteOffset      = bitOffsetFromStart / 8;
    size_t bitOffsetInByte = bitOffsetFromStart % 8;

    const char * pDoubleFirstByte = input_array + byteOffset;
    char aDoubleBytes[9]; // maximum bytes a single double can take.
    memcpy ( aDoubleBytes, pDoubleFirstByte, 9 );

    char BITCODE = ( ( aDoubleBytes[0] >> ( 6 - bitOffsetInByte ) ) & 0b00000011 );

    switch ( BITCODE )
    {
        case BITDOUBLE_NORMAL:
        {
            aDoubleBytes[0]  = ( aDoubleBytes[0] << ( bitOffsetInByte + 2 ) );
            aDoubleBytes[0] |= ( aDoubleBytes[1] >> ( 6 - bitOffsetInByte ) );
            aDoubleBytes[1]  = ( aDoubleBytes[1] << ( bitOffsetInByte + 2 ) );
            aDoubleBytes[1] |= ( aDoubleBytes[2] >> ( 6 - bitOffsetInByte ) );
            aDoubleBytes[2]  = ( aDoubleBytes[2] << ( bitOffsetInByte + 2 ) );
            aDoubleBytes[2] |= ( aDoubleBytes[3] >> ( 6 - bitOffsetInByte ) );
            aDoubleBytes[3]  = ( aDoubleBytes[3] << ( bitOffsetInByte + 2 ) );
            aDoubleBytes[3] |= ( aDoubleBytes[4] >> ( 6 - bitOffsetInByte ) );
            aDoubleBytes[4]  = ( aDoubleBytes[4] << ( bitOffsetInByte + 2 ) );
            aDoubleBytes[4] |= ( aDoubleBytes[5] >> ( 6 - bitOffsetInByte ) );
            aDoubleBytes[5]  = ( aDoubleBytes[5] << ( bitOffsetInByte + 2 ) );
            aDoubleBytes[5] |= ( aDoubleBytes[6] >> ( 6 - bitOffsetInByte ) );
            aDoubleBytes[6]  = ( aDoubleBytes[6] << ( bitOffsetInByte + 2 ) );
            aDoubleBytes[6] |= ( aDoubleBytes[7] >> ( 6 - bitOffsetInByte ) );
            aDoubleBytes[7]  = ( aDoubleBytes[7] << ( bitOffsetInByte + 2 ) );
            aDoubleBytes[7] |= ( aDoubleBytes[8] >> ( 6 - bitOffsetInByte ) );

            bitOffsetFromStart += 66;

            void * ptr = aDoubleBytes;
            double * result = static_cast< double *> ( ptr );

            return * result;
        }

        case BITDOUBLE_ONE_VALUE:
        {
            bitOffsetFromStart += 2;

            return 1.0f;
        }

        case BITDOUBLE_ZERO_VALUE:
        {
            bitOffsetFromStart += 2;

            return 0.0f;
        }

        case BITDOUBLE_NOT_USED:
        {
            bitOffsetFromStart += 2;

            return 0.0f;
        }
    }

    return 0.0f;
}

#endif

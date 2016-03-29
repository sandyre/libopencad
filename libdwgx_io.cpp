#include "libdwgx_io.h"

#include <fstream>
#include <vector>

#include "libdwgx_datatypes.h"

char Read3B ( const char * input_array, size_t& bitOffsetFromStart )
{
    char result = 0;
    size_t byteOffset      = bitOffsetFromStart / 8;
    size_t bitOffsetInByte = bitOffsetFromStart % 8;

    const char * p3BByte = input_array + byteOffset;
    unsigned char a3BBytes[2];
    memcpy ( a3BBytes, p3BByte, 2 );

    switch ( bitOffsetInByte )
    {
        case 6:
            result  = ( a3BBytes[0] & 0b00000011 ) << 1;
            result |= ( a3BBytes[1] & 0b10000000 ) >> 7;
            break;

        case 7:
            result  = ( a3BBytes[0] & 0b00000001 ) << 2;
            result |= ( a3BBytes[1] & 0b11000000 ) >> 6;
            break;

        default:
            result  = ( a3BBytes[0] >> ( 5 - bitOffsetInByte ) );
            result &= 0b00000111;
            break;
    }

    bitOffsetFromStart += 3;

    return result;
}

short ReadRAWSHORT ( const char * input_array, size_t& bitOffsetFromStart )
{
    short  result = 0;
    size_t byteOffset      = bitOffsetFromStart / 8;
    size_t bitOffsetInByte = bitOffsetFromStart % 8;

    const char * pShortFirstByte = input_array + byteOffset;
    unsigned char aShortBytes[3];
    memcpy ( aShortBytes, pShortFirstByte, 3 );

    switch ( bitOffsetInByte )
    {
        case 0:
            break;

        default:
            aShortBytes[0]  = ( aShortBytes[0] << bitOffsetInByte );
            aShortBytes[0] |= ( aShortBytes[1] >> ( 8 - bitOffsetInByte ) );
            aShortBytes[1]  = ( aShortBytes[1] >> bitOffsetInByte );
            aShortBytes[1] |= ( aShortBytes[2] >> ( 8 - bitOffsetInByte ) );
            break;
    }

    memcpy ( &result, aShortBytes, 2 );

    return result;
}

int ReadRAWLONG ( const char * input_array, size_t& bitOffsetFromStart )
{
    int result = 0;
    size_t byteOffset = bitOffsetFromStart / 8;
    size_t bitOffsetInByte = bitOffsetFromStart % 8;

    const char * pLongFirstByte = input_array + byteOffset;

    char aLongBytes[5];
    memcpy ( aLongBytes, pLongFirstByte, 5 );

    switch ( bitOffsetInByte )
    {
        case 0:
            break;

        default:
            aLongBytes[0]  = ( aLongBytes[0] << bitOffsetInByte );
            aLongBytes[0] |= ( aLongBytes[1] >> ( 8 - bitOffsetInByte ) );
            aLongBytes[1]  = ( aLongBytes[1] >> bitOffsetInByte );
            aLongBytes[1] |= ( aLongBytes[2] >> ( 8 - bitOffsetInByte ) );
            aLongBytes[2]  = ( aLongBytes[2] << bitOffsetInByte );
            aLongBytes[2] |= ( aLongBytes[3] >> ( 8 - bitOffsetInByte ) );
            aLongBytes[3]  = ( aLongBytes[3] >> bitOffsetInByte );
            aLongBytes[3] |= ( aLongBytes[4] >> ( 8 - bitOffsetInByte ) );
            break;
    }

    memcpy ( &result, aLongBytes, 4 );

    return result;
}

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
    unsigned char aShortBytes[4]; // maximum bytes a single short can take.
    memcpy ( aShortBytes, pShortFirstByte, 4 );

    // Exclusive case. It cant be processed by basic algorithm.
    if ( bitOffsetInByte == 7 )
    {
        char BITCODE = ( ( aShortBytes[0] & 0b00000001 ) << 1 ) | ( ( aShortBytes[1] >> 7 ) & 0b00000001 );

        switch( BITCODE )
        {
            case BITSHORT_NORMAL:
            {
                aShortBytes[1]  = ( aShortBytes[1] << 1 );
                aShortBytes[1] |= ( aShortBytes[2] >> 7 );
                aShortBytes[2]  = ( aShortBytes[2] << 1 );
                aShortBytes[2] |= ( aShortBytes[3] >> 7 );

                bitOffsetFromStart += 18;

                void * ptr = aShortBytes + 1;
                short * result = static_cast < short *> ( ptr );

                return * result;
            }

            case BITSHORT_UNSIGNED_CHAR:
            {
                aShortBytes[1]  = ( aShortBytes[1] << 1 );
                aShortBytes[1] |= ( aShortBytes[2] >> 7 );

                bitOffsetFromStart += 10;

                return ( unsigned char ) aShortBytes[1];
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
    }

    char BITCODE = ( ( aShortBytes[0] >> ( 6 - bitOffsetInByte ) ) & 0b00000011 );

    switch( BITCODE )
    {
        case BITSHORT_NORMAL:
        {
            if ( bitOffsetInByte == 6 )
            {
                aShortBytes[0] = aShortBytes[1];
                aShortBytes[1] = aShortBytes[2];
            }
            else
            {
                aShortBytes[0]  = ( aShortBytes[0] << ( bitOffsetInByte + 2 ) );
                aShortBytes[0] |= ( aShortBytes[1] >> ( 6 - bitOffsetInByte ) );
                aShortBytes[1]  = ( aShortBytes[1] << ( bitOffsetInByte + 2 ) );
                aShortBytes[1] |= ( aShortBytes[2] >> ( 6 - bitOffsetInByte ) );
            }

            bitOffsetFromStart += 18;

            void * ptr = aShortBytes;
            short * result = static_cast < short * > ( ptr );

            return * result;
        }

        case BITSHORT_UNSIGNED_CHAR:
        {
            if ( bitOffsetInByte == 6 )
            {
                aShortBytes[0] = aShortBytes[1];
            }
            else
            {
                aShortBytes[0] = (aShortBytes[0] << (bitOffsetInByte + 2));
                aShortBytes[0] |= (aShortBytes[1] >> (6 - bitOffsetInByte));
            }

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
    // TODO: bit offset is calculated, but function has nothing to do with it.
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

    unsigned char aMCharBytes[MCharBytesCount]; // 8 bytes is maximum.
    memcpy ( aMCharBytes, pMCharFirstByte, MCharBytesCount );

    std::reverse ( aMCharBytes, aMCharBytes + MCharBytesCount ); // LSB to MSB

    if ( ( aMCharBytes[0] & 0b01000000 ) == 0b01000000 )
    {
        aMCharBytes[0] &= 0b10111111;
        negative = true;
    }

    for ( size_t i = 0; i < MCharBytesCount; ++i )
    {
        aMCharBytes[i] &= 0b01111111;
    }

    // TODO: this code doesnt cover case when char.bytescount > 3, but its possible on large files.
    // I just can write an algorithm that does this.
    switch ( MCharBytesCount )
    {
        case 1:
            break;
        case 2:
        {
            char tmp = aMCharBytes[0] & 0b00000001;
            aMCharBytes[0] = aMCharBytes[0] >> 1;
            aMCharBytes[1] |= (tmp << 7);
            break;
        }
        case 3:
        {
            unsigned char tmp1 = aMCharBytes[0] & 0b00000011;
            unsigned char tmp2 = aMCharBytes[1] & 0b00000001;
            aMCharBytes[0] = aMCharBytes[0] >> 2;
            aMCharBytes[1] = aMCharBytes[1] >> 1;
            aMCharBytes[1] |= ( tmp1 << 6 );
            aMCharBytes[2] |= ( tmp2 << 7 );
            break;
        }
    }

    std::reverse ( aMCharBytes, aMCharBytes + MCharBytesCount ); // MSB to LSB

    memcpy ( &result, aMCharBytes, MCharBytesCount );

    if ( negative ) result *= -1;

    bitOffsetFromStart += MCharBytesCount * 8;

    return result;
}

int ReadMSHORT ( const char * input_array, size_t& bitOffsetFromStart )
{
    int    result = 0;
    size_t byteOffset      = bitOffsetFromStart / 8;
    size_t bitOffsetInByte = bitOffsetFromStart % 8;

    const char * pMShortFirstByte = input_array + byteOffset;

    // TODO: this function doesnot support MSHORTS longer than 4 bytes. ODA says its impossible, but not sure.
    size_t MShortBytesCount = 2;
    if ( pMShortFirstByte[0] & 0b10000000 )
        MShortBytesCount = 4;

    unsigned char aMShortBytes[MShortBytesCount]; // 8 bytes is maximum.
    memcpy ( aMShortBytes, pMShortFirstByte, MShortBytesCount );

    std::reverse ( aMShortBytes, aMShortBytes + MShortBytesCount ); // LSB to MSB

    if( MShortBytesCount == 2 )
    {
        std::reverse ( aMShortBytes, aMShortBytes + MShortBytesCount ); // MSB to LSB
        memcpy ( &result, aMShortBytes, 2 );
    }
    else if ( MShortBytesCount == 4 )
    {
        aMShortBytes[2] |= ( aMShortBytes[1] << 7 );
        aMShortBytes[1]  = ( aMShortBytes[1] >> 1 );
        aMShortBytes[1] |= ( aMShortBytes[0] << 7 );
        aMShortBytes[0]  = ( aMShortBytes[0] >> 1 );

        std::reverse ( aMShortBytes, aMShortBytes + MShortBytesCount ); // MSB to LSB
        memcpy ( &result, aMShortBytes, 4 );
    }

    bitOffsetFromStart += MShortBytesCount * 8;

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

#include "dwg_r2000.h"

#include "dwg_format_sentinels.h"
#include "libdwgx_datatypes.h"

#include <bitset>

int DWGFileR2000::ReadHeader()
{
    char * pabyBuf = new char[100];
    int32_t dImageSeeker, dSLRecords;
    int16_t dCodePage;

    fDWG.seekg ( 6, std::ios_base::seek_dir::beg ); // file version skipped.
    fDWG.seekg ( 7, std::ios_base::seek_dir::cur ); // meaningless data skipped.
    fDWG.read ( (char *) &dImageSeeker, 4 );

#ifdef _LIBDWGX_DEBUG
    printf ( "Image seeker readed: %d\n", dImageSeeker );
#endif

    fDWG.seekg ( 2, std::ios_base::seek_dir::cur );
    fDWG.read ( (char *) &dCodePage, 2 );

#ifdef _LIBDWGX_DEBUG
    printf ( "DWG Code page: %d\n",dCodePage );
#endif

    fDWG.read ( (char *) &dSLRecords, 4 );

#ifdef _LIBDWGX_DEBUG
    printf ( "Section-locator records count: %d\n", dSLRecords );
#endif

    for ( size_t i = 0; i < dSLRecords; ++i )
    {
        SLRecord readed_record;
        fDWG.read ( (char *) &readed_record.byRecordNumber, 1 );
        fDWG.read ( (char *) &readed_record.dSeeker, 4 );
        fDWG.read ( (char *) &readed_record.dSize, 4 );

        this->fileHeader.SLRecords.push_back( readed_record );
#ifdef _LIBDWGX_DEBUG
        printf ( "SL Record #%d : %d %d\n", this->fileHeader.SLRecords[i].byRecordNumber,
                                            this->fileHeader.SLRecords[i].dSeeker,
                                            this->fileHeader.SLRecords[i].dSize );
#endif
    }

/*      READ HEADER VARIBLES        */
    size_t dHeaderVarsSectionLength = 0;

    fDWG.seekg ( this->fileHeader.SLRecords[0].dSeeker, std::ios_base::seek_dir::beg );

    fDWG.read ( pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH );
    if ( memcmp ( pabyBuf, DWG_SENTINELS::HEADER_VARIABLES_START, DWG_SENTINELS::SENTINEL_LENGTH ) )
    {
        printf ( "File is corrupted (wrong pointer to HEADER_VARS section,"
                 "or HEADERVARS starting sentinel corrupted.)" );

        delete [] pabyBuf;

        return 0;
    }

    fDWG.read ( (char *) &dHeaderVarsSectionLength, 4 );
    printf ( "Header variables section length: %d\n", dHeaderVarsSectionLength );

    fDWG.seekg ( this->fileHeader.SLRecords[0].dSeeker + this->fileHeader.SLRecords[0].dSize - 16,
                 std::ios_base::seek_dir::beg );

    fDWG.read ( pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH );
    if ( memcmp ( pabyBuf, DWG_SENTINELS::HEADER_VARIABLES_END, DWG_SENTINELS::SENTINEL_LENGTH ) )
    {
        printf ( "File is corrupted (HEADERVARS section ending sentinel doesnt match.)" );

        delete [] pabyBuf;

        return 0;
    }

    delete [] pabyBuf;
}

int DWGFileR2000::ReadObjectMap()
{
    char * pabyBuf = new char[100];
    unsigned short section_size = 0;

    fDWG.seekg( this->fileHeader.SLRecords[2].dSeeker, std::ios_base::seek_dir::beg );

    while ( true )
    {
        fDWG.read ( (char *) &section_size, 2 );
        swapByteOrder( section_size );

        printf ( "Object map section size: %u\n", section_size );

        if ( section_size == 2) break;

        bool isNegative = false;
        int64_t MCharIntegerVal = 0;
        size_t MCharLength = 0;
        std::vector < char > MCharBuffer;
        char readed_byte = 0;

        for ( size_t bytes_readed = 0; bytes_readed < section_size; )
        {
            while ( true )
            {
                fDWG.read ( &readed_byte, 1 );
                ++bytes_readed;

                MCharBuffer.push_back( readed_byte );

                if ( ! ( (readed_byte >> 7) & 0 ) )
                {
                    std::reverse( MCharBuffer.begin(), MCharBuffer.end() );

                    if ( ( MCharBuffer[0] >> 6 ) & 1 )
                    {
                        isNegative = true;
                        MCharBuffer[0] &= 0b10111111;
                    }

                    // First bit set to 0, its no longer used.
                    for ( size_t i = 0; i < MCharBuffer.size(); ++i )
                    {
                        MCharBuffer[i] &= 0b01111111;
                    }

                    for ( size_t i = 0; i < MCharBuffer.size() - 1; ++i )
                    {
                        MCharBuffer[i] |= ( MCharBuffer[i + 1] & 0b00000001 );
                        MCharBuffer[i + 1] = MCharBuffer[i + 1] >> 1;
                    }

                    if ( isNegative )
                    {
                        MCharBuffer[0] |= 0b10000000;
                    }

                    std::reverse( MCharBuffer.begin(), MCharBuffer.end() );
                    memcpy ( &MCharIntegerVal, MCharBuffer.data(), MCharBuffer.size() );

                    printf ( "Modular char val: %d\n",  MCharIntegerVal );

                    MCharIntegerVal = 0;
                    MCharLength = 0;
                    MCharBuffer.clear();

                    break;
                }
            }
        }
    }
}

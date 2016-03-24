#include "dwg_r2000.h"
#include "dwg_format_sentinels.h"

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
    fDWG.seekg ( this->fileHeader.SLRecords[0].dSeeker, std::ios_base::seek_dir::beg );
    fDWG.read ( pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH );

    if ( !memcmp ( pabyBuf, DWG_SENTINELS::HEADER_VARIABLES_START, DWG_SENTINELS::SENTINEL_LENGTH ) )
    {
        printf ( "Successfully seeked to HEADER varibles." );
        fDWG.seekg ( this->fileHeader.SLRecords[0].dSize - ( DWG_SENTINELS::SENTINEL_LENGTH * 2 ),
                     std::ios_base::seek_dir::cur );
        fDWG.read ( pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH );

        if ( !memcmp ( pabyBuf, DWG_SENTINELS::HEADER_VARIABLES_END, DWG_SENTINELS::SENTINEL_LENGTH ) )
        {
            printf ( "And seeked through this." );
        }
    }

    delete [] pabyBuf;
}

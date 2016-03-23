#include "libdwgx.h"
#include <iostream>

char extractBits ( size_t start, size_t count, char * data )
{
    char mask = 0;
    for ( size_t i = 0; i < count; ++i )
    {
        mask |= 1;
        mask = mask << 1;
    }

    return ( char ) ( *data >> start ) & mask;
}

DWGHandle DWGOpen ( const char * filename )
{
    DWGHandle hDWG = (DWGHandle) calloc ( sizeof ( DWGInfo ), 1 );

    hDWG->fpDWG = fopen ( filename, "r+b" );

    if ( hDWG->fpDWG != NULL )
    {
        std::cout << "Successfully opened DWG file." << std::endl;
        return hDWG;
    }
    else
    {
        std::cout << "Error opening " << filename << " . Abort. " << std::endl;
        return nullptr;
    }
}

int DWGReadHeader ( DWGHandle hDWG )
{
    int bSupportedVersion = 0;
    char * pabyBuf = (char *) malloc ( 100 );

    fread ( hDWG->DWGVersion, DWG_VERSION_SIZE, 1, hDWG->fpDWG );

    for ( size_t index = 0; index < DWG_VERSION_COUNT; ++index )
    {
        if ( ! memcmp ( hDWG->DWGVersion, DWGVersions[index], DWG_VERSION_COUNT ) )
        {
            bSupportedVersion = true;
            break;
        }
    }

    if ( !bSupportedVersion )
    {
        printf ( "Unsupported version of .dwg file: %s \n. Check documentation.", hDWG->DWGVersion);

        free ( pabyBuf );
        return 0;
    }

    fseek ( hDWG->fpDWG, 7, SEEK_CUR ); // meaningful dataset.

    fread ( &hDWG->dImageSeeker, 4, 1, hDWG->fpDWG );
#ifdef _LIBDWGX_DEBUG
    printf ( "Image seeker readed: %d\n", hDWG->dImageSeeker );
#endif

    fseek ( hDWG->fpDWG, 2, SEEK_CUR );

    fread ( &hDWG->dCodePage, 2, 1, hDWG->fpDWG );

#ifdef _LIBDWGX_DEBUG
    printf ( "DWG Code page: %d\n", hDWG->dCodePage );
#endif

    fread ( &hDWG->dSLRecords, 4, 1, hDWG->fpDWG );

#ifdef _LIBDWGX_DEBUG
    printf ( "Section-locator records count: %d\n", hDWG->dSLRecords );
#endif

    hDWG->paSLRecords = ( SLRecord *) calloc ( sizeof ( SLRecord ), hDWG->dSLRecords );
    for ( size_t i = 0; i < hDWG->dSLRecords; ++i )
    {
        fread ( &hDWG->paSLRecords[i].byRecordNumber, 1, 1, hDWG->fpDWG );
        fread ( &hDWG->paSLRecords[i].dSeeker, 4, 1, hDWG->fpDWG );
        fread ( &hDWG->paSLRecords[i].dSize, 4, 1, hDWG->fpDWG );

#ifdef _LIBDWGX_DEBUG
        printf ( "SL Record #%d : %d %d\n", hDWG->paSLRecords[i].byRecordNumber,
                                          hDWG->paSLRecords[i].dSeeker,
                                          hDWG->paSLRecords[i].dSize );
#endif
    }

    free ( pabyBuf );

    return 1;
}

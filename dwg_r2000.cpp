#include "dwg_r2000.h"

#include "libdwgx_datatypes.h"

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
    printf ( "Header variables section length: %ld\n", dHeaderVarsSectionLength );

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

    return 0;
}

int DWGFileR2000::ReadClassesSection()
{
    char * pabySectionContent;
    char * pabyBuf = new char[100];
    int32_t section_size = 0;

    fDWG.seekg ( this->fileHeader.SLRecords[1].dSeeker, std::ios_base::seekdir::beg );

    fDWG.read ( pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH );
    if ( memcmp ( pabyBuf, DWG_SENTINELS::DS_CLASSES_START, DWG_SENTINELS::SENTINEL_LENGTH ) )
    {
        std::cerr << "File is corrupted (wrong pointer to CLASSES section,"
                 "or CLASSES starting sentinel corrupted.)\n";

        delete [] pabyBuf;

        return 0;
    }

    fDWG.read ( (char *) &section_size, 4 );
    printf ( "Classes section length: %d\n", section_size );

    pabySectionContent = new char[section_size];
    fDWG.read ( pabySectionContent, section_size );

    size_t bitOffsetFromStart = 0;

    while ( ( bitOffsetFromStart / 8 ) + 1 < section_size )
    {
        custom_classes.push_back ( ReadClass ( pabySectionContent, bitOffsetFromStart ) );
    }

    for ( int i = 0; i < custom_classes.size(); ++i )
    {
        std::cout << "/////// CLASS INFO ////////" << std::endl;
        std::cout << "CLASSNUM: " << custom_classes[i].CLASSNUM << std::endl;
        std::cout << "VERSION: " << custom_classes[i].VERSION << std::endl;
        std::cout << "APPNAME: " << custom_classes[i].APPNAME << std::endl;
        std::cout << "C++CLASSNAME: " << custom_classes[i].CPLUSPLUSCLASSNAME << std::endl;
        std::cout << "CLASSDXFNAME: " << custom_classes[i].CLASSDXFNAME << std::endl;
        std::cout << "WASAZOMBIE: " << custom_classes[i].WASAZOMBIE << std::endl;
        std::cout << "ITEMCLASSID: " << std::hex << custom_classes[i].ITEMCLASSID << std::dec  << std::endl << std::endl;
    }

    fDWG.read ( pabyBuf, 2 ); // CLASSES CRC!.

    // fDWG.seekg ( this->fileHeader.SLRecords[1].dSeeker + this->fileHeader.SLRecords[1].dSize - 16,
    //              std::ios_base::seek_dir::beg );

    fDWG.read ( pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH );
    if ( memcmp ( pabyBuf, DWG_SENTINELS::DS_CLASSES_END, DWG_SENTINELS::SENTINEL_LENGTH ) )
    {
        std::cerr << "File is corrupted (CLASSES section ending sentinel doesnt match.)\n";

        delete [] pabyBuf;

        return 0;
    }

    delete [] pabyBuf;

    return 0;
}

DWG2000_CLASS DWGFileR2000::ReadClass ( const char * input_array, size_t& bitOffsetFromStart )
{
    DWG2000_CLASS outputResult;

    outputResult.CLASSNUM           = ReadBITSHORT ( input_array, bitOffsetFromStart );
    outputResult.VERSION            = ReadBITSHORT ( input_array, bitOffsetFromStart );
    outputResult.APPNAME            = ReadTV ( input_array, bitOffsetFromStart );
    outputResult.CPLUSPLUSCLASSNAME = ReadTV ( input_array, bitOffsetFromStart );
    outputResult.CLASSDXFNAME       = ReadTV ( input_array, bitOffsetFromStart );
    outputResult.WASAZOMBIE         = ReadBIT ( input_array, bitOffsetFromStart );
    outputResult.ITEMCLASSID        = ReadBITSHORT ( input_array, bitOffsetFromStart );

    return outputResult;
}

int DWGFileR2000::ReadObjectMap()
{
    char * pabySectionContent;
    unsigned short section_size = 0;
    size_t bitOffsetFromStart = 0;

    fDWG.seekg( this->fileHeader.SLRecords[2].dSeeker, std::ios_base::seek_dir::beg );

    while ( true )
    {
        fDWG.read ( (char *) &section_size, 2 );
        swapByteOrder ( section_size );

        std::cout << "OBJECT MAP SECTION SIZE: " << section_size << std::endl;

        if ( section_size == 2 ) break;

        pabySectionContent = new char[section_size];
        fDWG.read ( pabySectionContent, section_size );

        while ( ( bitOffsetFromStart / 8 )  < section_size )
        {
            ObjHandleOffset tmp;
            tmp.first = ReadMCHAR ( pabySectionContent, bitOffsetFromStart );
            tmp.second = ReadMCHAR ( pabySectionContent, bitOffsetFromStart );

            object_map.push_back( tmp );
        }

        delete [] pabySectionContent;
    }

    return 0;
}

int DWGFileR2000::ReadObject()
{
    fDWG.seekg ( object_map[0].second );

}
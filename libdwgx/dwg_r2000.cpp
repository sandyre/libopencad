#include "dwg_r2000.h"
#include "dwg_objecttypes.h"

#include <algorithm>

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
        std::cout << "dClassNum: " << custom_classes[i].dClassNum << std::endl;
        std::cout << "VERSION: " << custom_classes[i].dVersion << std::endl;
        std::cout << "APPNAME: " << custom_classes[i].sAppName << std::endl;
        std::cout << "C++CLASSNAME: " << custom_classes[i].sCppClassName << std::endl;
        std::cout << "CLASSDXFNAME: " << custom_classes[i].sDXFClassName << std::endl;
        std::cout << "WASAZOMBIE: " << custom_classes[i].bWasAZombie << std::endl;
        std::cout << "ITEMCLASSID: " << std::hex << custom_classes[i].dItemClassID << std::dec  << std::endl << std::endl;
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

    outputResult.dClassNum           = ReadBITSHORT ( input_array, bitOffsetFromStart );
    outputResult.dVersion            = ReadBITSHORT ( input_array, bitOffsetFromStart );
    outputResult.sAppName            = ReadTV ( input_array, bitOffsetFromStart );
    outputResult.sCppClassName = ReadTV ( input_array, bitOffsetFromStart );
    outputResult.sDXFClassName       = ReadTV ( input_array, bitOffsetFromStart );
    outputResult.bWasAZombie         = ReadBIT ( input_array, bitOffsetFromStart );
    outputResult.dItemClassID        = ReadBITSHORT ( input_array, bitOffsetFromStart );

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

        while ( ( bitOffsetFromStart / 8 ) < section_size )
        {
            ObjHandleOffset tmp;
            tmp.first = ReadMCHAR ( pabySectionContent, bitOffsetFromStart );
            tmp.second = ReadMCHAR ( pabySectionContent, bitOffsetFromStart );

            object_map.push_back( tmp );
        }

        delete [] pabySectionContent;
    }

    // Rebuild object map so it will store absolute file offset, instead of offset from previous object handle.
    for ( size_t i = 1; i < object_map.size(); ++i )
    {
        object_map[ i ].second += object_map[ i - 1 ].second;
    }

    // Build geometries map from objectmap.
    for(size_t i = 0; i < object_map.size(); ++i)
    {
        bitOffsetFromStart = 0;
        pabySectionContent = new char[100];
        fDWG.seekg( object_map[i].second, std::ios_base::seekdir::beg );
        fDWG.read( pabySectionContent, 100 );

        DWG2000_CED ced;
        ced.dLength = ReadMSHORT ( pabySectionContent, bitOffsetFromStart );
        ced.dType   = ReadBITSHORT ( pabySectionContent, bitOffsetFromStart );

        if (std::find( DWG_GEOMETRIC_OBJECT_TYPES.begin (), DWG_GEOMETRIC_OBJECT_TYPES.end(), ced.dType )
                    != DWG_GEOMETRIC_OBJECT_TYPES.end() )
        {
            std::cout << "GEOMETRY TYPE: " << DWG_OBJECT_NAMES.at(ced.dType) << std::endl;
            geometries_map.push_back( object_map[i] );
        }
    }

    delete [] pabySectionContent;

    return 0;
}

int DWGFileR2000::ReadObject( size_t index )
{
    char * pabySectionContent = new char[100];
    size_t bitOffsetFromStart = 0;
    fDWG.seekg ( object_map[index].second );
    fDWG.read ( pabySectionContent, 100 );

    DWG2000_CED ced;
    ced.dLength         = ReadMSHORT ( pabySectionContent, bitOffsetFromStart );
    ced.dType           = ReadBITSHORT ( pabySectionContent, bitOffsetFromStart );
    ced.dObjSizeInBits  = ReadRAWLONG ( pabySectionContent, bitOffsetFromStart );
    ced.hHandle         = ReadHANDLE ( pabySectionContent, bitOffsetFromStart );
    ced.dNumReactors    = ReadBITLONG ( pabySectionContent, bitOffsetFromStart );

    delete [] pabySectionContent;
}

int DWGFileR2000::ReadGeometry( size_t index )
{
    char * pabySectionContent = new char[100];
    size_t bitOffsetFromStart = 0;
    fDWG.seekg ( geometries_map[index].second );
    fDWG.read ( pabySectionContent, 100 );

    DWG2000_CED ced;
    ced.dLength         = ReadMSHORT ( pabySectionContent, bitOffsetFromStart );
    ced.dType           = ReadBITSHORT ( pabySectionContent, bitOffsetFromStart );
    ced.dObjSizeInBits  = ReadRAWLONG ( pabySectionContent, bitOffsetFromStart );
    ced.hHandle         = ReadHANDLE ( pabySectionContent, bitOffsetFromStart );
    // TODO: EED is skipped, but it can be meaningful.
    ced.dEEDSize        = ReadBITSHORT ( pabySectionContent, bitOffsetFromStart );
    bitOffsetFromStart += ced.dEEDSize * 8; // skip EED size bytes.

    // TODO: Proxy Entity Graphics also skipped for now. If there is something
    // (ced.bGraphicPresentFlag is true), than it wont work properly at all.
    ced.bGraphicPresentFlag = ReadBIT ( pabySectionContent, bitOffsetFromStart );

    ced.dEntMode        = Read2B ( pabySectionContent, bitOffsetFromStart );
    ced.dNumReactors    = ReadBITLONG ( pabySectionContent, bitOffsetFromStart );
    ced.bNoLinks        = ReadBIT ( pabySectionContent, bitOffsetFromStart );
    ced.dCMColorIndex   = ReadBITSHORT ( pabySectionContent, bitOffsetFromStart );
    ced.dfLtypeScale    = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
    ced.ltype_flags     = Read2B ( pabySectionContent, bitOffsetFromStart );
    ced.plotstyle_flags = Read2B ( pabySectionContent, bitOffsetFromStart );
    ced.dInvisibility   = ReadBITSHORT ( pabySectionContent, bitOffsetFromStart );
    ced.cLineWeight     = ReadCHAR ( pabySectionContent, bitOffsetFromStart );

    // READ DATA DEPENDING ON ced.dType
    switch ( ced.dType )
    {
        case DWG_OBJECT_CIRCLE:
        {
            double centerx, centery, centerz;
            double rad, thickness;
            char   extrusion;
            centerx = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
            centery = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
            centerz = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
            rad     = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
            thickness = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
            break;
        }
        case DWG_OBJECT_POINT:
        {
            double centerx, centery, centerz;
            double thickness;
            char   extrusion;
            double x_axis_angle;
            centerx = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
            centery = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
            centerz = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
            thickness = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
        }
    }

    delete [] pabySectionContent;
}

int DWGFileR2000::GetGeometriesCount ()
{
    return geometries_map.size();
}
#include "dwg_r2000.h"
#include "dwg_objecttypes.h"

using namespace libdwgx::DWGGeometries;

int DWGFileR2000::ReadHeader()
{
    char * pabyBuf = new char[100];
    int32_t dImageSeeker, dSLRecords;
    int16_t dCodePage;

    fDWG.seekg ( 6, std::ios_base::seek_dir::beg ); // file version skipped.
    fDWG.seekg ( 7, std::ios_base::seek_dir::cur ); // meaningless data skipped.
    fDWG.read ( (char *) &dImageSeeker, 4 );

    printf ( "Image seeker readed: %d\n", dImageSeeker );

    fDWG.seekg ( 2, std::ios_base::seek_dir::cur );
    fDWG.read ( (char *) &dCodePage, 2 );

    printf ( "DWG Code page: %d\n",dCodePage );

    fDWG.read ( (char *) &dSLRecords, 4 );

    printf ( "Section-locator records count: %d\n", dSLRecords );

    for ( size_t i = 0; i < dSLRecords; ++i )
    {
        SLRecord readed_record;
        fDWG.read ( (char *) &readed_record.byRecordNumber, 1 );
        fDWG.read ( (char *) &readed_record.dSeeker, 4 );
        fDWG.read ( (char *) &readed_record.dSize, 4 );

        this->fileHeader.SLRecords.push_back( readed_record );
        printf ( "SL Record #%d : %d %d\n", this->fileHeader.SLRecords[i].byRecordNumber,
                                            this->fileHeader.SLRecords[i].dSeeker,
                                            this->fileHeader.SLRecords[i].dSize );
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

    fDWG.read (pabyBuf, 2); // CLASSES CRC!. TODO: add CRC computing & checking feature.

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
    // Seems like ODA specification is completely awful. CRC is included in section size.
    char * pabySectionContent;
    uint16_t section_crc  = 0;
    uint16_t section_size = 0;
    size_t bitOffsetFromStart = 0;

    fDWG.seekg ( this->fileHeader.SLRecords[2].dSeeker, std::ios_base::seek_dir::beg );

    while ( true )
    {
        bitOffsetFromStart = 0;
        fDWG.read ( ( char * ) &section_size, 2 );
        SwapEndianness ( section_size, sizeof ( section_size ) );

        std::cout << "OBJECT MAP SECTION SIZE: " << section_size << std::endl;

        if ( section_size == 2 ) break; // last section is empty.

        pabySectionContent = new char[section_size];
        fDWG.read ( pabySectionContent, section_size);

        while ( ( bitOffsetFromStart / 8 ) < (section_size - 2) )
        {
            ObjHandleOffset tmp;
            tmp.first  = ReadMCHAR ( pabySectionContent, bitOffsetFromStart );
            tmp.second = ReadMCHAR ( pabySectionContent, bitOffsetFromStart );
            object_map.push_back (tmp);
        }

        section_crc = ReadRAWSHORT ( pabySectionContent, bitOffsetFromStart );
        SwapEndianness ( section_crc, sizeof( section_crc ) );

        std::cout << "OBJECTS PARSED: " << object_map.size() << std::endl;

        delete [] pabySectionContent;
    }

    // Rebuild object map so it will store absolute file offset, instead of offset from previous object handle.
    for ( size_t i = 1; i < object_map.size(); ++i )
    {
        object_map[ i ].second += object_map[ i - 1 ].second;
    }

    // Build geometries map from objectmap.
    pabySectionContent = new char[100];
    for(size_t i = 0; i < object_map.size(); ++i)
    {
        bitOffsetFromStart = 0;
        fDWG.seekg ( object_map[i].second, std::ios_base::seekdir::beg );
        fDWG.read ( pabySectionContent, 100 );

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
    char   pabySectionContent[100];
    size_t bitOffsetFromStart = 0;
    fDWG.seekg ( object_map[index].second );
    fDWG.read ( pabySectionContent, 100 );

    DWG2000_CED ced;
    ced.dLength         = ReadMSHORT ( pabySectionContent, bitOffsetFromStart );
    ced.dType           = ReadBITSHORT ( pabySectionContent, bitOffsetFromStart );
    ced.dObjSizeInBits  = ReadRAWLONG ( pabySectionContent, bitOffsetFromStart );
    ced.hHandle         = ReadHANDLE ( pabySectionContent, bitOffsetFromStart );
    ced.dNumReactors    = ReadBITLONG ( pabySectionContent, bitOffsetFromStart );

    return 0;
}

Geometry * DWGFileR2000::ReadGeometry ( size_t index )
{
    Geometry * readed_geometry;

    // Get geometric entity size in bytes.
    char pabySectionSize[8];
    size_t bitOffsetFromStart = 0;
    int k = geometries_map[index].second;
    fDWG.clear ();
    fDWG.seekg ( geometries_map[index].second, std::ios_base::seekdir::beg );
    fDWG.read ( pabySectionSize, 8 );
    uint32_t dGeometrySize = ReadMSHORT ( pabySectionSize, bitOffsetFromStart );

    // And read whole data chunk into memory for future parsing.
    char * pabySectionContent = new char[dGeometrySize];
    bitOffsetFromStart = 0;
    fDWG.clear();
    fDWG.seekg ( geometries_map[index].second, std::ios_base::seekdir::beg );
    fDWG.read ( pabySectionContent, dGeometrySize );

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
            Circle * circle = new Circle ();
            circle->dfCenterX   = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            circle->dfCenterY   = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            circle->dfCenterZ   = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            circle->dfRadius    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            circle->dfThickness = ReadBIT (pabySectionContent, bitOffsetFromStart) ?
                                  0.0f : ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            if ( ReadBIT (pabySectionContent, bitOffsetFromStart) )
            {
                circle->dfExtrusionX = 0.0f;
                circle->dfExtrusionY = 0.0f;
                circle->dfExtrusionZ = 1.0f;
            }
            else
            {
                circle->dfExtrusionX = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                circle->dfExtrusionY = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                circle->dfExtrusionZ = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            }

            readed_geometry = circle;
            break;
        }

        case DWG_OBJECT_ELLIPSE:
        {
            Ellipse * ellipse        = new Ellipse ();
            ellipse->dfCenterX      = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfCenterY      = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfCenterZ      = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfWCSX         = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfWCSY         = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfWCSZ         = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfExtrusionX   = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfExtrusionY    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfExtrusionZ    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfAxisRatio     = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfStartingAngle = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfEndingAngle   = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            readed_geometry = ellipse;
            break;
        }

        case DWG_OBJECT_POINT:
        {
            Point * point      = new Point ();
            point->dfPointX    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            point->dfPointY    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            point->dfPointZ    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            point->dfThickness = ReadBIT (pabySectionContent, bitOffsetFromStart) ?
                                 0.0f : ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            if ( ReadBIT (pabySectionContent, bitOffsetFromStart) )
            {
                point->dfExtrusionX = 0.0f;
                point->dfExtrusionY = 0.0f;
                point->dfExtrusionZ = 1.0f;
            }
            else
            {
                point->dfExtrusionX = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                point->dfExtrusionY = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                point->dfExtrusionZ = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            }

            readed_geometry = point;
            break;
        }

        case DWG_OBJECT_LINE:
        {
            Line * line = new Line();
            bool bZCoordPresented = ReadBIT ( pabySectionContent, bitOffsetFromStart );
            line->dfStartX = ReadRAWDOUBLE ( pabySectionContent, bitOffsetFromStart );
            line->dfEndX   = ReadBITDOUBLEWD ( pabySectionContent, bitOffsetFromStart, line->dfStartX );
            line->dfStartY = ReadRAWDOUBLE ( pabySectionContent, bitOffsetFromStart );
            line->dfEndY   = ReadBITDOUBLEWD ( pabySectionContent, bitOffsetFromStart, line->dfStartY );

            if ( !bZCoordPresented )
            {
                line->dfStartZ = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
                line->dfEndZ   = ReadBITDOUBLEWD ( pabySectionContent, bitOffsetFromStart, line->dfStartZ );
            }

            line->dfThickness = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );

            if ( ReadBIT (pabySectionContent, bitOffsetFromStart) )
            {
                line->dfExtrusionX = 0.0f;
                line->dfExtrusionY = 0.0f;
                line->dfExtrusionZ = 1.0f;
            }
            else
            {
                line->dfExtrusionX = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                line->dfExtrusionY = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                line->dfExtrusionZ = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            }

            readed_geometry = line;

            break;
        }

        case DWG_OBJECT_LWPOLYLINE:
        {
            LWPolyline * polyline = new LWPolyline();
            int32_t nVertixesCount = 0, nBulges = 0;
            int16_t data_flag = ReadBITSHORT ( pabySectionContent, bitOffsetFromStart );
            if ( data_flag & 4 )
                polyline->dfConstWidth = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
            if ( data_flag & 8 )
                polyline->dfElevation = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
            if ( data_flag & 2 )
                polyline->dfThickness = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
            if ( data_flag & 1 )
            {
                polyline->dfExtrusionX = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
                polyline->dfExtrusionY = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
                polyline->dfExtrusionZ = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
            }

            nVertixesCount = ReadBITLONG ( pabySectionContent, bitOffsetFromStart );

            if ( data_flag &  16 )
            {
                nBulges = ReadBITLONG ( pabySectionContent, bitOffsetFromStart );
            }

            // First of all, read first vertex.
            Vertex2D vertex;
            vertex.X = ReadRAWDOUBLE ( pabySectionContent, bitOffsetFromStart );
            vertex.Y = ReadRAWDOUBLE ( pabySectionContent, bitOffsetFromStart );
            polyline->vertexes.push_back ( vertex );

            // All the others are not raw doubles; bitdoubles with default instead,
            // where default is previous point coords.
            for ( size_t i = 1; i < nVertixesCount; ++i )
            {
                vertex.X = ReadBITDOUBLEWD ( pabySectionContent, bitOffsetFromStart,
                                             polyline->vertexes[i-1].X );
                vertex.Y = ReadBITDOUBLEWD ( pabySectionContent, bitOffsetFromStart,
                                             polyline->vertexes[i-1].Y );
                polyline->vertexes.push_back ( vertex );
            }

            for ( size_t i = 0; i < nBulges; ++i )
            {
                double dfBulgeValue = ReadBITDOUBLE ( pabySectionContent, bitOffsetFromStart );
                polyline->bulges.push_back ( dfBulgeValue );
            }

            readed_geometry = polyline;
            break;
        }

        case DWG_OBJECT_ARC:
        {
            Arc * arc = new Arc();
            arc->dfCenterX      = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            arc->dfCenterY      = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            arc->dfCenterZ      = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            arc->dfRadius       = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            arc->dfThickness    = ReadBIT (pabySectionContent, bitOffsetFromStart) ?
                                 0.0f : ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            if ( ReadBIT (pabySectionContent, bitOffsetFromStart) )
            {
                arc->dfExtrusionX = 0.0f;
                arc->dfExtrusionY = 0.0f;
                arc->dfExtrusionZ = 1.0f;
            }
            else
            {
                arc->dfExtrusionX = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                arc->dfExtrusionY = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                arc->dfExtrusionZ = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            }
            arc->dfStartingAngle = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            arc->dfEndingAngle   = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            readed_geometry = arc;
            break;
        }

        case DWG_OBJECT_TEXT:
        {
            Text * text = new Text();
            int8_t DataFlag = ReadCHAR ( pabySectionContent, bitOffsetFromStart );

            if( !( DataFlag & 0x01 ) )
                text->dfElevation = ReadRAWDOUBLE ( pabySectionContent, bitOffsetFromStart );

            text->dfInsertionX = ReadRAWDOUBLE ( pabySectionContent, bitOffsetFromStart );
            text->dfInsertionY = ReadRAWDOUBLE ( pabySectionContent, bitOffsetFromStart );

            if( !( DataFlag & 0x02 ) )
            {
                text->dfAlignmentX = ReadBITDOUBLEWD ( pabySectionContent, bitOffsetFromStart, 10.0f );
                text->dfAlignmentY = ReadBITDOUBLEWD ( pabySectionContent, bitOffsetFromStart, 20.0f );
            }

            if ( ReadBIT (pabySectionContent, bitOffsetFromStart) )
            {
                text->dfExtrusionX = 0.0f;
                text->dfExtrusionY = 0.0f;
                text->dfExtrusionZ = 1.0f;
            }
            else
            {
                text->dfExtrusionX = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                text->dfExtrusionY = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                text->dfExtrusionZ = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            }

            text->dfThickness = ReadBIT (pabySectionContent, bitOffsetFromStart) ?
                                0.0f : ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            if ( !( DataFlag & 0x04 ) )
                text->dfObliqueAngle  = ReadRAWDOUBLE ( pabySectionContent, bitOffsetFromStart );

            if ( !( DataFlag & 0x08 ) )
                text->dfRotationAngle = ReadRAWDOUBLE ( pabySectionContent, bitOffsetFromStart );

            text->dfHeight        = ReadRAWDOUBLE ( pabySectionContent, bitOffsetFromStart );

            if ( !( DataFlag & 0x10 ) )
                text->dfWidthFactor   = ReadRAWDOUBLE ( pabySectionContent, bitOffsetFromStart );

            text->strTextValue    = ReadTV ( pabySectionContent, bitOffsetFromStart );

            if ( !( DataFlag & 0x20 ) )
                text->dGeneration = ReadBITSHORT ( pabySectionContent, bitOffsetFromStart );

            if ( !( DataFlag & 0x40 ) )
                text->dHorizontalAlignment = ReadBITSHORT ( pabySectionContent, bitOffsetFromStart );

            if ( !( DataFlag & 0x80 ) )
                text->dVerticalAlignment   = ReadBITSHORT ( pabySectionContent, bitOffsetFromStart );

            readed_geometry = text;
            break;
        }
    }

    delete [] pabySectionContent;

    return readed_geometry;
}

size_t DWGFileR2000::GetGeometriesCount ()
{
    return geometries_map.size();
}
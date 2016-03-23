#include "libdwgx.h"

#include "dwg_base.h"
#include "dwg_r2000.h"

#include <string>
#include <iostream>
#include <fstream>

using std::ifstream;
using std::string;

DWGFile * libdwgx::InitializeDWG( const char *filename )
{
    DWGFile * hDWG;
    ifstream dwg_file ( filename, std::ios_base::in | std::ios_base::binary );

    char pabyDWGVersion[6];
    dwg_file.read ( pabyDWGVersion, 6 );
    dwg_file.close();

    // TODO: switch would be better here.
    // if ( !memcmp ( pabyDWGVersion, DWG_VERSION_R13, DWG_VERSION_SIZE ) )
    //     hDWG = new DWGFileR13();

    if ( !memcmp ( pabyDWGVersion, DWG_VERSION_R2000, DWG_VERSION_SIZE ) )
        hDWG = new DWGFileR2000();

    if ( hDWG != nullptr )
    {
        hDWG->fDWG.open ( filename, std::ios_base::in | std::ios_base::binary );
        return hDWG;
    }

    else
    {
        std::cerr << "Unsupported DWG file version. Check documentation.";
        return nullptr;
    }
}

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

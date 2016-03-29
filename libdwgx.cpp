#include "libdwgx.h"

#include "dwg_r2000.h"

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
        std::cerr << "Unsupported DWG file version. Check documentation.\n";
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

void swapByteOrder(unsigned short& us)
{
    us = (us >> 8) |
         (us << 8);
}

void swapByteOrder(unsigned int& ui)
{
    ui = (ui >> 24) |
         ((ui<<8) & 0x00FF0000) |
         ((ui>>8) & 0x0000FF00) |
         (ui << 24);
}

void swapByteOrder(unsigned long long& ull)
{
    ull = (ull >> 56) |
          ((ull<<40) & 0x00FF000000000000) |
          ((ull<<24) & 0x0000FF0000000000) |
          ((ull<<8) & 0x000000FF00000000) |
          ((ull>>8) & 0x00000000FF000000) |
          ((ull>>24) & 0x0000000000FF0000) |
          ((ull>>40) & 0x000000000000FF00) |
          (ull << 56);
}

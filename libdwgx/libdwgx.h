#ifndef __LIBDWGX_H__
#define __LIBDWGX_H__

#include "dwg_base.h"
#include "dwg_format_sentinels.h"
#include "libdwgx_versions.h"
#include "libdwgx_io.h"
#include "dwg_geometries.h"

#include <iostream>
#include <string>
#include <fstream>

namespace libdwgx
{
    DWGFile * InitializeDWG( const char * filename );
};

// TODO: probably it would be better to have no dependencies on <algorithm>.
template<typename T, typename S>
void SwapEndianness ( T &object, S &&size )
{
    std::reverse (( char * ) & object, ( char * ) & object + size);
}

#endif

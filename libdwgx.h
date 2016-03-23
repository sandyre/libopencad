#ifndef __LIBDWGX_H__
#define __LIBDWGX_H__

#define _LIBDWGX_DEBUG

#include "dwg_base.h"
#include "libdwgx_versions.h"

#include <iostream>
#include <string>
#include <fstream>

static const char * DWGVersions[] =
{
    DWG_VERSION_R13, DWG_VERSION_R14,
    DWG_VERSION_R2000, DWG_VERSION_R2004,
    DWG_VERSION_R2007, DWG_VERSION_R2010,
    DWG_VERSION_R2013
};

// Section-Locator Record
struct SLRecord
{
    char byRecordNumber;
    int dSeeker;
    int dSize;
};

namespace libdwgx
{
    DWGFile * InitializeDWG( const char * filename );
};

char extractBits ( size_t start, size_t count, char * data );

#endif

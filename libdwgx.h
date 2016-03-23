#ifndef __LIBDWGX_H__
#define __LIBDWGX_H__

#define _LIBDWGX_DEBUG

// #include "libdwgx_datatypes.h"
#include "libdwgx_versions.h"
#include <stdio.h>

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

struct DWGInfo
{
    char DWGVersion[6];
    char DWGRevision;

    short dCodePage;
    int dImageSeeker;

    int dSLRecords;
    SLRecord *paSLRecords;

    FILE * fpDWG;
};

typedef DWGInfo * DWGHandle;

DWGHandle DWGOpen ( const char * filename );
int DWGReadHeader ( DWGHandle hDWG );

char extractBits ( size_t start, size_t count, char * data );

#endif

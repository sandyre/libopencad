#ifndef __LIBGDWX_DATATYPES_H__
#define __LIBGDWX_DATATYPES_H__

#include <vector>

#define BITSHORT_NORMAL         0x0
#define BITSHORT_UNSIGNED_CHAR  0x1
#define BITSHORT_ZERO_VALUE     0x2
#define BITSHORT_256            0x3

#define BITLONG_NORMAL          0x0
#define BITLONG_UNSIGNED_CHAR   0x1
#define BITLONG_ZERO_VALUE      0x2
#define BITLONG_NOT_USED        0x3

#define BITDOUBLE_NORMAL        0x0
#define BITDOUBLE_ONE_VALUE     0x1
#define BITDOUBLE_ZERO_VALUE    0x2
#define BITDOUBLE_NOT_USED      0x3

// Section-Locator Record
struct SLRecord
{
    char byRecordNumber;
    int  dSeeker;
    int  dSize;
};

struct DWG_HANDLE
{
    char code = 0;
    char counter = 0;
    char * handle_or_offset;
};

struct DWG_EED
{
    short length;
    DWG_HANDLE application_handle;
};

#endif

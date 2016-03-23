#ifndef __DWG_R2000_H__
#define __DWG_R2000_H__

#include "libdwgx.h"
#include "dwg_base.h"

struct DWGR2000_FILE_HEADER
{
    char pabyDWGVersion[6];
    int32_t dImageSeeker;
    int16_t dCodePage;

    SLRecord * paSLRecords;
};

class DWGFileR2000 : public DWGFile
{
public:
    DWGFileR2000() {}
    ~DWGFileR2000() override {}

    int ReadHeader() override;
private:
    DWGR2000_FILE_HEADER fileHeader;
};

#endif

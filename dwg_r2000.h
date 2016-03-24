#ifndef __DWG_R2000_H__
#define __DWG_R2000_H__

#include "libdwgx.h"
#include "dwg_base.h"

#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

struct DWGR2000_FILE_HEADER
{
    int32_t dImageSeeker;
    int16_t dCodePage;

    vector < SLRecord >  SLRecords;
    map < string, char > header_variables;
};

class DWGFileR2000 : public DWGFile
{
public:
    DWGFileR2000() {}
    ~DWGFileR2000() override {}

    int ReadHeader() override;
    int ReadObjectMap() override;
private:
    DWGR2000_FILE_HEADER fileHeader;
};

#endif

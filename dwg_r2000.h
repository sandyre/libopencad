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

static const map < string, string > DWG2000_CLASS_FIELDS
{
    std::make_pair ( "CLASSNUM", "BITSHORT"),
    std::make_pair ( "VERSION", "BITSHORT"),
    std::make_pair ( "APPNAME", "TV"),
    std::make_pair ( "CPLUSPPLUSCLASSNAME", "TV"),
    std::make_pair ( "CLASSDXFNAME", "TV"),
    std::make_pair ( "WASAZOMBIE", "BIT" ),
    std::make_pair ( "ITEMCLASSID", "BITSHORT" )
};

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
    int ReadClassesSection() override;
    int ReadObjectMap() override;

private:
    DWGR2000_FILE_HEADER fileHeader;
};

#endif

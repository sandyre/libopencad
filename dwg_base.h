#ifndef __DWG_BASE_H__
#define __DWG_BASE_H__

#include <fstream>
#include <string>

class DWGFile
{
public:
    DWGFile() {}
    virtual ~DWGFile() {}

    virtual int ReadHeader() {};
    virtual int ReadObjectMap() {};
    int ReadObject() { return 0; }

    std::ifstream fDWG;
protected:
    std::string sDWGVersion;
    std::string sDWGSubversion;
};

#endif

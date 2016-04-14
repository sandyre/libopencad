#ifndef __DWG_BASE_H__
#define __DWG_BASE_H__

#include <fstream>
#include <string>

#include "libdwgx_datatypes.h"
#include "dwg_geometries.h"

class DWGFile
{
public:
    DWGFile() {}
    virtual ~DWGFile() {}

    virtual int ReadHeader() { return 0;};
    virtual int ReadClassesSection() { return 0; };
    virtual int ReadObjectMap() { return 0;};
    virtual int ReadObject( size_t index ) { return 0; }

    virtual size_t GetGeometriesCount ()
    { return 0; }

    virtual libdwgx::DWGGeometries::Geometry *ReadGeometry ( size_t index )
    { return 0; }

    std::ifstream fDWG;
protected:
    std::string sDWGVersion;
    std::string sDWGSubversion;
};

#endif

#include "libdwgx.h"

int main(int argc, char *argv[])
{
    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/point2d_2000.dwg" );
    if (dwgfile == nullptr) return 0;
    dwgfile->ReadHeader();
    dwgfile->ReadObjectMap();
    dwgfile->ReadClassesSection();
    for ( size_t i = 0; i < 218; ++i )
        dwgfile->ReadObject(i);
}
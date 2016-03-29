#include "libdwgx.h"

int main(int argc, char *argv[])
{
    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/point_2000.dwg" );
    if (dwgfile == nullptr) return 0;
    dwgfile->ReadHeader();
    dwgfile->ReadObjectMap();
    dwgfile->ReadClassesSection();
    dwgfile->ReadObject();
}
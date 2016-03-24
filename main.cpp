#include <iostream>
#include "libdwgx.h"

int main ( int argc, char* argv[] )
{
    DWGFile *dwgfile = libdwgx::InitializeDWG( "./test/ACAD_r2000_sample.dwg" );
    dwgfile->ReadHeader();
    dwgfile->ReadObjectMap();
}

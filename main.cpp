#include <iostream>
#include "libdwgx.h"

int main ( int argc, char* argv[] )
{
    DWGFile *dwgfile = libdwgx::InitializeDWG( "./test/ACAD_r2000_libereco.dwg" );
    dwgfile->ReadHeader();
}

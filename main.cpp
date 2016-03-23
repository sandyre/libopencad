#include <iostream>
#include "libdwgx.h"

int main ( int argc, char* argv[] )
{
    DWGHandle handle = DWGOpen( "./test/ACAD_r2000_libereco.dwg" );
    DWGReadHeader( handle );
}

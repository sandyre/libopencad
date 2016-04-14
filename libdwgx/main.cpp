#include "libdwgx.h"

int main(int argc, char *argv[])
{
    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/triple_circle.dwg" );
    if (dwgfile == nullptr) return 0;
    dwgfile->ReadHeader();
    dwgfile->ReadObjectMap();
    for(auto i = 0; i < dwgfile->GetGeometriesCount (); ++i )
    {
        auto geom = dwgfile->ReadGeometry (i);
        if ( "Circle" == geom->sGeometryType )
        {
            auto geom1 = ( libdwgx::DWGGeometries::Circle * ) geom;
            int  a     = 0;
        }

        int a = 0;
        delete geom;
    }
}
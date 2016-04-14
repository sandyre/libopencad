#include "libdwgx.h"

int main(int argc, char *argv[])
{
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/Иркутск_обнов.dwg" );
    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/triple_circle.dwg" );
    if (dwgfile == nullptr) return 0;
    dwgfile->ReadHeader();
    dwgfile->ReadObjectMap();
    int g = dwgfile->GetGeometriesCount ();
    for(auto i = 0; i < dwgfile->GetGeometriesCount (); ++i )
    {
        auto geom = dwgfile->ReadGeometry (i);
        if ( geom->sGeometryType == libdwgx::DWGGeometries::DWGGeometryType::CIRCLE )
        {
            auto geom1 = ( libdwgx::DWGGeometries::Circle * ) geom;
            geom1->printInfo ();
            int b = 0;
        }
        else if ( geom->sGeometryType == libdwgx::DWGGeometries::DWGGeometryType::POINT )
        {
            auto geom1 = ( libdwgx::DWGGeometries::Point * ) geom;
            geom1->printInfo ();
            int b = 0;
        }
        else if ( geom->sGeometryType == libdwgx::DWGGeometries::DWGGeometryType::ELLIPSE)
        {
            auto geom1 = ( libdwgx::DWGGeometries::Ellipse * ) geom;
            int c = 0;
        }
        else if ( geom->sGeometryType == libdwgx::DWGGeometries::DWGGeometryType::LINE )
        {
            auto geom1 = ( libdwgx::DWGGeometries::Line * ) geom;
            geom1->printInfo ();
            int c = 0;
        }
        else if ( geom->sGeometryType == libdwgx::DWGGeometries::DWGGeometryType::LWPOLYLINE )
        {
            auto geom1 = ( libdwgx::DWGGeometries::LWPolyline * ) geom;
            geom1->printInfo ();
            int c = 0;
        }

        int a = 0;
    }
}
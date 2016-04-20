#include "libdwgx.h"

int main(int argc, char *argv[])
{
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/house_sample.dwg" );
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/г.Крымск.dwg" );
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/Иркутск_обнов2.dwg" );
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/Ситплан КГКМ.dwg" );
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/circle_2000.dwg" );
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/polyline2d_line_2000.dwg" );
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/triple_circle.dwg" );
    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/polyline3d.dwg" );
    if (dwgfile == nullptr) return 0;
    dwgfile->ReadHeader();
    dwgfile->ReadClassesSection ();
    dwgfile->ReadObjectMap();
    int g = dwgfile->GetGeometriesCount ();
    int circles_count = 0;
    int lines_count = 0;
    int ellipses_count = 0;
    int pline_count = 0;
    int point_count = 0;
    int arc_count = 0;
    int text_count = 0;
    std::cout << "Geometries readed: " << g << std::endl;
    for(auto i = 0; i < dwgfile->GetGeometriesCount (); ++i )
    {
        auto geom = dwgfile->ReadGeometry (i);
        if ( geom->sGeometryType == libdwgx::DWGGeometries::DWGGeometryType::CIRCLE )
        {
            auto geom1 = ( libdwgx::DWGGeometries::Circle * ) geom;
//            geom1->printInfo ();
            circles_count++;
        }
        else if ( geom->sGeometryType == libdwgx::DWGGeometries::DWGGeometryType::POINT )
        {
            auto geom1 = ( libdwgx::DWGGeometries::Point * ) geom;
//            geom1->printInfo ();
            point_count++;
        }
        else if ( geom->sGeometryType == libdwgx::DWGGeometries::DWGGeometryType::ELLIPSE)
        {
            auto geom1 = ( libdwgx::DWGGeometries::Ellipse * ) geom;
            ellipses_count++;
        }
        else if ( geom->sGeometryType == libdwgx::DWGGeometries::DWGGeometryType::LINE )
        {
            auto geom1 = ( libdwgx::DWGGeometries::Line * ) geom;
//            geom1->printInfo ();
            lines_count++;
        }
        else if ( geom->sGeometryType == libdwgx::DWGGeometries::DWGGeometryType::LWPOLYLINE )
        {
            auto geom1 = ( libdwgx::DWGGeometries::LWPolyline * ) geom;
//            geom1->printInfo ();
            pline_count++;
        }
        else if ( geom->sGeometryType == libdwgx::DWGGeometries::DWGGeometryType::ARC )
        {
            auto geom1 = ( libdwgx::DWGGeometries::Arc * ) geom;
//            geom1->printInfo ();
            arc_count++;
        }
        else if ( geom->sGeometryType == libdwgx::DWGGeometries::DWGGeometryType::TEXT )
        {
            auto geom1 = ( libdwgx::DWGGeometries::Text * ) geom;
            text_count++;
        }
    }

    std::cout << "Points: " << point_count << std::endl;
    std::cout << "Ellipses: " << ellipses_count << std::endl;
    std::cout << "Lines count: " << lines_count << std::endl;
    std::cout << "Plines count: " << pline_count << std::endl;
    std::cout << "Circles count: " << circles_count << std::endl;
    std::cout << "Arcs count: " << arc_count << std::endl;
    std::cout << "Texts count: " << text_count << std::endl;
}
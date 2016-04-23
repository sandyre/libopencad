#include "include/libopencad_api.h"
#include <iostream>
using namespace libopencad;

int main(int argc, char *argv[])
{
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/house_sample.dwg" );
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/г.Крымск.dwg" );
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/Иркутск_обнов2.dwg" );
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/Ситплан КГКМ.dwg" );
    CADFile *cadfile = OpenCADFile ( "/users/Aleksandr/libdwgx/samples/circle_2000.dwg" );
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/polyline2d_line_2000.dwg" );
//    DWGFile *dwgfile = libdwgx::InitializeDWG( "/users/Aleksandr/libdwgx/dwg_samples/mysamples/triple_circle.dwg" );
//    CADFile *cadfile = OpenCADFile ( "/users/Aleksandr/libdwgx/samples/mysamples/polyline3d.dwg" );
    if (cadfile == nullptr) return 0;
    int g = cadfile->getGeometriesCount ();
    int circles_count = 0;
    int lines_count = 0;
    int ellipses_count = 0;
    int pline_count = 0;
    int point_count = 0;
    int arc_count = 0;
    int text_count = 0;
    std::cout << "Geometries readed: " << g << std::endl;
    for(auto i = 0; i < g; ++i )
    {
        auto geom = cadfile->getGeometry (i);
        if ( geom->sGeometryType == CADGeometries::CADGeometryType::CIRCLE )
        {
            auto geom1 = ( CADGeometries::Circle * ) geom;
//            geom1->printInfo ();
            circles_count++;
        }
        else if ( geom->sGeometryType == CADGeometries::CADGeometryType::POINT )
        {
            auto geom1 = ( CADGeometries::Point * ) geom;
//            geom1->printInfo ();
            point_count++;
        }
        else if ( geom->sGeometryType == CADGeometries::CADGeometryType::ELLIPSE)
        {
            auto geom1 = ( CADGeometries::Ellipse * ) geom;
            ellipses_count++;
        }
        else if ( geom->sGeometryType == CADGeometries::CADGeometryType::LINE )
        {
            auto geom1 = ( CADGeometries::Line * ) geom;
//            geom1->printInfo ();
            lines_count++;
        }
        else if ( geom->sGeometryType == CADGeometries::CADGeometryType::LWPOLYLINE )
        {
            auto geom1 = ( CADGeometries::LWPolyline * ) geom;
//            geom1->printInfo ();
            pline_count++;
        }
        else if ( geom->sGeometryType == CADGeometries::CADGeometryType::ARC )
        {
            auto geom1 = ( CADGeometries::Arc * ) geom;
//            geom1->printInfo ();
            arc_count++;
        }
        else if ( geom->sGeometryType == CADGeometries::CADGeometryType::TEXT )
        {
            auto geom1 = ( CADGeometries::Text * ) geom;
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
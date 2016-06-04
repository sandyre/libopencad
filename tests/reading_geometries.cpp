#include "gtest/gtest.h"
#include "opencad_api.h"
#include "cadgeometries.h"

TEST(reading_circles, triplet)
{
    auto opened_dwg = OpenCADFile (GetDeafultFileIO("./data/r2000/triple_circles.dwg"));

    ASSERT_EQ (opened_dwg->GetLayersCount (), 1);
    // First circle. Should be 0,0,0 (x,y,z)
    // Radius 16.6 Thickness 1.2
    Layer * layer = opened_dwg->GetLayer (0);
    CADGeometry * geometry = layer->GetGeometry (0);
    ASSERT_EQ (geometry->GetType(), CADGeometry::CADGeometryType::CIRCLE);
    Circle *circle = ( Circle * ) geometry;
    ASSERT_NEAR (circle->vertPosition.X, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->vertPosition.Y, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->vertPosition.Z, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfRadius, 16.6f, 0.0001f);
    ASSERT_NEAR (circle->GetThickness(), 1.2f, 0.0001f);

    // Second circle. Should be 10,10,10 (x,y,z)
    // Radius 10 Thickness 1.8

    geometry = layer->GetGeometry (1);
    ASSERT_EQ (geometry->GetType(), CADGeometry::CADGeometryType::CIRCLE);
    circle = ( Circle * ) geometry;

    ASSERT_NEAR (circle->vertPosition.X, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->vertPosition.Y, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->vertPosition.Z, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->dfRadius, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->GetThickness(), 1.8f, 0.0001f);

    // Third circle. Should be -15,0,0 (x,y,z)
    // Radius 9.5 Thickness 0.8

    geometry = layer->GetGeometry (2);
    ASSERT_EQ (geometry->GetType(), CADGeometry::CADGeometryType::CIRCLE);
    circle = ( Circle * ) geometry;

    ASSERT_NEAR (circle->vertPosition.X, -15.0f, 0.0001f);
    ASSERT_NEAR (circle->vertPosition.Y, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->vertPosition.Z, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfRadius, 9.5f, 0.0001f);
    ASSERT_NEAR (circle->GetThickness(), 0.8f, 0.0001f);
}

// Following test demonstrates reading only actual geometries (deleted skipped).
TEST(reading_geometries, 24127_circles_128_lines)
{
    auto opened_dwg = OpenCADFile (GetDeafultFileIO("./data/r2000/24127_circles_128_lines.dwg"));
    auto circles_count = 0;
    auto lines_count = 0;

    ASSERT_EQ (opened_dwg->GetLayersCount (), 1);
    CADGeometry * geom;
    Layer * layer = opened_dwg->GetLayer (0);
    for ( auto i = 0; i < layer->GetGeometriesCount (); ++i )
    {
        geom = layer->GetGeometry (i);
        if ( geom->GetType() == CADGeometry::CADGeometryType::CIRCLE )
            ++circles_count;
        else if ( geom->GetType() == CADGeometry::CADGeometryType::LINE )
            ++lines_count;
        delete( geom );
    }

    ASSERT_EQ (circles_count, 24127);
    ASSERT_EQ (lines_count, 128);
    delete( layer );
    delete( opened_dwg );
}

TEST(reading_geometries, 256_polylines_7vertexes)
{
    auto opened_dwg = OpenCADFile (GetDeafultFileIO("./data/r2000/256_lwpolylines_7vertexes.dwg"));
    auto lwplines_count = 0;

    ASSERT_EQ (opened_dwg->GetLayersCount (), 1);

    CADGeometry * geom;
    Layer * layer = opened_dwg->GetLayer (0);
    for ( auto i = 0; i < layer->GetGeometriesCount (); ++i )
    {
        geom = layer->GetGeometry (i);
        if ( geom->GetType() == CADGeometry::CADGeometryType::LWPOLYLINE )
            ++lwplines_count;

        LWPolyline * poly = ( LWPolyline * ) geom;
        ASSERT_EQ( poly->vertexes.size(), 7);
        delete( geom );
    }

    ASSERT_EQ (256, lwplines_count);
    delete( layer );
    delete( opened_dwg );
}

TEST(reading_geometries, 18432_3dpolylines_6vertexes)
{
    auto opened_dwg = OpenCADFile (GetDeafultFileIO("./data/r2000/18432_3dpolylines_6vertexes.dwg"));
    auto plines3d_count = 0;

    ASSERT_EQ (opened_dwg->GetLayersCount (), 1);

    CADGeometry * geom;
    Layer * layer = opened_dwg->GetLayer (0);
    for ( auto i = 0; i < layer->GetGeometriesCount (); ++i )
    {
        geom = layer->GetGeometry (i);
        if ( geom->GetType() == CADGeometry::CADGeometryType::POLYLINE3D )
            ++plines3d_count;

        Polyline3D * poly = ( Polyline3D * ) geom;
        ASSERT_EQ( poly->vertexes.size(), 6);
        delete( geom );
    }

    ASSERT_EQ (18432, plines3d_count);
    delete( layer );
    delete( opened_dwg );
}

// TODO: Test isnt done fully.
TEST(reading_geometries, six_3dpolylines)
{
    auto opened_dwg = OpenCADFile ( GetDeafultFileIO ("./data/r2000/six_3dpolylines.dwg") ) ;
    ASSERT_EQ (opened_dwg->GetLayersCount (), 1);
    Layer * layer = opened_dwg->GetLayer (0);
    ASSERT_EQ (layer->GetGeometriesCount (), 6);

    /* First polyline3d
     * Closed? : false
     * Vertexes:
     * #0 X: 2.5116 Y: 5.0490 Z: 0
     * #1 X: 2.5116 Y: 17.4940 Z: 0
     * #2 X: 9.9976 Y: 17.5660 Z: 0
     * #3 X: 9.9976 Y: 12.5664 Z: 0
     * #4 X: 7.5503 Y: 12.5304 Z: 0
     * #5 X: 7.5503 Y: 15.0482 Z: 0
     * #6 X: 3.6633 Y: 14.9762 Z: 0
     * #7 X: 7.6273 Y: 7.2071 Z: 0
     * #8 X: 7.1544 Y: 7.3150 Z: 0
     * #9 X: 7.2623 Y: 10.1565 Z: 0
     * #10 X: 10.1416 Y: 9.9767 Z: 0
     * #11 X: 8.7380 Y: 2.9269 Z: 0
     * #12 X: 2.2957 Y: 2.9629 Z: 0
     * #13 X: 2.5116 Y: 5.0850 Z: 0 */
    CADGeometry * geometry = layer->GetGeometry (0);
    ASSERT_EQ (geometry->GetType(), CADGeometry::CADGeometryType::POLYLINE3D);
    Polyline3D * polyline3D = ( Polyline3D * ) geometry;

    Vertex3D vertex_readed = polyline3D->vertexes[0];
    ASSERT_NEAR ( vertex_readed.X, 2.5116, 0.0001f );
    ASSERT_NEAR ( vertex_readed.Y, 5.0490, 0.0001f );
    ASSERT_NEAR ( vertex_readed.Z, 0, 0.0001f );

    vertex_readed = polyline3D->vertexes[1];
    ASSERT_NEAR ( vertex_readed.X, 2.5116, 0.0001f );
    ASSERT_NEAR ( vertex_readed.Y, 17.4940, 0.0001f );
    ASSERT_NEAR ( vertex_readed.Z, 0, 0.0001f );

    vertex_readed = polyline3D->vertexes[2];
    ASSERT_NEAR ( vertex_readed.X, 9.9976, 0.0001f );
    ASSERT_NEAR ( vertex_readed.Y, 17.5660, 0.0001f );
    ASSERT_NEAR ( vertex_readed.Z, 0, 0.0001f );

    vertex_readed = polyline3D->vertexes[3];
    ASSERT_NEAR ( vertex_readed.X, 9.9976, 0.0001f );
    ASSERT_NEAR ( vertex_readed.Y, 12.5664, 0.0001f );
    ASSERT_NEAR ( vertex_readed.Z, 0, 0.0001f );

    vertex_readed = polyline3D->vertexes[4];
    ASSERT_NEAR ( vertex_readed.X, 7.5503, 0.0001f );
    ASSERT_NEAR ( vertex_readed.Y, 12.5304, 0.0001f );
    ASSERT_NEAR ( vertex_readed.Z, 0, 0.0001f );

    delete polyline3D;
}


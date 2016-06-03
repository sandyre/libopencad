#include "gtest/gtest.h"
#include "opencad_api.h"
#include "cadgeometry.h"

TEST(reading_circles, triplet)
{
    auto opened_dwg = OpenCADFile ("./data/r2000/triple_circles.dwg",
                                   CADFile::OpenOptions::READ_ALL);

    ASSERT_EQ (opened_dwg->getLayersCount (), 1);
    // First circle. Should be 0,0,0 (x,y,z)
    // Radius 16.6 Thickness 1.2
    CADLayer &layer = opened_dwg->getLayer (0);
    CADGeometry * geometry = layer.getGeometry (0);
    ASSERT_EQ (geometry->getType(), CADGeometry::GeometryType::CIRCLE);
    CADCircle *circle = ( CADCircle * ) geometry;
    ASSERT_NEAR (circle->getPosition().getX(), 0.0f, 0.0001f);
    ASSERT_NEAR (circle->getPosition().getY(), 0.0f, 0.0001f);
    ASSERT_NEAR (circle->getPosition().getZ(), 0.0f, 0.0001f);
    ASSERT_NEAR (circle->getRadius(), 16.6f, 0.0001f);
    ASSERT_NEAR (circle->getThickness(), 1.2f, 0.0001f);
    delete geometry;

    // Second circle. Should be 10,10,10 (x,y,z)
    // Radius 10 Thickness 1.8

    geometry = layer.getGeometry (1);
    ASSERT_EQ (geometry->getType(), CADGeometry::GeometryType::CIRCLE);
    circle = ( CADCircle * ) geometry;

    ASSERT_NEAR (circle->getPosition().getX(), 10.0f, 0.0001f);
    ASSERT_NEAR (circle->getPosition().getY(), 10.0f, 0.0001f);
    ASSERT_NEAR (circle->getPosition().getZ(), 10.0f, 0.0001f);
    ASSERT_NEAR (circle->getRadius(), 10.0f, 0.0001f);
    ASSERT_NEAR (circle->getThickness(), 1.8f, 0.0001f);
    delete geometry;

    // Third circle. Should be -15,0,0 (x,y,z)
    // Radius 9.5 Thickness 0.8

    geometry = layer.getGeometry (2);
    ASSERT_EQ (geometry->getType(), CADGeometry::GeometryType::CIRCLE);
    circle = ( CADCircle * ) geometry;

    ASSERT_NEAR (circle->getPosition().getX(), -15.0f, 0.0001f);
    ASSERT_NEAR (circle->getPosition().getY(), 0.0f, 0.0001f);
    ASSERT_NEAR (circle->getPosition().getZ(), 0.0f, 0.0001f);
    ASSERT_NEAR (circle->getRadius(), 9.5f, 0.0001f);
    ASSERT_NEAR (circle->getThickness(), 0.8f, 0.0001f);
    delete geometry;

    delete( opened_dwg );
}

// Following test demonstrates reading only actual geometries (deleted skipped).
TEST(reading_geometries, 24127_circles_128_lines)
{
    auto opened_dwg = OpenCADFile ("./data/r2000/24127_circles_128_lines.dwg",
                                   CADFile::OpenOptions::READ_ALL);
    auto circles_count = 0;
    auto lines_count = 0;

    ASSERT_EQ (opened_dwg->getLayersCount (), 1);
    CADGeometry * geom;
    CADLayer &layer = opened_dwg->getLayer (0);
    for ( auto i = 0; i < layer.getGeometryCount (); ++i )
    {
        geom = layer.getGeometry (i);
        if ( geom->getType() == CADGeometry::GeometryType::CIRCLE )
            ++circles_count;
        else if ( geom->getType() == CADGeometry::GeometryType::LINE )
            ++lines_count;
        delete geom;
    }

    ASSERT_EQ (circles_count, 24127);
    ASSERT_EQ (lines_count, 128);
    delete( opened_dwg );
}

// TODO: Test isnt done fully.
TEST(reading_geometries, six_3dpolylines)
{
    auto opened_dwg = OpenCADFile ( "./data/r2000/six_3dpolylines.dwg",
                                    CADFile::OpenOptions::READ_ALL) ;
    ASSERT_EQ (opened_dwg->getLayersCount (), 1);
    CADLayer &layer = opened_dwg->getLayer (0);
    ASSERT_EQ (layer.getGeometryCount (), 6);

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
    CADGeometry * geometry = layer.getGeometry (0);
    ASSERT_EQ (geometry->getType(), CADGeometry::GeometryType::POLYLINE3D);
    /*TODO: Vertex3D vertex_readed = polyline3D->vertexes[0];
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
    ASSERT_NEAR ( vertex_readed.Z, 0, 0.0001f );*/

    delete geometry;
}


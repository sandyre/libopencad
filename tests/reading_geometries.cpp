#include "gtest/gtest.h"
#include "opencad_api.h"
#include "cadgeometry.h"

// Following test demonstrates reading only actual geometries (deleted skipped).

TEST(reading_geometries, 24127_circles_128_lines)
{
    auto openedDwg = OpenCADFile ("./data/r2000/24127_circles_128_lines.dwg",
                                   CADFile::OpenOptions::READ_FAST);

    ASSERT_NE (openedDwg, nullptr);
    auto circles_count = 0;
    auto lines_count = 0;

    ASSERT_EQ (openedDwg->GetLayersCount (), 1);
    CADGeometry * geom;
    CADLayer &layer = openedDwg->GetLayer (0);
    for ( size_t i = 0; i < layer.getGeometryCount (); ++i )
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
    delete openedDwg;
}


TEST(reading_geometries, 256_polylines_7vertexes)
{
    auto opened_dwg = OpenCADFile ("./data/r2000/256_lwpolylines_7vertexes.dwg",
                                   CADFile::OpenOptions::READ_FAST);
    ASSERT_NE (opened_dwg, nullptr);
    auto lwplines_count = 0;

    ASSERT_EQ (opened_dwg->GetLayersCount (), 1);

    CADGeometry * geom;
    CADLayer &layer = opened_dwg->GetLayer (0);
    for ( size_t i = 0; i < layer.getGeometryCount (); ++i )
    {
        geom = layer.getGeometry (i);
        if ( geom->getType() == CADGeometry::LWPOLYLINE ){
            ++lwplines_count;

            CADLWPolyline * poly = static_cast<CADLWPolyline*>(geom);
            ASSERT_EQ( poly->getVertexCount(), 7);
        }
        delete geom;
    }

    ASSERT_EQ (256, lwplines_count);

    delete opened_dwg;
}

TEST(reading_geometries, 18432_3dpolylines_6vertexes)
{
    auto opened_dwg = OpenCADFile ("./data/r2000/18432_3dpolylines_6vertexes.dwg",
                                   CADFile::OpenOptions::READ_FAST);
    ASSERT_NE (opened_dwg, nullptr);
    auto plines3d_count = 0;

    ASSERT_EQ (opened_dwg->GetLayersCount (), 1);

    CADLayer &layer = opened_dwg->GetLayer (0);
    CADGeometry * geom;
    for ( size_t i = 0; i < layer.getGeometryCount (); ++i )
    {
        geom = layer.getGeometry (i);
        if ( geom->getType() == CADGeometry::POLYLINE3D ) {
            ++plines3d_count;

            CADPolyline3D * poly = static_cast<CADPolyline3D*>(geom);
            ASSERT_EQ( poly->getVertexCount (), 6);
        }
        delete geom;
    }

    ASSERT_EQ (18432, plines3d_count);
    delete opened_dwg;
}

// TODO: Test isn't done fully.
TEST(reading_geometries, six_3dpolylines)
{
    auto openedDwg = OpenCADFile ( "./data/r2000/six_3dpolylines.dwg",
                                    CADFile::OpenOptions::READ_FAST) ;
    ASSERT_NE (openedDwg, nullptr);

    ASSERT_EQ (openedDwg->GetLayersCount (), 1);
    CADLayer &layer = openedDwg->GetLayer (0);
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
    CADPolyline3D *polyline3D = static_cast<CADPolyline3D*>(geometry);

    ASSERT_GE (polyline3D->getVertexCount (), 5);

    const CADVector &vertexReaded0 = polyline3D->getVertex(0);
    ASSERT_NEAR ( vertexReaded0.getX(), 2.5116, 0.0001f );
    ASSERT_NEAR ( vertexReaded0.getY(), 5.0490, 0.0001f );
    ASSERT_NEAR ( vertexReaded0.getZ(), 0, 0.0001f );

    const CADVector &vertexReaded1 = polyline3D->getVertex(1);
    ASSERT_NEAR ( vertexReaded1.getX(), 2.5116, 0.0001f );
    ASSERT_NEAR ( vertexReaded1.getY(), 17.4940, 0.0001f );
    ASSERT_NEAR ( vertexReaded1.getZ(), 0, 0.0001f );

    const CADVector &vertexReaded2 = polyline3D->getVertex(2);
    ASSERT_NEAR ( vertexReaded2.getX(), 9.9976, 0.0001f );
    ASSERT_NEAR ( vertexReaded2.getY(), 17.5660, 0.0001f );
    ASSERT_NEAR ( vertexReaded2.getZ(), 0, 0.0001f );

    const CADVector &vertexReaded3 = polyline3D->getVertex(3);
    ASSERT_NEAR ( vertexReaded3.getX(), 9.9976, 0.0001f );
    ASSERT_NEAR ( vertexReaded3.getY(), 12.5664, 0.0001f );
    ASSERT_NEAR ( vertexReaded3.getZ(), 0, 0.0001f );

    const CADVector &vertexReaded4 = polyline3D->getVertex(4);
    ASSERT_NEAR ( vertexReaded4.getX(), 7.5503, 0.0001f );
    ASSERT_NEAR ( vertexReaded4.getY(), 12.5304, 0.0001f );
    ASSERT_NEAR ( vertexReaded4.getZ(), 0, 0.0001f );

    delete geometry;

    delete openedDwg;
}

TEST(reading_circles, triplet)
{
    auto openedDwg = OpenCADFile ("./data/r2000/triple_circles.dwg",
                                   CADFile::OpenOptions::READ_FAST);
    ASSERT_NE (openedDwg, nullptr);

    ASSERT_EQ (openedDwg->GetLayersCount (), 1);
    // First circle. Should be 0,0,0 (x,y,z)
    // Radius 16.6 Thickness 1.2
    CADLayer &layer = openedDwg->GetLayer (0);

    ASSERT_GE (layer.getGeometryCount (), 3);

    CADGeometry * geometry = layer.getGeometry (0);
    ASSERT_EQ (geometry->getType(), CADGeometry::GeometryType::CIRCLE);
    CADCircle *circle;
    circle = static_cast<CADCircle *>( geometry );
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
    circle = static_cast<CADCircle *>( geometry );

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
    circle = static_cast<CADCircle *>( geometry );

    ASSERT_NEAR (circle->getPosition().getX(), -15.0f, 0.0001f);
    ASSERT_NEAR (circle->getPosition().getY(), 0.0f, 0.0001f);
    ASSERT_NEAR (circle->getPosition().getZ(), 0.0f, 0.0001f);
    ASSERT_NEAR (circle->getRadius(), 9.5f, 0.0001f);
    ASSERT_NEAR (circle->getThickness(), 0.8f, 0.0001f);
    delete geometry;

    delete openedDwg;
}

TEST(reading_geometries, 3rays_5xlines)
{
    auto opened_dwg = OpenCADFile ("./data/r2000/5rays_3xlines.dwg",
                                   CADFile::OpenOptions::READ_FAST);
    ASSERT_NE (opened_dwg, nullptr);

    auto rays_count = 0;
    auto xlines_count = 0;

    ASSERT_EQ (opened_dwg->GetLayersCount (), 1);

    CADLayer &layer = opened_dwg->GetLayer (0);
    ASSERT_EQ ( layer.getGeometryCount(), 8 );

    CADGeometry * geom;
    for ( size_t i = 0; i < layer.getGeometryCount (); ++i )
    {
        geom = layer.getGeometry (i);
        if ( geom->getType() == CADGeometry::XLINE )
        {
            ++xlines_count;
        }
        if ( geom->getType() == CADGeometry::RAY )
        {
            ++rays_count;
        }

        delete geom;
    }

    ASSERT_EQ (5, rays_count);
    ASSERT_EQ (3, xlines_count);
    delete opened_dwg;
}

TEST(reading_geometries, 4solid)
{
    auto opened_dwg = OpenCADFile ("./data/r2000/4solids.dwg",
                                   CADFile::OpenOptions::READ_FAST);
    ASSERT_NE (opened_dwg, nullptr);

    auto solids_count = 0;

    ASSERT_EQ (opened_dwg->GetLayersCount (), 1);

    CADLayer &layer = opened_dwg->GetLayer (0);
    ASSERT_EQ ( layer.getGeometryCount(), 4 );

    CADGeometry * geom;
    for ( size_t i = 0; i < layer.getGeometryCount (); ++i )
    {
        geom = layer.getGeometry (i);
        if ( geom->getType() == CADGeometry::SOLID )
        {
            ++solids_count;
        }

        delete geom;
    }

    ASSERT_EQ (4, solids_count);
    delete opened_dwg;
}

TEST(reading_geometries, 1arc)
{
    auto opened_dwg = OpenCADFile ("./data/r2000/1arc.dwg",
                                   CADFile::OpenOptions::READ_FAST);
    ASSERT_NE (opened_dwg, nullptr);

    auto arcs_count = 0;

    ASSERT_EQ (opened_dwg->GetLayersCount (), 1);

    CADLayer &layer = opened_dwg->GetLayer (0);
    ASSERT_EQ ( layer.getGeometryCount(), 1 );

    CADGeometry * geom;
    for ( size_t i = 0; i < layer.getGeometryCount (); ++i )
    {
        geom = layer.getGeometry (i);
        if ( geom->getType() == CADGeometry::ARC )
        {
            ++arcs_count;
        }

        delete geom;
    }

    ASSERT_EQ (1, arcs_count);
    delete opened_dwg;
}


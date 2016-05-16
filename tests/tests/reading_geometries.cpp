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
    ASSERT_EQ (geometry->stGeometryType, CADGeometry::CADGeometryType::CIRCLE);
    Circle *circle = ( Circle * ) geometry;
    ASSERT_NEAR (circle->vertPosition.X, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->vertPosition.Y, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->vertPosition.Z, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfRadius, 16.6f, 0.0001f);
    ASSERT_NEAR (circle->dfThickness, 1.2f, 0.0001f);

    delete circle;

    // Second circle. Should be 10,10,10 (x,y,z)
    // Radius 10 Thickness 1.8

    geometry = layer->GetGeometry (1);
    ASSERT_EQ (geometry->stGeometryType, CADGeometry::CADGeometryType::CIRCLE);
    circle = ( Circle * ) geometry;

    ASSERT_NEAR (circle->vertPosition.X, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->vertPosition.Y, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->vertPosition.Z, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->dfRadius, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->dfThickness, 1.8f, 0.0001f);

    delete circle;

    // Third circle. Should be -15,0,0 (x,y,z)
    // Radius 9.5 Thickness 0.8

    geometry = layer->GetGeometry (2);
    ASSERT_EQ (geometry->stGeometryType, CADGeometry::CADGeometryType::CIRCLE);
    circle = ( Circle * ) geometry;

    ASSERT_NEAR (circle->vertPosition.X, -15.0f, 0.0001f);
    ASSERT_NEAR (circle->vertPosition.Y, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->vertPosition.Z, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfRadius, 9.5f, 0.0001f);
    ASSERT_NEAR (circle->dfThickness, 0.8f, 0.0001f);

    delete circle;
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
        if ( geom->stGeometryType == CADGeometry::CADGeometryType::CIRCLE )
            ++circles_count;
        else if ( geom->stGeometryType == CADGeometry::CADGeometryType::LINE )
            ++lines_count;
        delete( geom );
    }

    ASSERT_EQ (circles_count, 24127);
    ASSERT_EQ (lines_count, 128);
    delete( layer );
    delete( opened_dwg );
}

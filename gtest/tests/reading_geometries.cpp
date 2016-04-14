#include "gtest/gtest.h"
#include "libdwgx.h"

TEST(reading_circles, triplet)
{
    using namespace libdwgx::DWGGeometries;
    using namespace std;
    auto opened_dwg = libdwgx::InitializeDWG ("./data/triple_circles.dwg");
    opened_dwg->ReadHeader ();
    opened_dwg->ReadObjectMap ();

    // First circle. Should be 0,0,0 (x,y,z)
    // Radius 16.6 Thickness 1.2
    Geometry *geometry = opened_dwg->ReadGeometry (0);
    ASSERT_EQ (geometry->sGeometryType, "Circle");
    Circle *circle = ( Circle * ) geometry;
    ASSERT_NEAR (circle->dfCenterX, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfCenterY, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfCenterZ, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfRadius, 16.6f, 0.0001f);
    ASSERT_NEAR (circle->dfThickness, 1.2f, 0.0001f);

    delete circle;

    // Second circle. Should be 10,10,10 (x,y,z)
    // Radius 10 Thickness 1.8

    geometry = opened_dwg->ReadGeometry (1);
    ASSERT_EQ (geometry->sGeometryType, "Circle");
    circle = ( Circle * ) geometry;

    ASSERT_NEAR (circle->dfCenterX, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->dfCenterY, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->dfCenterZ, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->dfRadius, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->dfThickness, 1.8f, 0.0001f);

    delete circle;

    // Third circle. Should be -15,0,0 (x,y,z)
    // Radius 9.5 Thickness 0.8

    geometry = opened_dwg->ReadGeometry (2);
    ASSERT_EQ (geometry->sGeometryType, "Circle");
    circle = ( Circle * ) geometry;

    ASSERT_NEAR (circle->dfCenterX, -15.0f, 0.0001f);
    ASSERT_NEAR (circle->dfCenterY, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfCenterZ, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfRadius, 9.5f, 0.0001f);
    ASSERT_NEAR (circle->dfThickness, 0.8f, 0.0001f);

    delete circle;


}
#include "gtest/gtest.h"
#include "../../lib/opencad_api.h"

TEST(reading_circles, triplet)
{
    using namespace std;
    auto opened_dwg = OpenCADFile ("./data/triple_circles.dwg");

    // First circle. Should be 0,0,0 (x,y,z)
    // Radius 16.6 Thickness 1.2
    CADGeometry *geometry = opened_dwg->getGeometry (0);
    ASSERT_EQ (geometry->stGeometryType, CADGeometry::CADGeometryType::CIRCLE);
    Circle *circle = ( Circle * ) geometry;
    ASSERT_NEAR (circle->dfCenterX, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfCenterY, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfCenterZ, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfRadius, 16.6f, 0.0001f);
    ASSERT_NEAR (circle->dfThickness, 1.2f, 0.0001f);

    delete circle;

    // Second circle. Should be 10,10,10 (x,y,z)
    // Radius 10 Thickness 1.8

    geometry = opened_dwg->getGeometry (1);
    ASSERT_EQ (geometry->stGeometryType, CADGeometry::CADGeometryType::CIRCLE);
    circle = ( Circle * ) geometry;

    ASSERT_NEAR (circle->dfCenterX, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->dfCenterY, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->dfCenterZ, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->dfRadius, 10.0f, 0.0001f);
    ASSERT_NEAR (circle->dfThickness, 1.8f, 0.0001f);

    delete circle;

    // Third circle. Should be -15,0,0 (x,y,z)
    // Radius 9.5 Thickness 0.8

    geometry = opened_dwg->getGeometry (2);
    ASSERT_EQ (geometry->stGeometryType, CADGeometry::CADGeometryType::CIRCLE);
    circle = ( Circle * ) geometry;

    ASSERT_NEAR (circle->dfCenterX, -15.0f, 0.0001f);
    ASSERT_NEAR (circle->dfCenterY, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfCenterZ, 0.0f, 0.0001f);
    ASSERT_NEAR (circle->dfRadius, 9.5f, 0.0001f);
    ASSERT_NEAR (circle->dfThickness, 0.8f, 0.0001f);

    delete circle;
}
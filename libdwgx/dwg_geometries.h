#ifndef LIBDWGX_DWG_GEOMETRIES_H
#define LIBDWGX_DWG_GEOMETRIES_H

#include <string>
#include <iostream>

namespace libdwgx
{
namespace DWGGeometries
{

enum DWGGeometryType
{
    POINT, CIRCLE, LWPOLYLINE, ELLIPSE, LINE,
    POLYLINE3D
};

// TODO: for every geometry type, printinfo() should be replaced with overloaded << operator.

class Geometry
{
public:
    DWGGeometryType sGeometryType;
    double          dfThickness;
};

class Point : public Geometry
{
public:
    Point () : dfPointX (0.0f),
               dfPointY (0.0f),
               dfPointZ (0.0f),
               dfExtrusionX (0.0f),
               dfExtrusionY (0.0f),
               dfExtrusionZ (0.0f),
               dfXAxisAng (0.0f)
    {
        sGeometryType = DWGGeometryType::POINT;
    }

    void printInfo()
    {
        std::cout << "Point\nCoords:\t" << dfPointX << " " << dfPointY << " "
               << dfPointZ << "\nExtrusion:\t " << dfExtrusionX << " "
               << dfExtrusionY << " " << dfExtrusionZ
               << "\nAxis angle:\t" << dfXAxisAng
               << "\nThickness:\t" << dfThickness << std::endl;
    }

    double dfPointX;
    double dfPointY;
    double dfPointZ;
    double dfExtrusionX;
    double dfExtrusionY;
    double dfExtrusionZ;
    double dfXAxisAng;
};

class Line : public Geometry
{
public:
    Line() : dfStartX(0.0f),
             dfStartY(0.0f),
             dfStartZ(0.0f),
             dfEndX(0.0f),
             dfEndY(0.0f),
             dfEndZ(0.0f),
             dfExtrusionX(0.0f),
             dfExtrusionY(0.0f),
             dfExtrusionZ(0.0f)
    {
        sGeometryType = DWGGeometryType::LINE;
    }

    void printInfo()
    {
        std::cout << "\nLine\nCoords start: \t" << dfStartX << " " << dfStartY << " "
        << dfStartZ << "\nCoords end: \t" << dfEndX << " " << dfEndY << " " << dfEndZ
        << "\nExtrusion: \t " << dfExtrusionX << " "
        << dfExtrusionY << " " << dfExtrusionZ
        << "\nThickness: \t" << dfThickness << std::endl;
    }

    double dfStartX;
    double dfStartY;
    double dfStartZ;
    double dfEndX;
    double dfEndY;
    double dfEndZ;
    double dfExtrusionX;
    double dfExtrusionY;
    double dfExtrusionZ;
};

class LWPolyline : public Geometry
{
public:
    LWPolyline ()
    {
        sGeometryType = DWGGeometryType::LWPOLYLINE;
    }
};

class Polyline3D : public Geometry
{
public:
    Polyline3D()
    {
        sGeometryType = DWGGeometryType::POLYLINE3D;
    }
};

class Circle : public Geometry
{
public:
    Circle () : dfCenterX (0.0f),
                dfCenterY (0.0f),
                dfCenterZ (0.0f),
                dfExtrusionX (0.0f),
                dfExtrusionY (0.0f),
                dfExtrusionZ (0.0f),
                dfRadius (0.0f)
    {
        sGeometryType = DWGGeometryType::CIRCLE;
    }

    void printInfo()
    {
        std::cout << "\nCircle\nCoords: \t" << dfCenterX << " " << dfCenterY << " "
        << dfCenterY << "\nExtrusion: \t " << dfExtrusionX << " "
        << dfExtrusionY << " " << dfExtrusionZ
        << "\nRadius:\t" << dfRadius
        << "\nThickness:\t" << dfThickness << std::endl;
    }

    double dfCenterX;
    double dfCenterY;
    double dfCenterZ;
    double dfExtrusionX;
    double dfExtrusionY;
    double dfExtrusionZ;
    double dfRadius;
};

class Ellipse : public Geometry
{
public:
    Ellipse() : dfCenterX(0.0f),
                dfCenterY(0.0f),
                dfCenterZ(0.0f),
                dfWCSX(0.0f),
                dfWCSY(0.0f),
                dfWCSZ(0.0f),
                dfExtrusionX(0.0f),
                dfExtrusionY(0.0f),
                dfExtrusionZ(0.0f),
                dfAxisRatio(0.0f),
                dfStartingAngle(0.0f),
                dfEndingAngle(0.0f)
    {
        sGeometryType = DWGGeometryType::ELLIPSE;
    }

    double dfCenterX;
    double dfCenterY;
    double dfCenterZ;
    double dfWCSX;
    double dfWCSY;
    double dfWCSZ;
    double dfExtrusionX;
    double dfExtrusionY;
    double dfExtrusionZ;
    double dfAxisRatio;
    double dfStartingAngle;
    double dfEndingAngle;
};

}
}

#endif //LIBDWGX_DWG_GEOMETRIES_H

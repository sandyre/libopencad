#ifndef LIBDWGX_DWG_GEOMETRIES_H
#define LIBDWGX_DWG_GEOMETRIES_H

#include <string>

namespace libdwgx
{
namespace DWGGeometries
{

enum DWGGeometryType
{
    POINT, CIRCLE, POLYLINE, ELLIPSE, LINE
};

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
    Line()
    {
        sGeometryType = DWGGeometryType::LINE;
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

class Polyline : public Geometry
{
public:
    Polyline ()
    {

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

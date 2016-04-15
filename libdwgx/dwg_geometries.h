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
    POLYLINE3D, TEXT, ARC
};

struct Vertex2D
{
    Vertex2D() : X(0.0f),
                 Y(0.0f)
    {
    }

    double X;
    double Y;
};

struct Vertex3D
{
    Vertex3D() : X(0.0f),
                 Y(0.0f),
                 Z(0.0f)
    {
    }

    double X;
    double Y;
    double Z;
};

struct Vector2D
{
    Vector2D() : X(0.0f),
                 Y(0.0f)
    {
    }

    double X;
    double Y;
};

struct Vector3D
{
    Vector3D() : X(0.0f),
                 Y(0.0f),
                 Z(0.0f)
    {
    }

    double X;
    double Y;
    double Z;
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
    LWPolyline () : dfConstWidth(0.0f),
                    dfElevation(0.0f),
                    dfExtrusionX(0.0f),
                    dfExtrusionY(0.0f),
                    dfExtrusionZ(0.0f)
    {
        sGeometryType = DWGGeometryType::LWPOLYLINE;
    }

    void printInfo()
    {
        std::cout << "\nLWPolyline"
                  << "\nConst width: " << dfConstWidth
                  << "\nElevation: " << dfElevation
                  << "\nExtrusion: " << dfExtrusionX << " "
                  << dfExtrusionY << " " << dfExtrusionZ
                  << "\nVertexes: " << std::endl;
        for ( size_t i = 0; i < vertexes.size(); ++i )
        {
            std::cout << "#" << i << " X: \t " << vertexes[i].X
                    << " Y: \t " << vertexes[i].Y << std::endl;
        }

        std::cout << "Bulges:\n";
        for ( size_t i = 0; i < bulges.size(); ++i )
        {
            std::cout << "#" << i << " X: \t " << bulges[i] << std::endl;
        }
        std::cout << std::endl;
    }

    double dfConstWidth;
    double dfElevation;
    double dfExtrusionX;
    double dfExtrusionY;
    double dfExtrusionZ;
    std::vector< Vertex2D > vertexes;
    std::vector< double > bulges;
    std::vector< int16_t > vertexes_id;
    std::vector< std::pair< double, double > > widths; // start, end.
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

class Text : public Geometry
{
public:
    Text()
    {
        sGeometryType = DWGGeometryType::TEXT;
    }
};

class Arc : public Geometry
{
public:
    Arc() : dfCenterX(0.0f),
            dfCenterY(0.0f),
            dfCenterZ(0.0f),
            dfRadius(0.0f),
            dfExtrusionX(0.0f),
            dfExtrusionY(0.0f),
            dfExtrusionZ(0.0f),
            dfStartingAngle(0.0f),
            dfEndingAngle(0.0f)
    {
        sGeometryType = DWGGeometryType::ARC;
    }

    double dfCenterX;
    double dfCenterY;
    double dfCenterZ;
    double dfRadius;
    double dfExtrusionX;
    double dfExtrusionY;
    double dfExtrusionZ;
    double dfStartingAngle;
    double dfEndingAngle;
};

class Block
{
public:
    Block()
    {
    }

    std::vector< Geometry > geometries;
};

class Layer
{
public:
    Layer()
    {
    }

    std::vector< Block > blocks;
};

}
}

#endif //LIBDWGX_DWG_GEOMETRIES_H

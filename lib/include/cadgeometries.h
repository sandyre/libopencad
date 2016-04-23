/************************************************************************************
 *  Name: cadgeometries.h
 *  Project: libOpenCAD OpenSource CAD formats support library
 *  Author: Alexandr Borzykh, mush3d at gmail.com
 *  Language: C++
 ************************************************************************************
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2016 Alexandr Borzykh
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 ************************************************************************************/

#ifndef LIB_CADGEOMETRIES_H
#define LIB_CADGEOMETRIES_H

#include <vector>
#include <string>

namespace libopencad
{
namespace CADGeometries
{

enum CADGeometryType
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


class CADGeometry
{
public:
    CADGeometryType sGeometryType;
    double          dfThickness;
};

class Point : public CADGeometry
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
        sGeometryType = CADGeometryType::POINT;
    }

    double dfPointX;
    double dfPointY;
    double dfPointZ;
    double dfExtrusionX;
    double dfExtrusionY;
    double dfExtrusionZ;
    double dfXAxisAng;
};

class Line : public CADGeometry
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
        sGeometryType = CADGeometryType::LINE;
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

class LWPolyline : public CADGeometry
{
public:
    LWPolyline () : dfConstWidth(0.0f),
                    dfElevation(0.0f),
                    dfExtrusionX(0.0f),
                    dfExtrusionY(0.0f),
                    dfExtrusionZ(0.0f)
    {
        sGeometryType = CADGeometryType::LWPOLYLINE;
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

class Polyline3D : public CADGeometry
{
public:
    Polyline3D()
    {
        sGeometryType = CADGeometryType::POLYLINE3D;
    }

    std::vector< Vertex3D > vertexes;
};

class Circle : public CADGeometry
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
        sGeometryType = CADGeometryType::CIRCLE;
    }

    double dfCenterX;
    double dfCenterY;
    double dfCenterZ;
    double dfExtrusionX;
    double dfExtrusionY;
    double dfExtrusionZ;
    double dfRadius;
};

class Ellipse : public CADGeometry
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
        sGeometryType = CADGeometryType::ELLIPSE;
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

class Text : public CADGeometry
{
public:
    Text() : dfElevation(0.0f),
             dfInsertionX(0.0f),
             dfInsertionY(0.0f),
             dfAlignmentX(0.0f),
             dfAlignmentY(0.0f),
             dfExtrusionX(0.0f),
             dfExtrusionY(0.0f),
             dfExtrusionZ(0.0f),
             dfObliqueAngle(0.0f),
             dfRotationAngle(0.0f),
             dfHeight(0.0f),
             dfWidthFactor(0.0f),
             dGeneration(0),
             dHorizontalAlignment(0),
             dVerticalAlignment(0)
    {
        sGeometryType = CADGeometryType::TEXT;
    }

    double dfElevation;
    double dfInsertionX;
    double dfInsertionY;
    double dfAlignmentX;
    double dfAlignmentY;
    double dfExtrusionX;
    double dfExtrusionY;
    double dfExtrusionZ;
    double dfObliqueAngle;
    double dfRotationAngle;
    double dfHeight;
    double dfWidthFactor;
    std::string strTextValue;
    int16_t dGeneration;
    int16_t dHorizontalAlignment;
    int16_t dVerticalAlignment;
};

class Arc : public CADGeometry
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
        sGeometryType = CADGeometryType::ARC;
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

class CADBlock
{
public:
    CADBlock()
    {
    }

    std::vector< CADGeometry > geometries;
};

class CADLayer
{
public:
    CADLayer()
    {
    }

    std::vector< CADBlock > blocks;
};

}
}

#endif //LIB_CADGEOMETRIES_H

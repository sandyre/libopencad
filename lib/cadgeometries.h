/*******************************************************************************
 *  Project: libopencad
 *  Purpose: OpenSource CAD formats support library
 *  Author: Alexandr Borzykh, mush3d at gmail.com
 *  Author: Dmitry Baryshnikov, bishop.dev@gmail.com
 *  Language: C++
 *******************************************************************************
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2016 Alexandr Borzykh
 *  Copyright (c) 2016 NextGIS, <info@nextgis.com>
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
 *******************************************************************************/

#ifndef CADGEOMETRIES_H
#define CADGEOMETRIES_H

#include <vector>
#include <string>
#include <stdint.h>

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

/**
 * @brief Base CAD geometry class
 */
class CADGeometry
{
 public:
    /**
     * @brief The CAD geometry types enum
     */
    enum CADGeometryType
    {
        POINT = 1,
        CIRCLE,
        LWPOLYLINE,
        ELLIPSE,
        LINE,
        POLYLINE3D,
        TEXT,
        ARC,
        SPLINE
    };

public: //TODO: protected
    CADGeometryType stGeometryType;
    double          dfThickness;
};

/**
 * @brief Geometry class which a single Point
 */
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
        stGeometryType = POINT;
    }

    double dfPointX;
    double dfPointY;
    double dfPointZ;
    double dfExtrusionX;
    double dfExtrusionY;
    double dfExtrusionZ;
    double dfXAxisAng;
};

/**
 * @brief Geometry class which represents a simple Line
 */
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
        stGeometryType = LINE;
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

/**
 * @brief Geometry class which represents Lwpolyline
 */
class LWPolyline : public CADGeometry
{
public:
    LWPolyline () : dfConstWidth(0.0f),
                    dfElevation(0.0f),
                    dfExtrusionX(0.0f),
                    dfExtrusionY(0.0f),
                    dfExtrusionZ(0.0f)
    {
        stGeometryType = LWPOLYLINE;
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

/**
 * @brief Geometry class which represents Polyline 3D
 */
class Polyline3D : public CADGeometry
{
public:
    Polyline3D()
    {
        stGeometryType = POLYLINE3D;
    }

    std::vector< Vertex3D > vertexes;
};

/**
 * @brief Geometry class which represents Circle
 */
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
        stGeometryType = CIRCLE;
    }

    double dfCenterX;
    double dfCenterY;
    double dfCenterZ;
    double dfExtrusionX;
    double dfExtrusionY;
    double dfExtrusionZ;
    double dfRadius;
};

/**
 * @brief Geometry class which represents Ellipse
 */
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
        stGeometryType = ELLIPSE;
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

/**
 * @brief Geometry class which represents Text
 */
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
        stGeometryType = TEXT;
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

/**
 * @brief Geometry class which represents Arc
 */
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
        stGeometryType = ARC;
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

/**
 * @brief Geometry class which represents Spline
 */
class Spline : public CADGeometry
{
public:
    // TODO: understand what should Spline consist of.
    Spline()
    {
        stGeometryType = SPLINE;
    }
};

//class CADLayer
//{
//public:
//    CADLayer()
//    {
//    }
//
//    std::string sLayerName;
//    bool bFrozen;
//    bool bOn;
//    bool bFrozenByDefaultInNewVPORT;
//    bool bLocked;
//    bool bPlottingFlag;
//    int16_t dLineWeight;
//    int16_t dColor;
//
//    CADHandle hHandle;
//
//    std::vector < CADObject * > objects;
//};


#endif // CADGEOMETRIES_H

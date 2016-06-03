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

#include "cadobjects.h"

using namespace std;

/**
 * @brief Base CAD geometry class
 */
class CADGeometry
{
 public:
    CADGeometry();
    virtual ~CADGeometry();
    /**
     * @brief The CAD geometry types enum
     */
    enum GeometryType
    {
        UNDEFINED = 0,
        POINT,
        CIRCLE,
        LWPOLYLINE,
        ELLIPSE,
        LINE,
        POLYLINE3D,
        TEXT,
        ARC,
        SPLINE,
        SOLID,
        RAY,
        HATCH // NOT IMPLEMENTED
    };

    enum GeometryType getType() const;
    double getThickness() const;
    void setThickness(double thicknes);

    virtual void print () const = 0;
protected:
    enum GeometryType geometryType;
    double          thickness;
};

/* TODO: Point3D should be named Point, but because of possible redefenitions
 * its named Point3D
 *
 */

/**
 * @brief Geometry class which a single Point
 */
class CADPoint3D : public CADGeometry
{
public:
    CADPoint3D ();
    CADPoint3D (const CADVector &positionIn, double thickness);
    CADVector getPosition() const;
    void setPosition(const CADVector &value);

    CADVector getExtrusion() const;
    void setExtrusion(const CADVector &value);

    double getXAxisAng() const;
    void setXAxisAng(double value);

    virtual void print () const override;
protected:
    CADVector position;
    CADVector extrusion;
    double xAxisAng;
};

/**
 * @brief Geometry class which represents a simple Line
 */
class CADLine : public CADGeometry
{
public:
    CADLine();
    CADLine(const CADPoint3D& startIn, const CADPoint3D& endIn);
    CADPoint3D getStart() const;
    void setStart(const CADPoint3D &value);

    CADPoint3D getEnd() const;
    void setEnd(const CADPoint3D &value);

    virtual void print () const override;

protected:
    CADPoint3D start;
    CADPoint3D end;
};

/**
 * @brief Geometry class which represents Lwpolyline
 */
/*
class CADLWPolyline : public CADGeometry
{
public:
    CADLWPolyline ();
protected:
    double dfConstWidth;
    double dfElevation;
    Vector3D vectExtrusion;
    vector< Vertex2D > vertexes;
    vector< double > bulges;
    vector< int16_t > vertexes_id;
    vector< pair< double, double > > widths; // start, end.
};

/**
 * @brief Geometry class which represents Polyline 3D
 *//*
class Polyline3D : public CADGeometry
{
public:
    Polyline3D()
    {
        eGeometryType = CADGeometry::POLYLINE3D;
    }

    std::vector< Vertex3D > vertexes;
};

/**
 * @brief Geometry class which represents Circle
 */
class CADCircle : public CADPoint3D
{
public:
    CADCircle ();

    double getRadius() const;
    void setRadius(double value);
    virtual void print () const override;

protected:
    double radius;
};

/**
 * @brief Geometry class which represents Ellipse
 *//*
class Ellipse : public CADGeometry
{
public:
    Ellipse() : dfAxisRatio(0.0f),
                dfStartingAngle(0.0f),
                dfEndingAngle(0.0f)
    {
        eGeometryType = CADGeometry::ELLIPSE;
    }

    Vertex3D vertPosition;
    Vector3D vectWCS;
    Vector3D vectExtrusion;
    double dfAxisRatio;
    double dfStartingAngle;
    double dfEndingAngle;
};

/**
 * @brief Geometry class which represents Text
 *//*
class Text : public CADGeometry
{
public:
    Text() : dfElevation(0.0f),
             dfObliqueAngle(0.0f),
             dfRotationAngle(0.0f),
             dfHeight(0.0f),
             dfWidthFactor(0.0f),
             dGeneration(0),
             dHorizontalAlignment(0),
             dVerticalAlignment(0)
    {
        eGeometryType = CADGeometry::TEXT;
    }

    double dfElevation;
    Vertex2D vertInsertion;
    Vertex2D vertAlignment;
    Vector3D vectExtrusion;
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
class CADArc : public CADCircle
{
public:
    CADArc();

    double getStartingAngle() const;
    void setStartingAngle(double value);

    double getEndingAngle() const;
    void setEndingAngle(double value);
    virtual void print () const override;

protected:
    double startingAngle;
    double endingAngle;
};

/**
 * @brief Geometry class which represents Spline
 *//*
class Spline : public CADGeometry
{
public:
    Spline()
    {
        eGeometryType = CADGeometry::SPLINE;
    }

    long dScenario;
    bool bRational;
    bool bClosed;
    bool bPeriodic;
    bool bWeight;

    long   dDegree;
    double dfFitTol;
    double dfKnotTol;
    double dfCtrlTol;
    Vector3D vectBegTangDir;
    Vector3D vectEndTangDir;

    std::vector < double > adfKnots;
    std::vector < double > adfCtrlPointsWeight;
    std::vector < Vertex3D > avertCtrlPoints;
    std::vector < Vertex3D > averFitPoints;
};

class Solid : public CADGeometry
{
public:
    Solid()
    {
        eGeometryType = CADGeometry::SOLID;
    }

    double dfElevation;
    std::vector < Vertex2D > avertCorners;
    Vector3D vectExtrusion;
};

class Ray : public CADGeometry
{
public:
    Ray()
    {
        eGeometryType = CADGeometry::RAY;
    }

    Vertex3D vertPosition;
    Vector3D vectVector;
};

class Hatch : public CADGeometry
{
public:
    Hatch()
    {
        eGeometryType = CADGeometry::HATCH;
    }
};
*/

//
//class EXTERN LineType
//{
//public:
//    std::string sEntryName;
//    std::string sDescription;
//    double dfPatternLen;
//    char dAlignment;
//    char nNumDashes;
//    struct Dash
//    {
//        double dfLength;
//        short dComplexShapecode;
//        double dfXOffset;
//        double dfYOffset;
//        double dfScale;
//        double dfRotation;
//        short dShapeflag;
//    };
//    std::vector < char > abyTextArea; // TODO: what is it?
//    std::vector < CADHandle > hShapefiles; // TODO: one for each dash?
//};

//class EXTERN Block
//{
//public:
//    Block(CADFile * pCADFile)
//    {
//        pstCADFile_m = pCADFile;
//    }
//
//    std::string sBlockName;
//
//    CADFile * pstCADFile_m;
//
//    std::vector < std::pair < long long, short > > astAttachedGeometries;
//};


#endif // CADGEOMETRIES_H

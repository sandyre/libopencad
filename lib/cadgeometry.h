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
    CADPoint3D (const CADVector &positionIn, double thicknessIn);
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
 * @brief Geometry class which represents Polyline 3D
 */
class CADPolyline3D : public CADGeometry
{
public:
    CADPolyline3D();
    void addVertex(const CADVector& vertex);
    size_t getVertexCount() const;
    CADVector& getVertex(size_t index);
    virtual void print () const override;
protected:
    vector<CADVector> vertexes;
};

/**
 * @brief Geometry class which represents Lwpolyline
 */

class CADLWPolyline : public CADPolyline3D
{
public:
    CADLWPolyline ();
    virtual void print () const override;
    double getConstWidth() const;
    void setConstWidth(double value);

    double getElevation() const;
    void setElevation(double value);

    CADVector getVectExtrusion() const;
    void setVectExtrusion(const CADVector &value);

    vector<pair<double, double> > getWidths() const;
    void setWidths(const vector<pair<double, double> > &value);

protected:
    double constWidth;
    double elevation;
    CADVector vectExtrusion;
    vector< pair< double, double > > widths; // start, end.
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
 * @brief Geometry class which represents Text
 */
class CADText : public CADPoint3D
{
public:
    CADText();
    string getTextValue() const;
    void setTextValue(const string &value);

    double getHeight() const;
    void setHeight(double value);

    double getRotationAngle() const;
    void setRotationAngle(double value);

    double getObliqueAngle() const;
    void setObliqueAngle(double value);
    virtual void print () const override;

protected:
    double obliqueAngle;
    double rotationAngle;
    double height;
    string textValue;
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
 * @brief Geometry class which represents Ellipse
 */
class CADEllipse : public CADArc
{
public:
    CADEllipse();
    double getAxisRatio() const;
    void setAxisRatio(double value);
    virtual void print () const override;

protected:
    double axisRatio;
};

/**
 * @brief Geometry class which represents Spline
 */
class CADSpline : public CADGeometry
{
public:
    CADSpline();
    virtual void print () const override;
    long getScenario() const;
    void setScenario(long value);

    bool getRational() const;
    void setRational(bool value);

    bool getClosed() const;
    void setClosed(bool value);

    void addControlPointsWeight(double weight);
    void addControlPoint(const CADVector& point);
    void addFitPoint(const CADVector& point);
    bool getWeight() const;
    void setWeight(bool value);

    double getFitTollerance() const;
    void setFitTollerance(double value);

protected:
    long scenario;
    bool rational;
    bool closed;
    bool weight;
    double fitTollerance;

    std::vector < double > ctrlPointsWeight;
    std::vector < CADVector > avertCtrlPoints;
    std::vector < CADVector > averFitPoints;
};

class CADSolid : public CADPoint3D
{
public:
    CADSolid();
    virtual void print () const override;
    double getElevation() const;
    void setElevation(double value);
    void addAverCorner(const CADVector& corner);
protected:
    double elevation;
    vector < CADVector > avertCorners;
};

class CADRay : public CADPoint3D
{
public:
    CADRay();
    CADVector getVectVector() const;
    void setVectVector(const CADVector &value);
    virtual void print () const override;
};

class CADHatch : public CADGeometry
{
public:
    CADHatch();
};


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

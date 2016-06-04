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

#include "cadgeometry.h"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
// CADGeometry
//------------------------------------------------------------------------------

CADGeometry::CADGeometry() : geometryType(UNDEFINED), thickness(0)
{

}

CADGeometry::~CADGeometry()
{

}

CADGeometry::GeometryType CADGeometry::getType() const
{
    return geometryType;
}

double CADGeometry::getThickness() const
{
    return thickness;
}

void CADGeometry::setThickness(double thicknes){
    thickness = thicknes;
}

//------------------------------------------------------------------------------
// CADPoint3D
//------------------------------------------------------------------------------

CADPoint3D::CADPoint3D() : xAxisAng (0.0f)
{
    geometryType = CADGeometry::POINT;
}

CADPoint3D::CADPoint3D(const CADVector &positionIn, double thicknessIn)
{
    geometryType = CADGeometry::POINT;
    thickness = thicknessIn;
    position = positionIn;
}

CADVector CADPoint3D::getPosition() const
{
    return position;
}

void CADPoint3D::setPosition(const CADVector &value)
{
    position = value;
}

CADVector CADPoint3D::getExtrusion() const
{
    return extrusion;
}

void CADPoint3D::setExtrusion(const CADVector &value)
{
    extrusion = value;
}

double CADPoint3D::getXAxisAng() const
{
    return xAxisAng;
}

void CADPoint3D::setXAxisAng(double value)
{
    xAxisAng = value;
}

void CADPoint3D::print() const
{
    cout << "|---------Point---------|\n"
         << "Position: " << "\t"
         << position.getX () << "\t"
         << position.getY () << "\t"
         << position.getZ () << "\n"
         << endl;
}

//------------------------------------------------------------------------------
// CADLine
//------------------------------------------------------------------------------

CADLine::CADLine()
{
    geometryType = CADGeometry::LINE;
}

CADLine::CADLine(const CADPoint3D &startIn, const CADPoint3D &endIn)
{
    geometryType = CADGeometry::LINE;
    start = startIn;
    end = endIn;
}

CADPoint3D CADLine::getStart() const
{
    return start;
}

void CADLine::setStart(const CADPoint3D &value)
{
    start = value;
}

CADPoint3D CADLine::getEnd() const
{
    return end;
}

void CADLine::setEnd(const CADPoint3D &value)
{
    end = value;
}

void CADLine::print() const
{
    cout << "|---------Line---------|\n"
         << "Start Position: " << "\t"
         << start.getPosition ().getX () << "\t"
         << start.getPosition ().getY () << "\t"
         << start.getPosition ().getZ () << "\n"
         << "End Position: " << "\t"
         << end.getPosition ().getX () << "\t"
         << end.getPosition ().getY () << "\t"
         << end.getPosition ().getZ () << "\n"
         << endl;
}

//------------------------------------------------------------------------------
// CADLWPolyline
//------------------------------------------------------------------------------

/*
CADLWPolyline::CADLWPolyline() : dfConstWidth(0.0f), dfElevation(0.0f)
{
    eGeometryType = CADGeometry::LWPOLYLINE;
}
*/

//------------------------------------------------------------------------------
// CADCircle
//------------------------------------------------------------------------------

CADCircle::CADCircle() : radius (0.0f)
{
    geometryType = CADGeometry::CIRCLE;
}

double CADCircle::getRadius() const
{
    return radius;
}

void CADCircle::setRadius(double value)
{
    radius = value;
}

void CADCircle::print() const
{
    cout << "|---------Circle---------|\n"
         << "Position: " << "\t"
         << position.getX () << "\t"
         << position.getY () << "\t"
         << position.getZ () << "\n"
         << "Radius: " << radius << "\n"
         << endl;

}

//------------------------------------------------------------------------------
// CADArc
//------------------------------------------------------------------------------

CADArc::CADArc() : CADCircle(), startingAngle(0.0f), endingAngle(0.0f)
{
    geometryType = CADGeometry::ARC;
}

double CADArc::getStartingAngle() const
{
    return startingAngle;
}

void CADArc::setStartingAngle(double value)
{
    startingAngle = value;
}

double CADArc::getEndingAngle() const
{
    return endingAngle;
}

void CADArc::setEndingAngle(double value)
{
    endingAngle = value;
}

void CADArc::print() const
{
    cout << "|---------Arc---------|\n"
         << "Position: " << "\t"
         << position.getX () << "\t"
         << position.getY () << "\t"
         << position.getZ () << "\n"
         << "Radius: " << "\t" << radius << "\n"
         << "Beg & End angles: " << "\t"
         << startingAngle << "\t"
         << endingAngle << "\n"
         << endl;
}

//------------------------------------------------------------------------------
// CADPolyline3D
//------------------------------------------------------------------------------

CADPolyline3D::CADPolyline3D()
{
    geometryType = CADGeometry::POLYLINE3D;
}

void CADPolyline3D::addVertex(const CADVector &vertex)
{
    vertexes.push_back (vertex);
}

size_t CADPolyline3D::getVertexCount() const
{
    return vertexes.size ();
}

CADVector &CADPolyline3D::getVertex(size_t index)
{
    return vertexes[index];
}

void CADPolyline3D::print() const
{
    cout << "|------Polyline3D-----|" << endl;
    for ( size_t i = 0; i < vertexes.size(); ++i ){
        cout << "  #" << i
             << "X: " << vertexes[i].getX()
             << ", Y: " << vertexes[i].getY() << std::endl;
    }
    cout << endl;
}

//------------------------------------------------------------------------------
// CADLWPolyline
//------------------------------------------------------------------------------

CADLWPolyline::CADLWPolyline()
{
    geometryType = CADGeometry::LWPOLYLINE;
}

void CADLWPolyline::print() const
{
    cout << "|------LWPolyline-----|" << endl;
    for ( size_t i = 0; i < vertexes.size(); ++i ){
        cout << "  #" << i
             << "X: " << vertexes[i].getX()
             << ", Y: " << vertexes[i].getY() << std::endl;
    }
    cout << endl;
}

double CADLWPolyline::getConstWidth() const
{
    return constWidth;
}

void CADLWPolyline::setConstWidth(double value)
{
    constWidth = value;
}

double CADLWPolyline::getElevation() const
{
    return elevation;
}

void CADLWPolyline::setElevation(double value)
{
    elevation = value;
}

CADVector CADLWPolyline::getVectExtrusion() const
{
    return vectExtrusion;
}

void CADLWPolyline::setVectExtrusion(const CADVector &value)
{
    vectExtrusion = value;
}

vector<pair<double, double> > CADLWPolyline::getWidths() const
{
    return widths;
}

void CADLWPolyline::setWidths(const vector<pair<double, double> > &value)
{
    widths = value;
}

//------------------------------------------------------------------------------
// CADEllipse
//------------------------------------------------------------------------------

CADEllipse::CADEllipse(): CADArc(), axisRatio(0.0f)
{
    geometryType = CADGeometry::ELLIPSE;
}

double CADEllipse::getAxisRatio() const
{
    return axisRatio;
}

void CADEllipse::setAxisRatio(double value)
{
    axisRatio = value;
}

void CADEllipse::print() const
{
    cout << "|---------Ellipse---------|\n"
         << "Position: "
         << "\t" << position.getX()
         << "\t" << position.getY()
         << "\t" << position.getZ() << "\n"
         << "Beg & End angles: "
         << "\t" << startingAngle
         << "\t" << endingAngle << "\n"
         << endl;
}

//------------------------------------------------------------------------------
// CADText
//------------------------------------------------------------------------------

CADText::CADText() : CADPoint3D (), obliqueAngle(0), rotationAngle(0), height(0)
{
    geometryType = CADGeometry::TEXT;
}

string CADText::getTextValue() const
{
    return textValue;
}

void CADText::setTextValue(const string &value)
{
    textValue = value;
}

double CADText::getHeight() const
{
    return height;
}

void CADText::setHeight(double value)
{
    height = value;
}

double CADText::getRotationAngle() const
{
    return rotationAngle;
}

void CADText::setRotationAngle(double value)
{
    rotationAngle = value;
}

double CADText::getObliqueAngle() const
{
    return obliqueAngle;
}

void CADText::setObliqueAngle(double value)
{
    obliqueAngle = value;
}

void CADText::print() const
{
    cout << "|---------Text---------|\n"
         << "Position:"
         << "\t" << position.getX ()
         << "\t" << position.getY ()
         << "\n"
         << "Text value:\t" << textValue << "\n" << std::endl;
}

//------------------------------------------------------------------------------
// CADRay
//------------------------------------------------------------------------------

CADRay::CADRay() : CADPoint3D ()
{
    geometryType = CADGeometry::RAY;
}

CADVector CADRay::getVectVector() const
{
    return extrusion;
}

void CADRay::setVectVector(const CADVector &value)
{
    extrusion = value;
}

void CADRay::print() const
{
    cout << "|---------Ray---------|\n"
         << "Position:"
         << "\t" << position.getX ()
         << "\t" << position.getY ()
         << "\nVector:"
         << "\t" << extrusion.getX ()
         << "\t" << extrusion.getY ()
         << "\n" << std::endl;
}

//------------------------------------------------------------------------------
// CADHatch
//------------------------------------------------------------------------------

CADHatch::CADHatch()
{
    geometryType = CADGeometry::HATCH;
}

//------------------------------------------------------------------------------
// CADSpline
//------------------------------------------------------------------------------

CADSpline::CADSpline()
{
    geometryType = CADGeometry::SPLINE;
}

void CADSpline::print() const
{

    cout << "|---------Spline---------|\n"
         << "Is rational: \t" << rational << "\n"
         << "Is closed: \t" << closed << "\n"
         << "Control pts count: " << avertCtrlPoints.size() << std::endl;
    for ( size_t j = 0; j < avertCtrlPoints.size(); ++j )
    {
        cout << "  #" << j
             << "\t" << avertCtrlPoints[j].getX()
             << "\t" << avertCtrlPoints[j].getY()
             << "\t" << avertCtrlPoints[j].getZ() << "\t";
        if ( weight == true )
            cout << ctrlPointsWeight[j] << endl;
        else
            cout << endl;
    }

    cout << "Fit pts count: " << averFitPoints.size() << endl;
    for ( size_t j = 0; j < averFitPoints.size(); ++j )
    {
        cout << "  #" << j
             << "\t" << averFitPoints[j].getX()
             << "\t" << averFitPoints[j].getY()
             << "\t" << averFitPoints[j].getZ()
             << endl;
    }
    cout << endl;
}

long CADSpline::getScenario() const
{
    return scenario;
}

void CADSpline::setScenario(long value)
{
    scenario = value;
}

bool CADSpline::getRational() const
{
    return rational;
}

void CADSpline::setRational(bool value)
{
    rational = value;
}

bool CADSpline::getClosed() const
{
    return closed;
}

void CADSpline::setClosed(bool value)
{
    closed = value;
}

void CADSpline::addControlPointsWeight(double weight)
{
    ctrlPointsWeight.push_back (weight);
}

void CADSpline::addControlPoint(const CADVector &point)
{
    avertCtrlPoints.push_back (point);
}

void CADSpline::addFitPoint(const CADVector &point)
{
    averFitPoints.push_back (point);
}

bool CADSpline::getWeight() const
{
    return weight;
}

void CADSpline::setWeight(bool value)
{
    weight = value;
}

double CADSpline::getFitTollerance() const
{
    return fitTollerance;
}

void CADSpline::setFitTollerance(double value)
{
    fitTollerance = value;
}

//------------------------------------------------------------------------------
// CADSolid
//------------------------------------------------------------------------------

CADSolid::CADSolid()
{
    geometryType = CADGeometry::SOLID;
}

void CADSolid::print() const
{
    cout << "|---------Solid---------|" << endl;
    for ( size_t i = 0; i < avertCorners.size(); ++i )
    {
        cout << "  #" << i
             << "\t" << avertCorners[i].getX()
             << "\t" << avertCorners[i].getY()
             << "\n  Elevation: " << elevation << "\n";
    }
    cout << endl;
}

double CADSolid::getElevation() const
{
    return elevation;
}

void CADSolid::setElevation(double value)
{
    elevation = value;
}

void CADSolid::addAverCorner(const CADVector &corner)
{
    avertCorners.push_back (corner);
}
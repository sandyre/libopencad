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

CADPoint3D::CADPoint3D(const CADVector &positionIn, double thickness)
{
    geometryType = CADGeometry::POINT;
    thickness = thickness;
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

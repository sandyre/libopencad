/*******************************************************************************
 *  Project: libopencad
 *  Purpose: OpenSource CAD formats support library
 *  Author: Alexandr Borzykh, mush3d at gmail.com
 *  Language: C++
 *******************************************************************************
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
 *******************************************************************************/

#ifndef SIMPLEDATATYPES_H
#define SIMPLEDATATYPES_H

struct Vertex2D
{
    Vertex2D() : X(0.0f),
                 Y(0.0f)
    {
    }

    Vertex2D( double X_, double Y_) :
            X(X_), Y(Y_)
    {

    }

    double X;
    double Y;

    bool operator == (const Vertex2D &other)
    {
        return ( ( this->X == other.X ) && ( this->Y == other.Y ) );
    }
};

struct Vertex3D
{
    Vertex3D() : X(0.0f),
                 Y(0.0f),
                 Z(0.0f)
    {
    }

    Vertex3D( double X_, double Y_, double Z_) :
            X(X_), Y(Y_), Z(Z_)
    {

    }

    double X;
    double Y;
    double Z;

    bool operator == (const Vertex3D& second)
    {
        return ( ( this->X == second.X )  && ( this->Y == this->Y ) &&
                 ( this->Z == second.Z ) );
    }
};

struct Vector2D
{
    Vector2D() : X(0.0f),
                 Y(0.0f)
    {
    }

    double X;
    double Y;

    bool operator == (const Vector2D& second)
    {
        return ( ( this->X == second.X )  && ( this->Y == this->Y ) );
    }
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

    bool operator == (const Vector3D& second)
    {
        return ( ( this->X == second.X )  && ( this->Y == this->Y ) &&
                 ( this->Z == second.Z ) );
    }
};

#endif //SIMPLEDATATYPES_H

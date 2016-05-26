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

#include "opencad_api.h"
#include "cadgeometries.h"

#include <cstddef>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int Usage(const char* pszErrorMsg = nullptr)
{
    std::cout << "Usage: cadinfo [--help] [--formats][--version]\n"
            "               file_name" << std::endl;

    if( pszErrorMsg != nullptr )
    {
        std::cerr << "\nFAILURE: " << pszErrorMsg << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int Version()
{
    std::cout << "cadinfo was compiled against libopencad "
              << OCAD_VERSION << std::endl
              << "and is running against libopencad "
              << GetVersionString() << std::endl;

    return EXIT_SUCCESS;
}

static int Formats()
{
    std::cerr << GetCADFormats() << std::endl;

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if( argc < 1 )
       return -argc;

    const char  *pszCADFilePath = nullptr;

    for( int iArg = 1; iArg < argc; ++iArg)
    {
        if (strcmp(argv[iArg],"-h")==0 || strcmp(argv[iArg],"--help")==0)
        {
            return Usage();
        }
        else if(strcmp(argv[iArg],"-f")==0 || strcmp(argv[iArg],"--formats")==0)
        {
            return Formats ();
        }
        else if(strcmp(argv[iArg],"-v")==0 || strcmp(argv[iArg],"--version")==0)
        {
            return Version();
        }
        else
        {
            pszCADFilePath = argv[iArg];
        }
    }

    CADFile *poCadFile = OpenCADFile( GetDeafultFileIO(pszCADFilePath) );

    if (poCadFile == nullptr)
    {
        std::cerr << "Open CAD file " << pszCADFilePath << " failed." << std::endl;
        return EXIT_FAILURE;
    }

    const CADHeader& header = poCadFile->GetHeader ();
    header.Print ();
    std::cout << std::endl;

    const CADClasses& classes = poCadFile->GetClasses ();
    classes.Print ();
    std::cout << std::endl;

    int images_count = 0;
    int solids_count = 0;
    int splines_count = 0;
    int circles_count = 0;
    int lines_count = 0;
    int ellipses_count = 0;
    int pline_count = 0;
    int pline3d_count = 0;
    int point_count = 0;
    int arc_count = 0;
    int text_count = 0;
    std::cout << "Layers count: " << poCadFile->GetLayersCount () << std::endl;

    Layer * layer;
    for ( size_t i = 0; i < poCadFile->GetLayersCount (); ++i )
    {
        layer = poCadFile->GetLayer (i);
        std::cout << "Layer #" << i << " contains " << layer->GetGeometriesCount () << " geometries.\n";
        for ( size_t j = 0; j < layer->GetGeometriesCount (); ++j )
        {
            CADGeometry * geom = layer->GetGeometry (j);

            if ( geom != nullptr )
            {
                switch ( geom->GetType() )
                {
                    case CADGeometry::CIRCLE:
                    {
                        Circle * circle = ( Circle * ) geom;
                        std::cout << "|---------Circle---------|\n";
                        std::cout << "Position: " << "\t" << circle->vertPosition.X <<
                                "\t" << circle->vertPosition.Y << "\t" << circle->vertPosition.Z << std::endl;
                        std::cout << "Radius: " << circle->dfRadius << std::endl << std::endl;

                        ++circles_count;
                        break;
                    }
                    case CADGeometry::LWPOLYLINE:
                    {
                        LWPolyline * poly = ( LWPolyline * ) geom;
                        std::cout << "|---------LWPolyline---------|\n";
                        for ( size_t i = 0; i < poly->vertexes.size(); ++i )
                        {
                            std::cout << "#" << i << "\t" << poly->vertexes[i].X <<
                                "\t" << poly->vertexes[i].Y << std::endl;
                        }
                        std::cout << std::endl;

                        ++pline_count;
                        break;
                    }
                    case CADGeometry::POLYLINE3D:
                    {
                        Polyline3D * poly = ( Polyline3D * ) geom;
                        std::cout << "|---------Polyline3D---------|\n";
                        for ( size_t i = 0; i < poly->vertexes.size(); ++i )
                        {
                            std::cout << "#" << i << "\t" << poly->vertexes[i].X <<
                                    "\t" << poly->vertexes[i].Y
                                    << "\t" << poly->vertexes[i].Z << std::endl;
                        }
                        std::cout << std::endl;

                        ++pline3d_count;
                        break;
                    }
                    case CADGeometry::ARC:
                    {
                        Arc * arc = ( Arc * ) geom;
                        std::cout << "|---------Arc---------|\n";
                        std::cout << "Position: " << "\t" << arc->vertPosition.X <<
                                "\t" << arc->vertPosition.Y << "\t" << arc->vertPosition.Z << std::endl;
                        std::cout << "Radius: " << "\t" << arc->dfRadius << std::endl;
                        std::cout << "Beg & End angles: " << "\t" << arc->dfStartingAngle << "\t"
                                << arc->dfEndingAngle << std::endl << std::endl;

                        ++arc_count;
                        break;
                    }
                    case CADGeometry::POINT:
                    {
                        Point3D * point = ( Point3D * ) geom;
                        std::cout << "|---------Point---------|\n";
                        std::cout << "Position: " << "\t" << point->vertPosition.X <<
                            "\t" << point->vertPosition.Y << "\t" << point->vertPosition.Z << std::endl << std::endl;

                        ++point_count;
                        break;
                    }
                    case CADGeometry::ELLIPSE:
                    {
                        Ellipse * ellipse = ( Ellipse * ) geom;
                        std::cout << "|---------Ellipse---------|\n";
                        std::cout << "Position: " << "\t" << ellipse->vertPosition.X <<
                            "\t" << ellipse->vertPosition.Y << "\t" << ellipse->vertPosition.Z << std::endl;
                        std::cout << "Beg & End angles: " << "\t" << ellipse->dfStartingAngle << "\t"
                            << ellipse->dfEndingAngle << std::endl << std::endl;

                        ++ellipses_count;
                        break;
                    }
                    case CADGeometry::LINE:
                    {
                        Line * line = ( Line * ) geom;
                        std::cout << "|---------Line---------|\n";
                        std::cout << "Start Position: " << "\t" << line->vertStart.X <<
                            "\t" << line->vertStart.Y << "\t" << line->vertStart.Z << std::endl;
                        std::cout << "End Position: " << "\t" << line->vertEnd.X <<
                            "\t" << line->vertEnd.Y << "\t" << line->vertEnd.Z << std::endl << std::endl;

                        ++lines_count;
                        break;
                    }
                    case CADGeometry::SPLINE:
                    {
                        Spline * spline = ( Spline * ) geom;
                        std::cout << "|---------Spline---------|\n";
                        std::cout << "Degree: \t" << spline->dDegree << std::endl;
                        if ( spline->dScenario == 2 )
                        {
                            std::cout << "Fit tolerance: \t" << spline->dfFitTol << std::endl;
                            std::cout << "Beg tangent vector:\t" << spline->vectBegTangDir.X << "\t"
                                << spline->vectBegTangDir.Y << "\t" << spline->vectBegTangDir.Y << std::endl;
                            std::cout << "End tangent vector:\t" << spline->vectEndTangDir.X << "\t"
                                << spline->vectEndTangDir.Y << "\t" << spline->vectEndTangDir.Y << std::endl;

                            std::cout << "Knots count: " << spline->adfKnots.size() << std::endl;
                            for ( size_t j = 0; j < spline->adfKnots.size(); ++j )
                                std::cout << "#" << j << "\t" << spline->adfKnots[j] << std::endl;
                        }
                        if ( spline->dScenario == 1 )
                        {
                            std::cout << "Is rational: \t" << spline->bRational << std::endl;
                            std::cout << "Is closed: \t" << spline->bClosed << std::endl;
                            std::cout << "Is periodic: \t" << spline->bPeriodic << std::endl;
                            std::cout << "Knot tolerance: \t" << spline->bRational << std::endl;
                            std::cout << "Control tolerance: \t" << spline->bRational << std::endl;
                            std::cout << "Control pts weight presented: \t" << spline->bWeight << std::endl;
                        }

                        std::cout << "Control pts count: " << spline->avertCtrlPoints.size() << std::endl;
                        for ( size_t j = 0; j < spline->avertCtrlPoints.size(); ++j )
                        {
                            std::cout << "#" << j << "\t" << spline->avertCtrlPoints[j].X << "\t"
                            << spline->avertCtrlPoints[j].Y << "\t"
                            << spline->avertCtrlPoints[j].Z << "\t";
                            if ( spline->bWeight == true )
                                std::cout << spline->adfCtrlPointsWeight[j] << std::endl;
                            else std::cout << std::endl;
                        }

                        std::cout << "Fit pts count: " << spline->averFitPoints.size() << std::endl;
                        for ( size_t j = 0; j < spline->averFitPoints.size(); ++j )
                        {
                            std::cout << "#" << j << "\t" << spline->averFitPoints[j].X << "\t"
                            << spline->averFitPoints[j].Y << "\t"
                            << spline->averFitPoints[j].Z << std::endl;
                        }

                        std::cout << std::endl;
                        ++splines_count;
                        break;
                    }
                    case CADGeometry::TEXT:
                    {
                        // TODO: add other optional parameters.
                        Text * text = ( Text * ) geom;
                        std::cout << "|---------Text---------|\n";
                        std::cout << "Position:\t" << text->vertInsertion.X << "\t"
                            << text->vertInsertion.Y << std::endl;
                        std::cout << "Text value:\t" << text->strTextValue << std::endl << std::endl;

                        ++text_count;
                        break;
                    }
                    case CADGeometry::SOLID:
                    {
                        Solid * solid = ( Solid * ) geom;
                        std::cout << "|---------Solid---------|\n";
                        for ( size_t i = 0; i < solid->avertCorners.size(); ++i )
                        {
                            std::cout << "#" << i << "\t" << solid->avertCorners[i].X << "\t"
                                << solid->avertCorners[i].Y << solid->dfElevation << std::endl;
                        }
                        std::cout << std::endl;
                        ++solids_count;
                        break;
                    }
                    case CADGeometry::IMAGE:
                    {
                        Image * img = ( Image * ) geom;
                        std::cout << "|---------Image---------|\n";
                        std::cout << "Filepath: " << img->sFilePath << std::endl;
                        std::cout << "Insertion point: " << img->vertInsertionPoint.X << "\t"
                                 << img->vertInsertionPoint.Y << std::endl;
                        std::cout << "Show? : " << img->bShow << std::endl;
                        std::cout << "Transparent? : " << img->bTransparency << std::endl;
                        std::cout << "Brightness (0-100) : " << img->dBrightness << std::endl;
                        std::cout << "Contrast (0-100) : " << img->dContrast << std::endl;
                        std::cout << "Fade (0-100) : " << img->dFade << std::endl;
                        std::cout << "Clipping polygon:" << std::endl;
                        for ( size_t i = 0; i < img->avertClippingPolygon.size(); ++i )
                        {
                            std::cout << "#" << i << "\tX: " << img->avertClippingPolygon[i].X << " Y: "
                                << img->avertClippingPolygon[i].Y << std::endl;
                        }
                        std::cout << std::endl;
                        ++images_count;
                        break;
                    }
                }
            }
        }
    }

    std::cout << "Images: " << images_count << std::endl;
    std::cout << "Solids: " << solids_count << std::endl;
    std::cout << "Points: " << point_count << std::endl;
    std::cout << "Ellipses: " << ellipses_count << std::endl;
    std::cout << "Lines count: " << lines_count << std::endl;
    std::cout << "Plines count: " << pline_count << std::endl;
    std::cout << "Plines3d count: " << pline3d_count << std::endl;
    std::cout << "Splines count: " << splines_count << std::endl;
    std::cout << "Circles count: " << circles_count << std::endl;
    std::cout << "Arcs count: " << arc_count << std::endl;
    std::cout << "Texts count: " << text_count << std::endl;
}

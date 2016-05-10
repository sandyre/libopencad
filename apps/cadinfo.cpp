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

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int Usage(const char* pszErrorMsg = NULL)
{
    std::cout << "Usage: cadinfo [--help] [--formats][--version]\n"
            "               file_name" << std::endl;

    if( pszErrorMsg != NULL )
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

int main(int argc, char *argv[])
{
    if( argc < 1 )
       return -argc;

    const char  *pszCADFilePath = NULL;

    for( int iArg = 1; iArg < argc; ++iArg)
    {
        if (strcmp(argv[iArg],"-h")==0 || strcmp(argv[iArg],"--help")==0)
        {
            return Usage();
        }
        else if(strcmp(argv[iArg],"-f")==0 || strcmp(argv[iArg],"--formats")==0)
        {
            //TODO: return supported CAD formats
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

    if (poCadFile == NULL)
    {
        std::cerr << "Open CAD file " << pszCADFilePath << " failed." << std::endl;
        return EXIT_FAILURE;
    }

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
                switch ( geom->stGeometryType )
                {
                    case CADGeometry::CADGeometryType::CIRCLE:
                        ++circles_count;
                        break;
                    case CADGeometry::CADGeometryType::LWPOLYLINE:
                        ++pline_count;
                        break;
                    case CADGeometry::CADGeometryType::POLYLINE3D:
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
                    case CADGeometry::CADGeometryType::ARC:
                        ++arc_count;
                        break;
                    case CADGeometry::CADGeometryType::POINT:
                        ++point_count;
                        break;
                    case CADGeometry::CADGeometryType::ELLIPSE:
                        ++ellipses_count;
                        break;
                    case CADGeometry::CADGeometryType::LINE:
                        ++lines_count;
                        break;
                }
            }
        }
    }

    std::cout << "Points: " << point_count << std::endl;
    std::cout << "Ellipses: " << ellipses_count << std::endl;
    std::cout << "Lines count: " << lines_count << std::endl;
    std::cout << "Plines count: " << pline_count << std::endl;
    std::cout << "Plines3d count: " << pline3d_count << std::endl;
    std::cout << "Circles count: " << circles_count << std::endl;
    std::cout << "Arcs count: " << arc_count << std::endl;
    std::cout << "Texts count: " << text_count << std::endl;
}

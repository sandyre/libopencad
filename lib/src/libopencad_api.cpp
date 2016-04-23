/************************************************************************************
 *  Name: libopencad_api.cpp
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

#include "../include/libopencad_api.h"

#include <iostream>

using namespace libopencad::dwg;

namespace libopencad
{

CADFile * OpenCADFile( const char *filename )
{
    std::string filename_s(filename);
    if ( filename_s.substr(filename_s.size() - 4 ) == ".dxf"
         || filename_s.substr(filename_s.size() - 4 ) == ".DXF")
    {
        std::cerr << "DXF is not supported for now." << std::endl;
        return( nullptr );
    }

    CADFile * hCAD;
    std::ifstream dwg_file ( filename, std::ios_base::in | std::ios_base::binary );

    if ( !dwg_file.is_open () )
    {
        std::cerr << "Error opening CAD file. Check filename or filepath.\n";
        return nullptr;
    }

    char pabyDWGVersion[6];
    dwg_file.read ( pabyDWGVersion, 6 );
    dwg_file.close();

    // TODO: switch would be better here.
    // if ( !memcmp ( pabyDWGVersion, DWG_VERSION_R13, DWG_VERSION_SIZE ) )
    //     hDWG = new DWGFileR13();

    if ( !memcmp ( pabyDWGVersion, DWG_VERSION_R2000, DWG_VERSION_SIZE ) )
        hCAD = new DWGFileR2000();

    if ( hCAD != nullptr )
    {
        hCAD->fileStream.open ( filename, std::ios_base::in | std::ios_base::binary );
        hCAD->parseFile ();
        return hCAD;
    }

    else
    {
        std::cerr << "Unsupported CAD file version. Check documentation.\n";
        return nullptr;
    }
}

}

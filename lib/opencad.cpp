/*******************************************************************************
 *  Project: libopencad_api.cpp
 *  Purpose: libOpenCAD OpenSource CAD formats support library
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
#include "dwg/constants.h"
#include "dwg/r2000.h"

#include <iostream>
#include <cstring>

static int gLastError = 0;

/**
 * @brief Open CAD file
 * @param path to cad file
 * @return CADFile pointer or NULL if failed
 */
CADFile* OpenCADFile( const char *pszFileName )
{
    CADFile * poCAD = NULL;
    std::ifstream oCADFile ( pszFileName, std::ios_base::in |
                             std::ios_base::binary );

    if ( !oCADFile.is_open () )
    {
        gLastError = CADErrorCodes::FILE_OPEN_FAILED;
        return NULL;
    }

    // check if dxf - last 3 chars in path (case insensitive)
    // check if binary dxf

    char pabyDWGVersion[DWG_VERSION_SIZE];
    oCADFile.read ( pabyDWGVersion, DWG_VERSION_SIZE);
    oCADFile.close();

    if ( memcmp ( pabyDWGVersion, DWG_VERSION_R2000, DWG_VERSION_SIZE ) == 0 )
    {
        poCAD = new DWGFileR2000(pszFileName);
    }
    else
    {
        gLastError = CADErrorCodes::UNSUPPORTED_VERSION;
    }

    if(NULL != poCAD)
    {
        gLastError = poCAD->ParseFile();
        if(gLastError != CADErrorCodes::SUCCESS)
        {
            delete poCAD;
            poCAD = NULL;
        }
    }

    return poCAD;
}


/**
 * @brief Get library version number as major * 10000 + minor * 100 + rev
 * @return library version number
 */
int GetVersion()
{
    return OCAD_VERSION_NUM;
}

/**
 * @brief Get library version string
 * @return library version string
 */
const char* GetVersionString()
{
    return OCAD_VERSION;
}

int GetLastErrorCode()
{
    return gLastError;
}

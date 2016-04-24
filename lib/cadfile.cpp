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


#include "cadfile.h"
#include "opencad_api.h"

#include <iostream>

CADFile::CADFile(const char *pszFileName)
{
    m_pszFileName = pszFileName;
}

CADFile::~CADFile()
{
    m_oFileStream.close();
}

size_t CADFile::getGeometriesCount ()
{
    std::cerr << "CADFile::getGeometriesCount() called from abstract class.\n"
              << "This method should be overrided in derived classes. Abort.\n";
    return( 0 );
}

size_t CADFile::getLayersCount ()
{
    std::cerr << "CADFile::getLayersCount() called from abstract class.\n"
              << "This method should be overrided in derived classes. Abort.\n";
    return( 0 );
}

size_t CADFile::getBlocksCount ()
{
    std::cerr << "CADFile::getBlocksCount() called from abstract class.\n"
              << "This method should be overrided in derived classes. Abort.\n";
    return( 0 );
}

CADBlock * CADFile::getBlock ( size_t index )
{
    std::cerr << "CADFile::getBlock() called from abstract class.\n"
              << "This method should be overrided in derived classes. Abort.\n";
    return( nullptr );
}

CADLayer * CADFile::getLayer ( size_t index )
{
    std::cerr << "CADFile::getLayer() called from abstract class.\n"
              << "This method should be overrided in derived classes. Abort.\n";
    return( nullptr );
}

CADGeometry * CADFile::getGeometry ( size_t index )
{
    std::cerr << "CADFile::getGeometry() called from abstract class.\n"
              << "This method should be overrided in derived classes. Abort.\n";

    return( nullptr );
}

int CADFile::ParseFile()
{
    m_oFileStream = std::ifstream( m_pszFileName, std::ios_base::in |
                             std::ios_base::binary );
    if ( m_oFileStream.is_open () )
    {
        ReadHeader ();
        ReadClassesSection ();
        ReadObjectMap ();

        return CADErrorCodes::SUCCESS;
    }
    return CADErrorCodes::FILE_PARSE_FAILED;
}

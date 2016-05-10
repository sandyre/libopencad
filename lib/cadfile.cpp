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

CADFile::CADFile(CADFileIO* poFileIO)
{
    m_poFileIO = poFileIO;
    m_poHeader = new CADHeader();
}

CADFile::~CADFile()
{
    delete m_poHeader;
    if(nullptr != m_poFileIO)
        delete m_poFileIO;
}

const CADHeader *CADFile::GetHeader() const
{
    return m_poHeader;
}

size_t CADFile::GetLayersCount ()
{
    std::cerr << "CADFile::getLayersCount() called from abstract class.\n"
              << "This method should be overrided in derived classes. Abort.\n";
    return( 0 );
}

size_t CADFile::GetBlocksCount ()
{
    std::cerr << "CADFile::getBlocksCount() called from abstract class.\n"
              << "This method should be overrided in derived classes. Abort.\n";
    return( 0 );
}

CADBlock * CADFile::GetBlock ( size_t index )
{
    std::cerr << "CADFile::getBlock() called from abstract class.\n"
              << "This method should be overrided in derived classes. Abort.\n";
    return( nullptr );
}

Layer * CADFile::GetLayer ( size_t index )
{
    std::cerr << "CADFile::GetLayer() called from abstract class.\n"
              << "This method should be overrided in derived classes. Abort.\n";
    return( nullptr );
}

CADGeometry * CADFile::GetGeometry ( size_t layer_index, size_t index )
{
    std::cerr << "CADFile::getGeometry() called from abstract class.\n"
              << "This method should be overrided in derived classes. Abort.\n";

    return( nullptr );
}

int CADFile::ParseFile()
{
    if(nullptr == m_poFileIO)
        return CADErrorCodes::FILE_OPEN_FAILED;

    if(!m_poFileIO->IsOpened())
    {
        if(!m_poFileIO->Open(CADFileIO::read | CADFileIO::binary))
            return CADErrorCodes::FILE_OPEN_FAILED;
    }

    int nResultCode = ReadHeader ();
    if(nResultCode != CADErrorCodes::SUCCESS)
        return nResultCode;
    nResultCode = ReadClassesSection ();
    if(nResultCode != CADErrorCodes::SUCCESS)
        return nResultCode;
    nResultCode = ReadObjectMap ();
    if(nResultCode != CADErrorCodes::SUCCESS)
        return nResultCode;

    return CADErrorCodes::SUCCESS;
 }

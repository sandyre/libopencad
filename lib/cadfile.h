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

#ifndef LIB_CADFILE_H
#define LIB_CADFILE_H

#include "cadgeometries.h"

#include "opencad.h"

#include <fstream>

class EXTERN CADFile
{
public:
    CADFile (const char *pszFileName);
    virtual ~CADFile();

    virtual size_t getGeometriesCount();
    virtual size_t getLayersCount();
    virtual size_t getBlocksCount();
    virtual CADGeometry * getGeometry( size_t index );
    virtual CADBlock * getBlock( size_t index );
    virtual CADLayer * getLayer( size_t index );
    virtual int ParseFile();

protected:
    virtual void ReadHeader() = 0;
    virtual void ReadClassesSection() = 0;
    virtual void ReadObjectMap() = 0;

protected:
    std::ifstream m_oFileStream;
    const char *m_pszFileName;
};


#endif //LIB_CADFILE_H

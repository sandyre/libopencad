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

#ifndef CADFILE_H
#define CADFILE_H

#include "cadheader.h"
#include "cadfileio.h"
#include "cadobjects.h"

class Layer;
class CADGeometry;

/**
 * @brief The abstact CAD file class
 */
class EXTERN CADFile
{
    friend class Layer;
public:
    CADFile (CADFileIO* poFileIO);
    virtual ~CADFile();

public:
    const CADHeader* GetHeader() const;

public:
    virtual size_t GetLayersCount();
    virtual size_t GetBlocksCount();
    virtual Layer * GetLayer( size_t index );
    virtual CADBlock* GetBlock( size_t index );
    virtual int ParseFile();

protected:
    virtual CADObject * GetObject( size_t index );
    virtual CADGeometry * GetGeometry( size_t layer_index, size_t index );

    virtual int ReadHeader() = 0;
    virtual int ReadClassesSection() = 0;
    virtual int ReadObjectMap() = 0;

protected:
    CADFileIO* m_poFileIO;
    CADHeader* m_poHeader;
};


#endif // CADFILE_H

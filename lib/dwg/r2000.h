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

#ifndef DWG_R2000_H_H
#define DWG_R2000_H_H

#include "constants.h"
#include "cadfile.h"

#include <string>
#include <map>
#include <vector>
#include <cadgeometries.h>

struct SLRecord
{
    char byRecordNumber = 0;
    int  dSeeker        = 0;
    int  dSize          = 0;
};

struct DWG2000_CED
{
    int64_t     dLength;
    int16_t     dType;
    int32_t     dObjSizeInBits;
    CADHandle   hHandle;
    std::vector < CAD_EED > eEED;
    bool        bGraphicPresentFlag;

    int8_t      dEntMode;
    int32_t     dNumReactors;

    bool        bNoLinks;
    int16_t     dCMColorIndex;
    double      dfLtypeScale;

    int8_t      ltype_flags;
    int8_t      plotstyle_flags;

    int16_t     dInvisibility;
    int8_t      cLineWeight;
};

struct DWG2000_CEHD
{
    CADHandle hOwner;
    CADHandle hReactors;
    CADHandle hxdibobjhandle;
    CADHandle hprev_entity, hnext_entity;
    CADHandle hlayer;
    CADHandle hltype;
    CADHandle hplotstyle;
};

class DWGFileR2000 : public CADFile
{
public:
    DWGFileR2000(CADFileIO* poFileIO);
    virtual ~DWGFileR2000();

    virtual size_t GetLayersCount();
    virtual CADGeometry * GetGeometry( size_t layer_index, size_t index );

protected:
    virtual int ReadHeader() override;
    virtual int ReadClasses() override;
    virtual int CreateFileMap() override;
    virtual int ReadTables(enum OpenOptions eOptions) override;

    Layer * GetLayer( size_t index );
    CADObject * GetObject(long index );

protected:

    std::vector < Layer * > astPresentedLayers; // output usage
    std::vector < CADLayer * > astPresentedCADLayers; // internal usage

    //std::vector < ObjHandleOffset > geometries_map;

    int m_nImageSeeker;
    short m_nCodePage;
    std::vector<SLRecord> m_astSLRecords;
};

#endif // DWG_R2000_H_H

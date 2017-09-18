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

#include "cadfile.h"
#include "../internal/io/cadbitstreamreader.hpp"

using namespace libopencad;

struct SectionLocatorRecord
{
    char byRecordNumber = 0;
    int  dSeeker        = 0;
    int  dSize          = 0;
};

struct DWG2000Ced
{
    int32_t        dLength;
    short       dType;
    int         dObjSizeInBits;
    CADHandle   hHandle;
    CADEedArray eEED;
    bool        bGraphicPresentFlag;

    char dEntMode;
    int  dNumReactors;

    bool   bNoLinks;
    short  dCMColorIndex;
    double dfLtypeScale;

    char ltype_flags;
    char plotstyle_flags;

    short dInvisibility;
    char  cLineWeight;
};

struct DWG2000Cehd
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
    DWGFileR2000(CADFileIO * poFileIO);
    virtual             ~DWGFileR2000();

protected:
    virtual int ReadSectionLocators() override;
    virtual int ReadHeader(enum OpenOptions eOptions) override;
    virtual int ReadClasses(enum OpenOptions eOptions) override;
    virtual int CreateFileMap() override;

    virtual CADObject   * GetObject(int32_t dHandle, bool bHandlesOnly = false) override;
    virtual CADGeometry * GetGeometry(size_t iLayerIndex, int32_t dHandle, int32_t dBlockRefHandle = 0) override;

    CADDictionary GetNOD() override;

protected:
    CADBlockObject           * getBlock(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADEllipseObject         * getEllipse(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADSolidObject           * getSolid(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADPointObject           * getPoint(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADPolyline3DObject      * getPolyLine3D(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADRayObject             * getRay(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADXLineObject           * getXLine(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADLineObject            * getLine(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADTextObject            * getText(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADVertex2DObject        * getVertex2D(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADVertex3DObject        * getVertex3D(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADCircleObject          * getCircle(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADEndblkObject          * getEndBlock(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADPolyline2DObject      * getPolyline2D(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADAttribObject          * getAttributes(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADAttdefObject          * getAttributesDefn(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADLWPolylineObject      * getLWPolyLine(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADArcObject             * getArc(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADSplineObject          * getSpline(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADEntityObject          * getEntity(CADBitStreamReader& reader, int32_t objectType, int32_t objectSize, CADCommonED stCommonEntityData);
    CADInsertObject          * getInsert(CADBitStreamReader& reader, int32_t objectType, int32_t objectSize, CADCommonED stCommonEntityData);
    CADDictionaryObject      * getDictionary(CADBitStreamReader& reader, int32_t objectSize);
    CADXRecordObject         * getXRecord(CADBitStreamReader& reader, int32_t objectSize);
    CADLayerObject           * getLayerObject(CADBitStreamReader& reader, int32_t objectSize);
    CADLayerControlObject    * getLayerControl(CADBitStreamReader& reader, int32_t objectSize);
    CADBlockControlObject    * getBlockControl(CADBitStreamReader& reader, int32_t objectSize);
    CADBlockHeaderObject     * getBlockHeader(CADBitStreamReader& reader, int32_t objectSize);
    CADLineTypeControlObject * getLineTypeControl(CADBitStreamReader& reader, int32_t objectSize);
    CADLineTypeObject        * getLineType1(CADBitStreamReader& reader, int32_t objectSize);
    CADMLineObject           * getMLine(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADPolylinePFaceObject   * getPolylinePFace(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADImageObject           * getImage(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CAD3DFaceObject          * get3DFace(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADVertexMeshObject      * getVertexMesh(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADVertexPFaceObject     * getVertexPFace(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADDimensionObject       * getDimension(CADBitStreamReader& reader, int32_t dObjectType, int32_t objectSize, CADCommonED stCommonEntityData);
    CADMTextObject           * getMText(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData);
    CADImageDefObject        * getImageDef(CADBitStreamReader& reader, int32_t objectSize);
    CADImageDefReactorObject * getImageDefReactor(CADBitStreamReader& reader, int32_t objectSize);
    void                     fillCommonEntityHandleData(CADBitStreamReader& reader, CADEntityObject * pEnt);

protected:
    int                               imageSeeker;
    std::vector<SectionLocatorRecord> sectionLocatorRecords;
};

#endif // DWG_R2000_H_H

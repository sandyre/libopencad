/************************************************************************************
 *  Name: dwg_r2000.h
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

#ifndef LIB_DWG_R2000_H_H
#define LIB_DWG_R2000_H_H

#include "data_structs.h"
#include "constants.h"
#include "cadfile.h"

#include <string>
#include <map>
#include <vector>

struct DWG2000_CLASS
{
    int16_t     dClassNum; // BITSHORT
    int16_t     dVersion; // BITSHORT
    std::string      sAppName; // TV
    std::string      sCppClassName; // TV
    std::string      sDXFClassName; // TV
    bool        bWasAZombie; // BIT
    int16_t     dItemClassID; // BITSHORT
};

struct DWG2000_CED
{
    int64_t     dLength;
    int16_t     dType;
    int32_t     dObjSizeInBits;
    DWG_HANDLE  hHandle;
    int16_t     dEEDSize;
    DWG_EED     eEED;
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
    DWG_HANDLE hOwner;
    DWG_HANDLE hReactors;
    DWG_HANDLE hxdibobjhandle;
    DWG_HANDLE hprev_entity, hnext_entity;
    DWG_HANDLE hlayer;
    DWG_HANDLE hltype;
    DWG_HANDLE hplotstyle;
};

struct DWG2000_HEADER_VARIABLES
{
    double Unknown1;
    double Unknown2;
    double Unknown3;
    double Unknown4;
    std::string Unknown5;
    std::string Unknown6;
    std::string Unknown7;
    std::string Unknown8;
    long   Unknown9;
    long   Unknown10;
    bool DIMASO;
    bool DIMSHO;
    bool PLINEGEN;
    bool ORTHOMODE;
    bool REGENMODE;
    bool FILLMODE;
    bool QTEXTMODE;
    bool PSLTSCALE;
    bool LIMCHECK;
    bool MIRRTEXT;
    bool WORLDVIEW;
    bool TILEMODE;
    bool PLIMCHECK;
    bool VISRETAIN;
    bool DISPSILH;
    bool PELLIPSE;
    short PROXYGRAPHICS;
    short TREEDEPTH;
    short LUNITS;
    short LUPREC;
    short AUNITS;
    short AUPREC;
    short ATTMODE;
    short PDMODE;
    short USERI1;
    short USERI2;
    short USERI3;
    short USERI4;
    short USERI5;
    short SPLINESEGS;
    short SURFU;
    short SURFV;
    short SURFTYPE;
    short SURFTAB1;
    short SURFTAB2;
    short SPLINETYPE;
    short SHADEDGE;
    short SHADEDIF;
    short UNITMODE;
    short MAXACTVP;
    short ISOLINES;
    short CMLJUST;
    short TEXTQLTY;
    double LTSCALE;
    double TEXTSIZE;
    double TRACEWID;
    double SKETCHING;
    double FILLETRAD;
    double THICKNESS;
    double ANGBASE;
    double PDSIZE;
    double PLINEWID;
    double USERR1;
    double USERR2;
    double USERR3;
    double USERR4;
    double USERR5;
    double CHAMFERA;
    double CHAMFERB;
    double CHAMFERC;
    double CHAMFERD;
    double FACETRES;
    double CMLSCALE;
    double CELTSCALE;
    std::string MENUNAME;
    long   TDCREATE1; // TODO: WTF?
    long   TDCREATE2;
    long   TDUPDATE1;
    long   TDUPDATE2;

    long   TDINDWG1;
    long   TDINDWG2;
    long   TDUSRTIMER1;
    long   TDUSRTIMER2;

    short  CECOLOR;
};

struct DWGR2000_FILE_HEADER
{
    int32_t dImageSeeker;
    int16_t dCodePage;

    std::vector < SLRecord >  SLRecords;
    std::map < std::string, char > header_variables;
};

typedef std::pair< long long, long long > ObjHandleOffset;

class DWGFileR2000 : public CADFile
{
public:
    DWGFileR2000(const char *pszFileName);
    virtual ~DWGFileR2000();

    virtual size_t getGeometriesCount();
//    size_t getLayersCount();
//    size_t getBlocksCount();
    virtual CADGeometry * getGeometry( size_t index );
//    CADBlock * getBlock( size_t index );
//    CADLayer * getLayer( size_t index );

protected:
    virtual void ReadHeader();
    virtual void ReadClassesSection();
    virtual void ReadObjectMap();

    DWG2000_CLASS ReadClass( const char * input_array, size_t& bitOffset );

protected:
    std::vector < ObjHandleOffset > geometries_map;
    std::vector < ObjHandleOffset > object_map;
    std::vector < std::vector < ObjHandleOffset > > object_map_sections;
    std::vector < DWG2000_CLASS > custom_classes;
    DWGR2000_FILE_HEADER fileHeader;
};

#endif //LIB_DWG_R2000_H_H

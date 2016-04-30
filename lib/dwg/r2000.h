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

struct DWGR2000_HEADER_VARIABLES
{
    // Variables are in the order they appear in data stream.
    double      Unknown1;
    double      Unknown2;
    double      Unknown3;
    double      Unknown4;
    std::string Unknown5;
    std::string Unknown6;
    std::string Unknown7;
    std::string Unknown8;
    int32_t     Unknown9;
    int32_t     Unknown10;
    DWG_HANDLE  current_viewport_ent_header;
    bool        DIMASO;
    bool        DIMSHO;
    bool        PLINEGEN;
    bool        ORTHOMODE;
    bool        REGENMODE;
    bool        FILLMODE;
    bool        QTEXTMODE;
    bool        PSLTSCALE;
    bool        LIMCHECK;
    bool        USRTIMER;
    bool        SKPOLY;
    bool        ANGDIR;
    bool        SPLFRAME;
    bool        MIRRTEXT;
    bool        WORDLVIEW;
    bool        TILEMODE;
    bool        PLIMCHECK;
    bool        VISRETAIN;
    bool        DISPSILH;
    bool        PELLIPSE;
    int16_t     PROXYGRAPHICS;
    int16_t     TREEDEPTH;
    int16_t     LUNITS;
    int16_t     LUPREC;
    int16_t     AUNITS;
    int16_t     AUPREC;
    int16_t     ATTMODE;
    int16_t     PDMODE;
    int16_t     USERI1;
    int16_t     USERI2;
    int16_t     USERI3;
    int16_t     USERI4;
    int16_t     USERI5;
    int16_t     SPLINESEGS;
    int16_t     SURFU;
    int16_t     SURFV;
    int16_t     SURFTYPE;
    int16_t     SURFTAB1;
    int16_t     SURFTAB2;
    int16_t     SPLINETYPE;
    int16_t     SHADEDGE;
    int16_t     SHADEDIF;
    int16_t     UNITMODE;
    int16_t     MAXACTVP;
    int16_t     ISOLINES;
    int16_t     CMLJUST;
    int16_t     TEXTQLTY;
    double      LTSCALE;
    double      TEXTSIZE;
    double      TRACEWID;
    double      SKETCHINC;
    double      FILLETRAD;
    double      THICKNESS;
    double      ANGBASE;
    double      PDSIZE;
    double      PLINEWID;
    double      USERR1;
    double      USERR2;
    double      USERR3;
    double      USERR4;
    double      USERR5;
    double      CHAMFERA;
    double      CHAMFERB;
    double      CHAMFERC;
    double      CHAMFERD;
    double      FACETRES;
    double      CMLSCALE;
    double      CELTSCALE;
    std::string MENUNAME;
    int32_t     TDCREATE_DAY; // Julian day
    int32_t     TDCREATE_MS; // Milliseconds into the day
    int32_t     TDUPDATE_DAY;
    int32_t     TDUPDATE_MS;
    int32_t     TDINDWG_DAY;
    int32_t     TDINDWG_MS;
    int32_t     TDUSRTIMER_DAY;
    int32_t     TDUSRTIMER_MS;
    int16_t     CECOLOR; // in R15 its just CMC color index.
    DWG_HANDLE  HANDSEED; // TODO: is it relevant in R15?
    DWG_HANDLE  CLAYER;
    DWG_HANDLE  TEXTSTYLE;
    DWG_HANDLE  CELTYPE;
    DWG_HANDLE  DIMSTYLE;
    DWG_HANDLE  CMLSTYLE;
    double      PSVPSCALE;

    // PSPACE
    Vertex3D    INSBASE_PSPACE;
    Vertex3D    EXTMIN_PSPACE;
    Vertex3D    EXTMAX_PSPACE;
    Vertex2D    LIMMIN_PSPACE;
    Vertex2D    LIMMAX_PSPACE;
    double      ELEVATION_PSPACE;
    Vertex3D    UCSORG_PSPACE;
    Vertex3D    UCSXDIR_PSPACE;
    Vertex3D    UCSYDIR_PSPACE;
    DWG_HANDLE  UCSNAME_PSPACE;

    DWG_HANDLE  PUCSORTHOREF;
    int16_t     PUCSORTHOVIEW;
    DWG_HANDLE  PUCSBASE;
    Vertex3D    PUCSORGTOP;
    Vertex3D    PUCSORGBOTTOM;
    Vertex3D    PUCSORGLEFT;
    Vertex3D    PUCSORGRIGHT;
    Vertex3D    PUCSORGFRONT;
    Vertex3D    PUCSORGBACK;

    // MSPACE
    Vertex3D    INSBASE_MSPACE;
    Vertex3D    EXTMIN_MSPACE;
    Vertex3D    EXTMAX_MSPACE;
    Vertex2D    LIMMIN_MSPACE;
    Vertex2D    LIMMAX_MSPACE;
    double      ELEVATION_MSPACE;
    Vertex3D    UCSORG_MSPACE;
    Vertex3D    UCSXDIR_MSPACE;
    Vertex3D    UCSYDIR_MSPACE;
    DWG_HANDLE  UCSNAME_MSPACE;

    DWG_HANDLE  UCSORTHOREF;
    int16_t     UCSORTHOVIEW;
    DWG_HANDLE  UCSBASE;
    Vertex3D    UCSORGTOP;
    Vertex3D    UCSORGBOTTOM;
    Vertex3D    UCSORGLEFT;
    Vertex3D    UCSORGRIGHT;
    Vertex3D    UCSORGFRONT;
    Vertex3D    UCSORGBACK;
    std::string DIMPOST;
    std::string DIMAPOST;

    double      DIMSCALE;
    double      DIMASZ;
    double      DIMEXO;
    double      DIMDLI;
    double      DIMEXE;
    double      DIMRND;
    double      DIMDLE;
    double      DIMTP;
    double      DIMTM;
    bool        DIMTOL;
    bool        DIMLIM;
    bool        DIMTIH;
    bool        DIMTOH;
    bool        DIMSE1;
    bool        DIMSE2;
    int16_t     DIMTAD;
    int16_t     DIMZIN;
    int16_t     DIMAZIN;
    double      DIMTXT;
    double      DIMCEN;
    double      DIMTSZ;
    double      DIMALTF;
    double      DIMLFAC;
    double      DIMTVP;
    double      DIMTFAC;
    double      DIMGAP;
    double      DIMALTRND;
    bool        DIMALT;
    int16_t     DIMALTD;
    bool        DIMTOFL;
    bool        DIMSAH;
    bool        DIMTIX;
    bool        DIMSOXD;
    int16_t     DIMCLRD;
    int16_t     DIMCLRE;
    int16_t     DIMCLRT;

    int16_t     DIMADEC;
    int16_t     DIMDEC;
    int16_t     DIMTDEC;
    int16_t     DIMALTU;
    int16_t     DIMALTTD;
    int16_t     DIMAUNIT;
    int16_t     DIMFRAC;
    int16_t     DIMLUNIT;
    int16_t     DIMDSEP;
    int16_t     DIMTMOVE;
    int16_t     DIMJUST;
    bool        DIMSD1;
    bool        DIMSD2;
    int16_t     DIMTOLJ;
    int16_t     DIMTZIN;
    int16_t     DIMALTZ;
    int16_t     DIMALTTZ;
    bool        DIMUPT;
    int16_t     DIMATFIT;

    DWG_HANDLE  DIMTXSTY;
    DWG_HANDLE  DIMLDRBLK;
    DWG_HANDLE  DIMBLK;
    DWG_HANDLE  DIMBLK1;
    DWG_HANDLE  DIMBLK2;
    int16_t     DIMLWD;
    int16_t     DIMLWE;

    DWG_HANDLE  BLOCK_CONTROL_OBJ;
    DWG_HANDLE  LAYER_CONTROL_OBJ;
    DWG_HANDLE  STYLE_CONTROL_OBJ;
    DWG_HANDLE  LINETYPE_CONTROL_OBJ;
    DWG_HANDLE  VIEW_CONTROL_OBJ;
    DWG_HANDLE  UCS_CONTROL_OBJ;
    DWG_HANDLE  VPORT_CONTROL_OBJ;
    DWG_HANDLE  APPID_CONTROL_OBJ;
    DWG_HANDLE  DIMSTYLE_CONTROL_OBJ;

    // R13-R15
    DWG_HANDLE  VIEWPORT_ENT_HEADER_CONTROL_OBJ;

    // Common
    DWG_HANDLE  DICTIONARY_ACAD_GROUP;
    DWG_HANDLE  DICTIONARY_ACAD_MLINESTYLE;
    DWG_HANDLE  DICTIONARY_NAMED_OBJECTS;

    // R15+
    int16_t     TSTACKALIGN;
    int16_t     TSTACKSIZE;
    std::string HYPERLINKBASE;
    std::string STYLESHEET;
    DWG_HANDLE  DICTIONARY_LAYOUTS;
    DWG_HANDLE  DICTIONARY_PLOTSETTINGS;
    DWG_HANDLE  DICTIONARY_PLOTSTYLES;

    // R15+
    bool        CELWEIGHT;
    bool        ENDCAPS;
    bool        JOINSTYLE;
    bool        LWDISPLAY;
    bool        XEDIT;
    bool        EXTNAMES;
    bool        PSTYLEMODE;
    bool        OLESTARTUP;
    int16_t     INSUNITS;
    int16_t     CEPSNTYPE;
    DWG_HANDLE  CPSNID;
    std::string FINGERPRINTGUID;
    std::string VERSIONGUID;

    DWG_HANDLE  BLOCK_RECORD_PSPACE;
    DWG_HANDLE  BLOCK_RECORD_MSPACE;
    DWG_HANDLE  LTYPE_BYLAYER;
    DWG_HANDLE  LTYPE_BYBLOCK;
    DWG_HANDLE  LTYPE_CONTINUOUS;

    int16_t     UnknownShortInEnd1; // only 5/6 values are legal. But AutoCAD generated files contains various values.
    int16_t     UnknownShortInEnd2;
    int16_t     UnknownShortInEnd3;
    int16_t     UnknownShortInEnd4;
    uint16_t    CRC; // initial value is 0xC0C1
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
    DWGFileR2000(CADFileIO* poFileIO);
    virtual ~DWGFileR2000();

    virtual size_t getGeometriesCount();
    virtual size_t getLayersCount();
//    size_t getBlocksCount();
    virtual CADGeometry * getGeometry( size_t index );
//    CADBlock * getBlock( size_t index );
    virtual CADLayer * getLayer( size_t index );

protected:
    virtual void ReadHeader();
    virtual void ReadClassesSection();
    virtual void ReadObjectMap();

    DWG2000_CLASS ReadClass( const char * input_array, size_t& bitOffset );
//    DWGObject * getObject( size_t section, size_t index );
protected:
//    std::vector < DWGObject * > objects;
    std::vector < ObjHandleOffset > layer_map;
    std::vector < ObjHandleOffset > geometries_map;
    std::vector < std::vector < ObjHandleOffset > > object_map_sections;
    std::vector < DWG2000_CLASS > custom_classes;
    DWGR2000_FILE_HEADER file_header;
    DWGR2000_HEADER_VARIABLES header_variables;
};

#endif //LIB_DWG_R2000_H_H

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

#ifndef CADOBJECTS_H
#define CADOBJECTS_H

#include <stdint.h>
#include "simpledatatypes.h"
#include "dwg/data_structs.h"

class CADObject
{
public:
    enum CADObjectType
    {
        UNUSED = 0x0,
        TEXT = 0x1,
        ATTRIB = 0x2,
        ATTDEF = 0x3,
        BLOCK = 0x4,
        ENDBLK = 0x5,
        SEQEND = 0x6,
        INSERT = 0x7,
        MINSERT1 = 0x8,
        MINSERT2 = 0x9,
        VERTEX2D = 0x0A,
        VERTEX3D = 0x0B,
        VERTEX_MESH = 0x0C,
        VERTEX_PFACE = 0x0D,
        VERTEX_PFACE_FACE = 0x0E,
        POLYLINE2D = 0x0F,
        POLYLINE3D = 0x10,
        ARC = 0x11,
        CIRCLE = 0x12,
        LINE = 0x13,
        DIMENSION_ORDINATE = 0x14,
        DIMENSION_LINEAR = 0x15,
        DIMENSION_ALIGNED = 0x16,
        DIMENSION_ANG_3PT = 0x17,
        DIMENSION_ANG_2LN = 0x18,
        DIMENSION_RADIUS = 0x19,
        DIMENSION_DIAMETER = 0x1A,
        POINT = 0x1B,
        FACE3D = 0x1C,
        POLYLINE_PFACE = 0x1D,
        POLYLINE_MESH = 0x1E,
        SOLID = 0x1F,
        TRACE = 0x20,
        SHAPE = 0x21,
        VIEWPORT = 0x22,
        ELLIPSE = 0x23,
        SPLINE = 0x24,
        REGION = 0x25,
        SOLID3D = 0x26,
        BODY = 0x27,
        RAY = 0x28,
        XLINE = 0x29,
        DICTIONARY = 0x2A,
        OLEFRAME = 0x2B,
        MTEXT = 0x2C,
        LEADER = 0x2D,
        TOLERANCE = 0x2E,
        MLINE = 0x2F,
        BLOCK_CONTROL_OBJ = 0x30,
        BLOCK_HEADER = 0x31,
        LAYER_CONTROL_OBJ = 0x32,
        LAYER = 0x33,
        STYLE_CONTROL_OBJ = 0x34,
        STYLE1 = 0x35,
        STYLE2 = 0x36,
        STYLE3 = 0x37,
        LTYPE_CONTROL_OBJ = 0x38,
        LTYPE1 = 0x39,
        LTYPE2 = 0x3A,
        LTYPE3 = 0x3B,
        VIEW_CONTROL_OBJ = 0x3C,
        VIEW = 0x3D,
        UCS_CONTROL_OBJ = 0x3E,
        UCS = 0x3F,
        VPORT_CONTROL_OBJ = 0x40,
        VPORT = 0x41,
        APPID_CONTROL_OBJ = 0x42,
        APPID = 0x43,
        DIMSTYLE_CONTROL_OBJ = 0x44,
        DIMSTYLE = 0x45,
        VP_ENT_HDR_CTRL_OBJ = 0x46,
        VP_ENT_HDR = 0x47,
        GROUP = 0x48,
        MLINESTYLE = 0x49,
        OLE2FRAME = 0x4A,
        DUMMY = 0x4B,
        LONG_TRANSACTION = 0x4C,
        LWPOLYLINE = 0x4D,
        HATCH = 0x4E,
        XRECORD = 0x4F,
        ACDBPLACEHOLDER = 0x50,
        VBA_PROJECT = 0x51,
        LAYOUT = 0x52,
        // Codes below arent fixed, libopencad uses it for reading, in writing it will be different!
        CELLSTYLEMAP = 0x53,
        DBCOLOR = 0x54,
        DICTIONARYVAR = 0x55,
        DICTIONARYWDFLT = 0x56,
        FIELD = 0x57,
        GROUP_UNFIXED = 0x58,
        HATCH_UNFIXED = 0x59,
        IDBUFFER = 0x5A,
        IMAGE = 0x5B,
        IMAGEDEF = 0x5C,
        IMAGEDEFREACTOR = 0x5D,
        LAYER_INDEX = 0x5E,
        LAYOUT_UNFIXED = 0x5F,
        LWPOLYLINE_UNFIXED = 0x60,
        MATERIAL = 0x61,
        MLEADER = 0x62,
        MLEADERSTYLE = 0x63,
        OLE2FRAME_UNFIXED = 0x64,
        PLACEHOLDER = 0x65,
        PLOTSETTINGS = 0x66,
        RASTERVARIABLES = 0x67,
        SCALE = 0x68,
        SORTENTSTABLE = 0x69,
        SPATIAL_FILTER = 0x6A,
        SPATIAL_INDEX = 0x6B,
        TABLEGEOMETRY = 0x6C,
        TABLESTYLES = 0x6D,
        VBA_PROJECT_UNFIXED = 0x6E,
        VISUALSTYLE = 0x6F,
        WIPEOUTVARIABLE = 0x70,
        XRECORD_UNFIXED = 0x71
    };

    long  dObjectSize;
    CADObjectType eObjectType;

    short dCRC;
};

struct CADCommonED
{
    long nObjectSizeInBits;
    CADHandle hObjectHandle;
    std::vector < CAD_EED > aEED;

    bool bGraphicsPresented;
    std::vector < char > abyGraphicsData;

    char bbEntMode;
    long nNumReactors;

    bool bNoXDictionaryHandlePresent;
    bool bBinaryDataPresent;

    bool bIsByLayerLT; // R13-14 only

    bool bNoLinks;
    short nCMColor;

    double dfLTypeScale;
    char bbLTypeFlags;
    char bbPlotStyleFlags;
    char bbMaterialFlags;
    char nShadowFlags;

    short nInvisibility;
    char  nLineWeight;
};

struct CADCommonEHD
{
    CADHandle hOwner; // depends on entmode.
    std::vector < CADHandle > hReactors;
    CADHandle hXDictionary;
    CADHandle hLayer;
    CADHandle hLType;

    CADHandle hPrevEntity;
    CADHandle hNextEntity;

    CADHandle hColorBookHandle;

    CADHandle hMaterial;
    CADHandle hPlotStyle;

    CADHandle hFullVisualStyle;
    CADHandle hFaceVisualStyle;
    CADHandle hEdgeVisualStyle;
};

/*
 * @brief The abstract class, which contains data common to all entities
 */
class CADEntity : public CADObject
{
public:
    struct CADCommonED ced;
    struct CADCommonEHD ched;
};

class CADText : public CADEntity
{
public:
    CADText()
    {
        eObjectType = TEXT;
    }
    char   DataFlags;
    double dfElevation;
    Vertex2D vertInsetionPoint;
    Vertex2D vertAlignmentPoint;
    Vector3D vectExtrusion;
    double dfThickness;
    double dfObliqueAng;
    double dfRotationAng;
    double dfHeight;
    double dfWidthFactor;
    std::string sTextValue;
    short  dGeneration;
    short  dHorizAlign;
    short  dVertAlign;

    CADHandle hStyle;
};

class CADAttrib : public CADEntity
{
public:
    CADAttrib()
    {
        eObjectType = ATTRIB;
    }
    char   DataFlags;
    double dfElevation;
    Vertex2D vertInsetionPoint;
    Vertex2D vertAlignmentPoint;
    Vector3D vectExtrusion;
    double dfThickness;
    double dfObliqueAng;
    double dfRotationAng;
    double dfHeight;
    double dfWidthFactor;
    std::string sTextValue;
    short  dGeneration;
    short  dHorizAlign;
    short  dVertAlign;
    char   dVersion; // R2010+
    std::string sTag;
    short  nFieldLength;
    char   nFlags;
    bool   bLockPosition;

    CADHandle hStyle;
};

class CADAttdef : public CADEntity
{
public:
    CADAttdef()
    {
        eObjectType = ATTDEF;
    }
    char   DataFlags;
    double dfElevation;
    Vertex2D vertInsetionPoint;
    Vertex2D vertAlignmentPoint;
    Vector3D vectExtrusion;
    double dfThickness;
    double dfObliqueAng;
    double dfRotationAng;
    double dfHeight;
    double dfWidthFactor;
    std::string sDefaultValue;
    short  dGeneration;
    short  dHorizAlign;
    short  dVertAlign;
    char   dVersion; // R2010+
    std::string sTag;
    short  nFieldLength;
    char   nFlags;
    std::string sPrompt;
    bool   bLockPosition;

    CADHandle hStyle;
};

class CADBlock : public CADEntity
{
public:
    CADBlock()
    {
        eObjectType = BLOCK;
    }
    std::string sBlockName;
};

class CADEndblk : public CADEntity
{
public:
    CADEndblk()
    {
        eObjectType = ENDBLK;
    }
    // it actually has nothing more thatn CED and CEHD.
};

class CADSeqend : public CADEntity
{
public:
    CADSeqend()
    {
        eObjectType = SEQEND;
    }
    // it actually has nothing more thatn CED and CEHD.
};

class CADInsert : public CADEntity
{
public:
    CADInsert()
    {
        eObjectType = INSERT;
    }
    Vertex3D vertInsertionPoint;
    Vertex3D vertScales;
    double dfRotation;
    Vector3D vectExtrusion;
    bool    bHasAttribs;
    long    nObjectsOwned;

    CADHandle hBlockHeader;
    std::vector < CADHandle > hAtrribs;
    CADHandle hSeqend; // if bHasAttribs == true
};

class CADMInsert : public CADEntity
{
public:
    CADMInsert()
    {
        eObjectType = MINSERT1; // TODO: it has 2 type codes?
    }
    Vertex3D vertInsertionPoint;
    Vertex3D vertScales;
    double dfRotation;
    Vector3D vectExtrusion;
    bool    bHasAttribs;
    long    nObjectsOwned;

    short nNumCols;
    short nNumRows;
    short nColSpacing;
    short nRowSpacing;

    CADHandle hBlockHeader;
    std::vector < CADHandle > hAtrribs;
    CADHandle hSeqend; // if bHasAttribs == true
};

class CADVertex2D : public CADEntity
{
public:
    CADVertex2D()
    {
        eObjectType = VERTEX2D;
    }
    Vertex3D vertPosition; // Z must be taken from 2d polyline elevation.
    double   dfStartWidth;
    double   dfEndWidth;
    double   dfBulge;
    long     nVertexID;
    double   dfTangentDir;

/*    NOTES: Neither elevation nor thickness are present in the 2D VERTEX data. Both should be taken from the 2D POLYLINE entity (15) */
};

class CADVertex3D : public CADEntity
{
public:
    CADVertex3D()
    {
        eObjectType = VERTEX3D;
    }
    ~CADVertex3D ()
    {
    }

    Vertex3D vertPosition;
};

class CADVertexMesh : public CADEntity
{
public:
    CADVertexMesh()
    {
        eObjectType = VERTEX_MESH;
    }
    Vertex3D vertPosition;
};

class CADVertexPFace : public CADEntity
{
public:
    CADVertexPFace ()
    {
        eObjectType = VERTEX_PFACE;
    }
    Vertex3D vertPosition;
};

class CADVertexPFaceFace : public CADEntity
{
public:
    CADVertexPFaceFace()
    {
        eObjectType = VERTEX_PFACE_FACE;
    }
    // TODO: check DXF ref to get info what does it mean.
    short iVertexIndex1;
    short iVertexIndex2;
    short iVertexIndex3;
    short iVertexIndex4;
};

class CADPolyline2D : public CADEntity
{
public:
    CADPolyline2D()
    {
        eObjectType = POLYLINE2D;
    }

    short  dFlags;
    short  dCurveNSmoothSurfType;
    double dfStartWidth;
    double dfEndWidth;
    double dfThickness;
    double dfElevation;
    Vector3D vectExtrusion;

    long   nObjectsOwned;

    std::vector < CADHandle > hVertexes; // content really depends on DWG version.

    CADHandle hSeqend;
};

class CADPolyline3D : public CADEntity
{
public:
    CADPolyline3D()
    {
        eObjectType = POLYLINE3D;
    }
    char SplinedFlags;
    char ClosedFlags;

    long   nObjectsOwned;

    std::vector < CADHandle > hVertexes; // content really depends on DWG version.

    CADHandle hSeqend;
};

class CADArc : public CADEntity
{
public:
    CADArc()
    {
        eObjectType = ARC;
    }
    Vertex3D vertPosition;
    double   dfRadius;
    double   dfThickness;
    Vector3D vectExtrusion;
    double   dfStartAngle;
    double   dfEndAngle;
};

class CADCircle : public CADEntity
{
public:
    CADCircle()
    {
        eObjectType = CIRCLE;
    }
    Vertex3D vertPosition;
    double   dfRadius;
    double   dfThickness;
    Vector3D vectExtrusion;
};

class CADLine : public CADEntity
{
public:
    CADLine()
    {
        eObjectType = LINE;
    }
    Vertex3D vertStart;
    Vertex3D vertEnd;
    double   dfThickness;
    Vector3D vectExtrusion;
};

class CADBlockControl : public CADObject
{
public:
    CADBlockControl()
    {
        eObjectType = BLOCK_CONTROL_OBJ;
    }
    long nObjectSizeInBits;
    CADHandle hObjectHandle;
    std::vector < CAD_EED > aEED;
    long nNumReactors;
    bool bNoXDictionaryPresent;
    long nNumEntries; // doesnt count MODELSPACE and PAPERSPACE
    CADHandle hNull;
    CADHandle hXDictionary;
    std::vector < CADHandle > hBlocks; // ends with modelspace and paperspace handles.
};

class CADBlockHeader : public CADObject
{
public:
    CADBlockHeader()
    {
        eObjectType = BLOCK_HEADER;
    }
    long nObjectSizeInBits;
    CADHandle hObjectHandle;
    std::vector < CAD_EED > aEED;
    long nNumReactors;
    bool bNoXDictionaryPresent;
    std::string sEntryName;
    bool b64Flag;
    short dXRefIndex;
    bool bXDep;
    bool bAnonymous;
    bool bHasAtts;
    bool bBlkisXRef;
    bool bXRefOverlaid;
    bool bLoadedBit;
    long nOwnedObjectsCount;
    Vertex3D vertBasePoint;
    std::string sXRefPName;
    std::vector < char > adInsertCount; // TODO: ???
    std::string sBlockDescription;
    long nSizeOfPreviewData;
    std::vector < char > abyBinaryPreviewData;
    short nInsertUnits;
    bool bExplodable;
    char dBlockScaling;
    CADHandle hBlockControl;
    std::vector < CADHandle > hReactors;
    CADHandle hXDictionary;
    CADHandle hNull;
    CADHandle hBlockEntity;
    std::vector < CADHandle > hEntities;
    CADHandle hEndBlk;
    std::vector < CADHandle > hInsertHandles;
    CADHandle hLayout;
};

class CADLayerControl : public CADObject
{
public:
    CADLayerControl()
    {
        eObjectType = LAYER_CONTROL_OBJ;
    }

    long nObjectSizeInBits;
    CADHandle hObjectHandle;
    std::vector < CAD_EED > aEED;
    long nNumReactors;
    bool bNoXDictionaryPresent;
    long nNumEntries; // counts layer "0"
    CADHandle hNull;
    CADHandle hXDictionary;
    std::vector < CADHandle > hLayers;
};

class CADLayer : public CADObject
{
public:
    CADLayer()
    {
        eObjectType = LAYER;
    }

    long nObjectSizeInBits;
    CADHandle hObjectHandle;
    std::vector < CAD_EED > aEED;
    long nNumReactors;
    bool bNoXDictionaryPresent;
    std::string sLayerName;
    bool b64Flag;
    short dXRefIndex;
    bool bXDep;
    bool bFrozen;
    bool bOn;
    bool bFrozenInNewVPORT;
    bool bLocked;
    bool bPlottingFlag;
    short dLineWeight;
    short dCMColor;

    CADHandle hLayerControl;
    std::vector < CADHandle > hReactors;
    CADHandle hXDictionary;
    CADHandle hExternalRefBlockHandle;
    CADHandle hPlotStyle;
    CADHandle hMaterial;
    CADHandle hLType;
    CADHandle hUnknownHandle;
};

class CADLineTypeControl : public CADObject
{
public:
    CADLineTypeControl()
    {
        eObjectType = LTYPE_CONTROL_OBJ;
    }

    long nObjectSizeInBits;
    CADHandle hObjectHandle;
    std::vector < CAD_EED > aEED;
    long nNumReactors;
    bool bNoXDictionaryPresent;
    long nNumEntries; // doesnt count BYBLOCK / BYLAYER.
    CADHandle hNull;
    CADHandle hXDictionary;
    std::vector < CADHandle > hLTypes;
};

class CADLineType : public CADObject
{
public:
    CADLineType()
    {
        eObjectType = LTYPE1;
    }

    long nObjectSizeInBits;
    CADHandle hObjectHandle;
    std::vector < CAD_EED > aEED;
    long nNumReactors;
    bool bNoXDictionaryPresent;
    std::string sEntryName;
    bool b64Flag;
    short dXRefIndex;
    bool bXDep;
    std::string sDescription;
    double dfPatternLen;
    char dAlignment;
    char nNumDashes;
    struct Dash
    {
        double dfLength;
        short dComplexShapecode;
        double dfXOffset;
        double dfYOffset;
        double dfScale;
        double dfRotation;
        short dShapeflag;
    };
    std::vector < Dash > astDashes;
    std::vector < char > abyTextArea; // TODO: what is it?
    CADHandle hLTControl;
    std::vector < CADHandle > hReactors;
    CADHandle hXDictionary;
    CADHandle hXRefBlock;
    std::vector < CADHandle > hShapefiles; // TODO: one for each dash?
};

class CADPoint : public CADEntity
{
public:
    CADPoint()
    {
        eObjectType = POINT;
    }

    Vertex3D vertPosition;
    double   dfThickness;
    Vector3D vectExtrusion;
    double   dfXAxisAng;
};

class CADSolid : public CADEntity
{
public:
    CADSolid()
    {
        eObjectType = SOLID;
        avertCorners.reserve ( 4 );
    }

    double dfThickness;
    double dfElevation;
    std::vector < Vertex2D > avertCorners;
    Vector3D vectExtrusion;
};

class CADEllipse : public CADEntity
{
public:
    CADEllipse()
    {
        eObjectType = ELLIPSE;
    }

    Vertex3D vertPosition;
    Vector3D vectSMAxis;
    Vector3D vectExtrusion;
    double   dfAxisRatio;
    double   dfBegAngle;
    double   dfEndAngle;
};

class CADRay : public CADEntity
{
public:
    CADRay()
    {
        eObjectType = RAY;
    }

    Vertex3D vertPosition;
    Vector3D vectVector;
};

class CADXLine : public CADEntity
{
public:
    CADXLine()
    {
        eObjectType = XLINE;
    }

    Vertex3D vertPosition;
    Vector3D vectVector;
};

class CADDictionary : public CADObject
{
public:
    CADDictionary()
    {
        eObjectType = DICTIONARY;
    }

    long nObjectSizeInBits;
    CADHandle hObjectHandle;
    std::vector < CAD_EED > aEED;
    long nNumReactors;
    bool bNoXDictionaryPresent;
    long nNumItems;
    short dCloningFlag;
    char  dHardOwnerFlag;

    std::string sDictionaryEntryName;
    std::vector < std::string > sItemNames;

    CADHandle hParentHandle;
    std::vector < CADHandle > hReactors;
    CADHandle hXDictionary;
    std::vector < CADHandle > hItemHandles;
};

class CADLWPolyline : public CADEntity
{
public:
    CADLWPolyline()
    {
        eObjectType = LWPOLYLINE;
    }

    double dfConstWidth;
    double dfElevation;
    double dfThickness;
    Vector3D vectExtrusion;
    std::vector< Vertex2D > avertVertexes;
    std::vector< double > adfBulges;
    std::vector< int16_t > adVertexesID;
    std::vector< std::pair< double, double > > astWidths; // start, end.
};

class CADSpline : public CADEntity
{
public:
    CADSpline() : nNumFitPts(0),
                  nNumKnots(0),
                  nNumCtrlPts(0) // should be zeroed.
    {
        eObjectType = SPLINE;
    }

    long dScenario;
    long dSplineFlags; // 2013+
    long dKnotParameter; // 2013+

    long   dDegree;
    double dfFitTol;
    Vector3D vectBegTangDir;
    Vector3D vectEndTangDir;
    long   nNumFitPts;

    bool bRational;
    bool bClosed;
    bool bPeriodic;
    double dfKnotTol;
    double dfCtrlTol;
    long nNumKnots;
    long nNumCtrlPts;
    bool bWeight;

    std::vector < double > adfKnots;
    std::vector < double > adfCtrlPointsWeight;
    std::vector < Vertex3D > avertCtrlPoints;
    std::vector < Vertex3D > averFitPoints;
};

struct CommonDimensionData
{
    char dVersion;
    Vector3D vectExtrusion;
    Vertex2D vertTextMidPt;
    double dfElevation;
    char dFlags;
    std::string sUserText;
    double dfTextRotation;
    double dfHorizDir;
    double dfInsXScale;
    double dfInsYScale;
    double dfInsZScale;
    double dfInsRotation;

    short dAttachmentPoint;
    short dLineSpacingStyle;
    double dfLineSpacingFactor;
    double dfActualMeasurement;

    bool bUnknown;
    bool bFlipArrow1;
    bool bFlipArrow2;

    Vertex2D vert12Pt;
};

class CADDimensionOrdinate : public CADEntity
{
public:
    CADDimensionOrdinate()
    {
        eObjectType = DIMENSION_ORDINATE;
    }

    struct CommonDimensionData cdd;
    Vertex3D vert10pt;
    Vertex3D vert13pt;
    Vertex3D vert14pt;
    char Flags2;

    CADHandle hDimstyle;
    CADHandle hAnonymousBlock;
};

class CADDimensionLinear : public CADEntity
{
public:
    CADDimensionLinear()
    {
        eObjectType = DIMENSION_LINEAR;
    }

    struct CommonDimensionData cdd;
    Vertex3D vert13pt;
    Vertex3D vert14pt;
    Vertex3D vert10pt;
    double dfExtLnRot;
    double dfDimRot;

    CADHandle hDimstyle;
    CADHandle hAnonymousBlock;
};

class CADDimensionAligned : public CADEntity
{
public:
    CADDimensionAligned()
    {
        eObjectType = DIMENSION_ALIGNED;
    }

    struct CommonDimensionData cdd;
    Vertex3D vert13pt;
    Vertex3D vert14pt;
    Vertex3D vert10pt;
    double dfExtLnRot;

    CADHandle hDimstyle;
    CADHandle hAnonymousBlock;
};

class CADDimensionAngular3Pt : public CADEntity
{
public:
    CADDimensionAngular3Pt()
    {
        eObjectType = DIMENSION_ANG_3PT;
    }

    struct CommonDimensionData cdd;
    Vertex3D vert10pt;
    Vertex3D vert13pt;
    Vertex3D vert14pt;
    Vertex3D vert15pt;

    CADHandle hDimstyle;
    CADHandle hAnonymousBlock;
};

class CADDimensionAngular2Ln : public CADEntity
{
public:
    CADDimensionAngular2Ln()
    {
        eObjectType = DIMENSION_ANG_2LN;
    }

    struct CommonDimensionData cdd;
    Vertex3D vert16pt;
    Vertex3D vert13pt;
    Vertex3D vert14pt;
    Vertex3D vert15pt;
    Vertex3D vert10pt;

    CADHandle hDimstyle;
    CADHandle hAnonymousBlock;
};

class CADDimensionRadius : public CADEntity
{
public:
    CADDimensionRadius()
    {
        eObjectType = DIMENSION_RADIUS;
    }

    struct CommonDimensionData cdd;
    Vertex3D vert10pt;
    Vertex3D vert15pt;
    double dfLeaderLen;

    CADHandle hDimstyle;
    CADHandle hAnonymousBlock;
};

class CADDimensionDiameter : public CADEntity
{
public:
    CADDimensionDiameter()
    {
        eObjectType = DIMENSION_DIAMETER;
    }

    struct CommonDimensionData cdd;
    Vertex3D vert15pt;
    Vertex3D vert10pt;
    double dfLeaderLen;

    CADHandle hDimstyle;
    CADHandle hAnonymousBlock;
};

class CADImage : public CADEntity
{
public:
    CADImage()
    {
        eObjectType = IMAGE;
    }

    long dClassVersion;
    Vertex3D vertInsertion;
    Vector3D vectUDirection;
    Vector3D vectVDirection;
    double dfSizeX;
    double dfSizeY;
    /*  display properties (bit coded), 1==show image,
        2==show image when not aligned with screen, 4==use
        clipping boundary, 8==transparency on */
    short dDisplayProps;

    bool bClipping;
    char dBrightness;
    char dContrast;
    char dFade;
    bool bClipMode; // R2010+

    short dClipBoundaryType;

    long nNumberVertexesInClipPolygon;
    std::vector < Vertex2D > avertClippingPolygonVertexes;

    CADHandle hImageDef;
    CADHandle hImageDefReactor;
};

class CADImageDef : public CADObject
{
public:
    CADImageDef()
    {
        eObjectType = IMAGEDEF;
    }
    long nObjectSizeInBits;
    CADHandle hObjectHandle;
    std::vector < CAD_EED > aEED;
    long nNumReactors;
    bool bNoXDictionaryPresent;
    long dClassVersion;
    double dfXImageSizeInPx;
    double dfYImageSizeInPx;
    std::string sFilePath;
    bool bIsLoaded;
    char dResUnits; // 0 == none, 2 == centimeters, 5 == inches
    double dfXPixelSize; // size of 1 pixel in autocad units
    double dfYPixelSize;

    CADHandle hParentHandle;
    std::vector < CADHandle > hReactors;
    CADHandle hXDictionary;
};

class CADImageDefReactor : public CADObject
{
public:
    CADImageDefReactor()
    {
        eObjectType = IMAGEDEFREACTOR;
    }
    long nObjectSizeInBits;
    CADHandle hObjectHandle;
    std::vector < CAD_EED > aEED;
    long nNumReactors;
    bool bNoXDictionaryPresent;
    long dClassVersion;
    CADHandle hParentHandle;
    std::vector < CADHandle > hReactors;
    CADHandle hXDictionary;
};

class CADMText : public CADEntity
{
public:
    CADMText()
    {
        eObjectType = MTEXT;
    }

    Vertex3D vertInsertionPoint;
    Vector3D vectExtrusion;
    Vector3D vectXAxisDir;
    double dfRectWidth;
    double dfTextHeight;
    short dAttachment; // TODO: meaning unknown
    short dDrawingDir;
    double dfExtents; // TODO: meaning unknown
    double dfExtentsWidth; // TODO: meaning unknown
    std::string sTextValue;
    short dLineSpacingStyle;
    short dLineSpacingFactor;
    bool bUnknownBit;
    long dBackgroundFlags;
    long dBackgroundScaleFactor;
    short dBackgroundColor;
    long dBackgroundTransparency;
    CADHandle hStyle;
};

#endif //CADOBJECTS_H

/************************************************************************************
 *  Name: dwg_r2000.cpp
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

#include "r2000.h"
#include "io.h"
#include "cadgeometries.h"
#include "cadobjects.h"
#include "opencad_api.h"

#include <iostream>
#include <cstring>
#include <MacTypes.h>

#define UNKNOWN1 CADHeader::MAX_HEADER_CONSTANT + 1
#define UNKNOWN2 CADHeader::MAX_HEADER_CONSTANT + 2
#define UNKNOWN3 CADHeader::MAX_HEADER_CONSTANT + 3
#define UNKNOWN4 CADHeader::MAX_HEADER_CONSTANT + 4
#define UNKNOWN5 CADHeader::MAX_HEADER_CONSTANT + 5
#define UNKNOWN6 CADHeader::MAX_HEADER_CONSTANT + 6
#define UNKNOWN7 CADHeader::MAX_HEADER_CONSTANT + 7
#define UNKNOWN8 CADHeader::MAX_HEADER_CONSTANT + 8
#define UNKNOWN9 CADHeader::MAX_HEADER_CONSTANT + 9
#define UNKNOWN10 CADHeader::MAX_HEADER_CONSTANT + 10
#define UNKNOWN11 CADHeader::MAX_HEADER_CONSTANT + 11
#define UNKNOWN12 CADHeader::MAX_HEADER_CONSTANT + 12
#define UNKNOWN13 CADHeader::MAX_HEADER_CONSTANT + 13
#define UNKNOWN14 CADHeader::MAX_HEADER_CONSTANT + 14
#define UNKNOWN15 CADHeader::MAX_HEADER_CONSTANT + 15

int DWGFileR2000::ReadHeader ()
{
    char    *pabyBuf = new char[100];
    int     dImageSeeker, dSLRecords;
    short   dCodePage;

    m_poFileIO->Rewind();
    memset(pabyBuf, 0, DWG_VERSION_STR_SIZE + 1);
    m_poFileIO->Read(pabyBuf, DWG_VERSION_STR_SIZE);
    m_poHeader->AddValue(CADHeader::ACADVER, pabyBuf);
    memset(pabyBuf, 0, 8);
    m_poFileIO->Read(pabyBuf, 7);
    m_poHeader->AddValue(CADHeader::ACADMAINTVER, pabyBuf);

    m_poFileIO->Read (&dImageSeeker, 4);

    DebugMsg("Image seeker readed: %d\n", dImageSeeker);

    m_poFileIO->Seek (2, CADFileIO::SeekOrigin::CUR); // 19
    m_poFileIO->Read (&dCodePage, 2);
    m_poHeader->AddValue(CADHeader::DWGCODEPAGE, dCodePage);

    DebugMsg("DWG Code page: %d\n", dCodePage);

    m_poFileIO->Read (&dSLRecords, 4); // 21

    DebugMsg("Section-locator records count: %d\n", dSLRecords);

    for ( size_t i = 0; i < dSLRecords; ++i )
    {
        SLRecord readed_record;
        m_poFileIO->Read (&readed_record.byRecordNumber, 1);
        m_poFileIO->Read (&readed_record.dSeeker, 4);
        m_poFileIO->Read (&readed_record.dSize, 4);

        SLRecords.push_back (readed_record);
        DebugMsg("SL Record #%d : %d %d\n",
                 SLRecords[i].byRecordNumber,
                 SLRecords[i].dSeeker,
                 SLRecords[i].dSize);
    }

/*      READ HEADER VARIABLES        */
    size_t dHeaderVarsSectionLength = 0;

    m_poFileIO->Seek (SLRecords[0].dSeeker, CADFileIO::SeekOrigin::BEG);
    m_poFileIO->Read (pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH);
    if ( memcmp (pabyBuf, DWG_SENTINELS::HEADER_VARIABLES_START,
                 DWG_SENTINELS::SENTINEL_LENGTH) )
    {
        DebugMsg("File is corrupted (wrong pointer to HEADER_VARS section,"
                        "or HEADERVARS starting sentinel corrupted.)");

        delete[] pabyBuf;

        return CADErrorCodes::HEADER_SECTION_READ_FAILED;
    }

    delete[] pabyBuf;

    m_poFileIO->Read (&dHeaderVarsSectionLength, 4);
    DebugMsg("Header variables section length: %ld\n", dHeaderVarsSectionLength);

    size_t nBitOffsetFromStart = 0;
    pabyBuf = new char[dHeaderVarsSectionLength];
    m_poFileIO->Read ( pabyBuf, dHeaderVarsSectionLength + 2 );

    m_poHeader->AddValue(UNKNOWN1, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN2, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN3, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN4, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN5, ReadTV (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN6, ReadTV (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN7, ReadTV (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN8, ReadTV (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN9, ReadBITLONG (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN10, ReadBITLONG (pabyBuf, nBitOffsetFromStart));

    CADHandle stCurrentViewportTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMASO, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMSHO, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PLINEGEN, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::ORTHOMODE, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::REGENMODE, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::FILLMODE, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::QTEXTMODE, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PSLTSCALE, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::LIMCHECK, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::USRTIMER, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SKPOLY, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::ANGDIR, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SPLFRAME, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::MIRRTEXT, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::WORDLVIEW, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::TILEMODE, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PLIMCHECK, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::VISRETAIN, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DISPSILH, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PELLIPSE, ReadBIT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::PROXYGRAPHICS, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::TREEDEPTH, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::LUNITS, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::LUPREC, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::AUNITS, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::AUPREC, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::ATTMODE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PDMODE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::USERI1, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::USERI2, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::USERI3, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::USERI4, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::USERI5, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SPLINESEGS, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SURFU, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SURFV, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SURFTYPE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SURFTAB1, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SURFTAB2, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SPLINETYPE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SHADEDGE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SHADEDIF, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::UNITMODE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::MAXACTVP, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::ISOLINES, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::CMLJUST, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::TEXTQLTY, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::LTSCALE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::TEXTSIZE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::TRACEWID, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SKETCHINC, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::FILLETRAD, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::THICKNESS, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::ANGBASE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PDSIZE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PLINEWID, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::USERR1, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::USERR2, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::USERR3, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::USERR4, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::USERR5, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::CHAMFERA, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::CHAMFERB, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::CHAMFERC, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::CHAMFERD, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::FACETRES, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::CMLSCALE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::CELTSCALE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::MENU, ReadTV (pabyBuf, nBitOffsetFromStart));

    long juliandate, millisec;
    juliandate = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    millisec = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TDCREATE, juliandate, millisec);
    juliandate = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    millisec = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TDUPDATE, juliandate, millisec);
    juliandate = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    millisec = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TDINDWG, juliandate, millisec);
    juliandate = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    millisec = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TDUSRTIMER, juliandate, millisec);

    m_poHeader->AddValue(CADHeader::CECOLOR, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::HANDSEED, ReadHANDLE8BLENGTH (pabyBuf, nBitOffsetFromStart)); // CHECK THIS CASE.

    m_poHeader->AddValue(CADHeader::CLAYER, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::TEXTSTYLE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::CELTYPE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMSTYLE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::CMLSTYLE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::PSVPSCALE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    double dX, dY, dZ;
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PINSBASE, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PEXTMIN, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PEXTMAX, dX, dY, dZ);
    dX = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PLIMMIN, dX, dY);
    dX = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PLIMMAX, dX, dY);

    m_poHeader->AddValue(CADHeader::PELEVATION, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));

    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORG, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSXDIR, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSYDIR, dX, dY, dZ);

    m_poHeader->AddValue(CADHeader::PUCSNAME, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PUCSORTHOREF, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::PUCSORTHOVIEW, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PUCSBASE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGTOP, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGBOTTOM, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGLEFT, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGRIGHT, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGFRONT, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGBACK, dX, dY, dZ);

    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::INSBASE, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMIN, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMAX, dX, dY, dZ);
    dX = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LIMMIN, dX, dY);
    dX = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LIMMAX, dX, dY);

    m_poHeader->AddValue(CADHeader::ELEVATION, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORG, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSXDIR, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSYDIR, dX, dY, dZ);

    m_poHeader->AddValue(CADHeader::UCSNAME, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::UCSORTHOREF, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::UCSORTHOVIEW, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::UCSBASE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGTOP, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGBOTTOM, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGLEFT, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGRIGHT, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGFRONT, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGBACK, dX, dY, dZ);

    m_poHeader->AddValue(CADHeader::DIMPOST, ReadTV (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMAPOST, ReadTV (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMSCALE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMASZ, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMEXO, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMDLI, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMEXE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMRND, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMDLE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMTP, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMTM, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMTOL, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMLIM, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMTIH, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMTOH, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMSE1, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMSE2, ReadBIT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMTAD, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMZIN, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMAZIN, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMTXT, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMCEN, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMTSZ, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMALTF, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMLFAC, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMTVP, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMTFAC, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMGAP, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMALTRND, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMALT, ReadBIT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMALTD, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMTOFL, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMSAH, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMTIX, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMSOXD, ReadBIT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMCLRD, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMCLRE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMCLRT, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMADEC, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMDEC, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMTDEC, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMALTU, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMALTTD, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMAUNIT, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMFRAC, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMLUNIT, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMDSEP, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMTMOVE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMJUST, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMSD1, ReadBIT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMSD2, ReadBIT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMTOLJ, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMTZIN, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMALTZ, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMALTTZ, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMUPT, ReadBIT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMATFIT, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMTXSTY, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMLDRBLK, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMBLK, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMBLK1, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMBLK2, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::DIMLWD, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMLWE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    CADHandle stBlocksTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stLayersTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stStyleTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stLineTypesTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stViewTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stUCSTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stViewportTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stAPPIDTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMSTYLE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    CADHandle stEntityTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stACADGroupDict = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stACADMLineStyleDict = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stNamedObjectsDict = ReadHANDLE (pabyBuf, nBitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::TSTACKALIGN, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::TSTACKSIZE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::HYPERLINKBASE, ReadTV (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::STYLESHEET, ReadTV (pabyBuf, nBitOffsetFromStart));

    CADHandle stLayoutsDict = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stPlotSettingsDict = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stPlotStylesDict = ReadHANDLE (pabyBuf, nBitOffsetFromStart);

    int Flags = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CELWEIGHT, Flags & 0x001F);
    m_poHeader->AddValue(CADHeader::ENDCAPS, (bool)(Flags & 0x0060));
    m_poHeader->AddValue(CADHeader::JOINSTYLE, (bool)(Flags & 0x0180));
    m_poHeader->AddValue(CADHeader::LWDISPLAY, (bool)!(Flags & 0x0200));
    m_poHeader->AddValue(CADHeader::XEDIT, (bool)!(Flags & 0x0400));
    m_poHeader->AddValue(CADHeader::EXTNAMES, (bool)(Flags & 0x0800));
    m_poHeader->AddValue(CADHeader::PSTYLEMODE, (bool)(Flags & 0x2000));
    m_poHeader->AddValue(CADHeader::OLESTARTUP, (bool)(Flags & 0x4000));

    m_poHeader->AddValue(CADHeader::INSUNITS, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    short nCEPSNTYPE = ReadBITSHORT (pabyBuf, nBitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CEPSNTYPE, nCEPSNTYPE);

    if ( nCEPSNTYPE == 3 )
        m_poHeader->AddValue(CADHeader::CEPSNID, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::FINGERPRINTGUID, ReadTV (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::VERSIONGUID, ReadTV (pabyBuf, nBitOffsetFromStart));

    CADHandle stBlockRecordPaperSpace = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle stBlockRecordModelSpace = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    // TODO: is this part of the header?
    CADHandle LTYPE_BYLAYER = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle LTYPE_BYBLOCK = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    CADHandle LTYPE_CONTINUOUS = ReadHANDLE (pabyBuf, nBitOffsetFromStart);

    m_poHeader->AddValue(UNKNOWN11, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN12, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN13, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN14, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    short nCRC = ReadRAWSHORT (pabyBuf, nBitOffsetFromStart);
    uint16_t calculated_crc = CalculateCRC8 ((unsigned short)0xC0C1, pabyBuf, dHeaderVarsSectionLength); // TODO: CRC is calculated wrong every time.

    m_poFileIO->Read (pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH);
    if ( memcmp (pabyBuf, DWG_SENTINELS::HEADER_VARIABLES_END,
                 DWG_SENTINELS::SENTINEL_LENGTH) )
    {
        DebugMsg("File is corrupted (HEADERVARS section ending sentinel doesnt match.)");

        delete[] pabyBuf;

        return CADErrorCodes::HEADER_SECTION_READ_FAILED;
    }

    delete[] pabyBuf;

    return CADErrorCodes::SUCCESS;
}

int DWGFileR2000::ReadClassesSection ()
{
    char    *pabySectionContent;
    char    *pabyBuf     = new char[100];
    int32_t dSectionSize = 0;

    m_poFileIO->Seek (SLRecords[1].dSeeker, CADFileIO::SeekOrigin::BEG);

    m_poFileIO->Read (pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH);
    if ( memcmp (pabyBuf, DWG_SENTINELS::DS_CLASSES_START,
                 DWG_SENTINELS::SENTINEL_LENGTH) )
    {
        std::cerr << "File is corrupted (wrong pointer to CLASSES section,"
                "or CLASSES starting sentinel corrupted.)\n";

        delete[] pabyBuf;

        return CADErrorCodes::OBJECTS_SECTION_READ_FAILED;
    }

    m_poFileIO->Read (&dSectionSize, 4);
    DebugMsg ("Classes section length: %d\n", dSectionSize);

    pabySectionContent = new char[dSectionSize];
    m_poFileIO->Read (pabySectionContent, dSectionSize);

    size_t nBitOffsetFromStart = 0;

    while ( ( nBitOffsetFromStart / 8 ) + 1 < dSectionSize )
    {
        custom_classes.push_back (ReadClass (pabySectionContent, nBitOffsetFromStart));
    }

    for ( int i = 0; i < custom_classes.size (); ++i )
    {
        DebugMsg ("CLASS INFO\n"
                          "Class Number: %d\n"
                          "Version: %d\n"
                          "App name: %s\n"
                          "C++ Class Name: %s\n"
                          "DXF Class name: %s\n"
                          "Was a zombie? %x\n"
                          "Item class ID: %d\n\n",
                  custom_classes[i].dClassNum,
                  custom_classes[i].dVersion,
                  custom_classes[i].sAppName.c_str(),
                  custom_classes[i].sCppClassName.c_str(),
                  custom_classes[i].sDXFClassName.c_str(),
                  custom_classes[i].bWasAZombie,
                  custom_classes[i].dItemClassID);
    }

    m_poFileIO->Read (pabyBuf, 2); // CLASSES CRC!. TODO: add CRC computing & checking feature.

    m_poFileIO->Read (pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH);
    if ( memcmp (pabyBuf, DWG_SENTINELS::DS_CLASSES_END, DWG_SENTINELS::SENTINEL_LENGTH) )
    {
        std::cerr << "File is corrupted (CLASSES section ending sentinel doesnt match.)\n";

        delete[] pabyBuf;

        return CADErrorCodes::OBJECTS_SECTION_READ_FAILED;
    }

    delete[] pabyBuf;

    return CADErrorCodes::SUCCESS;
}

DWG2000_CLASS DWGFileR2000::ReadClass ( const char *input_array, size_t &nBitOffsetFromStart )
{
    DWG2000_CLASS outputResult;

    outputResult.dClassNum     = ReadBITSHORT (input_array, nBitOffsetFromStart);
    outputResult.dVersion      = ReadBITSHORT (input_array, nBitOffsetFromStart);
    outputResult.sAppName      = ReadTV (input_array, nBitOffsetFromStart);
    outputResult.sCppClassName = ReadTV (input_array, nBitOffsetFromStart);
    outputResult.sDXFClassName = ReadTV (input_array, nBitOffsetFromStart);
    outputResult.bWasAZombie   = ReadBIT (input_array, nBitOffsetFromStart);
    outputResult.dItemClassID  = ReadBITSHORT (input_array, nBitOffsetFromStart);

    return outputResult;
}

int DWGFileR2000::ReadObjectMap ()
{
    // Seems like ODA specification is completely awful. CRC is included in section size.
    char     *pabySectionContent;
    uint16_t dSectionCRC         = 0;
    uint16_t dSectionSize        = 0;
    uint32_t iCurrentSection     = 0;
    size_t   nBitOffsetFromStart = 0;

    m_poFileIO->Seek (SLRecords[2].dSeeker, CADFileIO::SeekOrigin::BEG);

    std::vector < int > aObjectMapSectionsSize;
    while ( true )
    {
        int niRecordsInSection = 0;
        nBitOffsetFromStart = 0;
        std::vector<ObjHandleOffset> obj_map_section;
        m_poFileIO->Read (& dSectionSize, 2);
        SwapEndianness (dSectionSize, sizeof (dSectionSize));

        DebugMsg ("Object map section #%d size: %d\n", iCurrentSection, dSectionSize);

        if ( dSectionSize == 2 ) break; // last section is empty.

        pabySectionContent = new char[dSectionSize];
        m_poFileIO->Read (pabySectionContent, dSectionSize);

        while ( ( nBitOffsetFromStart / 8 ) < ( dSectionSize - 2 ) )
        {
            ObjHandleOffset tmp;
            tmp.first  = ReadUMCHAR (pabySectionContent, nBitOffsetFromStart);
            tmp.second = ReadMCHAR (pabySectionContent, nBitOffsetFromStart);
            astObjectMap.push_back ( tmp );
            ++niRecordsInSection;
        }

        aObjectMapSectionsSize.push_back (niRecordsInSection);

        dSectionCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);
        SwapEndianness (dSectionCRC, sizeof (dSectionCRC));

        ++iCurrentSection;
        delete[] pabySectionContent;
    }

    int niCurrentSectionIndex = 0;
    int niCurrentObjectMapRecordIndex = 1;
    for ( size_t index = 1; index < astObjectMap.size (); ++index )
    {
        if ( niCurrentObjectMapRecordIndex == aObjectMapSectionsSize[niCurrentSectionIndex] )
        {
            astObjectMap[index].first = astObjectMap[index].first;
            astObjectMap[index].second = astObjectMap[index].second;

            niCurrentObjectMapRecordIndex = 1;
            ++niCurrentSectionIndex;
        }
        else
        {
            astObjectMap[index].first += astObjectMap[index - 1].first;
            astObjectMap[index].second += astObjectMap[index - 1].second;

            ++niCurrentObjectMapRecordIndex;
        }
    }

    // Filling the amapObjectMap with astObjectMap.
    for ( size_t i = 0; i < astObjectMap.size(); ++i )
    {
        amapObjectMap.insert ( astObjectMap[i] );
    }

    pabySectionContent = new char[400];
    for ( auto iterator = amapObjectMap.begin (); iterator != amapObjectMap.end(); ++iterator )
    {
        nBitOffsetFromStart = 0;
        m_poFileIO->Seek (iterator->second, CADFileIO::SeekOrigin::BEG);
        m_poFileIO->Read (pabySectionContent, 400);

        DWG2000_CED ced;
        ced.dLength = ReadMSHORT (pabySectionContent, nBitOffsetFromStart);
        ced.dType   = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);

        if ( ced.dType == DWG_OBJECT_LAYER )
        {
            Layer * layer = new Layer(this);
            CADLayer * obj_layer = ( CADLayer * ) this->GetObject (iterator->first);

            layer->sLayerName = obj_layer->sLayerName;
            layer->bFrozen = obj_layer->bFrozen;
            layer->bOn = obj_layer->bOn;
            layer->bFrozenByDefaultInNewVPORT = obj_layer->bFrozenInNewVPORT;
            layer->bLocked = obj_layer->bLocked;
            layer->dLineWeight = obj_layer->dLineWeight;
            layer->dColor = obj_layer->dCMColor;
            layer->dLayerID = astPresentedLayers.size();

            astPresentedLayers.push_back (layer);
            astPresentedCADLayers.push_back (obj_layer);
        }
    }

    // Now, fill vector of layers with objects associated with those layers.
    for ( auto iterator = amapObjectMap.begin (); iterator != amapObjectMap.end(); ++iterator )
    {
        nBitOffsetFromStart = 0;
        m_poFileIO->Seek (iterator->second, CADFileIO::SeekOrigin::BEG);
        m_poFileIO->Read (pabySectionContent, 400);

        DWG2000_CED ced;
        ced.dLength = ReadMSHORT (pabySectionContent, nBitOffsetFromStart);
        ced.dType   = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);

        try
        {
            // TODO: only objects with fixed Type code is handled. Others which are based on custom_classes are skipped.
            DWG_OBJECT_NAMES.at (ced.dType);
            DebugMsg ("Object type: %s"
                              " Handle: %d\n",
                      DWG_OBJECT_NAMES.at (ced.dType).c_str (),
                      iterator->first
            );

            if ( ced.dType != DWG_OBJECT_LAYER )
            {
                if ( std::find (DWG_ENTITIES_CODES.begin (), DWG_ENTITIES_CODES.end (), ced.dType) !=
                     DWG_ENTITIES_CODES.end () )
                {
                    CADEntity *ent = ( CADEntity * ) this->GetObject (iterator->first);

                    for ( size_t ind = 0; ind < astPresentedLayers.size (); ++ind )
                    {
                        if ( ent->ched.hLayer.GetAsLong (ent->ced.hObjectHandle) ==
                                astPresentedCADLayers[ind]->hObjectHandle.GetAsLong () )
                        {
                            DebugMsg ("Object with type: %s is attached to layer named: %s\n",
                                      DWG_OBJECT_NAMES.at (ced.dType).c_str (),
                                      astPresentedCADLayers[ind]->sLayerName.c_str ());

                            if ( std::find (DWG_GEOMETRIC_OBJECT_TYPES.begin (), DWG_GEOMETRIC_OBJECT_TYPES.end (), ced.dType)
                                 != DWG_GEOMETRIC_OBJECT_TYPES.end () )
                            {
                                astPresentedLayers[ind]->astAttachedGeometries.push_back ( std::make_pair ( iterator->first, ent->dObjectType ) );
                            }
                        }
                    }
                }
            }
        }
        catch ( std::exception e )
        {
            DebugMsg ("Object type: %s\n",
                      custom_classes[ced.dType - 500].sCppClassName.c_str ()
            );
        }
    }

    delete[] pabySectionContent;

    return CADErrorCodes::SUCCESS;
}

CADObject * DWGFileR2000::GetObject ( size_t index )
{
    CADObject * readed_object;

    char pabyObjectSize[8];
    size_t nBitOffsetFromStart = 0;
    m_poFileIO->Seek (amapObjectMap[index], CADFileIO::SeekOrigin::BEG);
    m_poFileIO->Read (pabyObjectSize, 8);
    uint32_t dObjectSize = ReadMSHORT (pabyObjectSize, nBitOffsetFromStart);

    // And read whole data chunk into memory for future parsing.
    // + nBitOffsetFromStart/8 + 2 is because dObjectSize doesnot cover CRC and itself.
    char * pabySectionContent = new char[dObjectSize + nBitOffsetFromStart/8 + 2];
    m_poFileIO->Seek (amapObjectMap[index], CADFileIO::SeekOrigin::BEG);
    m_poFileIO->Read (pabySectionContent, dObjectSize + nBitOffsetFromStart/8 + 2);

    nBitOffsetFromStart = 0;
    dObjectSize = ReadMSHORT (pabySectionContent, nBitOffsetFromStart);
    int16_t dObjectType = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);

    // Entities handling
    if ( std::find ( DWG_ENTITIES_CODES.begin(), DWG_ENTITIES_CODES.end(), dObjectType ) != DWG_ENTITIES_CODES.end() )
    {
        struct CADCommonED common_entity_data; // common for all entities ofc.

        common_entity_data.nObjectSizeInBits = ReadRAWLONG (pabySectionContent, nBitOffsetFromStart);
        common_entity_data.hObjectHandle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

        int16_t dEEDSize = 0;
        while ( (dEEDSize = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart)) != 0 )
        {
            CAD_EED dwg_eed;
            dwg_eed.length = dEEDSize;
            dwg_eed.application_handle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

            for ( size_t i = 0; i < dEEDSize; ++i )
            {
                dwg_eed.data.push_back(ReadCHAR (pabySectionContent, nBitOffsetFromStart));
            }

            common_entity_data.aEED.push_back (dwg_eed);
        }

        common_entity_data.bGraphicsPresented = ReadBIT (pabySectionContent, nBitOffsetFromStart);
        common_entity_data.bbEntMode = Read2B (pabySectionContent, nBitOffsetFromStart);
        common_entity_data.nNumReactors = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);
        common_entity_data.bNoLinks = ReadBIT (pabySectionContent, nBitOffsetFromStart);
        common_entity_data.nCMColor = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
        common_entity_data.dfLTypeScale = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
        common_entity_data.bbLTypeFlags = Read2B (pabySectionContent, nBitOffsetFromStart);
        common_entity_data.bbPlotStyleFlags = Read2B (pabySectionContent, nBitOffsetFromStart);
        common_entity_data.nInvisibility = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
        common_entity_data.nLineWeight = ReadCHAR (pabySectionContent, nBitOffsetFromStart);

        switch ( dObjectType )
        {
            case DWG_OBJECT_BLOCK:
            {
                CADBlock * block = new CADBlock();

                block->dObjectSize = dObjectSize;
                block->ced = common_entity_data;

                block->sBlockName = ReadTV (pabySectionContent, nBitOffsetFromStart);

                if (block->ced.bbEntMode == 0 )
                    block->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < block->ced.nNumReactors; ++i )
                    block->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                block->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !block->ced.bNoLinks )
                {
                    block->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    block->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                block->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( block->ced.bbLTypeFlags == 0x03 )
                    block->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( block->ced.bbPlotStyleFlags == 0x03 )
                    block->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
                block->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = block;
                break;
            }

            case DWG_OBJECT_ELLIPSE:
            {
                CADEllipse * ellipse = new CADEllipse();

                ellipse->dObjectSize = dObjectSize;
                ellipse->ced = common_entity_data;

                ellipse->vertPosition.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                ellipse->vertPosition.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                ellipse->vertPosition.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                ellipse->vectSMAxis.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                ellipse->vectSMAxis.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                ellipse->vectSMAxis.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                ellipse->vectExtrusion.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                ellipse->vectExtrusion.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                ellipse->vectExtrusion.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                ellipse->dfAxisRatio = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                ellipse->dfBegAngle = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                ellipse->dfEndAngle = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( ellipse->ced.bbEntMode == 0 )
                    ellipse->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < ellipse->ced.nNumReactors; ++i )
                    ellipse->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                ellipse->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !ellipse->ced.bNoLinks )
                {
                    ellipse->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    ellipse->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                ellipse->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( ellipse->ced.bbLTypeFlags == 0x03 )
                    ellipse->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( ellipse->ced.bbPlotStyleFlags == 0x03 )
                    ellipse->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
                ellipse->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = ellipse;
                break;
            }

            case DWG_OBJECT_POINT:
            {
                CADPoint * point = new CADPoint();

                point->dObjectSize = dObjectSize;
                point->ced = common_entity_data;

                point->vertPosition.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                point->vertPosition.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                point->vertPosition.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                point->dfThickness = ReadBIT (pabySectionContent, nBitOffsetFromStart) ?
                                      0.0f : ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( ReadBIT (pabySectionContent, nBitOffsetFromStart) )
                {
                    point->vectExtrusion.X = 0.0f;
                    point->vectExtrusion.Y = 0.0f;
                    point->vectExtrusion.Z = 1.0f;
                }
                else
                {
                    point->vectExtrusion.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    point->vectExtrusion.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    point->vectExtrusion.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                }

                point->dfXAxisAng = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( point->ced.bbEntMode == 0 )
                    point->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < point->ced.nNumReactors; ++i )
                    point->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                point->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !point->ced.bNoLinks )
                {
                    point->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    point->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                point->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( point->ced.bbLTypeFlags == 0x03 )
                    point->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( point->ced.bbPlotStyleFlags == 0x03 )
                    point->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
                point->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = point;
                break;
            }

            case DWG_OBJECT_POLYLINE3D:
            {
                CADPolyline3D * polyline = new CADPolyline3D();

                polyline->dObjectSize = dObjectSize;
                polyline->ced = common_entity_data;

                polyline->SplinedFlags = ReadCHAR (pabySectionContent, nBitOffsetFromStart);
                polyline->ClosedFlags = ReadCHAR (pabySectionContent, nBitOffsetFromStart);

                if ( polyline->ced.bbEntMode == 0 )
                    polyline->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < polyline->ced.nNumReactors; ++i )
                    polyline->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                polyline->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !polyline->ced.bNoLinks )
                {
                    polyline->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    polyline->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                polyline->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( polyline->ced.bbLTypeFlags == 0x03 )
                    polyline->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( polyline->ced.bbPlotStyleFlags == 0x03 )
                    polyline->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                polyline->hVertexes.push_back ( ReadHANDLE (pabySectionContent, nBitOffsetFromStart) ); // 1st vertex
                polyline->hVertexes.push_back ( ReadHANDLE (pabySectionContent, nBitOffsetFromStart) ); // last vertex

                polyline->hSeqend = ReadHANDLE (pabyObjectSize, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
                polyline->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = polyline;
                break;
            }

            case DWG_OBJECT_RAY:
            {
                CADRay * ray = new CADRay();

                ray->dObjectSize = dObjectSize;
                ray->ced = common_entity_data;

                ray->vertPosition.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                ray->vertPosition.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                ray->vertPosition.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                ray->vectVector.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                ray->vectVector.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                ray->vectVector.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( ray->ced.bbEntMode == 0 )
                    ray->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < ray->ced.nNumReactors; ++i )
                    ray->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                ray->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !ray->ced.bNoLinks )
                {
                    ray->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    ray->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                ray->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( ray->ced.bbLTypeFlags == 0x03 )
                    ray->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( ray->ced.bbPlotStyleFlags == 0x03 )
                    ray->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
                ray->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = ray;
                break;
            }

            case DWG_OBJECT_XLINE:
            {
                CADXLine * xline = new CADXLine();

                xline->dObjectSize = dObjectSize;
                xline->ced = common_entity_data;

                xline->vertPosition.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                xline->vertPosition.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                xline->vertPosition.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                xline->vectVector.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                xline->vectVector.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                xline->vectVector.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( xline->ced.bbEntMode == 0 )
                    xline->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < xline->ced.nNumReactors; ++i )
                    xline->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                xline->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !xline->ced.bNoLinks )
                {
                    xline->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    xline->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                xline->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( xline->ced.bbLTypeFlags == 0x03 )
                    xline->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( xline->ced.bbPlotStyleFlags == 0x03 )
                    xline->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
                xline->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = xline;
                break;
            }

            case DWG_OBJECT_LINE:
            {
                CADLine * line = new CADLine();

                line->dObjectSize = dObjectSize;
                line->ced = common_entity_data;

                bool bZsAreZeros = ReadBIT (pabySectionContent, nBitOffsetFromStart);
                line->vertStart.X = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                line->vertEnd.X   = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart, line->vertStart.X);
                line->vertStart.Y = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                line->vertEnd.Y   = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart, line->vertStart.Y);

                if ( !bZsAreZeros )
                {
                    line->vertStart.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    line->vertEnd.Z   = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart, line->vertStart.Z);
                }

                line->dfThickness = ReadBIT (pabySectionContent, nBitOffsetFromStart) ?
                                    0.0f : ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( ReadBIT (pabySectionContent, nBitOffsetFromStart) )
                {
                    line->vectExtrusion.X = 0.0f;
                    line->vectExtrusion.Y = 0.0f;
                    line->vectExtrusion.Z = 1.0f;
                }
                else
                {
                    line->vectExtrusion.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    line->vectExtrusion.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    line->vectExtrusion.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                }

                if ( line->ced.bbEntMode == 0 )
                    line->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < line->ced.nNumReactors; ++i )
                    line->ched.hReactors.push_back ( ReadHANDLE (pabySectionContent, nBitOffsetFromStart) );

                line->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !line->ced.bNoLinks )
                {
                    line->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    line->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                line->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( line->ced.bbLTypeFlags == 0x03 )
                {
                    line->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                if ( line->ced.bbPlotStyleFlags == 0x03 )
                {
                    line->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
                line->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = line;
                break;
            }

            case DWG_OBJECT_TEXT:
            {
                CADText * text = new CADText();

                text->dObjectSize = dObjectSize;
                text->ced = common_entity_data;

                text->DataFlags = ReadCHAR (pabySectionContent, nBitOffsetFromStart);

                if ( !( text->DataFlags & 0x01 ) )
                    text->dfElevation = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);

                text->vertInsetionPoint.X = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                text->vertInsetionPoint.Y = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( !( text->DataFlags & 0x02 ) )
                {
                    text->vertAlignmentPoint.X = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart,
                                                                  text->vertInsetionPoint.X);
                    text->vertAlignmentPoint.Y = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart,
                                                                  text->vertInsetionPoint.Y);
                }

                if ( ReadBIT (pabySectionContent, nBitOffsetFromStart) )
                {
                    text->vectExtrusion.X = 0.0f;
                    text->vectExtrusion.Y = 0.0f;
                    text->vectExtrusion.Z = 1.0f;
                }
                else
                {
                    text->vectExtrusion.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    text->vectExtrusion.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    text->vectExtrusion.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                }

                text->dfThickness = ReadBIT (pabySectionContent, nBitOffsetFromStart) ?
                                      0.0f : ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( !( text->DataFlags & 0x04 ) )
                    text->dfObliqueAng = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                if ( !( text->DataFlags & 0x08 ) )
                    text->dfRotationAng = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);

                text->dfHeight = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( !( text->DataFlags & 0x10 ) )
                    text->dfWidthFactor = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);

                text->sTextValue = ReadTV (pabySectionContent, nBitOffsetFromStart);

                if ( !( text->DataFlags & 0x20 ) )
                    text->dGeneration = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                if ( !( text->DataFlags & 0x40 ) )
                    text->dHorizAlign = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                if ( !( text->DataFlags & 0x80 ) )
                    text->dVertAlign = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( text->ced.bbEntMode == 0 )
                    text->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < text->ced.nNumReactors; ++i )
                    text->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                text->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !text->ced.bNoLinks )
                {
                    text->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    text->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                text->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( text->ced.bbLTypeFlags == 0x03 )
                    text->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( text->ced.bbPlotStyleFlags == 0x03 )
                    text->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                text->hStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
                text->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = text;
                break;
            }

            case DWG_OBJECT_VERTEX3D:
            {
                CADVertex3D * vertex = new CADVertex3D();

                vertex->dObjectSize = dObjectSize;
                vertex->ced = common_entity_data;

                char Flags = ReadCHAR (pabySectionContent, nBitOffsetFromStart);
                vertex->vertPosition.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                vertex->vertPosition.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                vertex->vertPosition.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( vertex->ced.bbEntMode == 0 )
                    vertex->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < vertex->ced.nNumReactors; ++i )
                    vertex->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                vertex->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !vertex->ced.bNoLinks )
                {
                    vertex->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    vertex->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                vertex->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( vertex->ced.bbLTypeFlags == 0x03 )
                    vertex->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( vertex->ced.bbPlotStyleFlags == 0x03 )
                    vertex->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
                vertex->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = vertex;
                break;
            }

            case DWG_OBJECT_CIRCLE:
            {
                CADCircle * circle = new CADCircle();

                circle->dObjectSize = dObjectSize;
                circle->ced = common_entity_data;

                circle->vertPosition.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                circle->vertPosition.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                circle->vertPosition.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                circle->dfRadius    = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                circle->dfThickness = ReadBIT (pabySectionContent, nBitOffsetFromStart) ?
                                      0.0f : ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( ReadBIT (pabySectionContent, nBitOffsetFromStart) )
                {
                    circle->vectExtrusion.X = 0.0f;
                    circle->vectExtrusion.Y = 0.0f;
                    circle->vectExtrusion.Z = 1.0f;
                }
                else
                {
                    circle->vectExtrusion.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    circle->vectExtrusion.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    circle->vectExtrusion.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                }

                if ( circle->ced.bbEntMode == 0 )
                    circle->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < circle->ced.nNumReactors; ++i )
                    circle->ched.hReactors.push_back ( ReadHANDLE (pabySectionContent, nBitOffsetFromStart) );

                circle->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !circle->ced.bNoLinks )
                {
                    circle->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    circle->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                circle->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( circle->ced.bbLTypeFlags == 0x03 )
                {
                    circle->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                if ( circle->ced.bbPlotStyleFlags == 0x03 )
                {
                    circle->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
                circle->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = circle;
                break;
            }

            case DWG_OBJECT_ENDBLK:
            {
                CADEndblk * endblk = new CADEndblk();

                endblk->dObjectSize = dObjectSize;
                endblk->ced = common_entity_data;

                if ( endblk->ced.bbEntMode == 0 )
                    endblk->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < endblk->ced.nNumReactors; ++i )
                    endblk->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                endblk->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !endblk->ced.bNoLinks )
                {
                    endblk->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    endblk->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                endblk->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( endblk->ced.bbLTypeFlags == 0x03 )
                    endblk->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( endblk->ced.bbPlotStyleFlags == 0x03 )
                    endblk->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
                endblk->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = endblk;
                break;
            }

            case DWG_OBJECT_POLYLINE2D:
            {
                CADPolyline2D * polyline = new CADPolyline2D();

                polyline->dObjectSize = dObjectSize;
                polyline->ced = common_entity_data;

                polyline->dFlags = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                polyline->dCurveNSmoothSurfType = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);

                polyline->dfStartWidth = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                polyline->dfEndWidth = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                polyline->dfThickness = ReadBIT (pabySectionContent, nBitOffsetFromStart) ?
                                        0.0f : ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                polyline->dfElevation = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( ReadBIT (pabySectionContent, nBitOffsetFromStart) )
                {
                    polyline->vectExtrusion.X = 0.0f;
                    polyline->vectExtrusion.Y = 0.0f;
                    polyline->vectExtrusion.Z = 1.0f;
                }
                else
                {
                    polyline->vectExtrusion.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    polyline->vectExtrusion.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    polyline->vectExtrusion.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                }

                if ( polyline->ced.bbEntMode == 0 )
                    polyline->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < polyline->ced.nNumReactors; ++i )
                    polyline->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                polyline->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !polyline->ced.bNoLinks )
                {
                    polyline->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    polyline->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                polyline->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( polyline->ced.bbLTypeFlags == 0x03 )
                    polyline->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( polyline->ced.bbPlotStyleFlags == 0x03 )
                    polyline->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                polyline->hVertexes.push_back ( ReadHANDLE (pabySectionContent, nBitOffsetFromStart) ); // 1st vertex
                polyline->hVertexes.push_back ( ReadHANDLE (pabySectionContent, nBitOffsetFromStart) ); // last vertex

                polyline->hSeqend = ReadHANDLE (pabyObjectSize, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
                polyline->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = polyline;
                break;
            }

            case DWG_OBJECT_ATTRIB:
            {
                CADAttrib * attrib = new CADAttrib();

                attrib->ced = common_entity_data;
                attrib->DataFlags = ReadCHAR (pabySectionContent, nBitOffsetFromStart);

                if ( !(attrib->DataFlags & 0x01) )
                    attrib->dfElevation = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);

                attrib->vertInsetionPoint.X = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                attrib->vertInsetionPoint.Y = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( !(attrib->DataFlags & 0x02) )
                {
                    attrib->vertAlignmentPoint.X = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart, attrib->vertInsetionPoint.X);
                    attrib->vertAlignmentPoint.Y = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart, attrib->vertInsetionPoint.Y);
                }

                if ( ReadBIT (pabySectionContent, nBitOffsetFromStart) )
                {
                    attrib->vectExtrusion.X = 0.0f;
                    attrib->vectExtrusion.Y = 0.0f;
                    attrib->vectExtrusion.Z = 1.0f;
                }
                else
                {
                    attrib->vectExtrusion.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    attrib->vectExtrusion.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    attrib->vectExtrusion.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                }

                attrib->dfThickness = ReadBIT (pabySectionContent, nBitOffsetFromStart) ?
                                    0.0f : ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( !(attrib->DataFlags & 0x04) )
                    attrib->dfObliqueAng = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                if ( !(attrib->DataFlags & 0x08) )
                    attrib->dfRotationAng = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                attrib->dfHeight = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                if ( !(attrib->DataFlags & 0x10) )
                    attrib->dfWidthFactor = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                attrib->sTextValue = ReadTV (pabySectionContent, nBitOffsetFromStart);
                if ( !(attrib->DataFlags & 0x20) )
                    attrib->dGeneration = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                if ( !(attrib->DataFlags & 0x40) )
                    attrib->dHorizAlign = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                if ( !(attrib->DataFlags & 0x80) )
                    attrib->dVertAlign = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);

                attrib->sTag = ReadTV (pabySectionContent, nBitOffsetFromStart);
                attrib->nFieldLength = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                attrib->nFlags = ReadCHAR (pabySectionContent, nBitOffsetFromStart);

                if (attrib->ced.bbEntMode == 0 )
                    attrib->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < attrib->ced.nNumReactors; ++i )
                    attrib->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                attrib->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !attrib->ced.bNoLinks )
                {
                    attrib->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    attrib->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                attrib->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( attrib->ced.bbLTypeFlags == 0x03 )
                    attrib->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                if ( attrib->ced.bbPlotStyleFlags == 0x03 )
                    attrib->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                attrib->hStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
                attrib->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = attrib;
                break;
            }

            case DWG_OBJECT_ATTDEF:
            {
                CADAttdef * attdef = new CADAttdef();

                attdef->ced = common_entity_data;
                attdef->DataFlags = ReadCHAR (pabySectionContent, nBitOffsetFromStart);

                if ( !(attdef->DataFlags & 0x01) )
                    attdef->dfElevation = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);

                attdef->vertInsetionPoint.X = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                attdef->vertInsetionPoint.Y = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( !(attdef->DataFlags & 0x02) )
                {
                    attdef->vertAlignmentPoint.X = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart, attdef->vertInsetionPoint.X);
                    attdef->vertAlignmentPoint.Y = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart, attdef->vertInsetionPoint.Y);
                }

                if ( ReadBIT (pabySectionContent, nBitOffsetFromStart) )
                {
                    attdef->vectExtrusion.X = 0.0f;
                    attdef->vectExtrusion.Y = 0.0f;
                    attdef->vectExtrusion.Z = 1.0f;
                }
                else
                {
                    attdef->vectExtrusion.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    attdef->vectExtrusion.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    attdef->vectExtrusion.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                }

                attdef->dfThickness = ReadBIT (pabySectionContent, nBitOffsetFromStart) ?
                                      0.0f : ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( !(attdef->DataFlags & 0x04) )
                    attdef->dfObliqueAng = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                if ( !(attdef->DataFlags & 0x08) )
                    attdef->dfRotationAng = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                attdef->dfHeight = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                if ( !(attdef->DataFlags & 0x10) )
                    attdef->dfWidthFactor = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                attdef->sDefaultValue = ReadTV (pabySectionContent, nBitOffsetFromStart);
                if ( !(attdef->DataFlags & 0x20) )
                    attdef->dGeneration = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                if ( !(attdef->DataFlags & 0x40) )
                    attdef->dHorizAlign = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                if ( !(attdef->DataFlags & 0x80) )
                    attdef->dVertAlign = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);

                attdef->sTag = ReadTV (pabySectionContent, nBitOffsetFromStart);
                attdef->nFieldLength = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                attdef->nFlags = ReadCHAR (pabySectionContent, nBitOffsetFromStart);

                attdef->sPrompt = ReadTV (pabySectionContent, nBitOffsetFromStart);

                if (attdef->ced.bbEntMode == 0 )
                    attdef->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < attdef->ced.nNumReactors; ++i )
                    attdef->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                attdef->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !attdef->ced.bNoLinks )
                {
                    attdef->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    attdef->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                attdef->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( attdef->ced.bbLTypeFlags == 0x03 )
                    attdef->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                if ( attdef->ced.bbPlotStyleFlags == 0x03 )
                    attdef->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                attdef->hStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
                attdef->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = attdef;
                break;
            }

            case DWG_OBJECT_LWPOLYLINE:
            {
                CADLWPolyline * polyline   = new CADLWPolyline ();

                polyline->dObjectSize = dObjectSize;
                polyline->ced = common_entity_data;

                int32_t    nVertixesCount  = 0, nBulges = 0;
                int16_t    data_flag       = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                if ( data_flag & 4 )
                    polyline->dfConstWidth = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                if ( data_flag & 8 )
                    polyline->dfElevation  = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                if ( data_flag & 2 )
                    polyline->dfThickness  = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                if ( data_flag & 1 )
                {
                    polyline->vectExtrusion.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    polyline->vectExtrusion.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    polyline->vectExtrusion.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                }

                nVertixesCount = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);

                if ( data_flag & 16 )
                {
                    nBulges = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);
                }

                // First of all, read first vertex.
                Vertex2D vertex;
                vertex.X = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                vertex.Y = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
                polyline->vertexes.push_back (vertex);

                // All the others are not raw doubles; bitdoubles with default instead,
                // where default is previous point coords.
                for ( size_t i = 1; i < nVertixesCount; ++i )
                {
                    vertex.X = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart,
                                                polyline->vertexes[i - 1].X);
                    vertex.Y = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart,
                                                polyline->vertexes[i - 1].Y);
                    polyline->vertexes.push_back (vertex);
                }

                for ( size_t i = 0; i < nBulges; ++i )
                {
                    double dfBulgeValue = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    polyline->bulges.push_back (dfBulgeValue);
                }

                if (polyline->ced.bbEntMode == 0 )
                    polyline->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < polyline->ced.nNumReactors; ++i )
                    polyline->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                polyline->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !polyline->ced.bNoLinks )
                {
                    polyline->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    polyline->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                polyline->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( polyline->ced.bbLTypeFlags == 0x03 )
                {
                    polyline->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                if ( polyline->ced.bbPlotStyleFlags == 0x03 )
                {
                    polyline->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
                int16_t crc = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = polyline;
                break;
            }

            case DWG_OBJECT_ARC:
            {
                CADArc * arc = new CADArc();

                arc->ced = common_entity_data;

                arc->vertPosition.X  = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                arc->vertPosition.Y  = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                arc->vertPosition.Z  = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                arc->dfRadius        = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                arc->dfThickness     = ReadBIT (pabySectionContent, nBitOffsetFromStart) ?
                                       0.0f : ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if ( ReadBIT (pabySectionContent, nBitOffsetFromStart) )
                {
                    arc->vectExtrusion.X = 0.0f;
                    arc->vectExtrusion.Y = 0.0f;
                    arc->vectExtrusion.Z = 1.0f;
                }
                else
                {
                    arc->vectExtrusion.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    arc->vectExtrusion.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    arc->vectExtrusion.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                }

                arc->dfStartAngle = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                arc->dfEndAngle   = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);

                if (arc->ced.bbEntMode == 0 )
                    arc->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < arc->ced.nNumReactors; ++i )
                    arc->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                arc->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !arc->ced.bNoLinks )
                {
                    arc->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    arc->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                arc->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( arc->ced.bbLTypeFlags == 0x03 )
                {
                    arc->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                if ( arc->ced.bbPlotStyleFlags == 0x03 )
                {
                    arc->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
                arc->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = arc;
                break;
            }

            case DWG_OBJECT_SPLINE:
            {
                CADSpline * spline = new CADSpline();

                spline->dObjectSize = dObjectSize;
                spline->ced = common_entity_data;

                spline->dScenario = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);
                spline->dDegree  = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);

                if ( spline->dScenario == 2 )
                {
                    spline->dfFitTol = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    spline->vectBegTangDir.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    spline->vectBegTangDir.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    spline->vectBegTangDir.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    spline->vectEndTangDir.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    spline->vectEndTangDir.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    spline->vectEndTangDir.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    spline->nNumFitPts = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);
                }
                else if ( spline->dScenario == 1 )
                {
                    spline->bRational = ReadBIT (pabySectionContent, nBitOffsetFromStart);
                    spline->bClosed = ReadBIT (pabySectionContent, nBitOffsetFromStart);
                    spline->bPeriodic = ReadBIT (pabySectionContent, nBitOffsetFromStart);
                    spline->dfKnotTol = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    spline->dfCtrlTol = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    spline->nNumKnots = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);
                    spline->nNumCtrlPts = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);
                    spline->bWeight = ReadBIT (pabySectionContent, nBitOffsetFromStart);
                }
#ifdef _DEBUG
                else
                {
                    DebugMsg ("Spline scenario != {1,2} readed: error.");
                }
#endif
                for ( size_t i = 0; i < spline->nNumKnots; ++i )
                    spline->adfKnots.push_back ( ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart) );
                for ( size_t i = 0; i < spline->nNumCtrlPts; ++i )
                {
                    Vertex3D vertex;
                    vertex.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    vertex.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    vertex.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    spline->avertCtrlPoints.push_back ( vertex );
                    if ( spline->bWeight )
                        spline->adfCtrlPointsWeight.push_back ( ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart) );
                }
                for ( size_t i = 0; i < spline->nNumFitPts; ++i )
                {
                    Vertex3D vertex;
                    vertex.X = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    vertex.Y = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    vertex.Z = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
                    spline->averFitPoints.push_back ( vertex );
                }

                if (spline->ced.bbEntMode == 0 )
                    spline->ched.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < spline->ced.nNumReactors; ++i )
                    spline->ched.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

                spline->ched.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( !spline->ced.bNoLinks )
                {
                    spline->ched.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                    spline->ched.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                spline->ched.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                if ( spline->ced.bbLTypeFlags == 0x03 )
                {
                    spline->ched.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                if ( spline->ced.bbPlotStyleFlags == 0x03 )
                {
                    spline->ched.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                }

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
                spline->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = spline;
                break;
            }
        }
    }

    else
    {
        switch ( dObjectType )
        {
            case DWG_OBJECT_DICTIONARY:
            {
                /*
                 * FIXME: ODA has a lot of mistypes in spec. for this objects,
                 * it doesnt work for now (error begins in handles stream).
                 * Nonetheless, dictionary->sItemNames is 100% array,
                 * not a single obj as pointer by their docs.
                 */
                CADDictionary * dictionary = new CADDictionary();

                dictionary->dObjectSize = dObjectSize;
                dictionary->nObjectSizeInBits = ReadRAWLONG (pabySectionContent, nBitOffsetFromStart);
                dictionary->hObjectHandle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                int16_t dEEDSize = 0;
                while ( (dEEDSize = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart)) != 0 )
                {
                    CAD_EED dwg_eed;
                    dwg_eed.length = dEEDSize;
                    dwg_eed.application_handle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                    for ( size_t i = 0; i < dEEDSize; ++i )
                    {
                        dwg_eed.data.push_back(ReadCHAR (pabySectionContent, nBitOffsetFromStart));
                    }

                    dictionary->aEED.push_back (dwg_eed);
                }

                dictionary->nNumReactors = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                dictionary->nNumItems = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);
                dictionary->dCloningFlag = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                dictionary->dHardOwnerFlag = ReadCHAR (pabySectionContent, nBitOffsetFromStart);

                dictionary->sDictionaryEntryName = ReadTV (pabySectionContent, nBitOffsetFromStart);
                for ( size_t i = 0; i < dictionary->nNumItems; ++i )
                    dictionary->sItemNames.push_back ( ReadTV (pabySectionContent, nBitOffsetFromStart) );

                dictionary->hParentHandle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                for ( size_t i = 0; i < dictionary->nNumReactors; ++i )
                    dictionary->hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart) );
                dictionary->hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                for ( size_t i = 0; i < dictionary->nNumItems; ++i )
                    dictionary->hItemHandles.push_back ( ReadHANDLE (pabySectionContent, nBitOffsetFromStart) );

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
                dictionary->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = dictionary;
                break;
            }

            case DWG_OBJECT_LAYER:
            {
                CADLayer * layer = new CADLayer();

                layer->dObjectSize = dObjectSize;
                layer->nObjectSizeInBits = ReadRAWLONG (pabySectionContent, nBitOffsetFromStart);
                layer->hObjectHandle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                int16_t dEEDSize = 0;
                while ( (dEEDSize = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart)) != 0 )
                {
                    CAD_EED dwg_eed;
                    dwg_eed.length = dEEDSize;
                    dwg_eed.application_handle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                    for ( size_t i = 0; i < dEEDSize; ++i )
                    {
                        dwg_eed.data.push_back(ReadCHAR (pabySectionContent, nBitOffsetFromStart));
                    }

                    layer->aEED.push_back (dwg_eed);
                }

                layer->nNumReactors = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);
                layer->sLayerName = ReadTV (pabySectionContent, nBitOffsetFromStart);
                layer->b64Flag = ReadBIT (pabySectionContent, nBitOffsetFromStart);
                layer->dXRefIndex = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                layer->bXDep = ReadBIT (pabySectionContent, nBitOffsetFromStart);

                int16_t dFlags = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                layer->bFrozen = dFlags & 0x01;
                layer->bOn     = dFlags & 0x02;
                layer->bFrozenInNewVPORT = dFlags & 0x04;
                layer->bLocked = dFlags & 0x08;
                layer->bPlottingFlag = dFlags & 0x10;
                layer->dLineWeight = dFlags & 0x03E0;
                layer->dCMColor = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);
                layer->hLayerControl = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                for ( size_t i = 0; i < layer->nNumReactors; ++i )
                    layer->hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart) );
                layer->hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                layer->hExternalRefBlockHandle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                layer->hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
                layer->hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                /*
                 * FIXME: ODA says that this handle should be null hard pointer. It is not.
                 * Also, after reading it dObjectSize is != actual readed structure's size.
                 * Not used anyway, so no point to read it for now.
                 * It also means that CRC cannot be computed correctly.
                 */
//                layer->hUnknownHandle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

                nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
                layer->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

                if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
                    DebugMsg ("Assertion failed at %d in %s\n", __LINE__, __FILE__);

                readed_object = layer;
                break;
            }
        }
    }

    return readed_object;
}

// FIXME: 'delete' sometimes causes crash, need investigation.
CADGeometry * DWGFileR2000::GetGeometry ( size_t layer_index, size_t index )
{
    CADGeometry * result_geometry = nullptr;
    CADObject * readed_object = this->GetObject ( astPresentedLayers[layer_index]->astAttachedGeometries[index].first );

    switch ( readed_object->dObjectType )
    {
        case DWG_OBJECT_ARC:
        {
            Arc * arc = new Arc();
            CADArc * cadArc = ( CADArc * ) readed_object;

            arc->vertPosition = cadArc->vertPosition;
            arc->vectExtrusion = cadArc->vectExtrusion;
            arc->dfRadius = cadArc->dfRadius;
            arc->dfThickness = cadArc->dfThickness;
            arc->dfStartingAngle = cadArc->dfStartAngle;
            arc->dfEndingAngle = cadArc->dfEndAngle;

//            delete( cadArc );

            result_geometry = arc;
            break;
        }

        case DWG_OBJECT_POINT:
        {
            Point3D * point = new Point3D();
            CADPoint * cadPoint = ( CADPoint * ) readed_object;

            point->vertPosition = cadPoint->vertPosition;
            point->vectExtrusion = cadPoint->vectExtrusion;
            point->dfXAxisAng = cadPoint->dfXAxisAng;
            point->dfThickness = cadPoint->dfThickness;

//            delete( cadPoint );

            result_geometry = point;
            break;
        }

        case DWG_OBJECT_POLYLINE3D:
        {
            Polyline3D * polyline = new Polyline3D();
            CADPolyline3D * cadPolyline3D = ( CADPolyline3D * ) readed_object;

            // TODO: code can be much simplified if CADHandle will be used.
            // to do so, == and ++ operators should be implemented.
            CADVertex3D * vertex;
            long long currentVertexH = cadPolyline3D->hVertexes[0].GetAsLong ();
            while ( currentVertexH != 0 )
            {
                vertex = ( CADVertex3D * ) this->GetObject (currentVertexH);
                currentVertexH = vertex->ced.hObjectHandle.GetAsLong ();
                polyline->vertexes.push_back ( vertex->vertPosition );
                if ( vertex->ced.bNoLinks == true )
                {
                    ++currentVertexH;
                }
                else
                {
                    currentVertexH = vertex->ched.hNextEntity.GetAsLong (vertex->ced.hObjectHandle );
                }

                // Last vertex is reached. read it and break reading.
                if ( currentVertexH == cadPolyline3D->hVertexes[1].GetAsLong () )
                {
                    vertex = ( CADVertex3D * ) this->GetObject (currentVertexH);
                    polyline->vertexes.push_back ( vertex->vertPosition );
                    break;
                }
            }

//            delete( cadPolyline3D );

            result_geometry = polyline;
            break;
        }

        case DWG_OBJECT_LWPOLYLINE:
        {
            LWPolyline * lwPolyline = new LWPolyline();
            CADLWPolyline * cadlwPolyline = ( CADLWPolyline * ) readed_object;

            lwPolyline->dfConstWidth = cadlwPolyline->dfConstWidth;
            lwPolyline->dfElevation = cadlwPolyline->dfElevation;
            lwPolyline->vertexes = cadlwPolyline->vertexes;
            lwPolyline->vectExtrusion = cadlwPolyline->vectExtrusion;
            lwPolyline->bulges = cadlwPolyline->bulges;
            lwPolyline->widths = cadlwPolyline->widths;

//            delete( cadlwPolyline );

            result_geometry = lwPolyline;
            break;
        }

        case DWG_OBJECT_CIRCLE:
        {
            Circle * circle = new Circle();
            CADCircle * cadCircle = ( CADCircle * ) readed_object;

            circle->vertPosition = cadCircle->vertPosition;
            circle->vectExtrusion = cadCircle->vectExtrusion;
            circle->dfRadius = cadCircle->dfRadius;
            circle->dfThickness = cadCircle->dfThickness;

//            delete( cadCircle );

            result_geometry = circle;
            break;
        }

        case DWG_OBJECT_ELLIPSE:
        {
            Ellipse * ellipse = new Ellipse();
            CADEllipse * cadEllipse = ( CADEllipse * ) readed_object;

            ellipse->vertPosition = cadEllipse->vertPosition;
            ellipse->vectExtrusion = cadEllipse->vectExtrusion;
            ellipse->vectWCS = cadEllipse->vectSMAxis; // FIXME: WCS for both or SMAxis?
            ellipse->dfAxisRatio = cadEllipse->dfAxisRatio;
            ellipse->dfEndingAngle = cadEllipse->dfEndAngle;
            ellipse->dfStartingAngle = cadEllipse->dfBegAngle;

//            delete( cadEllipse );

            result_geometry = ellipse;
            break;
        }

        case DWG_OBJECT_LINE:
        {
            Line * line = new Line();
            CADLine * cadLine = ( CADLine * ) readed_object;

            line->vertStart = cadLine->vertStart;
            line->vertEnd = cadLine->vertEnd;
            line->dfThickness = cadLine->dfThickness;

//            delete( cadLine );

            result_geometry = line;
            break;
        }

        case DWG_OBJECT_SPLINE:
        {
            Spline * spline = new Spline();
            CADSpline * cadSpline = ( CADSpline * ) readed_object;

            spline->dScenario = cadSpline->dScenario;
            spline->dDegree = cadSpline->dDegree;
            if ( spline->dScenario == 2 )
            {
                spline->dfFitTol = cadSpline->dfFitTol;
                spline->vectBegTangDir = cadSpline->vectBegTangDir;
                spline->vectEndTangDir = cadSpline->vectEndTangDir;
            }
            else if ( spline->dScenario == 1 )
            {
                spline->bRational = cadSpline->bRational;
                spline->bPeriodic = cadSpline->bPeriodic;
                spline->bClosed = cadSpline->bClosed;
                spline->dfKnotTol = cadSpline->dfKnotTol;
                spline->dfCtrlTol = cadSpline->dfCtrlTol;
                spline->bWeight = cadSpline->bWeight;
            }
            spline->averFitPoints = cadSpline->averFitPoints;
            spline->avertCtrlPoints = cadSpline->avertCtrlPoints;

//            delete( cadSpline );

            result_geometry = spline;
            break;
        }

        case DWG_OBJECT_TEXT:
        {
            Text * text = new Text();
            CADText * cadText = ( CADText * ) readed_object;

            text->vertAlignment = cadText->vertAlignmentPoint;
            text->vertInsertion = cadText->vertInsetionPoint;
            text->strTextValue = cadText->sTextValue;
            text->dVerticalAlignment = cadText->dVertAlign;
            text->dHorizontalAlignment = cadText->dHorizAlign;
            text->dGeneration = cadText->dGeneration;
            text->dfRotationAngle = cadText->dfRotationAng;
            text->dfObliqueAngle = cadText->dfObliqueAng;
            text->dfThickness = cadText->dfThickness;
            text->dfHeight = cadText->dfElevation;
            text->dfElevation = cadText->dfElevation;

//            delete( cadText );

            result_geometry = text;
            break;
        }

        default:
        {
            std::cerr << "Asked geometry has unsupported type.\n";
        }
    }

    return result_geometry;
}

size_t DWGFileR2000::GetLayersCount ()
{
    return astPresentedLayers.size();
}

Layer * DWGFileR2000::GetLayer ( size_t index )
{
    return astPresentedLayers[index];
}

DWGFileR2000::DWGFileR2000(CADFileIO* poFileIO) : CADFile(poFileIO)
{
    m_poHeader->AddValue(CADHeader::OPENCADVER, CADVersions::DWG_R2000);
}

DWGFileR2000::~DWGFileR2000()
{

}

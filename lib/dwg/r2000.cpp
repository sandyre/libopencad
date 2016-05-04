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
#include "opencad_api.h"

#include <iostream>
#include <cstring>

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
    int32_t section_size = 0;

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

    m_poFileIO->Read (&section_size, 4);
    DebugMsg ("Classes section length: %d\n", section_size);

    pabySectionContent = new char[section_size];
    m_poFileIO->Read (pabySectionContent, section_size);

    size_t bitOffsetFromStart = 0;

    while ( ( bitOffsetFromStart / 8 ) + 1 < section_size )
    {
        custom_classes.push_back (ReadClass (pabySectionContent, bitOffsetFromStart));
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

DWG2000_CLASS DWGFileR2000::ReadClass ( const char *input_array, size_t &bitOffsetFromStart )
{
    DWG2000_CLASS outputResult;

    outputResult.dClassNum     = ReadBITSHORT (input_array, bitOffsetFromStart);
    outputResult.dVersion      = ReadBITSHORT (input_array, bitOffsetFromStart);
    outputResult.sAppName      = ReadTV (input_array, bitOffsetFromStart);
    outputResult.sCppClassName = ReadTV (input_array, bitOffsetFromStart);
    outputResult.sDXFClassName = ReadTV (input_array, bitOffsetFromStart);
    outputResult.bWasAZombie   = ReadBIT (input_array, bitOffsetFromStart);
    outputResult.dItemClassID  = ReadBITSHORT (input_array, bitOffsetFromStart);

    return outputResult;
}

int DWGFileR2000::ReadObjectMap ()
{
    // Seems like ODA specification is completely awful. CRC is included in section size.
    char     *pabySectionContent;
    uint16_t dSectionCRC        = 0;
    uint16_t dSectionSize       = 0;
    uint32_t iCurrentSection = 0;
    size_t   nBitOffsetFromStart = 0;

    m_poFileIO->Seek (SLRecords[2].dSeeker, CADFileIO::SeekOrigin::BEG);

    while ( true )
    {
        nBitOffsetFromStart = 0;
        std::vector <ObjHandleOffset> obj_map_section;
        m_poFileIO->Read (&dSectionSize, 2);
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
            obj_map_section.push_back (tmp);
        }


        dSectionCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);
        SwapEndianness (dSectionCRC, sizeof (dSectionCRC));

        object_map_sections.push_back (obj_map_section);
        DebugMsg ("Objects in this section: %d\n", object_map_sections[iCurrentSection].size());

        ++iCurrentSection;
        delete[] pabySectionContent;
    }

    // Rebuild object map so it will store absolute file offset, instead of offset from previous object handle.
    for ( size_t index = 0; index < object_map_sections.size (); ++index )
    {
        for ( size_t i = 1; i < object_map_sections[index].size (); ++i )
        {
            object_map_sections[index][i].first  += object_map_sections[index][i-1].first;
            object_map_sections[index][i].second += object_map_sections[index][i-1].second;
        }
    }

    pabySectionContent = new char[400];

    for ( size_t i = 0; i < object_map_sections.size (); ++i )
    {
        for ( size_t j = 0; j < object_map_sections[i].size (); ++j )
        {
            nBitOffsetFromStart = 0;
            m_poFileIO->Seek (object_map_sections[i][j].second, CADFileIO::SeekOrigin::BEG);
            m_poFileIO->Read (pabySectionContent, 400);

            DWG2000_CED ced;
            ced.dLength = ReadMSHORT (pabySectionContent, nBitOffsetFromStart);
            ced.dType   = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);

            try
            {
                DWG_OBJECT_NAMES.at (ced.dType);
//                DebugMsg ( "Object type: %s"
//                                   " Handle: %d\n",
//                           DWG_OBJECT_NAMES.at(ced.dType).c_str(),
//                           object_map_sections[i][j].first
//                );

                if ( ced.dType == DWG_OBJECT_LAYER )
                    layer_map.push_back (object_map_sections[i][j]);
            }
            catch ( std::exception e )
            {
//                DebugMsg ( "Object type: %s\n",
//                           custom_classes[ced.dType - 500].sCppClassName.c_str()
//                );
            }

            if ( std::find ( DWG_GEOMETRIC_OBJECT_TYPES.begin (), DWG_GEOMETRIC_OBJECT_TYPES.end (), ced.dType )
                 != DWG_GEOMETRIC_OBJECT_TYPES.end () )
            {
                geometries_map.push_back (object_map_sections[i][j]);
            }
        }
    }


    delete[] pabySectionContent;

    return CADErrorCodes::SUCCESS;
}

/*
DWGObject * DWGFileR2000::getObject ( size_t section, size_t index )
{
    DWGObject * readed_object;

    char pabyObjectSize[8];
    size_t bitOffsetFromStart = 0;
    // m_oFileStream.clear (); Do we need it?
    m_poFileIO->Seek (object_map_sections[section][index].second, CADFileIO::BEG);
    m_poFileIO->Read (pabyObjectSize, 8);
    uint32_t dObjectSize = ReadMSHORT (pabyObjectSize, bitOffsetFromStart);

    // And read whole data chunk into memory for future parsing.
    char * pabySectionContent = new char[dObjectSize];
    bitOffsetFromStart = 0;
    //m_oFileStream.clear ();
    m_poFileIO->Seek (geometries_map[index].second, CADFileIO::BEG);
    m_poFileIO->Read (pabySectionContent, dObjectSize);

    dObjectSize = ReadMSHORT (pabySectionContent, bitOffsetFromStart);
    int16_t dObjectType = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);
    switch ( dObjectType )
    {
        case DWG_OBJECT_LAYER:
        {
            DWGLayer * layer = new DWGLayer();
            layer->dObjLength = dObjectSize;
            layer->dObjBitLength = ReadRAWLONG (pabySectionContent, bitOffsetFromStart);
            layer->hObjHandle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            int16_t dEEDSize = 0;
            while ( (dEEDSize = ReadBITSHORT (pabySectionContent, bitOffsetFromStart)) != 0 )
            {
                DWG_EED dwg_eed;
                dwg_eed.length = dEEDSize;
                dwg_eed.application_handle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
                dwg_eed.data = new char[dEEDSize];
                for ( size_t i = 0; i < dEEDSize; ++i )
                {
                    dwg_eed.data[i] = ReadCHAR (pabySectionContent, bitOffsetFromStart);
                }
                layer->astObjEED.push_back (dwg_eed);
            }
            layer->dObjNumReactors = ReadBITLONG (pabySectionContent, bitOffsetFromStart);
            layer->sLayerName = ReadTV (pabySectionContent, bitOffsetFromStart);
            layer->bFlag64 = ReadBIT (pabySectionContent, bitOffsetFromStart);
            layer->dXRefIndex = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);
            layer->bXDep = ReadBIT (pabySectionContent, bitOffsetFromStart);

            int16_t bFlags = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);
            if ( bFlags & 1 ) layer->bFrozen = true;
            if ( bFlags & 2 ) layer->bOn = true;
            if ( bFlags & 4 ) layer->bFrozenByDefaultInNewViewports = true;
            if ( bFlags & 8 ) layer->bLocked = true;
            if ( bFlags & 16 ) layer->bPlottingFlag = true;
            layer->dLineWeight = bFlags & 0x03E0;
            layer->dCMColorIndex = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);

            readed_object = layer;
        }
    }
}
*/

//int DWGFileR2000::ReadObject ( size_t index )
//{
//    char   pabySectionContent[100];
//    size_t bitOffsetFromStart = 0;
//    fileStream.seekg (object_map[index].second);
//    fileStream.read (pabySectionContent, 100);
//
//    DWG2000_CED ced;
//    ced.dLength        = ReadMSHORT (pabySectionContent, bitOffsetFromStart);
//    ced.dType          = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);
//    ced.dObjSizeInBits = ReadRAWLONG (pabySectionContent, bitOffsetFromStart);
//    ced.hHandle        = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
//    ced.dNumReactors   = ReadBITLONG (pabySectionContent, bitOffsetFromStart);
//
//    return 0;
//}

CADGeometry * DWGFileR2000::GetGeometry ( size_t index )
{
    CADGeometry * readed_geometry;

    // Get geometric entity size in bytes.
    char   pabySectionSize[8];
    size_t bitOffsetFromStart = 0;
    // m_oFileStream.clear ();
    m_poFileIO->Seek (geometries_map[index].second, CADFileIO::SeekOrigin::BEG);
    m_poFileIO->Read (pabySectionSize, 8);
    uint32_t dGeometrySize = ReadMSHORT (pabySectionSize, bitOffsetFromStart);

    // And read whole data chunk into memory for future parsing.
    // +N is because dGeometrySize does not cover ced.dLength length, and CRC length (so, ced.dLength can be
    // maximum 4 bytes long, and crc is 2 bytes long).
    char * pabySectionContent = new char[dGeometrySize + (bitOffsetFromStart / 8 + 2)];
    // m_oFileStream.clear ();
    m_poFileIO->Seek (geometries_map[index].second, CADFileIO::SeekOrigin::BEG);
    m_poFileIO->Read (pabySectionContent, dGeometrySize + (bitOffsetFromStart / 8 + 2));

    bitOffsetFromStart = 0;
    DWG2000_CED ced;
    ced.dLength        = ReadMSHORT (pabySectionContent, bitOffsetFromStart);
    ced.dType          = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);
    ced.dObjSizeInBits = ReadRAWLONG (pabySectionContent, bitOffsetFromStart);
    ced.hHandle        = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
    // TODO: EED is skipped, but it can be meaningful.
    // TODO: Also, EED is not a single struct; it can be repeated, so, need to
    // parse it ASAP, because it can cause errors.
    ced.dEEDSize       = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);
    bitOffsetFromStart += ced.dEEDSize * 8; // skip EED size bytes.

    // TODO: Proxy Entity Graphics also skipped for now. If there is something
    // (ced.bGraphicPresentFlag is true), than it wont work properly at all.
    ced.bGraphicPresentFlag = ReadBIT (pabySectionContent, bitOffsetFromStart);

    ced.dEntMode        = Read2B (pabySectionContent, bitOffsetFromStart);
    ced.dNumReactors    = ReadBITLONG (pabySectionContent, bitOffsetFromStart);
    ced.bNoLinks        = ReadBIT (pabySectionContent, bitOffsetFromStart);
    ced.dCMColorIndex   = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);
    ced.dfLtypeScale    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
    ced.ltype_flags     = Read2B (pabySectionContent, bitOffsetFromStart);
    ced.plotstyle_flags = Read2B (pabySectionContent, bitOffsetFromStart);
    ced.dInvisibility   = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);
    ced.cLineWeight     = ReadCHAR (pabySectionContent, bitOffsetFromStart);

    // READ DATA DEPENDING ON ced.dType
    switch ( ced.dType )
    {
        case DWG_OBJECT_CIRCLE:
        {
            Circle * circle = new Circle ();
            circle->dfCenterX   = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            circle->dfCenterY   = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            circle->dfCenterZ   = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            circle->dfRadius    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            circle->dfThickness = ReadBIT (pabySectionContent, bitOffsetFromStart) ?
                                  0.0f : ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            if ( ReadBIT (pabySectionContent, bitOffsetFromStart) )
            {
                circle->dfExtrusionX = 0.0f;
                circle->dfExtrusionY = 0.0f;
                circle->dfExtrusionZ = 1.0f;
            }
            else
            {
                circle->dfExtrusionX = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                circle->dfExtrusionY = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                circle->dfExtrusionZ = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            }

            DWG2000_CEHD common_entity_handle_data;
            common_entity_handle_data.hxdibobjhandle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( !ced.bNoLinks )
            {
                common_entity_handle_data.hprev_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
                common_entity_handle_data.hnext_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            common_entity_handle_data.hlayer = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( ced.ltype_flags == 0x03 )
            {
                common_entity_handle_data.hltype = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            if ( ced.plotstyle_flags == 0x03 )
            {
                common_entity_handle_data.hplotstyle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            bitOffsetFromStart += 8 - ( bitOffsetFromStart % 8 );
            int16_t crc = ReadRAWSHORT (pabySectionContent, bitOffsetFromStart);

            readed_geometry = circle;
            break;
        }

        case DWG_OBJECT_ELLIPSE:
        {
            Ellipse * ellipse = new Ellipse ();
            ellipse->dfCenterX       = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfCenterY       = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfCenterZ       = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfWCSX          = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfWCSY          = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfWCSZ          = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfExtrusionX    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfExtrusionY    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfExtrusionZ    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfAxisRatio     = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfStartingAngle = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            ellipse->dfEndingAngle   = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            DWG2000_CEHD common_entity_handle_data;
            common_entity_handle_data.hxdibobjhandle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( !ced.bNoLinks )
            {
                common_entity_handle_data.hprev_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
                common_entity_handle_data.hnext_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            common_entity_handle_data.hlayer = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( ced.ltype_flags == 0x03 )
            {
                common_entity_handle_data.hltype = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            if ( ced.plotstyle_flags == 0x03 )
            {
                common_entity_handle_data.hplotstyle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            bitOffsetFromStart += 8 - ( bitOffsetFromStart % 8 );
            int16_t crc = ReadRAWSHORT (pabySectionContent, bitOffsetFromStart);

            readed_geometry = ellipse;
            break;
        }

        case DWG_OBJECT_POINT:
        {
            Point * point = new Point ();
            point->dfPointX    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            point->dfPointY    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            point->dfPointZ    = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            point->dfThickness = ReadBIT (pabySectionContent, bitOffsetFromStart) ?
                                 0.0f : ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            if ( ReadBIT (pabySectionContent, bitOffsetFromStart) )
            {
                point->dfExtrusionX = 0.0f;
                point->dfExtrusionY = 0.0f;
                point->dfExtrusionZ = 1.0f;
            }
            else
            {
                point->dfExtrusionX = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                point->dfExtrusionY = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                point->dfExtrusionZ = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            }

            DWG2000_CEHD common_entity_handle_data;
            common_entity_handle_data.hxdibobjhandle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( !ced.bNoLinks )
            {
                common_entity_handle_data.hprev_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
                common_entity_handle_data.hnext_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            common_entity_handle_data.hlayer = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( ced.ltype_flags == 0x03 )
            {
                common_entity_handle_data.hltype = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            if ( ced.plotstyle_flags == 0x03 )
            {
                common_entity_handle_data.hplotstyle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            bitOffsetFromStart += 8 - ( bitOffsetFromStart % 8 );
            int16_t crc = ReadRAWSHORT (pabySectionContent, bitOffsetFromStart);

            readed_geometry = point;
            break;
        }

        case DWG_OBJECT_LINE:
        {
            Line * line            = new Line ();
            bool bZCoordPresented = ReadBIT (pabySectionContent, bitOffsetFromStart);
            line->dfStartX = ReadRAWDOUBLE (pabySectionContent, bitOffsetFromStart);
            line->dfEndX   = ReadBITDOUBLEWD (pabySectionContent, bitOffsetFromStart, line->dfStartX);
            line->dfStartY = ReadRAWDOUBLE (pabySectionContent, bitOffsetFromStart);
            line->dfEndY   = ReadBITDOUBLEWD (pabySectionContent, bitOffsetFromStart, line->dfStartY);

            if ( !bZCoordPresented )
            {
                line->dfStartZ = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                line->dfEndZ   = ReadBITDOUBLEWD (pabySectionContent, bitOffsetFromStart, line->dfStartZ);
            }

            line->dfThickness = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            if ( ReadBIT (pabySectionContent, bitOffsetFromStart) )
            {
                line->dfExtrusionX = 0.0f;
                line->dfExtrusionY = 0.0f;
                line->dfExtrusionZ = 1.0f;
            }
            else
            {
                line->dfExtrusionX = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                line->dfExtrusionY = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                line->dfExtrusionZ = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            }

            DWG2000_CEHD common_entity_handle_data;
            common_entity_handle_data.hxdibobjhandle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( !ced.bNoLinks )
            {
                common_entity_handle_data.hprev_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
                common_entity_handle_data.hnext_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            common_entity_handle_data.hlayer = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( ced.ltype_flags == 0x03 )
            {
                common_entity_handle_data.hltype = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            if ( ced.plotstyle_flags == 0x03 )
            {
                common_entity_handle_data.hplotstyle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            bitOffsetFromStart += 8 - ( bitOffsetFromStart % 8 );
            int16_t crc = ReadRAWSHORT (pabySectionContent, bitOffsetFromStart);

            readed_geometry = line;

            break;
        }

        case DWG_OBJECT_LWPOLYLINE:
        {
            LWPolyline * polyline      = new LWPolyline ();
            int32_t    nVertixesCount  = 0, nBulges = 0;
            int16_t    data_flag       = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);
            if ( data_flag & 4 )
                polyline->dfConstWidth = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            if ( data_flag & 8 )
                polyline->dfElevation  = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            if ( data_flag & 2 )
                polyline->dfThickness  = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            if ( data_flag & 1 )
            {
                polyline->dfExtrusionX = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                polyline->dfExtrusionY = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                polyline->dfExtrusionZ = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            }

            nVertixesCount = ReadBITLONG (pabySectionContent, bitOffsetFromStart);

            if ( data_flag & 16 )
            {
                nBulges = ReadBITLONG (pabySectionContent, bitOffsetFromStart);
            }

            // First of all, read first vertex.
            Vertex2D vertex;
            vertex.X = ReadRAWDOUBLE (pabySectionContent, bitOffsetFromStart);
            vertex.Y = ReadRAWDOUBLE (pabySectionContent, bitOffsetFromStart);
            polyline->vertexes.push_back (vertex);

            // All the others are not raw doubles; bitdoubles with default instead,
            // where default is previous point coords.
            for ( size_t i = 1; i < nVertixesCount; ++i )
            {
                vertex.X = ReadBITDOUBLEWD (pabySectionContent, bitOffsetFromStart,
                                            polyline->vertexes[i - 1].X);
                vertex.Y = ReadBITDOUBLEWD (pabySectionContent, bitOffsetFromStart,
                                            polyline->vertexes[i - 1].Y);
                polyline->vertexes.push_back (vertex);
            }

            for ( size_t i = 0; i < nBulges; ++i )
            {
                double dfBulgeValue = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                polyline->bulges.push_back (dfBulgeValue);
            }

            DWG2000_CEHD common_entity_handle_data;
            common_entity_handle_data.hxdibobjhandle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( !ced.bNoLinks )
            {
                common_entity_handle_data.hprev_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
                common_entity_handle_data.hnext_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            common_entity_handle_data.hlayer = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( ced.ltype_flags == 0x03 )
            {
                common_entity_handle_data.hltype = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            if ( ced.plotstyle_flags == 0x03 )
            {
                common_entity_handle_data.hplotstyle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            bitOffsetFromStart += 8 - ( bitOffsetFromStart % 8 );
            int16_t crc = ReadRAWSHORT (pabySectionContent, bitOffsetFromStart);

            readed_geometry = polyline;
            break;
        }

        case DWG_OBJECT_ARC:
        {
            Arc * arc = new Arc ();
            arc->dfCenterX       = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            arc->dfCenterY       = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            arc->dfCenterZ       = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            arc->dfRadius        = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            arc->dfThickness     = ReadBIT (pabySectionContent, bitOffsetFromStart) ?
                                   0.0f : ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            if ( ReadBIT (pabySectionContent, bitOffsetFromStart) )
            {
                arc->dfExtrusionX = 0.0f;
                arc->dfExtrusionY = 0.0f;
                arc->dfExtrusionZ = 1.0f;
            }
            else
            {
                arc->dfExtrusionX = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                arc->dfExtrusionY = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                arc->dfExtrusionZ = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            }
            arc->dfStartingAngle = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            arc->dfEndingAngle   = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            DWG2000_CEHD common_entity_handle_data;
            common_entity_handle_data.hxdibobjhandle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( !ced.bNoLinks )
            {
                common_entity_handle_data.hprev_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
                common_entity_handle_data.hnext_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            common_entity_handle_data.hlayer = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( ced.ltype_flags == 0x03 )
            {
                common_entity_handle_data.hltype = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            if ( ced.plotstyle_flags == 0x03 )
            {
                common_entity_handle_data.hplotstyle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            bitOffsetFromStart += 8 - ( bitOffsetFromStart % 8 );
            int16_t crc = ReadRAWSHORT (pabySectionContent, bitOffsetFromStart);

            readed_geometry = arc;
            break;
        }

        case DWG_OBJECT_TEXT:
        {
            Text   * text    = new Text ();
            int8_t DataFlag = ReadCHAR (pabySectionContent, bitOffsetFromStart);

            if ( !( DataFlag & 0x01 ) )
                text->dfElevation = ReadRAWDOUBLE (pabySectionContent, bitOffsetFromStart);

            text->dfInsertionX = ReadRAWDOUBLE (pabySectionContent, bitOffsetFromStart);
            text->dfInsertionY = ReadRAWDOUBLE (pabySectionContent, bitOffsetFromStart);

            if ( !( DataFlag & 0x02 ) )
            {
                text->dfAlignmentX = ReadBITDOUBLEWD (pabySectionContent, bitOffsetFromStart, 10.0f);
                text->dfAlignmentY = ReadBITDOUBLEWD (pabySectionContent, bitOffsetFromStart, 20.0f);
            }

            if ( ReadBIT (pabySectionContent, bitOffsetFromStart) )
            {
                text->dfExtrusionX = 0.0f;
                text->dfExtrusionY = 0.0f;
                text->dfExtrusionZ = 1.0f;
            }
            else
            {
                text->dfExtrusionX = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                text->dfExtrusionY = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
                text->dfExtrusionZ = ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);
            }

            text->dfThickness = ReadBIT (pabySectionContent, bitOffsetFromStart) ?
                                0.0f : ReadBITDOUBLE (pabySectionContent, bitOffsetFromStart);

            if ( !( DataFlag & 0x04 ) )
                text->dfObliqueAngle = ReadRAWDOUBLE (pabySectionContent, bitOffsetFromStart);

            if ( !( DataFlag & 0x08 ) )
                text->dfRotationAngle = ReadRAWDOUBLE (pabySectionContent, bitOffsetFromStart);

            text->dfHeight = ReadRAWDOUBLE (pabySectionContent, bitOffsetFromStart);

            if ( !( DataFlag & 0x10 ) )
                text->dfWidthFactor = ReadRAWDOUBLE (pabySectionContent, bitOffsetFromStart);

            text->strTextValue = ReadTV (pabySectionContent, bitOffsetFromStart);

            if ( !( DataFlag & 0x20 ) )
                text->dGeneration = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);

            if ( !( DataFlag & 0x40 ) )
                text->dHorizontalAlignment = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);

            if ( !( DataFlag & 0x80 ) )
                text->dVerticalAlignment = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);

            DWG2000_CEHD common_entity_handle_data;
            common_entity_handle_data.hxdibobjhandle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( !ced.bNoLinks )
            {
                common_entity_handle_data.hprev_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
                common_entity_handle_data.hnext_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            common_entity_handle_data.hlayer = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( ced.ltype_flags == 0x03 )
            {
                common_entity_handle_data.hltype = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            if ( ced.plotstyle_flags == 0x03 )
            {
                common_entity_handle_data.hplotstyle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            bitOffsetFromStart += 8 - ( bitOffsetFromStart % 8 );
            int16_t crc = ReadRAWSHORT (pabySectionContent, bitOffsetFromStart);

            readed_geometry = text;
            break;
        }

        case DWG_OBJECT_POLYLINE3D:
        {
            Polyline3D * pline = new Polyline3D ();

            int8_t DataFlag = ReadCHAR (pabySectionContent, bitOffsetFromStart);
            int8_t DataFlag2 = ReadCHAR (pabySectionContent, bitOffsetFromStart);

            DWG2000_CEHD common_entity_handle_data;
            common_entity_handle_data.hxdibobjhandle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( !ced.bNoLinks )
            {
                common_entity_handle_data.hprev_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
                common_entity_handle_data.hnext_entity = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            common_entity_handle_data.hlayer = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            if ( ced.ltype_flags == 0x03 )
            {
                common_entity_handle_data.hltype = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            if ( ced.plotstyle_flags == 0x03 )
            {
                common_entity_handle_data.hplotstyle = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            }

            CADHandle firstvertex = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            CADHandle lastvertex  = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            CADHandle seqend      = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

            bitOffsetFromStart += 8 - ( bitOffsetFromStart % 8 );
            int16_t crc = ReadRAWSHORT (pabySectionContent, bitOffsetFromStart);

            readed_geometry = pline;
            break;
        }
    }

    delete[] pabySectionContent;

    return readed_geometry;
}

CADLayer * DWGFileR2000::GetLayer ( size_t index )
{
    CADLayer * layer = new CADLayer();

    for ( size_t i = 0; i < GetGeometriesCount (); ++i )
    {

    }

    return layer;
}

size_t DWGFileR2000::GetGeometriesCount ()
{
    return geometries_map.size ();
}

size_t DWGFileR2000::GetLayersCount ()
{
    return layer_map.size();
}

DWGFileR2000::DWGFileR2000(CADFileIO* poFileIO) : CADFile(poFileIO)
{
    m_poHeader->AddValue(CADHeader::OPENCADVER, CADVersions::DWG_R2000);
}

DWGFileR2000::~DWGFileR2000()
{

}

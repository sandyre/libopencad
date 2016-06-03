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

#include "r2000.h"
#include "io.h"
#include "cadgeometry.h"
#include "cadobjects.h"
#include "opencad_api.h"

#include <iostream>
#include <cstring>
#include <cassert>
#include <memory>

#ifdef __APPLE__
#include <MacTypes.h>
#endif

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

int DWGFileR2000::readHeader ()
{
    char    *pabyBuf = new char[100];
    int     dImageSeeker, dSLRecords;
    short   dCodePage;

    fileIO->Rewind();
    memset(pabyBuf, 0, DWG_VERSION_STR_SIZE + 1);
    fileIO->Read(pabyBuf, DWG_VERSION_STR_SIZE);
    header.addValue(CADHeader::ACADVER, pabyBuf);
    memset(pabyBuf, 0, 8);
    fileIO->Read(pabyBuf, 7);
    header.addValue(CADHeader::ACADMAINTVER, pabyBuf);

    fileIO->Read (&dImageSeeker, 4);

    DebugMsg("Image seeker readed: %d\n", dImageSeeker);

    fileIO->Seek (2, CADFileIO::SeekOrigin::CUR); // 19
    fileIO->Read (&dCodePage, 2);
    header.addValue(CADHeader::DWGCODEPAGE, dCodePage);

    DebugMsg("DWG Code page: %d\n", dCodePage);

    fileIO->Read (&dSLRecords, 4); // 21

    DebugMsg("Section-locator records count: %d\n", dSLRecords);

    for ( size_t i = 0; i < static_cast<size_t>(dSLRecords); ++i )
    {
        SLRecord readedRecord;
        fileIO->Read (&readedRecord.byRecordNumber, 1);
        fileIO->Read (&readedRecord.dSeeker, 4);
        fileIO->Read (&readedRecord.dSize, 4);

        SLRecords.push_back (readedRecord);
        DebugMsg("SL Record #%d : %d %d\n",
                 SLRecords[i].byRecordNumber,
                 SLRecords[i].dSeeker,
                 SLRecords[i].dSize);
    }

/*      READ HEADER VARIABLES        */
    size_t dHeaderVarsSectionLength = 0;

    fileIO->Seek (SLRecords[0].dSeeker, CADFileIO::SeekOrigin::BEG);
    fileIO->Read (pabyBuf, DWGSentinelLength);
    if ( memcmp (pabyBuf, DWGHeaderVariablesStart, DWGSentinelLength) )
    {
        DebugMsg("File is corrupted (wrong pointer to HEADER_VARS section,"
                        "or HEADERVARS starting sentinel corrupted.)");

        delete[] pabyBuf;

        return CADErrorCodes::HEADER_SECTION_READ_FAILED;
    }

    delete[] pabyBuf;

    fileIO->Read (&dHeaderVarsSectionLength, 4);
    DebugMsg("Header variables section length: %ld\n", dHeaderVarsSectionLength);

    size_t nBitOffsetFromStart = 0;
    pabyBuf = new char[dHeaderVarsSectionLength];
    fileIO->Read ( pabyBuf, dHeaderVarsSectionLength + 2 );

    header.addValue(UNKNOWN1, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(UNKNOWN2, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(UNKNOWN3, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(UNKNOWN4, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(UNKNOWN5, ReadTV (pabyBuf, nBitOffsetFromStart));
    header.addValue(UNKNOWN6, ReadTV (pabyBuf, nBitOffsetFromStart));
    header.addValue(UNKNOWN7, ReadTV (pabyBuf, nBitOffsetFromStart));
    header.addValue(UNKNOWN8, ReadTV (pabyBuf, nBitOffsetFromStart));
    header.addValue(UNKNOWN9, ReadBITLONG (pabyBuf, nBitOffsetFromStart));
    header.addValue(UNKNOWN10, ReadBITLONG (pabyBuf, nBitOffsetFromStart));

    CADHandle stCurrentViewportTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::CurrentViewportTable,
                        stCurrentViewportTable);

    header.addValue(CADHeader::DIMASO, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMSHO, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::PLINEGEN, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::ORTHOMODE, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::REGENMODE, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::FILLMODE, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::QTEXTMODE, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::PSLTSCALE, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::LIMCHECK, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::USRTIMER, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::SKPOLY, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::ANGDIR, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::SPLFRAME, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::MIRRTEXT, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::WORDLVIEW, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::TILEMODE, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::PLIMCHECK, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::VISRETAIN, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DISPSILH, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::PELLIPSE, ReadBIT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::PROXYGRAPHICS, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::TREEDEPTH, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::LUNITS, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::LUPREC, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::AUNITS, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::AUPREC, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::ATTMODE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::PDMODE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::USERI1, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::USERI2, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::USERI3, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::USERI4, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::USERI5, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::SPLINESEGS, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::SURFU, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::SURFV, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::SURFTYPE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::SURFTAB1, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::SURFTAB2, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::SPLINETYPE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::SHADEDGE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::SHADEDIF, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::UNITMODE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::MAXACTVP, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::ISOLINES, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::CMLJUST, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::TEXTQLTY, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::LTSCALE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::TEXTSIZE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::TRACEWID, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::SKETCHINC, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::FILLETRAD, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::THICKNESS, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::ANGBASE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::PDSIZE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::PLINEWID, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::USERR1, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::USERR2, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::USERR3, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::USERR4, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::USERR5, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::CHAMFERA, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::CHAMFERB, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::CHAMFERC, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::CHAMFERD, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::FACETRES, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::CMLSCALE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::CELTSCALE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::MENU, ReadTV (pabyBuf, nBitOffsetFromStart));

    long juliandate, millisec;
    juliandate = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    millisec = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::TDCREATE, juliandate, millisec);
    juliandate = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    millisec = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::TDUPDATE, juliandate, millisec);
    juliandate = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    millisec = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::TDINDWG, juliandate, millisec);
    juliandate = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    millisec = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::TDUSRTIMER, juliandate, millisec);

    header.addValue(CADHeader::CECOLOR, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::HANDSEED, ReadHANDLE8BLENGTH (pabyBuf, nBitOffsetFromStart)); // CHECK THIS CASE.

    header.addValue(CADHeader::CLAYER, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::TEXTSTYLE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::CELTYPE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMSTYLE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::CMLSTYLE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::PSVPSCALE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    double dX, dY, dZ;
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PINSBASE, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PEXTMIN, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PEXTMAX, dX, dY, dZ);
    dX = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PLIMMIN, dX, dY);
    dX = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PLIMMAX, dX, dY);

    header.addValue(CADHeader::PELEVATION, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));

    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PUCSORG, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PUCSXDIR, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PUCSYDIR, dX, dY, dZ);

    header.addValue(CADHeader::PUCSNAME, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::PUCSORTHOREF, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::PUCSORTHOVIEW, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::PUCSBASE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PUCSORGTOP, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PUCSORGBOTTOM, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PUCSORGLEFT, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PUCSORGRIGHT, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PUCSORGFRONT, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::PUCSORGBACK, dX, dY, dZ);

    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::INSBASE, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::EXTMIN, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::EXTMAX, dX, dY, dZ);
    dX = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::LIMMIN, dX, dY);
    dX = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadRAWDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::LIMMAX, dX, dY);

    header.addValue(CADHeader::ELEVATION, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::UCSORG, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::UCSXDIR, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::UCSYDIR, dX, dY, dZ);

    header.addValue(CADHeader::UCSNAME, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::UCSORTHOREF, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::UCSORTHOVIEW, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::UCSBASE, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::UCSORGTOP, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::UCSORGBOTTOM, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::UCSORGLEFT, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::UCSORGRIGHT, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::UCSORGFRONT, dX, dY, dZ);
    dX = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dY = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    dZ = ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::UCSORGBACK, dX, dY, dZ);

    header.addValue(CADHeader::DIMPOST, ReadTV (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMAPOST, ReadTV (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMSCALE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMASZ, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMEXO, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMDLI, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMEXE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMRND, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMDLE, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMTP, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMTM, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMTOL, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMLIM, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMTIH, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMTOH, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMSE1, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMSE2, ReadBIT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMTAD, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMZIN, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMAZIN, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMTXT, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMCEN, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMTSZ, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMALTF, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMLFAC, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMTVP, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMTFAC, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMGAP, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMALTRND, ReadBITDOUBLE (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMALT, ReadBIT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMALTD, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMTOFL, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMSAH, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMTIX, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMSOXD, ReadBIT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMCLRD, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMCLRE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMCLRT, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMADEC, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMDEC, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMTDEC, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMALTU, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMALTTD, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMAUNIT, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMFRAC, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMLUNIT, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMDSEP, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMTMOVE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMJUST, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMSD1, ReadBIT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMSD2, ReadBIT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMTOLJ, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMTZIN, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMALTZ, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMALTTZ, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMUPT, ReadBIT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMATFIT, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMTXSTY, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMLDRBLK, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMBLK, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMBLK1, ReadHANDLE (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMBLK2, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::DIMLWD, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::DIMLWE, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    CADHandle stBlocksTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::BlocksTable, stBlocksTable);

    CADHandle stLayersTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::LayersTable, stLayersTable);

    CADHandle stStyleTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::StyleTable, stStyleTable);

    CADHandle stLineTypesTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::LineTypesTable, stLineTypesTable);

    CADHandle stViewTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::ViewTable, stViewTable);

    CADHandle stUCSTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::UCSTable, stUCSTable);

    CADHandle stViewportTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::ViewportTable, stViewportTable);

    CADHandle stAPPIDTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::APPIDTable, stAPPIDTable);

    header.addValue(CADHeader::DIMSTYLE, ReadHANDLE (pabyBuf,
                                                        nBitOffsetFromStart));

    CADHandle stEntityTable = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::EntityTable, stEntityTable);

    CADHandle stACADGroupDict = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::ACADGroupDict, stACADGroupDict);

    CADHandle stACADMLineStyleDict = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::ACADMLineStyleDict, stACADMLineStyleDict);

    CADHandle stNamedObjectsDict = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::NamedObjectsDict, stNamedObjectsDict);

    header.addValue(CADHeader::TSTACKALIGN, ReadBITSHORT (pabyBuf,
                                                        nBitOffsetFromStart));
    header.addValue(CADHeader::TSTACKSIZE, ReadBITSHORT (pabyBuf,
                                                        nBitOffsetFromStart));
    header.addValue(CADHeader::HYPERLINKBASE, ReadTV (pabyBuf,
                                                         nBitOffsetFromStart));
    header.addValue(CADHeader::STYLESHEET, ReadTV (pabyBuf,
                                                      nBitOffsetFromStart));

    CADHandle stLayoutsDict = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::LayoutsDict, stLayoutsDict);

    CADHandle stPlotSettingsDict = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::PlotSettingsDict, stPlotSettingsDict);

    CADHandle stPlotStylesDict = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::PlotStylesDict, stPlotStylesDict);

    int Flags = ReadBITLONG (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::CELWEIGHT, Flags & 0x001F);
    header.addValue(CADHeader::ENDCAPS, static_cast<bool>(Flags & 0x0060));
    header.addValue(CADHeader::JOINSTYLE, static_cast<bool>(Flags & 0x0180));
    header.addValue(CADHeader::LWDISPLAY, static_cast<bool>(!(Flags & 0x0200)));
    header.addValue(CADHeader::XEDIT, static_cast<bool>(!(Flags & 0x0400)));
    header.addValue(CADHeader::EXTNAMES, static_cast<bool>(Flags & 0x0800));
    header.addValue(CADHeader::PSTYLEMODE, static_cast<bool>(Flags & 0x2000));
    header.addValue(CADHeader::OLESTARTUP, static_cast<bool>(Flags & 0x4000));

    header.addValue(CADHeader::INSUNITS, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    short nCEPSNTYPE = ReadBITSHORT (pabyBuf, nBitOffsetFromStart);
    header.addValue(CADHeader::CEPSNTYPE, nCEPSNTYPE);

    if ( nCEPSNTYPE == 3 )
        header.addValue(CADHeader::CEPSNID, ReadHANDLE (pabyBuf, nBitOffsetFromStart));

    header.addValue(CADHeader::FINGERPRINTGUID, ReadTV (pabyBuf, nBitOffsetFromStart));
    header.addValue(CADHeader::VERSIONGUID, ReadTV (pabyBuf, nBitOffsetFromStart));

    CADHandle stBlockRecordPaperSpace = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::BlockRecordPaperSpace,
                        stBlockRecordPaperSpace);

    CADHandle stBlockRecordModelSpace = ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    tables.addTable (CADTables::BlockRecordModelSpace, stBlockRecordModelSpace);

    // Is this part of the header?

    /*CADHandle LTYPE_BYLAYER = */ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    /*CADHandle LTYPE_BYBLOCK = */ReadHANDLE (pabyBuf, nBitOffsetFromStart);
    /*CADHandle LTYPE_CONTINUOUS = */ReadHANDLE (pabyBuf, nBitOffsetFromStart);

    header.addValue(UNKNOWN11, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(UNKNOWN12, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(UNKNOWN13, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));
    header.addValue(UNKNOWN14, ReadBITSHORT (pabyBuf, nBitOffsetFromStart));

    /*short nCRC =*/ ReadRAWSHORT (pabyBuf, nBitOffsetFromStart);
    unsigned short initial = 0xC0C1;
    /*short calculated_crc = */ CalculateCRC8 (initial, pabyBuf,
                                    static_cast<int>(dHeaderVarsSectionLength));
    // TODO: CRC is calculated wrong every time.

    fileIO->Read (pabyBuf, DWGSentinelLength);
    if ( memcmp (pabyBuf, DWGHeaderVariablesEnd, DWGSentinelLength) )
    {
        DebugMsg("File is corrupted (HEADERVARS section ending sentinel doesnt match.)");

        delete[] pabyBuf;

        return CADErrorCodes::HEADER_SECTION_READ_FAILED;
    }

    delete[] pabyBuf;

    return CADErrorCodes::SUCCESS;
}

int DWGFileR2000::readClasses ()
{
    char   *pabySectionContent;
    char    buffer[255];
    size_t dSectionSize = 0;
    size_t nBitOffsetFromStart = 0;

    fileIO->Seek (SLRecords[1].dSeeker, CADFileIO::SeekOrigin::BEG);

    fileIO->Read (buffer, DWGSentinelLength);
    if ( memcmp (buffer, DWGDSClassesStart, DWGSentinelLength) )
    {
        std::cerr << "File is corrupted (wrong pointer to CLASSES section,"
                "or CLASSES starting sentinel corrupted.)\n";

        return CADErrorCodes::CLASSES_SECTION_READ_FAILED;
    }

    fileIO->Read (&dSectionSize, 4);
    DebugMsg ("Classes section length: %d\n", dSectionSize);

    pabySectionContent = new char[dSectionSize];
    fileIO->Read (pabySectionContent, dSectionSize);

    while ( ( nBitOffsetFromStart / 8 ) + 1 < dSectionSize )
    {
        struct CADClass stClass;
        stClass.dClassNum = ReadBITSHORT (pabySectionContent,
                                          nBitOffsetFromStart);
        stClass.dProxyCapFlag = ReadBITSHORT (pabySectionContent,
                                            nBitOffsetFromStart);
        stClass.sApplicationName = ReadTV (pabySectionContent,
                                           nBitOffsetFromStart);
        stClass.sCppClassName = ReadTV (pabySectionContent,
                                        nBitOffsetFromStart);
        stClass.sDXFRecordName = ReadTV (pabySectionContent,
                                         nBitOffsetFromStart);
        stClass.bWasZombie = ReadBIT (pabySectionContent, nBitOffsetFromStart);
        stClass.bIsEntity  = ReadBITSHORT (pabySectionContent,
                                    nBitOffsetFromStart) == 0x1F2 ? true : false;

        classes.addClass (stClass);
    }

    delete [] pabySectionContent;

    fileIO->Read (buffer, 2); // CLASSES CRC!. TODO: add CRC computing & checking feature.

    fileIO->Read (buffer, DWGSentinelLength);
    if ( memcmp (buffer, DWGDSClassesEnd, DWGSentinelLength) )
    {
        std::cerr << "File is corrupted (CLASSES section ending sentinel doesnt match.)\n";
        return CADErrorCodes::CLASSES_SECTION_READ_FAILED;
    }
    return CADErrorCodes::SUCCESS;
}

int DWGFileR2000::createFileMap ()
{
    return CADErrorCodes::SUCCESS;
    // Seems like ODA specification is completely awful. CRC is included in
    // section size
    char *pabySectionContent;
    unsigned short dSectionSize;
    size_t nRecordsInSection;
    size_t nRecord = 0;
    size_t nBitOffsetFromStart;

    typedef std::pair<long, long> ObjHandleOffset;

    // seek to the begining of the objects map
    fileIO->Seek (SLRecords[2].dSeeker, CADFileIO::SeekOrigin::BEG);
    while ( true )
    {
        nBitOffsetFromStart = 0;
        nRecordsInSection = 0;
        dSectionSize = 0;

        // read section size
        fileIO->Read (&dSectionSize, 2);
        //SwapEndianness (dSectionSize, sizeof (dSectionSize));

        DebugMsg ("Object map section #%d size: %d\n", ++nRecord,
                  dSectionSize);

        if ( dSectionSize == 2 )
            break; // last section is empty.

        pabySectionContent = new char[dSectionSize];

        // read section data
        fileIO->Read (pabySectionContent, dSectionSize);

        ObjHandleOffset previousObjHandleOffset;
        while ( ( nBitOffsetFromStart / 8 ) < ( dSectionSize - 2 ) )
        {
            ObjHandleOffset tmp;
            tmp.first  = ReadUMCHAR (pabySectionContent, nBitOffsetFromStart);
            tmp.second = ReadMCHAR (pabySectionContent, nBitOffsetFromStart);


            if(0 == nRecordsInSection) {
                previousObjHandleOffset = tmp;
            }
            else {
                // fix index and offset relatively first record
                previousObjHandleOffset.first += tmp.first;
                previousObjHandleOffset.second += tmp.second;
            }
            objectsMap.insert (previousObjHandleOffset);
            ++nRecordsInSection;
        }

        /* Unused
        dSectionCRC = */ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);/*
        SwapEndianness (dSectionCRC, sizeof (dSectionCRC));
        */

        delete[] pabySectionContent;
    }

    return CADErrorCodes::SUCCESS;
}

CADObject * DWGFileR2000::getObject (long handle )
{
    CADObject * readed_object = nullptr;

    char pabyObjectSize[8];
    size_t nBitOffsetFromStart = 0;
    fileIO->Seek (objectsMap[handle], CADFileIO::SeekOrigin::BEG);
    fileIO->Read (pabyObjectSize, 8);
    unsigned int dObjectSize = ReadMSHORT (pabyObjectSize, nBitOffsetFromStart);

    // And read whole data chunk into memory for future parsing.
    // + nBitOffsetFromStart/8 + 2 is because dObjectSize doesn't cover CRC and itself.
    size_t nSectionSize = dObjectSize + nBitOffsetFromStart/8 + 2;
    char * pabySectionContent = new char[nSectionSize];
    fileIO->Seek (objectsMap[handle], CADFileIO::SeekOrigin::BEG);
    fileIO->Read (pabySectionContent, nSectionSize);

    nBitOffsetFromStart = 0;
    dObjectSize = ReadMSHORT (pabySectionContent, nBitOffsetFromStart);
    short dObjectType = ReadBITSHORT (pabySectionContent, nBitOffsetFromStart);

    // Entities handling
    if ( isGeometryType(dObjectType) )
    {
        struct CADCommonED stCommonEntityData; // common for all entities ofc.

        stCommonEntityData.nObjectSizeInBits = ReadRAWLONG (pabySectionContent,
                                                            nBitOffsetFromStart);
        stCommonEntityData.hObjectHandle = ReadHANDLE (pabySectionContent,
                                                       nBitOffsetFromStart);

        short dEEDSize;
        CADEed dwgEed;
        while ( (dEEDSize = ReadBITSHORT (pabySectionContent,
                                          nBitOffsetFromStart)) != 0 )
        {
            dwgEed.dLength = dEEDSize;
            dwgEed.hApplication = ReadHANDLE (pabySectionContent,
                                              nBitOffsetFromStart);

            for ( short i = 0; i < dEEDSize; ++i )
            {
                dwgEed.acData.push_back(ReadCHAR (pabySectionContent,
                                                  nBitOffsetFromStart));
            }

            stCommonEntityData.aEED.push_back (dwgEed);
        }

        stCommonEntityData.bGraphicsPresented = ReadBIT (pabySectionContent,
                                                         nBitOffsetFromStart);
        stCommonEntityData.bbEntMode = Read2B (pabySectionContent,
                                               nBitOffsetFromStart);
        stCommonEntityData.nNumReactors = ReadBITLONG (pabySectionContent,
                                                       nBitOffsetFromStart);
        stCommonEntityData.bNoLinks = ReadBIT (pabySectionContent,
                                               nBitOffsetFromStart);
        stCommonEntityData.nCMColor = ReadBITSHORT (pabySectionContent,
                                                    nBitOffsetFromStart);
        stCommonEntityData.dfLTypeScale = ReadBITDOUBLE (pabySectionContent,
                                                         nBitOffsetFromStart);
        stCommonEntityData.bbLTypeFlags = Read2B (pabySectionContent,
                                                  nBitOffsetFromStart);
        stCommonEntityData.bbPlotStyleFlags = Read2B (pabySectionContent,
                                                      nBitOffsetFromStart);
        stCommonEntityData.nInvisibility = ReadBITSHORT (pabySectionContent,
                                                         nBitOffsetFromStart);
        stCommonEntityData.nLineWeight = ReadCHAR (pabySectionContent,
                                                   nBitOffsetFromStart);

        switch ( dObjectType )
        {
            case CADObject::BLOCK:
                return getBlock(dObjectSize, stCommonEntityData,
                                pabySectionContent, nBitOffsetFromStart);

            case CADObject::ELLIPSE:
                return getEllipse(dObjectSize, stCommonEntityData,
                                  pabySectionContent, nBitOffsetFromStart);

            case CADObject::SOLID:
                return getSolid(dObjectSize, stCommonEntityData,
                                pabySectionContent, nBitOffsetFromStart);

            case CADObject::POINT:
                return getPoint(dObjectSize, stCommonEntityData,
                                pabySectionContent, nBitOffsetFromStart);

            case CADObject::POLYLINE3D:
                return getPolyLine3D(dObjectSize, stCommonEntityData,
                                pabySectionContent, nBitOffsetFromStart);

            case CADObject::RAY:
                return getRay(dObjectSize, stCommonEntityData,
                                pabySectionContent, nBitOffsetFromStart);
            case CADObject::XLINE:
                return getXLine(dObjectSize, stCommonEntityData,
                                pabySectionContent, nBitOffsetFromStart);

            case CADObject::LINE:
                return getLine(dObjectSize, stCommonEntityData,
                               pabySectionContent, nBitOffsetFromStart);

            case CADObject::TEXT:
                return getText(dObjectSize, stCommonEntityData,
                               pabySectionContent, nBitOffsetFromStart);

            case CADObject::VERTEX3D:
                return getVertex3D(dObjectSize, stCommonEntityData,
                               pabySectionContent, nBitOffsetFromStart);

            case CADObject::CIRCLE:
                return getCircle(dObjectSize, stCommonEntityData,
                                 pabySectionContent, nBitOffsetFromStart);

            case CADObject::ENDBLK:
                return getEndBlock(dObjectSize, stCommonEntityData,
                                   pabySectionContent, nBitOffsetFromStart);

            case CADObject::POLYLINE2D:
                return getPolyline2D(dObjectSize, stCommonEntityData,
                                     pabySectionContent, nBitOffsetFromStart);

            case CADObject::ATTRIB:
                return getAttributes(dObjectSize, stCommonEntityData,
                                     pabySectionContent, nBitOffsetFromStart);

            case CADObject::ATTDEF:
                return getAttributesDefn(dObjectSize, stCommonEntityData,
                                     pabySectionContent, nBitOffsetFromStart);

            case CADObject::LWPOLYLINE:
                return getLWPolyLine(dObjectSize, stCommonEntityData,
                                     pabySectionContent, nBitOffsetFromStart);

            case CADObject::ARC:
                return getArc(dObjectSize, stCommonEntityData,
                              pabySectionContent, nBitOffsetFromStart);

            case CADObject::SPLINE:
                return getSpline(dObjectSize, stCommonEntityData,
                                 pabySectionContent, nBitOffsetFromStart);

            default:
                return getEntity(dObjectType, dObjectSize, stCommonEntityData,
                                 pabySectionContent, nBitOffsetFromStart);
        }
    }
    else
    {
        switch ( dObjectType )
        {
            case CADObject::DICTIONARY:
                return getDictionary(dObjectSize, pabySectionContent,
                                     nBitOffsetFromStart);

            case CADObject::LAYER:
                return getLayer(dObjectSize, pabySectionContent,
                                nBitOffsetFromStart);

            case CADObject::LAYER_CONTROL_OBJ:
                return getLayerControl(dObjectSize, pabySectionContent,
                                       nBitOffsetFromStart);

            case CADObject::BLOCK_CONTROL_OBJ:
                return getBlockControl(dObjectSize, pabySectionContent,
                                       nBitOffsetFromStart);

            case CADObject::BLOCK_HEADER:
                return getBlockHeader(dObjectSize, pabySectionContent,
                                      nBitOffsetFromStart);

            case CADObject::LTYPE_CONTROL_OBJ:
                return getLineTypeControl(dObjectSize, pabySectionContent,
                                     nBitOffsetFromStart);

            case CADObject::LTYPE1:
                return getLineType1(dObjectSize, pabySectionContent,
                                 nBitOffsetFromStart);

        }
    }

    return readed_object;
}

CADGeometry *DWGFileR2000::getGeometry(long handle)
{
    unique_ptr<CADObject> readedObject( getObject(handle) );

    if(nullptr == readedObject)
        return nullptr;

    switch ( readedObject->eObjectType )
    {
        case CADObject::ARC:
        {
            CADArc * arc = new CADArc();
            CADArcObject * cadArc = static_cast<CADArcObject*>(
                        readedObject.get());

            arc->setPosition (cadArc->vertPosition);
            arc->setExtrusion (cadArc->vectExtrusion);
            arc->setRadius (cadArc->dfRadius);
            arc->setThickness(cadArc->dfThickness);
            arc->setStartingAngle (cadArc->dfStartAngle);
            arc->setEndingAngle (cadArc->dfEndAngle);

            return arc;
        }

        case CADObject::POINT:
        {
            CADPoint3D * point = new CADPoint3D();
            CADPointObject * cadPoint = static_cast<CADPointObject*>(
                        readedObject.get());

            point->setPosition (cadPoint->vertPosition);
            point->setExtrusion (cadPoint->vectExtrusion);
            point->setXAxisAng (cadPoint->dfXAxisAng);
            point->setThickness(cadPoint->dfThickness);

            return point;
        }

        /*case CADObject::POLYLINE3D:
        {
            Polyline3D * polyline = new Polyline3D();
            CADPolyline3DObject * cadPolyline3D = ( CADPolyline3DObject * ) readedObject;

            // TODO: code can be much simplified if CADHandle will be used.
            // to do so, == and ++ operators should be implemented.
            CADVertex3DObject * vertex;
            long long currentVertexH = cadPolyline3D->hVertexes[0].GetAsLong ();
            while ( currentVertexH != 0 )
            {
                vertex = ( CADVertex3DObject * ) this->GetObject (currentVertexH);

                if ( vertex == nullptr ) break;

                currentVertexH = vertex->stCed.hObjectHandle.GetAsLong ();
                polyline->vertexes.push_back ( vertex->vertPosition );
                if ( vertex->stCed.bNoLinks == true )
                {
                    ++currentVertexH;
                }
                else
                {
                    currentVertexH = vertex->stChed.hNextEntity.GetAsLong (vertex->stCed.hObjectHandle );
                }

                // Last vertex is reached. read it and break reading.
                if ( currentVertexH == cadPolyline3D->hVertexes[1].GetAsLong () )
                {
                    vertex = ( CADVertex3DObject * ) this->GetObject (currentVertexH);
                    polyline->vertexes.push_back ( vertex->vertPosition );
                    break;
                }
            }

            delete( cadPolyline3D );

            result_geometry = polyline;
            break;
        }

        case CADObject::LWPOLYLINE:
        {
            CADLWPolyline * lwPolyline = new CADLWPolyline();
            CADLWPolylineObject * cadlwPolyline = ( CADLWPolylineObject * ) readedObject;

            lwPolyline->dfConstWidth = cadlwPolyline->dfConstWidth;
            lwPolyline->dfElevation = cadlwPolyline->dfElevation;
            lwPolyline->vertexes = cadlwPolyline->avertVertexes;
            lwPolyline->vectExtrusion = cadlwPolyline->vectExtrusion;
            lwPolyline->bulges = cadlwPolyline->adfBulges;
            lwPolyline->widths = cadlwPolyline->astWidths;

            delete( cadlwPolyline );

            result_geometry = lwPolyline;
            break;
        }*/

        case CADObject::CIRCLE:
        {
            CADCircle * circle = new CADCircle();
            CADCircleObject * cadCircle = static_cast<CADCircleObject*>(
                        readedObject.get());

            circle->setPosition (cadCircle->vertPosition);
            circle->setExtrusion (cadCircle->vectExtrusion);
            circle->setRadius (cadCircle->dfRadius);
            circle->setThickness(cadCircle->dfThickness);

            return circle;
        }
/*
        case CADObject::ELLIPSE:
        {
            Ellipse * ellipse = new Ellipse();
            CADEllipseObject * cadEllipse = ( CADEllipseObject * ) readedObject;

            ellipse->vertPosition = cadEllipse->vertPosition;
            ellipse->vectExtrusion = cadEllipse->vectExtrusion;
            ellipse->vectWCS = cadEllipse->vectSMAxis; // FIXME: WCS for both or SMAxis?
            ellipse->dfAxisRatio = cadEllipse->dfAxisRatio;
            ellipse->dfEndingAngle = cadEllipse->dfEndAngle;
            ellipse->dfStartingAngle = cadEllipse->dfBegAngle;

            delete( cadEllipse );

            result_geometry = ellipse;
            break;
        }*/

        case CADObject::LINE:
        {
            CADLineObject * cadLine = static_cast<CADLineObject *>(
                        readedObject.get());

            CADPoint3D ptBeg(cadLine->vertStart, cadLine->dfThickness);
            CADPoint3D ptEnd(cadLine->vertEnd, cadLine->dfThickness);

            return new CADLine(ptBeg, ptEnd);
        }
/*
        case CADObject::RAY:
        {
            Ray * ray = new Ray();
            CADRayObject * cadRay = ( CADRayObject * ) readedObject;

            ray->vectVector = cadRay->vectVector;
            ray->vertPosition = cadRay->vertPosition;

            delete( cadRay );

            break;
        }

        case CADObject::SPLINE:
        {
            Spline * spline = new Spline();
            CADSplineObject * cadSpline = ( CADSplineObject * ) readedObject;

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

            delete( cadSpline );

            result_geometry = spline;
            break;
        }

        case CADObject::TEXT:
        {
            Text * text = new Text();
            CADTextObject * cadText = ( CADTextObject * ) readedObject;

            text->vertAlignment = cadText->vertAlignmentPoint;
            text->vertInsertion = cadText->vertInsetionPoint;
            text->strTextValue = cadText->sTextValue;
            text->dVerticalAlignment = cadText->dVertAlign;
            text->dHorizontalAlignment = cadText->dHorizAlign;
            text->dGeneration = cadText->dGeneration;
            text->dfRotationAngle = cadText->dfRotationAng;
            text->dfObliqueAngle = cadText->dfObliqueAng;
            text->setThickness(cadText->dfThickness);
            text->dfHeight = cadText->dfElevation;
            text->dfElevation = cadText->dfElevation;

            delete( cadText );

            result_geometry = text;
            break;
        }

        case CADObject::SOLID:
        {
            Solid * solid = new Solid();
            CADSolidObject * cadSolid = ( CADSolidObject * ) readedObject;

            solid->dfElevation = cadSolid->dfElevation;
            solid->setThickness(cadSolid->dfThickness);
            solid->avertCorners = cadSolid->avertCorners;
            solid->vectExtrusion = cadSolid->vectExtrusion;

            delete( cadSolid );

            result_geometry = solid;
            break;
        }
*/
        default:
        {
            cerr << "Asked geometry has unsupported type." << endl;
        }
    }

    return nullptr;
}

CADBlockObject *DWGFileR2000::getBlock(long dObjectSize,
                                       struct CADCommonED stCommonEntityData,
                                       const char * pabyInput,
                                       size_t& nBitOffsetFromStart)
{
    CADBlockObject * pBlock = new CADBlockObject();

    pBlock->dObjectSize = dObjectSize;
    pBlock->stCed = stCommonEntityData;

    pBlock->sBlockName = ReadTV (pabyInput, nBitOffsetFromStart);

    if (pBlock->stCed.bbEntMode == 0 )
        pBlock->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < pBlock->stCed.nNumReactors; ++i )
        pBlock->stChed.hReactors.push_back (ReadHANDLE (pabyInput,
                                                        nBitOffsetFromStart));

    pBlock->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !pBlock->stCed.bNoLinks )
    {
        pBlock->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        pBlock->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    pBlock->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( pBlock->stCed.bbLTypeFlags == 0x03 )
        pBlock->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( pBlock->stCed.bbPlotStyleFlags == 0x03 )
        pBlock->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    pBlock->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif // _DEBUG

    return pBlock;
}

CADEllipseObject *DWGFileR2000::getEllipse(long dObjectSize,
                                         CADCommonED stCommonEntityData,
                                         const char *pabyInput,
                                         size_t &nBitOffsetFromStart)
{
    CADEllipseObject * ellipse = new CADEllipseObject();

    ellipse->dObjectSize = dObjectSize;
    ellipse->stCed = stCommonEntityData;

    CADVector vertPosition(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));

    ellipse->vertPosition = vertPosition;

    CADVector vectSMAxis(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                         ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                         ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));

    ellipse->vectSMAxis = vectSMAxis;

    CADVector vectExtrusion(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                            ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                            ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));

    ellipse->vectExtrusion = vectExtrusion;

    ellipse->dfAxisRatio = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);
    ellipse->dfBegAngle = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);
    ellipse->dfEndAngle = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    if ( ellipse->stCed.bbEntMode == 0 )
        ellipse->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < ellipse->stCed.nNumReactors; ++i )
        ellipse->stChed.hReactors.push_back (ReadHANDLE (pabyInput,
                                                         nBitOffsetFromStart));

    ellipse->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !ellipse->stCed.bNoLinks )
    {
        ellipse->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        ellipse->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    ellipse->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( ellipse->stCed.bbLTypeFlags == 0x03 )
        ellipse->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( ellipse->stCed.bbPlotStyleFlags == 0x03 )
        ellipse->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    ellipse->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif // _DEBUG

    return ellipse;
}

CADSolidObject *DWGFileR2000::getSolid(long dObjectSize,
                                         CADCommonED stCommonEntityData,
                                         const char *pabyInput,
                                         size_t &nBitOffsetFromStart)
{
    CADSolidObject * solid = new CADSolidObject();

    solid->dObjectSize = dObjectSize;
    solid->stCed = stCommonEntityData;

    solid->dfThickness = ReadBIT (pabyInput, nBitOffsetFromStart) ?
                         0.0f : ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    solid->dfElevation = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    CADVector oCorner;
    for ( size_t i = 0; i < 4; ++i )
    {
        oCorner.setX(ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart));
        oCorner.setY(ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart));
        solid->avertCorners.push_back ( oCorner );
    }

    if ( ReadBIT (pabyInput, nBitOffsetFromStart) )
    {
        solid->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    }
    else
    {
        CADVector vectExtrusion(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
        solid->vectExtrusion = vectExtrusion;
    }

    if ( solid->stCed.bbEntMode == 0 )
        solid->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < solid->stCed.nNumReactors; ++i )
        solid->stChed.hReactors.push_back (ReadHANDLE (pabyInput,
                                                       nBitOffsetFromStart));

    solid->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !solid->stCed.bNoLinks )
    {
        solid->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        solid->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    solid->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( solid->stCed.bbLTypeFlags == 0x03 )
        solid->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( solid->stCed.bbPlotStyleFlags == 0x03 )
        solid->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    solid->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif //_DEBUG

    return solid;
}

CADPointObject *DWGFileR2000::getPoint(long dObjectSize,
                                       CADCommonED stCommonEntityData,
                                       const char *pabyInput,
                                       size_t &nBitOffsetFromStart)
{

    CADPointObject * point = new CADPointObject();

    point->dObjectSize = dObjectSize;
    point->stCed = stCommonEntityData;

    CADVector vertPosition(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));

    point->vertPosition = vertPosition;

    point->dfThickness = ReadBIT (pabyInput, nBitOffsetFromStart) ?
                          0.0f : ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    if ( ReadBIT (pabyInput, nBitOffsetFromStart) )
    {
        point->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    }
    else
    {
        CADVector vectExtrusion(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
        point->vectExtrusion = vectExtrusion;
    }

    point->dfXAxisAng = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    if ( point->stCed.bbEntMode == 0 )
        point->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( size_t i = 0; i < point->stCed.nNumReactors; ++i )
        point->stChed.hReactors.push_back (ReadHANDLE (pabyInput,
                                                       nBitOffsetFromStart));

    point->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !point->stCed.bNoLinks )
    {
        point->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        point->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    point->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( point->stCed.bbLTypeFlags == 0x03 )
        point->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( point->stCed.bbPlotStyleFlags == 0x03 )
        point->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    point->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif // _DEBUG

    return point;
}

CADPolyline3DObject *DWGFileR2000::getPolyLine3D(long dObjectSize,
                                           CADCommonED stCommonEntityData,
                                           const char *pabyInput,
                                           size_t &nBitOffsetFromStart)
{
    CADPolyline3DObject * polyline = new CADPolyline3DObject();

    polyline->dObjectSize = dObjectSize;
    polyline->stCed = stCommonEntityData;

    polyline->SplinedFlags = ReadCHAR (pabyInput, nBitOffsetFromStart);
    polyline->ClosedFlags = ReadCHAR (pabyInput, nBitOffsetFromStart);

    if ( polyline->stCed.bbEntMode == 0 )
        polyline->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < polyline->stCed.nNumReactors; ++i )
        polyline->stChed.hReactors.push_back (ReadHANDLE (pabyInput,
                                                          nBitOffsetFromStart));

    polyline->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !polyline->stCed.bNoLinks )
    {
        polyline->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        polyline->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    polyline->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( polyline->stCed.bbLTypeFlags == 0x03 )
        polyline->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( polyline->stCed.bbPlotStyleFlags == 0x03 )
        polyline->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    polyline->hVertexes.push_back ( ReadHANDLE (pabyInput, nBitOffsetFromStart) ); // 1st vertex
    polyline->hVertexes.push_back ( ReadHANDLE (pabyInput, nBitOffsetFromStart) ); // last vertex

    polyline->hSeqend = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
    polyline->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif // _DEBUG

    return polyline;
}

CADRayObject *DWGFileR2000::getRay(long dObjectSize,
                                   CADCommonED stCommonEntityData,
                                   const char *pabyInput,
                                   size_t &nBitOffsetFromStart)
{
    CADRayObject * ray = new CADRayObject();

    ray->dObjectSize = dObjectSize;
    ray->stCed = stCommonEntityData;

    CADVector vertPosition(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));

    ray->vertPosition = vertPosition;

    CADVector vectVector(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                         ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                         ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
    ray->vectVector = vectVector;

    if ( ray->stCed.bbEntMode == 0 )
        ray->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < ray->stCed.nNumReactors; ++i )
        ray->stChed.hReactors.push_back (ReadHANDLE (pabyInput, nBitOffsetFromStart));

    ray->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !ray->stCed.bNoLinks )
    {
        ray->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        ray->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    ray->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( ray->stCed.bbLTypeFlags == 0x03 )
        ray->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( ray->stCed.bbPlotStyleFlags == 0x03 )
        ray->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
    ray->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif //_DEBUG

    return ray;
}

CADXLineObject *DWGFileR2000::getXLine(long dObjectSize,
                                       CADCommonED stCommonEntityData,
                                       const char *pabyInput,
                                       size_t &nBitOffsetFromStart)
{
    CADXLineObject * xline = new CADXLineObject();

    xline->dObjectSize = dObjectSize;
    xline->stCed = stCommonEntityData;

    CADVector vertPosition(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));

    xline->vertPosition = vertPosition;

    CADVector vectVector(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                         ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                         ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
    xline->vectVector = vectVector;

    if ( xline->stCed.bbEntMode == 0 )
        xline->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < xline->stCed.nNumReactors; ++i )
        xline->stChed.hReactors.push_back (ReadHANDLE (pabyInput, nBitOffsetFromStart));

    xline->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !xline->stCed.bNoLinks )
    {
        xline->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        xline->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    xline->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( xline->stCed.bbLTypeFlags == 0x03 )
        xline->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( xline->stCed.bbPlotStyleFlags == 0x03 )
        xline->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
    xline->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif // _DEBUG

    return xline;
}

CADLineObject *DWGFileR2000::getLine(long dObjectSize,
                                     CADCommonED stCommonEntityData,
                                     const char *pabyInput,
                                     size_t &nBitOffsetFromStart)
{
    CADLineObject * line = new CADLineObject();

    line->dObjectSize = dObjectSize;
    line->stCed = stCommonEntityData;

    bool bZsAreZeros = ReadBIT (pabyInput, nBitOffsetFromStart);

    CADVector vertStart, vertEnd;
    vertStart.setX (ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart));
    vertEnd.setX (ReadBITDOUBLEWD (pabyInput, nBitOffsetFromStart,
                                   vertStart.getX()));
    vertStart.setY (ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart));
    vertEnd.setY (ReadBITDOUBLEWD (pabyInput, nBitOffsetFromStart,
                                   vertStart.getY()));

    if ( !bZsAreZeros )
    {
        vertStart.setZ(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
        vertEnd.setZ(ReadBITDOUBLEWD (pabyInput, nBitOffsetFromStart,
                                      vertStart.getZ()));
    }

    line->vertStart = vertStart;
    line->vertEnd   = vertEnd;

    line->dfThickness = ReadBIT (pabyInput, nBitOffsetFromStart) ?
                        0.0f : ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    if ( ReadBIT (pabyInput, nBitOffsetFromStart) )
    {
        line->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    }
    else
    {
        CADVector vectExtrusion(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
        line->vectExtrusion = vectExtrusion;
    }

    if ( line->stCed.bbEntMode == 0 )
        line->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < line->stCed.nNumReactors; ++i )
        line->stChed.hReactors.push_back ( ReadHANDLE (pabyInput, nBitOffsetFromStart) );

    line->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !line->stCed.bNoLinks )
    {
        line->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        line->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    line->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( line->stCed.bbLTypeFlags == 0x03 )
    {
        line->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    if ( line->stCed.bbPlotStyleFlags == 0x03 )
    {
        line->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
    line->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif // _DEBUG
    return line;
}

CADTextObject *DWGFileR2000::getText(long dObjectSize,
                                     CADCommonED stCommonEntityData,
                                     const char *pabyInput,
                                     size_t &nBitOffsetFromStart)
{
    CADTextObject * text = new CADTextObject();

    text->dObjectSize = dObjectSize;
    text->stCed = stCommonEntityData;

    text->DataFlags = ReadCHAR (pabyInput, nBitOffsetFromStart);

    if ( !( text->DataFlags & 0x01 ) )
        text->dfElevation = ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart);

    CADVector vertInsetionPoint(ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart));

    text->vertInsetionPoint = vertInsetionPoint;

    if ( !( text->DataFlags & 0x02 ) )
    {
        CADVector vertAlignmentPoint(ReadBITDOUBLEWD (pabyInput,
                                nBitOffsetFromStart, vertInsetionPoint.getX()),
                                     ReadBITDOUBLEWD (pabyInput,
                                nBitOffsetFromStart, vertInsetionPoint.getY()));
        text->vertAlignmentPoint = vertAlignmentPoint;
    }

    if ( ReadBIT (pabyInput, nBitOffsetFromStart) )
    {
        text->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    }
    else
    {
        CADVector vectExtrusion(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
        text->vectExtrusion = vectExtrusion;
    }

    text->dfThickness = ReadBIT (pabyInput, nBitOffsetFromStart) ?
                          0.0f : ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    if ( !( text->DataFlags & 0x04 ) )
        text->dfObliqueAng = ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart);
    if ( !( text->DataFlags & 0x08 ) )
        text->dfRotationAng = ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart);

    text->dfHeight = ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart);

    if ( !( text->DataFlags & 0x10 ) )
        text->dfWidthFactor = ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart);

    text->sTextValue = ReadTV (pabyInput, nBitOffsetFromStart);

    if ( !( text->DataFlags & 0x20 ) )
        text->dGeneration = ReadBITSHORT (pabyInput, nBitOffsetFromStart);
    if ( !( text->DataFlags & 0x40 ) )
        text->dHorizAlign = ReadBITSHORT (pabyInput, nBitOffsetFromStart);
    if ( !( text->DataFlags & 0x80 ) )
        text->dVertAlign = ReadBITSHORT (pabyInput, nBitOffsetFromStart);

    if ( text->stCed.bbEntMode == 0 )
        text->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < text->stCed.nNumReactors; ++i )
        text->stChed.hReactors.push_back (ReadHANDLE (pabyInput, nBitOffsetFromStart));

    text->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !text->stCed.bNoLinks )
    {
        text->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        text->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    text->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( text->stCed.bbLTypeFlags == 0x03 )
        text->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( text->stCed.bbPlotStyleFlags == 0x03 )
        text->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    text->hStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
    text->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif // _DEBUG

    return text;
}

CADVertex3DObject *DWGFileR2000::getVertex3D(long dObjectSize,
                                             CADCommonED stCommonEntityData,
                                             const char *pabyInput,
                                             size_t &nBitOffsetFromStart)
{
    CADVertex3DObject * vertex = new CADVertex3DObject();

    vertex->dObjectSize = dObjectSize;
    vertex->stCed = stCommonEntityData;

    /*unsigned char Flags = */ReadCHAR (pabyInput, nBitOffsetFromStart);

    CADVector vertPosition(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));

    vertex->vertPosition = vertPosition;

    if ( vertex->stCed.bbEntMode == 0 )
        vertex->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < vertex->stCed.nNumReactors; ++i )
        vertex->stChed.hReactors.push_back (ReadHANDLE (pabyInput,
                                                        nBitOffsetFromStart));

    vertex->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !vertex->stCed.bNoLinks )
    {
        vertex->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        vertex->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    vertex->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( vertex->stCed.bbLTypeFlags == 0x03 )
        vertex->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( vertex->stCed.bbPlotStyleFlags == 0x03 )
        vertex->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
    vertex->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif // _DEBUG
    return vertex;
}

CADCircleObject *DWGFileR2000::getCircle(long dObjectSize,
                                         CADCommonED stCommonEntityData,
                                         const char *pabyInput,
                                         size_t &nBitOffsetFromStart)
{
    CADCircleObject * circle = new CADCircleObject();

    circle->dObjectSize = dObjectSize;
    circle->stCed = stCommonEntityData;

    CADVector vertPosition(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
    circle->vertPosition = vertPosition;
    circle->dfRadius    = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);
    circle->dfThickness = ReadBIT (pabyInput, nBitOffsetFromStart) ?
                          0.0f : ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    if ( ReadBIT (pabyInput, nBitOffsetFromStart) )
    {
        circle->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    }
    else
    {
        CADVector vectExtrusion(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
        circle->vectExtrusion = vectExtrusion;
    }

    if ( circle->stCed.bbEntMode == 0 )
        circle->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < circle->stCed.nNumReactors; ++i )
        circle->stChed.hReactors.push_back ( ReadHANDLE (pabyInput, nBitOffsetFromStart) );

    circle->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !circle->stCed.bNoLinks )
    {
        circle->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        circle->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    circle->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( circle->stCed.bbLTypeFlags == 0x03 )
    {
        circle->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    if ( circle->stCed.bbPlotStyleFlags == 0x03 )
    {
        circle->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
    circle->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif // _DEBUG
    return circle;
}

CADEndblkObject *DWGFileR2000::getEndBlock(long dObjectSize,
                                           CADCommonED stCommonEntityData,
                                           const char *pabyInput,
                                           size_t &nBitOffsetFromStart)
{
    CADEndblkObject * endblk = new CADEndblkObject();

    endblk->dObjectSize = dObjectSize;
    endblk->stCed = stCommonEntityData;

    if ( endblk->stCed.bbEntMode == 0 )
        endblk->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < endblk->stCed.nNumReactors; ++i )
        endblk->stChed.hReactors.push_back (ReadHANDLE (pabyInput, nBitOffsetFromStart));

    endblk->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !endblk->stCed.bNoLinks )
    {
        endblk->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        endblk->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    endblk->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( endblk->stCed.bbLTypeFlags == 0x03 )
        endblk->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( endblk->stCed.bbPlotStyleFlags == 0x03 )
        endblk->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    endblk->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));

    return endblk;
}

CADPolyline2DObject *DWGFileR2000::getPolyline2D(long dObjectSize,
                                                 CADCommonED stCommonEntityData,
                                                 const char *pabyInput,
                                                 size_t &nBitOffsetFromStart)
{
    CADPolyline2DObject * polyline = new CADPolyline2DObject();

    polyline->dObjectSize = dObjectSize;
    polyline->stCed = stCommonEntityData;

    polyline->dFlags = ReadBITSHORT (pabyInput, nBitOffsetFromStart);
    polyline->dCurveNSmoothSurfType = ReadBITSHORT (pabyInput, nBitOffsetFromStart);

    polyline->dfStartWidth = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);
    polyline->dfEndWidth = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    polyline->dfThickness = ReadBIT (pabyInput, nBitOffsetFromStart) ?
                            0.0f : ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    polyline->dfElevation = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    if ( ReadBIT (pabyInput, nBitOffsetFromStart) )
    {
        polyline->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    }
    else
    {
        CADVector vectExtrusion(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
        polyline->vectExtrusion = vectExtrusion;
    }

    if ( polyline->stCed.bbEntMode == 0 )
        polyline->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < polyline->stCed.nNumReactors; ++i )
        polyline->stChed.hReactors.push_back (ReadHANDLE (pabyInput, nBitOffsetFromStart));

    polyline->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !polyline->stCed.bNoLinks )
    {
        polyline->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        polyline->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    polyline->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( polyline->stCed.bbLTypeFlags == 0x03 )
        polyline->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( polyline->stCed.bbPlotStyleFlags == 0x03 )
        polyline->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    polyline->hVertexes.push_back ( ReadHANDLE (pabyInput, nBitOffsetFromStart) ); // 1st vertex
    polyline->hVertexes.push_back ( ReadHANDLE (pabyInput, nBitOffsetFromStart) ); // last vertex

    polyline->hSeqend = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 ); // padding bits to next byte boundary
    polyline->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif // _DEBUG
    return polyline;
}

CADAttribObject *DWGFileR2000::getAttributes(long dObjectSize,
                                            CADCommonED stCommonEntityData,
                                            const char *pabyInput,
                                            size_t &nBitOffsetFromStart)
{
    CADAttribObject * attrib = new CADAttribObject();

/*    attrib->stCed = stCommonEntityData;
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

    if (attrib->stCed.bbEntMode == 0 )
        attrib->stChed.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    for ( size_t i = 0; i < attrib->stCed.nNumReactors; ++i )
        attrib->stChed.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

    attrib->stChed.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    if ( !attrib->stCed.bNoLinks )
    {
        attrib->stChed.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
        attrib->stChed.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
    }

    attrib->stChed.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    if ( attrib->stCed.bbLTypeFlags == 0x03 )
        attrib->stChed.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
    if ( attrib->stCed.bbPlotStyleFlags == 0x03 )
        attrib->stChed.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    attrib->hStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    attrib->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
*/
    return attrib;
}

CADAttdefObject *DWGFileR2000::getAttributesDefn(long dObjectSize,
                                                 CADCommonED stCommonEntityData,
                                                 const char *pabyInput,
                                                 size_t &nBitOffsetFromStart)
{
    CADAttdefObject * attdef = new CADAttdefObject();
/*
    attdef->stCed = stCommonEntityData;
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

    if (attdef->stCed.bbEntMode == 0 )
        attdef->stChed.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    for ( size_t i = 0; i < attdef->stCed.nNumReactors; ++i )
        attdef->stChed.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

    attdef->stChed.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    if ( !attdef->stCed.bNoLinks )
    {
        attdef->stChed.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
        attdef->stChed.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
    }

    attdef->stChed.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    if ( attdef->stCed.bbLTypeFlags == 0x03 )
        attdef->stChed.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
    if ( attdef->stCed.bbPlotStyleFlags == 0x03 )
        attdef->stChed.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    attdef->hStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    attdef->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
*/
    return  attdef;
}

CADLWPolylineObject *DWGFileR2000::getLWPolyLine(long dObjectSize,
                                                 CADCommonED stCommonEntityData,
                                                 const char *pabyInput,
                                                 size_t &nBitOffsetFromStart)
{
    CADLWPolylineObject * polyline   = new CADLWPolylineObject ();
/*
    polyline->dObjectSize = dObjectSize;
    polyline->stCed = stCommonEntityData;

    int32_t    nVertixesCount  = 0, nBulges = 0, nNumWidths = 0;
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

    // TODO: tell ODA that R2000 contains nNumWidths flag
    if ( data_flag & 32 )
    {
        nNumWidths = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);
    }

    // First of all, read first vertex.
    Vertex2D vertex;
    vertex.X = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
    vertex.Y = ReadRAWDOUBLE (pabySectionContent, nBitOffsetFromStart);
    polyline->avertVertexes.push_back (vertex);

    // All the others are not raw doubles; bitdoubles with default instead,
    // where default is previous point coords.
    for ( size_t i = 1; i < nVertixesCount; ++i )
    {
        vertex.X = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart,
                                    polyline->avertVertexes[i - 1].X);
        vertex.Y = ReadBITDOUBLEWD (pabySectionContent, nBitOffsetFromStart,
                                    polyline->avertVertexes[i - 1].Y);
        polyline->avertVertexes.push_back (vertex);
    }

    for ( size_t i = 0; i < nBulges; ++i )
    {
        double dfBulgeValue = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
        polyline->adfBulges.push_back (dfBulgeValue);
    }

    for ( size_t i = 0; i < nNumWidths; ++i )
    {
        double dfStartWidth = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
        double dfEndWidth = ReadBITDOUBLE (pabySectionContent, nBitOffsetFromStart);
        polyline->astWidths.push_back ( std::make_pair ( dfStartWidth, dfEndWidth ) );
    }

    if (polyline->stCed.bbEntMode == 0 )
        polyline->stChed.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    for ( size_t i = 0; i < polyline->stCed.nNumReactors; ++i )
        polyline->stChed.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

    polyline->stChed.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    if ( !polyline->stCed.bNoLinks )
    {
        polyline->stChed.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
        polyline->stChed.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
    }

    polyline->stChed.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    if ( polyline->stCed.bbLTypeFlags == 0x03 )
    {
        polyline->stChed.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
    }

    if ( polyline->stCed.bbPlotStyleFlags == 0x03 )
    {
        polyline->stChed.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
    }

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    int16_t crc = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, ( nBitOffsetFromStart / 8 - dObjectSize - 4 ));
*/
    return polyline;
}

CADArcObject *DWGFileR2000::getArc(long dObjectSize,
                                   CADCommonED stCommonEntityData,
                                   const char *pabyInput,
                                   size_t &nBitOffsetFromStart)
{
    CADArcObject * arc = new CADArcObject();

    arc->stCed = stCommonEntityData;

    CADVector vertPosition(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                           ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
    arc->vertPosition    = vertPosition;
    arc->dfRadius        = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);
    arc->dfThickness     = ReadBIT (pabyInput, nBitOffsetFromStart) ?
                           0.0f : ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    if ( ReadBIT (pabyInput, nBitOffsetFromStart) )
    {
        arc->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    }
    else
    {
        CADVector vectExtrusion(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                                ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
        arc->vectExtrusion = vectExtrusion;
    }

    arc->dfStartAngle = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);
    arc->dfEndAngle   = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);

    if (arc->stCed.bbEntMode == 0 )
        arc->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < arc->stCed.nNumReactors; ++i )
        arc->stChed.hReactors.push_back (ReadHANDLE (pabyInput, nBitOffsetFromStart));

    arc->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !arc->stCed.bNoLinks )
    {
        arc->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        arc->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    arc->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( arc->stCed.bbLTypeFlags == 0x03 )
    {
        arc->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    if ( arc->stCed.bbPlotStyleFlags == 0x03 )
    {
        arc->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    arc->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);
#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif // _DEBUG
    return arc;
}

CADSplineObject *DWGFileR2000::getSpline(long dObjectSize,
                                         CADCommonED stCommonEntityData,
                                         const char *pabyInput,
                                         size_t &nBitOffsetFromStart)
{
    CADSplineObject * spline = new CADSplineObject();

    spline->dObjectSize = dObjectSize;
    spline->stCed = stCommonEntityData;
/*
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

    if (spline->stCed.bbEntMode == 0 )
        spline->stChed.hOwner = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    for ( size_t i = 0; i < spline->stCed.nNumReactors; ++i )
        spline->stChed.hReactors.push_back (ReadHANDLE (pabySectionContent, nBitOffsetFromStart));

    spline->stChed.hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    if ( !spline->stCed.bNoLinks )
    {
        spline->stChed.hPrevEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
        spline->stChed.hNextEntity = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
    }

    spline->stChed.hLayer = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    if ( spline->stCed.bbLTypeFlags == 0x03 )
    {
        spline->stChed.hLType = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
    }

    if ( spline->stCed.bbPlotStyleFlags == 0x03 )
    {
        spline->stChed.hPlotStyle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
    }

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    spline->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
*/
    return spline;
}

CADEntityObject *DWGFileR2000::getEntity(int dObjectType,
                                         long dObjectSize,
                                         CADCommonED stCommonEntityData,
                                         const char *pabyInput,
                                         size_t &nBitOffsetFromStart)
{
    CADEntityObject * entity = new CADEntityObject();

    entity->eObjectType = static_cast<CADObject::ObjectType>(dObjectType);
    entity->dObjectSize = dObjectSize;
    entity->stCed = stCommonEntityData;

    nBitOffsetFromStart = static_cast<size_t>(
                entity->stCed.nObjectSizeInBits + 16);

    if (entity->stCed.bbEntMode == 0 )
        entity->stChed.hOwner = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < entity->stCed.nNumReactors; ++i )
        entity->stChed.hReactors.push_back (ReadHANDLE (pabyInput, nBitOffsetFromStart));

    entity->stChed.hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( !entity->stCed.bNoLinks )
    {
        entity->stChed.hPrevEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
        entity->stChed.hNextEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    entity->stChed.hLayer = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    if ( entity->stCed.bbLTypeFlags == 0x03 )
    {
        entity->stChed.hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    if ( entity->stCed.bbPlotStyleFlags == 0x03 )
    {
        entity->stChed.hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    }

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    entity->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);
#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif //_DEBUG
    return entity;
}

CADDictionaryObject *DWGFileR2000::getDictionary(long dObjectSize,
                                                 const char *pabyInput,
                                                 size_t &nBitOffsetFromStart)
{
    /*
     * FIXME: ODA has a lot of mistypes in spec. for this objects,
     * it doesnt work for now (error begins in handles stream).
     * Nonetheless, dictionary->sItemNames is 100% array,
     * not a single obj as pointer by their docs.
     */
    CADDictionaryObject * dictionary = new CADDictionaryObject();

    dictionary->dObjectSize = dObjectSize;
    dictionary->nObjectSizeInBits = ReadRAWLONG (pabyInput, nBitOffsetFromStart);
    dictionary->hObjectHandle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    short dEEDSize = 0;
    CADEed dwgEed;
    while ( (dEEDSize = ReadBITSHORT (pabyInput, nBitOffsetFromStart)) != 0 )
    {
        dwgEed.dLength = dEEDSize;
        dwgEed.hApplication = ReadHANDLE (pabyInput, nBitOffsetFromStart);

        for ( short i = 0; i < dEEDSize; ++i )
        {
            dwgEed.acData.push_back(ReadCHAR (pabyInput, nBitOffsetFromStart));
        }

        dictionary->aEED.push_back (dwgEed);
    }

    dictionary->nNumReactors = ReadBITSHORT (pabyInput, nBitOffsetFromStart);
    dictionary->nNumItems = ReadBITLONG (pabyInput, nBitOffsetFromStart);
    dictionary->dCloningFlag = ReadBITSHORT (pabyInput, nBitOffsetFromStart);
    dictionary->dHardOwnerFlag = ReadCHAR (pabyInput, nBitOffsetFromStart);

    dictionary->sDictionaryEntryName = ReadTV (pabyInput, nBitOffsetFromStart);
    for ( long i = 0; i < dictionary->nNumItems; ++i )
        dictionary->sItemNames.push_back ( ReadTV (pabyInput,
                                                   nBitOffsetFromStart) );

    dictionary->hParentHandle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < dictionary->nNumReactors; ++i )
        dictionary->hReactors.push_back (ReadHANDLE (pabyInput,
                                                     nBitOffsetFromStart) );
    dictionary->hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    for ( long i = 0; i < dictionary->nNumItems; ++i )
        dictionary->hItemHandles.push_back ( ReadHANDLE (pabyInput,
                                                         nBitOffsetFromStart) );

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    dictionary->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif // _DEBUG
    return dictionary;
}

CADLayerObject *DWGFileR2000::getLayer(long dObjectSize,
                                       const char *pabyInput,
                                       size_t &nBitOffsetFromStart)
{
    CADLayerObject * layer = new CADLayerObject();

    layer->dObjectSize = dObjectSize;
    layer->nObjectSizeInBits = ReadRAWLONG (pabyInput, nBitOffsetFromStart);
    layer->hObjectHandle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    short dEEDSize = 0;
    CADEed dwgEed;
    while ( (dEEDSize = ReadBITSHORT (pabyInput, nBitOffsetFromStart)) != 0 )
    {
        dwgEed.dLength = dEEDSize;
        dwgEed.hApplication = ReadHANDLE (pabyInput, nBitOffsetFromStart);

        for ( short i = 0; i < dEEDSize; ++i )
        {
            dwgEed.acData.push_back(ReadCHAR (pabyInput, nBitOffsetFromStart));
        }

        layer->aEED.push_back (dwgEed);
    }

    layer->nNumReactors = ReadBITLONG (pabyInput, nBitOffsetFromStart);
    layer->sLayerName = ReadTV (pabyInput, nBitOffsetFromStart);
    layer->b64Flag = ReadBIT (pabyInput, nBitOffsetFromStart);
    layer->dXRefIndex = ReadBITSHORT (pabyInput, nBitOffsetFromStart);
    layer->bXDep = ReadBIT (pabyInput, nBitOffsetFromStart);

    short dFlags = ReadBITSHORT (pabyInput, nBitOffsetFromStart);
    layer->bFrozen = dFlags & 0x01;
    layer->bOn     = dFlags & 0x02;
    layer->bFrozenInNewVPORT = dFlags & 0x04;
    layer->bLocked = dFlags & 0x08;
    layer->bPlottingFlag = dFlags & 0x10;
    layer->dLineWeight = dFlags & 0x03E0;
    layer->dCMColor = ReadBITSHORT (pabyInput, nBitOffsetFromStart);
    layer->hLayerControl = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    for ( long i = 0; i < layer->nNumReactors; ++i )
        layer->hReactors.push_back (ReadHANDLE (pabyInput, nBitOffsetFromStart) );
    layer->hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    layer->hExternalRefBlockHandle = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    layer->hPlotStyle = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    layer->hLType = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    /*
     * FIXME: ODA says that this handle should be null hard pointer. It is not.
     * Also, after reading it dObjectSize is != actual readed structure's size.
     * Not used anyway, so no point to read it for now.
     * It also means that CRC cannot be computed correctly.
     */
//                layer->hUnknownHandle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    layer->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);

#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif //_DEBUG
    return layer;
}

CADLayerControlObject *DWGFileR2000::getLayerControl(long dObjectSize,
                                                     const char *pabyInput,
                                                     size_t &nBitOffsetFromStart)
{
    CADLayerControlObject * layerControl = new CADLayerControlObject();

    layerControl->dObjectSize = dObjectSize;
    layerControl->nObjectSizeInBits = ReadRAWLONG (pabyInput, nBitOffsetFromStart);
    layerControl->hObjectHandle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    short dEEDSize = 0;
    CADEed dwgEed;
    while ( (dEEDSize = ReadBITSHORT (pabyInput, nBitOffsetFromStart)) != 0 )
    {

        dwgEed.dLength = dEEDSize;
        dwgEed.hApplication = ReadHANDLE (pabyInput, nBitOffsetFromStart);

        for ( short i = 0; i < dEEDSize; ++i )
        {
            dwgEed.acData.push_back(ReadCHAR (pabyInput, nBitOffsetFromStart));
        }

        layerControl->aEED.push_back (dwgEed);
    }

    layerControl->nNumReactors = ReadBITLONG (pabyInput, nBitOffsetFromStart);
    layerControl->nNumEntries = ReadBITLONG (pabyInput, nBitOffsetFromStart);
    layerControl->hNull = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    layerControl->hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    for ( long i = 0; i < layerControl->nNumEntries; ++i )
        layerControl->hLayers.push_back( ReadHANDLE (pabyInput, nBitOffsetFromStart) );

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    layerControl->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);
#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif
    return layerControl;
}

CADBlockControlObject *DWGFileR2000::getBlockControl(long dObjectSize,
                                                     const char *pabyInput,
                                                     size_t &nBitOffsetFromStart)
{
    CADBlockControlObject * blockControl = new CADBlockControlObject();

    blockControl->dObjectSize = dObjectSize;
    blockControl->nObjectSizeInBits = ReadRAWLONG (pabyInput, nBitOffsetFromStart);
    blockControl->hObjectHandle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    short dEEDSize = 0;
    CADEed dwgEed;
    while ( (dEEDSize = ReadBITSHORT (pabyInput, nBitOffsetFromStart)) != 0 )
    {
        dwgEed.dLength = dEEDSize;
        dwgEed.hApplication = ReadHANDLE (pabyInput, nBitOffsetFromStart);

        for ( short i = 0; i < dEEDSize; ++i )
        {
            dwgEed.acData.push_back(ReadCHAR (pabyInput, nBitOffsetFromStart));
        }

        blockControl->aEED.push_back (dwgEed);
    }

    blockControl->nNumReactors = ReadBITLONG (pabyInput, nBitOffsetFromStart);
    blockControl->nNumEntries = ReadBITLONG (pabyInput, nBitOffsetFromStart);

    blockControl->hNull = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    blockControl->hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < blockControl->nNumEntries + 2; ++i )
    {
        blockControl->hBlocks.push_back ( ReadHANDLE (pabyInput, nBitOffsetFromStart) );
    }

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    blockControl->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);
#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif //_DEBUG
    return blockControl;

}

CADBlockHeaderObject *DWGFileR2000::getBlockHeader(long dObjectSize,
                                                   const char *pabyInput,
                                                   size_t &nBitOffsetFromStart)
{
    CADBlockHeaderObject * blockHeader = new CADBlockHeaderObject();

    blockHeader->dObjectSize = dObjectSize;
    blockHeader->nObjectSizeInBits = ReadRAWLONG (pabyInput, nBitOffsetFromStart);
    blockHeader->hObjectHandle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    short dEEDSize;
    CADEed dwgEed;
    while ( (dEEDSize = ReadBITSHORT (pabyInput, nBitOffsetFromStart)) != 0 )
    {
        dwgEed.dLength = dEEDSize;
        dwgEed.hApplication = ReadHANDLE (pabyInput, nBitOffsetFromStart);

        for ( short i = 0; i < dEEDSize; ++i )
        {
            dwgEed.acData.push_back(ReadCHAR (pabyInput, nBitOffsetFromStart));
        }

        blockHeader->aEED.push_back (dwgEed);
    }

    blockHeader->nNumReactors = ReadBITLONG (pabyInput, nBitOffsetFromStart);
    blockHeader->sEntryName = ReadTV (pabyInput, nBitOffsetFromStart);
    blockHeader->b64Flag = ReadBIT (pabyInput, nBitOffsetFromStart);
    blockHeader->dXRefIndex = ReadBITSHORT (pabyInput, nBitOffsetFromStart);
    blockHeader->bXDep = ReadBIT (pabyInput, nBitOffsetFromStart);
    blockHeader->bAnonymous = ReadBIT (pabyInput, nBitOffsetFromStart);
    blockHeader->bHasAtts = ReadBIT (pabyInput, nBitOffsetFromStart);
    blockHeader->bBlkisXRef = ReadBIT (pabyInput, nBitOffsetFromStart);
    blockHeader->bXRefOverlaid = ReadBIT (pabyInput, nBitOffsetFromStart);
    blockHeader->bLoadedBit = ReadBIT (pabyInput, nBitOffsetFromStart);

    CADVector vertBasePoint(ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                            ReadBITDOUBLE (pabyInput, nBitOffsetFromStart),
                            ReadBITDOUBLE (pabyInput, nBitOffsetFromStart));
    blockHeader->vertBasePoint = vertBasePoint;
    blockHeader->sXRefPName = ReadTV (pabyInput, nBitOffsetFromStart);
    unsigned char Tmp;
    do
    {
        Tmp = ReadCHAR (pabyInput, nBitOffsetFromStart );
        blockHeader->adInsertCount.push_back(Tmp);
    } while ( Tmp != 0 );

    blockHeader->sBlockDescription = ReadTV (pabyInput, nBitOffsetFromStart);
    blockHeader->nSizeOfPreviewData = ReadBITLONG (pabyInput, nBitOffsetFromStart);
    for ( long i = 0; i < blockHeader->nSizeOfPreviewData; ++i )
        blockHeader->abyBinaryPreviewData.push_back ( ReadCHAR (pabyInput,
                                                        nBitOffsetFromStart) );

    blockHeader->hBlockControl = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    for ( long i = 0; i < blockHeader->nNumReactors; ++i )
        blockHeader->hReactors.push_back ( ReadHANDLE (pabyInput, nBitOffsetFromStart) );
    blockHeader->hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    blockHeader->hNull = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    blockHeader->hBlockEntity = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    if ( !blockHeader->bBlkisXRef && !blockHeader->bXRefOverlaid )
    {
        blockHeader->hEntities.push_back ( ReadHANDLE(pabyInput, nBitOffsetFromStart) ); // first
        blockHeader->hEntities.push_back ( ReadHANDLE(pabyInput, nBitOffsetFromStart) ); // last
    }

    blockHeader->hEndBlk = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    for ( size_t i = 0; i < blockHeader->adInsertCount.size() - 1; ++i )
        blockHeader->hInsertHandles.push_back ( ReadHANDLE (pabyInput, nBitOffsetFromStart) );
    blockHeader->hLayout = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    blockHeader->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);
#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n",
                  __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif //_DEBUG
    return blockHeader;
}

CADLineTypeControlObject *DWGFileR2000::getLineTypeControl(long dObjectSize,
                                                           const char *pabyInput,
                                                           size_t &nBitOffsetFromStart)
{
    CADLineTypeControlObject * ltype_control = new CADLineTypeControlObject();
/*
    ltype_control->dObjectSize = dObjectSize;
    ltype_control->nObjectSizeInBits = ReadRAWLONG (pabySectionContent, nBitOffsetFromStart);
    ltype_control->hObjectHandle = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

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

        ltype_control->aEED.push_back (dwg_eed);
    }

    ltype_control->nNumReactors = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);
    ltype_control->nNumEntries = ReadBITLONG (pabySectionContent, nBitOffsetFromStart);

    ltype_control->hNull = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);
    ltype_control->hXDictionary = ReadHANDLE (pabySectionContent, nBitOffsetFromStart);

    // hLTypes ends with BYLAYER and BYBLOCK
    for ( size_t i = 0; i < ltype_control->nNumEntries + 2; ++i )
        ltype_control->hLTypes.push_back( ReadHANDLE (pabySectionContent, nBitOffsetFromStart) );

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    ltype_control->dCRC = ReadRAWSHORT (pabySectionContent, nBitOffsetFromStart);

    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("Assertion failed at %d in %s\nSize difference: %d\n"
                , __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
*/
    return ltype_control;
}

CADLineTypeObject *DWGFileR2000::getLineType1(long dObjectSize,
                                              const char *pabyInput,
                                              size_t &nBitOffsetFromStart)
{
    CADLineTypeObject * ltype = new CADLineTypeObject();

    ltype->dObjectSize = dObjectSize;
    ltype->nObjectSizeInBits = ReadRAWLONG (pabyInput, nBitOffsetFromStart);
    ltype->hObjectHandle = ReadHANDLE (pabyInput, nBitOffsetFromStart);
/*
    int16_t dEEDSize = 0;
    while ( (dEEDSize = ReadBITSHORT (pabyInput, nBitOffsetFromStart)) != 0 )
    {
        CAD_EED dwg_eed;
        dwg_eed.length = dEEDSize;
        dwg_eed.application_handle = ReadHANDLE (pabyInput, nBitOffsetFromStart);

        for ( size_t i = 0; i < dEEDSize; ++i )
        {
            dwg_eed.data.push_back(ReadCHAR (pabyInput, nBitOffsetFromStart));
        }

        ltype->aEED.push_back (dwg_eed);
    }

    ltype->nNumReactors = ReadBITLONG (pabyInput, nBitOffsetFromStart);
    ltype->sEntryName = ReadTV (pabyInput, nBitOffsetFromStart);
    ltype->b64Flag = ReadBIT (pabyInput, nBitOffsetFromStart);
    ltype->dXRefIndex = ReadBITSHORT (pabyInput, nBitOffsetFromStart);
    ltype->bXDep = ReadBIT (pabyInput, nBitOffsetFromStart);
    ltype->sDescription = ReadTV (pabyInput, nBitOffsetFromStart);
    ltype->dfPatternLen = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);
    ltype->dAlignment = ReadCHAR (pabyInput, nBitOffsetFromStart);
    ltype->nNumDashes = ReadCHAR (pabyInput, nBitOffsetFromStart);

    CADLineTypeObject::Dash dash;
    for ( size_t i = 0; i < ltype->nNumDashes; ++i )
    {
        dash.dfLength = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);
        dash.dComplexShapecode = ReadBITSHORT (pabyInput, nBitOffsetFromStart);
        dash.dfXOffset = ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart);
        dash.dfYOffset = ReadRAWDOUBLE (pabyInput, nBitOffsetFromStart);
        dash.dfScale = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);
        dash.dfRotation = ReadBITDOUBLE (pabyInput, nBitOffsetFromStart);
        dash.dShapeflag = ReadBITSHORT (pabyInput, nBitOffsetFromStart); // TODO: what to do with it?

        ltype->astDashes.push_back ( dash );
    }
*/
    for ( short i = 0; i < 256; ++i )
        ltype->abyTextArea.push_back ( ReadCHAR (pabyInput, nBitOffsetFromStart ) );

    ltype->hLTControl = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    for ( long i = 0; i < ltype->nNumReactors; ++i )
        ltype->hReactors.push_back ( ReadHANDLE (pabyInput, nBitOffsetFromStart) );

    ltype->hXDictionary = ReadHANDLE (pabyInput, nBitOffsetFromStart);
    ltype->hXRefBlock = ReadHANDLE (pabyInput, nBitOffsetFromStart);

    // TODO: shapefile for dash/shape (1 each). Does it mean that we have nNumDashes * 2 handles, or what?

    nBitOffsetFromStart += 8 - ( nBitOffsetFromStart % 8 );
    ltype->dCRC = ReadRAWSHORT (pabyInput, nBitOffsetFromStart);
#ifdef _DEBUG
    if ( (nBitOffsetFromStart/8) != (dObjectSize + 4) )
        DebugMsg ("[NOT IMPORTANT, CAUSE NOT IMPLEMENTATION NOT COMPLETED] "
                          "Assertion failed at %d in %s\nSize difference: %d\n"
                , __LINE__, __FILE__, (nBitOffsetFromStart/8 - dObjectSize - 4));
#endif //_DEBUG
    return ltype;
}

DWGFileR2000::DWGFileR2000(CADFileIO* poFileIO) : CADFile(poFileIO)
{
    header.addValue(CADHeader::OPENCADVER, CADVersions::DWG_R2000);
}

DWGFileR2000::~DWGFileR2000()
{
}



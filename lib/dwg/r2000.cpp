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
#include <cmath>

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

int DWGFileR2000::ReadHeader(OpenOptions eOptions)
{
    std::array<uint8_t, 256> sentinelBuffer;
    size_t headerVarsSectionLength = 0;

    pFileIO->Seek(sectionLocatorRecords[0].dSeeker, CADFileIO::SeekOrigin::BEG);
    pFileIO->Read(sentinelBuffer.data(), DWGSentinelLength);

    if (memcmp(sentinelBuffer.data(), DWGHeaderVariablesStart, DWGSentinelLength))
    {
        DebugMsg("File is corrupted (wrong pointer to HEADER_VARS section, or HEADERVARS starting sentinel corrupted.)");

        return CADErrorCodes::HEADER_SECTION_READ_FAILED;
    }

    pFileIO->Read(&headerVarsSectionLength, 4);
    DebugMsg("Header variables section length: %zd\n", headerVarsSectionLength);

    std::vector<uint8_t> buffer(headerVarsSectionLength + 4);
    pFileIO->Read(buffer.data(), headerVarsSectionLength + 2);

    CADBitStreamReader reader(buffer);

    if (eOptions == OpenOptions::READ_ALL)
    {
        oHeader.addValue(UNKNOWN1, reader.ReadBitDouble());
        oHeader.addValue(UNKNOWN2, reader.ReadBitDouble());
        oHeader.addValue(UNKNOWN3, reader.ReadBitDouble());
        oHeader.addValue(UNKNOWN4, reader.ReadBitDouble());
        oHeader.addValue(UNKNOWN5, reader.ReadTv());
        oHeader.addValue(UNKNOWN6, reader.ReadTv());
        oHeader.addValue(UNKNOWN7, reader.ReadTv());
        oHeader.addValue(UNKNOWN8, reader.ReadTv());
        oHeader.addValue(UNKNOWN9, reader.ReadBitLong());
        oHeader.addValue(UNKNOWN10, reader.ReadBitLong());
    }
    else
    {
        reader.SeekBitDouble();
        reader.SeekBitDouble();
        reader.SeekBitDouble();
        reader.SeekBitDouble();
        reader.SeekTv();
        reader.SeekTv();
        reader.SeekTv();
        reader.SeekTv();
        reader.SeekBitLong();
        reader.SeekBitLong();
    }

    CADHandle stCurrentViewportTable = reader.ReadHandle();
    oTables.AddTable(CADTables::CurrentViewportTable, stCurrentViewportTable);

    if (eOptions == OpenOptions::READ_ALL)
    {
        oHeader.addValue(CADHeader::DIMASO, reader.ReadBit());     // 1
        oHeader.addValue(CADHeader::DIMSHO, reader.ReadBit());     // 2
        oHeader.addValue(CADHeader::PLINEGEN, reader.ReadBit());   // 3
        oHeader.addValue(CADHeader::ORTHOMODE, reader.ReadBit());  // 4
        oHeader.addValue(CADHeader::REGENMODE, reader.ReadBit());  // 5
        oHeader.addValue(CADHeader::FILLMODE, reader.ReadBit());   // 6
        oHeader.addValue(CADHeader::QTEXTMODE, reader.ReadBit());  // 7
        oHeader.addValue(CADHeader::PSLTSCALE, reader.ReadBit());  // 8
        oHeader.addValue(CADHeader::LIMCHECK, reader.ReadBit());   // 9
        oHeader.addValue(CADHeader::USRTIMER, reader.ReadBit());   // 10
        oHeader.addValue(CADHeader::SKPOLY, reader.ReadBit());     // 11
        oHeader.addValue(CADHeader::ANGDIR, reader.ReadBit());     // 12
        oHeader.addValue(CADHeader::SPLFRAME, reader.ReadBit());   // 13
        oHeader.addValue(CADHeader::MIRRTEXT, reader.ReadBit());   // 14
        oHeader.addValue(CADHeader::WORDLVIEW, reader.ReadBit());  // 15
        oHeader.addValue(CADHeader::TILEMODE, reader.ReadBit());   // 16
        oHeader.addValue(CADHeader::PLIMCHECK, reader.ReadBit());  // 17
        oHeader.addValue(CADHeader::VISRETAIN, reader.ReadBit());  // 18
        oHeader.addValue(CADHeader::DISPSILH, reader.ReadBit());   // 19
        oHeader.addValue(CADHeader::PELLIPSE, reader.ReadBit());   // 20
    }
    else
        reader.SeekBits(20);

    if (eOptions == OpenOptions::READ_ALL)
    {
        oHeader.addValue(CADHeader::PROXYGRAPHICS, reader.ReadBitShort()); // 1
        oHeader.addValue(CADHeader::TREEDEPTH, reader.ReadBitShort());     // 2
        oHeader.addValue(CADHeader::LUNITS, reader.ReadBitShort());        // 3
        oHeader.addValue(CADHeader::LUPREC, reader.ReadBitShort());        // 4
        oHeader.addValue(CADHeader::AUNITS, reader.ReadBitShort());        // 5
        oHeader.addValue(CADHeader::AUPREC, reader.ReadBitShort());        // 6
    }
    else
        for (size_t idx = 0; idx < 6; ++idx)
            reader.SeekBitShort();

    oHeader.addValue(CADHeader::ATTMODE, reader.ReadBitShort());
    oHeader.addValue(CADHeader::PDMODE, reader.ReadBitShort());

    if (eOptions == OpenOptions::READ_ALL)
    {
        oHeader.addValue(CADHeader::USERI1, reader.ReadBitShort());    // 1
        oHeader.addValue(CADHeader::USERI2, reader.ReadBitShort());    // 2
        oHeader.addValue(CADHeader::USERI3, reader.ReadBitShort());    // 3
        oHeader.addValue(CADHeader::USERI4, reader.ReadBitShort());    // 4
        oHeader.addValue(CADHeader::USERI5, reader.ReadBitShort());    // 5
        oHeader.addValue(CADHeader::SPLINESEGS, reader.ReadBitShort());// 6
        oHeader.addValue(CADHeader::SURFU, reader.ReadBitShort());     // 7
        oHeader.addValue(CADHeader::SURFV, reader.ReadBitShort());     // 8
        oHeader.addValue(CADHeader::SURFTYPE, reader.ReadBitShort());  // 9
        oHeader.addValue(CADHeader::SURFTAB1, reader.ReadBitShort());  // 10
        oHeader.addValue(CADHeader::SURFTAB2, reader.ReadBitShort());  // 11
        oHeader.addValue(CADHeader::SPLINETYPE, reader.ReadBitShort());// 12
        oHeader.addValue(CADHeader::SHADEDGE, reader.ReadBitShort());  // 13
        oHeader.addValue(CADHeader::SHADEDIF, reader.ReadBitShort());  // 14
        oHeader.addValue(CADHeader::UNITMODE, reader.ReadBitShort());  // 15
        oHeader.addValue(CADHeader::MAXACTVP, reader.ReadBitShort());  // 16
        oHeader.addValue(CADHeader::ISOLINES, reader.ReadBitShort());  // 17
        oHeader.addValue(CADHeader::CMLJUST, reader.ReadBitShort());   // 18
        oHeader.addValue(CADHeader::TEXTQLTY, reader.ReadBitShort());  // 19
    }
    else
        for (size_t idx = 0; idx < 19; ++idx)
            reader.SeekBitShort();

    oHeader.addValue(CADHeader::LTSCALE, reader.ReadBitDouble());
    oHeader.addValue(CADHeader::TEXTSIZE, reader.ReadBitDouble());
    oHeader.addValue(CADHeader::TRACEWID, reader.ReadBitDouble());
    oHeader.addValue(CADHeader::SKETCHINC, reader.ReadBitDouble());
    oHeader.addValue(CADHeader::FILLETRAD, reader.ReadBitDouble());
    oHeader.addValue(CADHeader::THICKNESS, reader.ReadBitDouble());
    oHeader.addValue(CADHeader::ANGBASE, reader.ReadBitDouble());
    oHeader.addValue(CADHeader::PDSIZE, reader.ReadBitDouble());
    oHeader.addValue(CADHeader::PLINEWID, reader.ReadBitDouble());

    if (eOptions == OpenOptions::READ_ALL)
    {
        oHeader.addValue(CADHeader::USERR1, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::USERR2, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::USERR3, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::USERR4, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::USERR5, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::CHAMFERA, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::CHAMFERB, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::CHAMFERC, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::CHAMFERD, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::FACETRES, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::CMLSCALE, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::CELTSCALE, reader.ReadBitDouble());

        oHeader.addValue(CADHeader::MENU, reader.ReadTv());
    }
    else
    {
        for (size_t idx = 0; idx < 12; ++idx)
            reader.SeekBitDouble();

        reader.SeekTv();
    }

    long juliandate, millisec;
    juliandate = reader.ReadBitLong();
    millisec = reader.ReadBitLong();
    oHeader.addValue(CADHeader::TDCREATE, juliandate, millisec);

    juliandate = reader.ReadBitLong();
    millisec   = reader.ReadBitLong();
    oHeader.addValue(CADHeader::TDUPDATE, juliandate, millisec);

    juliandate = reader.ReadBitLong();
    millisec   = reader.ReadBitLong();
    oHeader.addValue(CADHeader::TDINDWG, juliandate, millisec);

    juliandate = reader.ReadBitLong();
    millisec   = reader.ReadBitLong();
    oHeader.addValue(CADHeader::TDUSRTIMER, juliandate, millisec);

    oHeader.addValue(CADHeader::CECOLOR, reader.ReadBitShort());

    oHeader.addValue(CADHeader::HANDSEED, reader.ReadHandle8BitsLength()); // CHECK THIS CASE.

    oHeader.addValue(CADHeader::CLAYER, reader.ReadHandle());
    oHeader.addValue(CADHeader::TEXTSTYLE, reader.ReadHandle());
    oHeader.addValue(CADHeader::CELTYPE, reader.ReadHandle());
    oHeader.addValue(CADHeader::DIMSTYLE, reader.ReadHandle());
    oHeader.addValue(CADHeader::CMLSTYLE, reader.ReadHandle());

    oHeader.addValue(CADHeader::PSVPSCALE, reader.ReadBitDouble());

    double x, y, z;
    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::PINSBASE, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::PEXTMIN, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::PEXTMAX, x, y, z);

    x = reader.ReadRawDouble();
    y = reader.ReadRawDouble();
    oHeader.addValue(CADHeader::PLIMMIN, x, y);

    x = reader.ReadRawDouble();
    y = reader.ReadRawDouble();
    oHeader.addValue(CADHeader::PLIMMAX, x, y);

    oHeader.addValue(CADHeader::PELEVATION, reader.ReadBitDouble());

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::PUCSORG, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::PUCSXDIR, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::PUCSYDIR, x, y, z);

    oHeader.addValue(CADHeader::PUCSNAME, reader.ReadHandle());
    oHeader.addValue(CADHeader::PUCSORTHOREF, reader.ReadHandle());

    oHeader.addValue(CADHeader::PUCSORTHOVIEW, reader.ReadBitShort());
    oHeader.addValue(CADHeader::PUCSBASE, reader.ReadHandle());

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::PUCSORGTOP, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::PUCSORGBOTTOM, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::PUCSORGLEFT, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::PUCSORGRIGHT, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::PUCSORGFRONT, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::PUCSORGBACK, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::INSBASE, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::EXTMIN, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::EXTMAX, x, y, z);

    x = reader.ReadRawDouble();
    y = reader.ReadRawDouble();
    oHeader.addValue(CADHeader::LIMMIN, x, y);

    x = reader.ReadRawDouble();
    y = reader.ReadRawDouble();
    oHeader.addValue(CADHeader::LIMMAX, x, y);

    oHeader.addValue(CADHeader::ELEVATION, reader.ReadBitDouble());

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::UCSORG, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::UCSXDIR, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::UCSYDIR, x, y, z);

    oHeader.addValue(CADHeader::UCSNAME, reader.ReadHandle());
    oHeader.addValue(CADHeader::UCSORTHOREF, reader.ReadHandle());

    oHeader.addValue(CADHeader::UCSORTHOVIEW, reader.ReadBitShort());

    oHeader.addValue(CADHeader::UCSBASE, reader.ReadHandle());

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::UCSORGTOP, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::UCSORGBOTTOM, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::UCSORGLEFT, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::UCSORGRIGHT, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::UCSORGFRONT, x, y, z);

    x = reader.ReadBitDouble();
    y = reader.ReadBitDouble();
    z = reader.ReadBitDouble();
    oHeader.addValue(CADHeader::UCSORGBACK, x, y, z);

    if (eOptions == OpenOptions::READ_ALL)
    {
        oHeader.addValue(CADHeader::DIMPOST, reader.ReadTv());
        oHeader.addValue(CADHeader::DIMAPOST, reader.ReadTv());

        oHeader.addValue(CADHeader::DIMSCALE, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMASZ, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMEXO, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMDLI, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMEXE, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMRND, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMDLE, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMTP, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMTM, reader.ReadBitDouble());

        oHeader.addValue(CADHeader::DIMTOL, reader.ReadBit());
        oHeader.addValue(CADHeader::DIMLIM, reader.ReadBit());
        oHeader.addValue(CADHeader::DIMTIH, reader.ReadBit());
        oHeader.addValue(CADHeader::DIMTOH, reader.ReadBit());
        oHeader.addValue(CADHeader::DIMSE1, reader.ReadBit());
        oHeader.addValue(CADHeader::DIMSE2, reader.ReadBit());

        oHeader.addValue(CADHeader::DIMTAD, reader.ReadBitShort());
        oHeader.addValue(CADHeader::DIMZIN, reader.ReadBitShort());
        oHeader.addValue(CADHeader::DIMAZIN, reader.ReadBitShort());

        oHeader.addValue(CADHeader::DIMTXT, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMCEN, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMTSZ, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMALTF, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMLFAC, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMTVP, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMTFAC, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMGAP, reader.ReadBitDouble());
        oHeader.addValue(CADHeader::DIMALTRND, reader.ReadBitDouble());

        oHeader.addValue(CADHeader::DIMALT, reader.ReadBit());

        oHeader.addValue(CADHeader::DIMALTD, reader.ReadBitShort());

        oHeader.addValue(CADHeader::DIMTOFL, reader.ReadBit());
        oHeader.addValue(CADHeader::DIMSAH, reader.ReadBit());
        oHeader.addValue(CADHeader::DIMTIX, reader.ReadBit());
        oHeader.addValue(CADHeader::DIMSOXD, reader.ReadBit());

        oHeader.addValue(CADHeader::DIMCLRD, reader.ReadBitShort());   // 1
        oHeader.addValue(CADHeader::DIMCLRE, reader.ReadBitShort());   // 2
        oHeader.addValue(CADHeader::DIMCLRT, reader.ReadBitShort());   // 3
        oHeader.addValue(CADHeader::DIMADEC, reader.ReadBitShort());   // 4
        oHeader.addValue(CADHeader::DIMDEC, reader.ReadBitShort());    // 5
        oHeader.addValue(CADHeader::DIMTDEC, reader.ReadBitShort());   // 6
        oHeader.addValue(CADHeader::DIMALTU, reader.ReadBitShort());   // 7
        oHeader.addValue(CADHeader::DIMALTTD, reader.ReadBitShort());  // 8
        oHeader.addValue(CADHeader::DIMAUNIT, reader.ReadBitShort());  // 9
        oHeader.addValue(CADHeader::DIMFRAC, reader.ReadBitShort());   // 10
        oHeader.addValue(CADHeader::DIMLUNIT, reader.ReadBitShort());  // 11
        oHeader.addValue(CADHeader::DIMDSEP, reader.ReadBitShort());   // 12
        oHeader.addValue(CADHeader::DIMTMOVE, reader.ReadBitShort());  // 13
        oHeader.addValue(CADHeader::DIMJUST, reader.ReadBitShort());   // 14

        oHeader.addValue(CADHeader::DIMSD1, reader.ReadBit());
        oHeader.addValue(CADHeader::DIMSD2, reader.ReadBit());

        oHeader.addValue(CADHeader::DIMTOLJ, reader.ReadBitShort());
        oHeader.addValue(CADHeader::DIMTZIN, reader.ReadBitShort());
        oHeader.addValue(CADHeader::DIMALTZ, reader.ReadBitShort());
        oHeader.addValue(CADHeader::DIMALTTZ, reader.ReadBitShort());

        oHeader.addValue(CADHeader::DIMUPT, reader.ReadBit());

        oHeader.addValue(CADHeader::DIMATFIT, reader.ReadBitShort());

        oHeader.addValue(CADHeader::DIMTXSTY, reader.ReadHandle());
        oHeader.addValue(CADHeader::DIMLDRBLK, reader.ReadHandle());
        oHeader.addValue(CADHeader::DIMBLK, reader.ReadHandle());
        oHeader.addValue(CADHeader::DIMBLK1, reader.ReadHandle());
        oHeader.addValue(CADHeader::DIMBLK2, reader.ReadHandle());

        oHeader.addValue(CADHeader::DIMLWD, reader.ReadBitShort());
        oHeader.addValue(CADHeader::DIMLWE, reader.ReadBitShort());
    }
    else
    {
        reader.SeekTv();
        reader.SeekTv();

        for (size_t idx = 0; idx < 9; ++idx)
            reader.SeekBitDouble();

        reader.SeekBits(6);

        for (size_t idx = 0; idx < 3; ++idx)
            reader.SeekBitShort();

        for (size_t idx = 0; idx < 9; ++idx)
            reader.SeekBitDouble();

        reader.SeekBits(1);
        reader.SeekBitShort();
        reader.SeekBits(4);

        for (size_t idx = 0; idx < 14; ++idx)
            reader.SeekBitShort();

        reader.SeekBits(2);

        for (size_t idx = 0; idx < 4; ++idx)
            reader.SeekBitShort();

        reader.SeekBits(1);
        reader.SeekBitShort();

        for (size_t idx = 0; idx < 5; ++idx)
            reader.SeekHandle();

        reader.SeekBitShort();
        reader.SeekBitShort();
    }

    CADHandle stBlocksTable = reader.ReadHandle();
    oTables.AddTable(CADTables::BlocksTable, stBlocksTable);

    CADHandle stLayersTable = reader.ReadHandle();
    oTables.AddTable(CADTables::LayersTable, stLayersTable);

    CADHandle stStyleTable = reader.ReadHandle();
    oTables.AddTable(CADTables::StyleTable, stStyleTable);

    CADHandle stLineTypesTable = reader.ReadHandle();
    oTables.AddTable(CADTables::LineTypesTable, stLineTypesTable);

    CADHandle stViewTable = reader.ReadHandle();
    oTables.AddTable(CADTables::ViewTable, stViewTable);

    CADHandle stUCSTable = reader.ReadHandle();
    oTables.AddTable(CADTables::UCSTable, stUCSTable);

    CADHandle stViewportTable = reader.ReadHandle();
    oTables.AddTable(CADTables::ViewportTable, stViewportTable);

    CADHandle stAPPIDTable = reader.ReadHandle();
    oTables.AddTable(CADTables::APPIDTable, stAPPIDTable);

    if (eOptions == OpenOptions::READ_ALL)
        oHeader.addValue(CADHeader::DIMSTYLE, reader.ReadHandle());
    else
        reader.SeekHandle();

    CADHandle stEntityTable = reader.ReadHandle();
    oTables.AddTable(CADTables::EntityTable, stEntityTable);

    CADHandle stACADGroupDict = reader.ReadHandle();
    oTables.AddTable(CADTables::ACADGroupDict, stACADGroupDict);

    CADHandle stACADMLineStyleDict = reader.ReadHandle();
    oTables.AddTable(CADTables::ACADMLineStyleDict, stACADMLineStyleDict);

    CADHandle stNamedObjectsDict = reader.ReadHandle();
    oTables.AddTable(CADTables::NamedObjectsDict, stNamedObjectsDict);

    if (eOptions == OpenOptions::READ_ALL)
    {
        oHeader.addValue(CADHeader::TSTACKALIGN, reader.ReadBitShort());
        oHeader.addValue(CADHeader::TSTACKSIZE, reader.ReadBitShort());
    }
    else
    {
        reader.SeekBitShort();
        reader.SeekBitShort();
    }

    oHeader.addValue(CADHeader::HYPERLINKBASE, reader.ReadTv());
    oHeader.addValue(CADHeader::STYLESHEET, reader.ReadTv());

    CADHandle stLayoutsDict = reader.ReadHandle();
    oTables.AddTable(CADTables::LayoutsDict, stLayoutsDict);

    CADHandle stPlotSettingsDict = reader.ReadHandle();
    oTables.AddTable(CADTables::PlotSettingsDict, stPlotSettingsDict);

    CADHandle stPlotStylesDict = reader.ReadHandle();
    oTables.AddTable(CADTables::PlotStylesDict, stPlotStylesDict);

    if (eOptions == OpenOptions::READ_ALL)
    {
        int Flags = reader.ReadBitLong();
        oHeader.addValue(CADHeader::CELWEIGHT, Flags & 0x001F);
        oHeader.addValue(CADHeader::ENDCAPS, static_cast<bool>(Flags & 0x0060));
        oHeader.addValue(CADHeader::JOINSTYLE, static_cast<bool>(Flags & 0x0180));
        oHeader.addValue(CADHeader::LWDISPLAY, static_cast<bool>(!(Flags & 0x0200)));
        oHeader.addValue(CADHeader::XEDIT, static_cast<bool>(!(Flags & 0x0400)));
        oHeader.addValue(CADHeader::EXTNAMES, static_cast<bool>(Flags & 0x0800));
        oHeader.addValue(CADHeader::PSTYLEMODE, static_cast<bool>(Flags & 0x2000));
        oHeader.addValue(CADHeader::OLESTARTUP, static_cast<bool>(Flags & 0x4000));
    }
    else
        reader.SeekBitLong();

    oHeader.addValue(CADHeader::INSUNITS, reader.ReadBitShort());
    int16_t nCEPSNTYPE = reader.ReadBitShort();
    oHeader.addValue(CADHeader::CEPSNTYPE, nCEPSNTYPE);

    if (nCEPSNTYPE == 3)
        oHeader.addValue(CADHeader::CEPSNID, reader.ReadHandle());

    oHeader.addValue(CADHeader::FINGERPRINTGUID, reader.ReadTv());
    oHeader.addValue(CADHeader::VERSIONGUID, reader.ReadTv());

    CADHandle stBlockRecordPaperSpace = reader.ReadHandle();
    oTables.AddTable(CADTables::BlockRecordPaperSpace, stBlockRecordPaperSpace);
    // TODO: is this part of the header?
    CADHandle stBlockRecordModelSpace = reader.ReadHandle();
    oTables.AddTable(CADTables::BlockRecordModelSpace, stBlockRecordModelSpace);

    if (eOptions == OpenOptions::READ_ALL)
    {
        // Is this part of the header?

        /*CADHandle LTYPE_
		= */reader.ReadHandle();
        /*CADHandle LTYPE_BYBLOCK = */reader.ReadHandle();
        /*CADHandle LTYPE_CONTINUOUS = */reader.ReadHandle();

        oHeader.addValue(UNKNOWN11, reader.ReadBitShort());
        oHeader.addValue(UNKNOWN12, reader.ReadBitShort());
        oHeader.addValue(UNKNOWN13, reader.ReadBitShort());
        oHeader.addValue(UNKNOWN14, reader.ReadBitShort());
    }
    else
    {
        reader.SeekHandle();
        reader.SeekHandle();
        reader.SeekHandle();
        reader.SeekBitShort();
        reader.SeekBitShort();
        reader.SeekBitShort();
        reader.SeekBitShort();
    }

    /*int16_t nCRC =*/reader.ReadRawShort();
    uint16_t initial = 0xC0C1;
    // /*int16_t calculated_crc = */ CalculateCRC8(initial, pabyBuf,
    //                                            static_cast<int>(headerVarsSectionLength)); // TODO: CRC is calculated wrong every time.

    int returnCode = CADErrorCodes::SUCCESS;
    pFileIO->Read(buffer.data(), DWGSentinelLength);

    if (memcmp(buffer.data(), DWGHeaderVariablesEnd, DWGSentinelLength))
    {
        DebugMsg("File is corrupted (HEADERVARS section ending sentinel doesnt match.)");

        returnCode = CADErrorCodes::HEADER_SECTION_READ_FAILED;
    }

    return returnCode;
}

int DWGFileR2000::ReadClasses(enum OpenOptions eOptions)
{
    if (eOptions == OpenOptions::READ_ALL || eOptions == OpenOptions::READ_FAST)
    {
        std::array<uint8_t, 256> sentinelBuffer;

        pFileIO->Seek(sectionLocatorRecords[1].dSeeker, CADFileIO::SeekOrigin::BEG);

        pFileIO->Read(sentinelBuffer.data(), DWGSentinelLength);
        if (memcmp(sentinelBuffer.data(), DWGDSClassesStart, DWGSentinelLength))
        {
            cerr << "File is corrupted (wrong pointer to CLASSES section,"
                    "or CLASSES starting sentinel corrupted.)\n";

            return CADErrorCodes::CLASSES_SECTION_READ_FAILED;
        }

        size_t sectionSize = 0;
        pFileIO->Read(&sectionSize, 4);
        DebugMsg("Classes section length: %zd\n", sectionSize);

        ByteArray sectionContent(sectionSize + 4);
        pFileIO->Read(sectionContent.data(), sectionSize);

        CADBitStreamReader reader(sectionContent);

        while((reader.GetOffset() / 8 + 1) < sectionSize)
        {
            CADClass stClass;
            stClass.dClassNum        = reader.ReadBitShort();
            stClass.dProxyCapFlag    = reader.ReadBitShort();
            stClass.sApplicationName = reader.ReadTv();
            stClass.sCppClassName    = reader.ReadTv();
            stClass.sDXFRecordName   = reader.ReadTv();
            stClass.bWasZombie       = reader.ReadBit();
            stClass.bIsEntity        = reader.ReadBitShort();

            oClasses.addClass(stClass);
        }

        pFileIO->Read(sectionContent.data(), 2); // CLASSES CRC!. TODO: add CRC computing & checking feature.

        pFileIO->Read(sentinelBuffer.data(), DWGSentinelLength);
        if (memcmp(sentinelBuffer.data(), DWGDSClassesEnd, DWGSentinelLength))
        {
            cerr << "File is corrupted (CLASSES section ending sentinel doesnt match.)\n";

            return CADErrorCodes::CLASSES_SECTION_READ_FAILED;
        }
    }

    return CADErrorCodes::SUCCESS;
}

int DWGFileR2000::CreateFileMap()
{
    // Seems like ODA specification is completely awful. CRC is included in section size.
    uint16_t sectionSize;
    size_t recordsInSection;
    size_t section = 0;

    using ObjHandleOffset = std::pair<int32_t, int32_t>;
    ObjHandleOffset previousObjHandleOffset;
    ObjHandleOffset tmpOffset;

    mapObjects.clear();

    // seek to the begining of the objects map
    pFileIO->Seek(sectionLocatorRecords[2].dSeeker, CADFileIO::SeekOrigin::BEG);

    while (true)
    {
        sectionSize = 0;

        // read section size
        pFileIO->Read(&sectionSize, 2);
        SwapEndianness(sectionSize, sizeof(sectionSize));

        DebugMsg("Object map section #%zd size: %hu\n", ++section, sectionSize);

        if (sectionSize == 2)
            break; // last section is empty.

        ByteArray sectionContent(sectionSize + 4);
        recordsInSection = 0;

        // read section data
        pFileIO->Read(sectionContent.data(), sectionSize);

        CADBitStreamReader reader(sectionContent);

        while((reader.GetOffset() / 8) < ((size_t)sectionSize - 2))
        {
            tmpOffset.first  = (uint32_t)reader.ReadMChar();
            tmpOffset.second = (uint32_t)reader.ReadMChar();

            if (!recordsInSection)
                previousObjHandleOffset = tmpOffset;
            else
            {
                previousObjHandleOffset.first += tmpOffset.first;
                previousObjHandleOffset.second += tmpOffset.second;
            }
#ifdef _DEBUG
            assert(mapObjects.find(previousObjHandleOffset.first) == mapObjects.end());
#endif //_DEBUG
            mapObjects.insert(previousObjHandleOffset);
            ++recordsInSection;
        }

        /* Unused
        dSectionCRC = */reader.ReadRawShort();/*
        SwapEndianness (dSectionCRC, sizeof (dSectionCRC));
        */
    }

    return CADErrorCodes::SUCCESS;
}

CADObject * DWGFileR2000::GetObject(int32_t handle, bool handlesOnly)
{
    CADObject * readedObject = nullptr;

    ByteArray objectSizeArray(8);
    pFileIO->Seek(mapObjects[handle], CADFileIO::SeekOrigin::BEG);
    pFileIO->Read(objectSizeArray.data(), 8);

    CADBitStreamReader reader(objectSizeArray);
    uint32_t objectSize = reader.ReadMShort();

    // And read whole data chunk into memory for future parsing.
    // + reader.GetOffset()/8 + 2 is because objectSize doesn't cover CRC and itself.
    size_t sectionSize = objectSize + reader.GetOffset() / 8 + 2;

    ByteArray sectionContent(sectionSize + 4);
    pFileIO->Seek(mapObjects[handle], CADFileIO::SeekOrigin::BEG);
    pFileIO->Read(sectionContent.data(), sectionSize);

    reader = CADBitStreamReader(sectionContent);

    objectSize = reader.ReadMShort();
    int16_t objectType = reader.ReadBitShort();

    if (objectType >= 500)
    {
        CADClass cadClass = oClasses.getClassByNum(objectType);
        // FIXME: replace strcmp() with C++ analog
        if (!strcmp(cadClass.sCppClassName.c_str(), "AcDbRasterImage"))
        {
            objectType = CADObject::IMAGE;
        }
        else if (!strcmp(cadClass.sCppClassName.c_str(), "AcDbRasterImageDef"))
        {
            objectType = CADObject::IMAGEDEF;
        }
        else if (!strcmp(cadClass.sCppClassName.c_str(), "AcDbRasterImageDefReactor"))
        {
            objectType = CADObject::IMAGEDEFREACTOR;
        }
        else if (!strcmp(cadClass.sCppClassName.c_str(), "AcDbWipeout"))
        {
            objectType = CADObject::WIPEOUT;
        }
    }

    // Entities handling
    if (isCommonEntityType(objectType))
    {
        struct CADCommonED stCommonEntityData; // common for all entities

        stCommonEntityData.nObjectSizeInBits = reader.ReadRawLong();
        stCommonEntityData.hObjectHandle     = reader.ReadHandle();

        int16_t eedSize;
        CADEed dwgEed;

        while((eedSize = reader.ReadBitShort()) != 0)
        {
            dwgEed.dLength      = eedSize;
            dwgEed.hApplication = reader.ReadHandle();

            for (int16_t idx = 0; idx < eedSize; ++idx)
                dwgEed.acData.push_back(reader.ReadChar());

            stCommonEntityData.aEED.push_back(dwgEed);
        }

        stCommonEntityData.bGraphicsPresented = reader.ReadBit();
        if (stCommonEntityData.bGraphicsPresented)
        {
            size_t nGraphicsDataSize = static_cast<size_t>(reader.ReadRawLong());
            // skip read graphics data
            reader.SeekBits(nGraphicsDataSize * 8);
        }
        stCommonEntityData.bbEntMode        = reader.Read2Bits();
        stCommonEntityData.nNumReactors     = reader.ReadBitLong();
        stCommonEntityData.bNoLinks         = reader.ReadBit();
        stCommonEntityData.nCMColor         = reader.ReadBitShort();
        stCommonEntityData.dfLTypeScale     = reader.ReadBitDouble();
        stCommonEntityData.bbLTypeFlags     = reader.Read2Bits();
        stCommonEntityData.bbPlotStyleFlags = reader.Read2Bits();
        stCommonEntityData.nInvisibility    = reader.ReadBitShort();
        stCommonEntityData.nLineWeight      = reader.ReadChar();

        // Skip entitity-specific data, we dont need it if handlesOnly == true
        if (handlesOnly == true)
            return getEntity(reader, objectType, objectSize, stCommonEntityData);

        switch (objectType)
        {
        case CADObject::BLOCK:
            return getBlock(reader, objectSize, stCommonEntityData);

        case CADObject::ELLIPSE:
            return getEllipse(reader, objectSize, stCommonEntityData);

        case CADObject::MLINE:
            return getMLine(reader, objectSize, stCommonEntityData);

        case CADObject::SOLID:
            return getSolid(reader, objectSize, stCommonEntityData);

        case CADObject::POINT:
            return getPoint(reader, objectSize, stCommonEntityData);

        case CADObject::POLYLINE3D:
            return getPolyLine3D(reader, objectSize, stCommonEntityData);

        case CADObject::RAY:
            return getRay(reader, objectSize, stCommonEntityData);

        case CADObject::XLINE:
            return getXLine(reader, objectSize, stCommonEntityData);

        case CADObject::LINE:
            return getLine(reader, objectSize, stCommonEntityData);

        case CADObject::TEXT:
            return getText(reader, objectSize, stCommonEntityData);

		case CADObject::VERTEX2D:
			return getVertex2D(reader, objectSize, stCommonEntityData);

        case CADObject::VERTEX3D:
            return getVertex3D(reader, objectSize, stCommonEntityData);

        case CADObject::CIRCLE:
            return getCircle(reader, objectSize, stCommonEntityData);

        case CADObject::ENDBLK:
            return getEndBlock(reader, objectSize, stCommonEntityData);

        case CADObject::POLYLINE2D:
            return getPolyline2D(reader, objectSize, stCommonEntityData);

        case CADObject::ATTRIB:
            return getAttributes(reader, objectSize, stCommonEntityData);

        case CADObject::ATTDEF:
            return getAttributesDefn(reader, objectSize, stCommonEntityData);

        case CADObject::LWPOLYLINE:
            return getLWPolyLine(reader, objectSize, stCommonEntityData);

        case CADObject::ARC:
            return getArc(reader, objectSize, stCommonEntityData);

        case CADObject::SPLINE:
            return getSpline(reader, objectSize, stCommonEntityData);

        case CADObject::POLYLINE_PFACE:
            return getPolylinePFace(reader, objectSize, stCommonEntityData);

        case CADObject::IMAGE:
            return getImage(reader, objectSize, stCommonEntityData);

        case CADObject::FACE3D:
            return get3DFace(reader, objectSize, stCommonEntityData);

        case CADObject::VERTEX_MESH:
            return getVertexMesh(reader, objectSize, stCommonEntityData);

        case CADObject::VERTEX_PFACE:
            return getVertexPFace(reader, objectSize, stCommonEntityData);

        case CADObject::MTEXT:
            return getMText(reader, objectSize, stCommonEntityData);

        case CADObject::DIMENSION_RADIUS:
        case CADObject::DIMENSION_DIAMETER:
        case CADObject::DIMENSION_ALIGNED:
        case CADObject::DIMENSION_ANG_3PT:
        case CADObject::DIMENSION_ANG_2LN:
        case CADObject::DIMENSION_ORDINATE:
        case CADObject::DIMENSION_LINEAR:
            return getDimension(reader, objectType, objectSize, stCommonEntityData);

        case CADObject::INSERT:
            return getInsert(reader, objectType, objectSize, stCommonEntityData);

        default:
            return getEntity(reader, objectType, objectSize, stCommonEntityData);
        }
    }
    else
    {
        switch (objectType)
        {
        case CADObject::DICTIONARY:
            return getDictionary(reader, objectSize);

        case CADObject::LAYER:
            return getLayerObject(reader, objectSize);

        case CADObject::LAYER_CONTROL_OBJ:
            return getLayerControl(reader, objectSize);

        case CADObject::BLOCK_CONTROL_OBJ:
            return getBlockControl(reader, objectSize);

        case CADObject::BLOCK_HEADER:
            return getBlockHeader(reader, objectSize);

        case CADObject::LTYPE_CONTROL_OBJ:
            return getLineTypeControl(reader, objectSize);

        case CADObject::LTYPE1:
            return getLineType1(reader, objectSize);

        case CADObject::IMAGEDEF:
            return getImageDef(reader, objectSize);

        case CADObject::IMAGEDEFREACTOR:
            return getImageDefReactor(reader, objectSize);

        case CADObject::XRECORD:
            return getXRecord(reader, objectSize);
        }
    }

    return readedObject;
}

CADGeometry * DWGFileR2000::GetGeometry(size_t iLayerIndex, int32_t handle, int32_t dBlockRefHandle)
{
    CADGeometry * poGeometry = nullptr;
    unique_ptr<CADEntityObject> readedObject(static_cast<CADEntityObject *>(GetObject(handle)));

    if (nullptr == readedObject)
        return nullptr;

    switch (readedObject->getType())
    {
        case CADObject::ARC:
        {
            CADArc       * arc    = new CADArc();
            CADArcObject * cadArc = static_cast<CADArcObject *>(
                    readedObject.get());

            arc->setPosition(cadArc->vertPosition);
            arc->setExtrusion(cadArc->vectExtrusion);
            arc->setRadius(cadArc->dfRadius);
            arc->setThickness(cadArc->dfThickness);
            arc->setStartingAngle(cadArc->dfStartAngle);
            arc->setEndingAngle(cadArc->dfEndAngle);

            poGeometry = arc;
            break;
        }

        case CADObject::POINT:
        {
            CADPoint3D     * point    = new CADPoint3D();
            CADPointObject * cadPoint = static_cast<CADPointObject *>(
                    readedObject.get());

            point->setPosition(cadPoint->vertPosition);
            point->setExtrusion(cadPoint->vectExtrusion);
            point->setXAxisAng(cadPoint->dfXAxisAng);
            point->setThickness(cadPoint->dfThickness);

            poGeometry = point;
            break;
        }

        case CADObject::POLYLINE3D:
        {
            CADPolyline3D       * polyline               = new CADPolyline3D();
            CADPolyline3DObject * cadPolyline3D          = static_cast<CADPolyline3DObject *>(
                    readedObject.get());

			polyline->setClosed(cadPolyline3D->bClosed);
			polyline->setSplined(cadPolyline3D->bSplined);

            // TODO: code can be much simplified if CADHandle will be used.
            // to do so, == and ++ operators should be implemented.
            unique_ptr<CADVertex3DObject> vertex;
            int32_t                          currentVertexH = cadPolyline3D->hVertexes[0].getAsLong();
            while(currentVertexH != 0)
            {
                vertex.reset(static_cast<CADVertex3DObject *>(
                                      GetObject(currentVertexH)));

                if (vertex == nullptr)
                    break;

                currentVertexH = vertex->stCed.hObjectHandle.getAsLong();
				if (! (vertex->vFlags & 2 || vertex->vFlags & 16)) // ignore tangent / spline frame pts  
					polyline->addVertex(vertex->vertPosition);
                if (vertex->stCed.bNoLinks == true)
                {
                    ++currentVertexH;
                } else
                {
                    currentVertexH = vertex->stChed.hNextEntity.getAsLong(vertex->stCed.hObjectHandle);
                }

                // Last vertex is reached. read it and break reading.
                if (currentVertexH == cadPolyline3D->hVertexes[1].getAsLong())
                {
                    vertex.reset(static_cast<CADVertex3DObject *>(
                                          GetObject(currentVertexH)));
					if (!(vertex->vFlags & 2 || vertex->vFlags & 16)) // ignore tangent / spline frame pts 
						polyline->addVertex(vertex->vertPosition);
                    break;
                }
            }

            poGeometry = polyline;
            break;
        }

        case CADObject::LWPOLYLINE:
        {
            CADLWPolyline       * lwPolyline    = new CADLWPolyline();
            CADLWPolylineObject * cadlwPolyline = static_cast<CADLWPolylineObject *>(
                    readedObject.get());

			lwPolyline->setBulges(cadlwPolyline->adfBulges);
            lwPolyline->setClosed(cadlwPolyline->bClosed);
            lwPolyline->setConstWidth(cadlwPolyline->dfConstWidth);
            lwPolyline->setElevation(cadlwPolyline->dfElevation);
            for (const CADVector& vertex : cadlwPolyline->avertVertexes)
                lwPolyline->addVertex(vertex);
            lwPolyline->setVectExtrusion(cadlwPolyline->vectExtrusion);
            lwPolyline->setWidths(cadlwPolyline->astWidths);

            poGeometry = lwPolyline;
            break;
        }

		case CADObject::POLYLINE2D:
		{
			CADPolyline2D       * polyline2D = new CADPolyline2D();
			CADPolyline2DObject * cadPolyline2D = static_cast<CADPolyline2DObject *>(
                    readedObject.get());
			
			polyline2D->setClosed(cadPolyline2D->bClosed);
			polyline2D->setSplined(cadPolyline2D->bSplined);
			polyline2D->setStartSegWidth(cadPolyline2D->dfStartWidth);
			polyline2D->setEndSegWidth(cadPolyline2D->dfEndWidth);
			polyline2D->setElevation(cadPolyline2D->dfElevation);
			polyline2D->setVectExtrusion(cadPolyline2D->vectExtrusion);

			std::vector<double> bulges;
			vector<pair<double, double> > widths;

			// TODO: code can be much simplified if CADHandle will be used.
			// to do so, == and ++ operators should be implemented.
			unique_ptr<CADVertex2DObject> vertex;
			int32_t                          currentVertexH = cadPolyline2D->hVertexes[0].getAsLong();
			while (currentVertexH != 0)
			{
				vertex.reset(static_cast<CADVertex2DObject *>(
					GetObject(currentVertexH)));

				if (vertex == nullptr)
					break;

				currentVertexH = vertex->stCed.hObjectHandle.getAsLong();
				if (!(vertex->vFlags & 2 || vertex->vFlags & 16)) // ignore tangent / spline frame pts  
				{
					polyline2D->addVertex(CADVector(vertex->vertPosition));
					bulges.push_back(vertex->dfBulge);
					widths.push_back(make_pair(vertex->dfStartWidth, vertex->dfEndWidth));
				}
				if (vertex->stCed.bNoLinks == true)
				{
					++currentVertexH;
				}
				else
				{
					currentVertexH = vertex->stChed.hNextEntity.getAsLong(vertex->stCed.hObjectHandle);
				}

				// Last vertex is reached. read it and break reading.
				if (currentVertexH == cadPolyline2D->hVertexes[1].getAsLong())
				{
					vertex.reset(static_cast<CADVertex2DObject *>(
						GetObject(currentVertexH)));
					if (!(vertex->vFlags & 2 || vertex->vFlags & 16)) // ignore tangent / spline frame pts  
					{
						polyline2D->addVertex(CADVector(vertex->vertPosition));
						widths.push_back(make_pair(vertex->dfStartWidth, vertex->dfEndWidth));
					}
					break;
				}
			}

			polyline2D->setBulges(bulges);
			polyline2D->setWidths(widths);

			poGeometry = polyline2D;
            break;
		}

        case CADObject::CIRCLE:
        {
            CADCircle       * circle    = new CADCircle();
            CADCircleObject * cadCircle = static_cast<CADCircleObject *>(
                    readedObject.get());

            circle->setPosition(cadCircle->vertPosition);
            circle->setExtrusion(cadCircle->vectExtrusion);
            circle->setRadius(cadCircle->dfRadius);
            circle->setThickness(cadCircle->dfThickness);

            poGeometry = circle;
            break;
        }

        case CADObject::ATTRIB:
        {
            CADAttrib       * attrib    = new CADAttrib();
            CADAttribObject * cadAttrib = static_cast<CADAttribObject *>(
                    readedObject.get());

            attrib->setPosition(cadAttrib->vertInsetionPoint);
            attrib->setExtrusion(cadAttrib->vectExtrusion);
            attrib->setRotationAngle(cadAttrib->dfRotationAng);
            attrib->setAlignmentPoint(cadAttrib->vertAlignmentPoint);
            attrib->setElevation(cadAttrib->dfElevation);
            attrib->setHeight(cadAttrib->dfHeight);
            attrib->setObliqueAngle(cadAttrib->dfObliqueAng);
            attrib->setPositionLocked(cadAttrib->bLockPosition);
            attrib->setTag(cadAttrib->sTag);
            attrib->setTextValue(cadAttrib->sTextValue);
            attrib->setThickness(cadAttrib->dfThickness);

            poGeometry = attrib;
            break;
        }

        case CADObject::ATTDEF:
        {
            CADAttdef       * attdef    = new CADAttdef();
            CADAttdefObject * cadAttrib = static_cast<CADAttdefObject *>(
                    readedObject.get());

            attdef->setPosition(cadAttrib->vertInsetionPoint);
            attdef->setExtrusion(cadAttrib->vectExtrusion);
            attdef->setRotationAngle(cadAttrib->dfRotationAng);
            attdef->setAlignmentPoint(cadAttrib->vertAlignmentPoint);
            attdef->setElevation(cadAttrib->dfElevation);
            attdef->setHeight(cadAttrib->dfHeight);
            attdef->setObliqueAngle(cadAttrib->dfObliqueAng);
            attdef->setPositionLocked(cadAttrib->bLockPosition);
            attdef->setTag(cadAttrib->sTag);
            attdef->setTextValue(cadAttrib->sTextValue);
            attdef->setThickness(cadAttrib->dfThickness);
            attdef->setPrompt(cadAttrib->sPrompt);

            poGeometry = attdef;
            break;
        }

        case CADObject::ELLIPSE:
        {
            CADEllipse       * ellipse    = new CADEllipse();
            CADEllipseObject * cadEllipse = static_cast<CADEllipseObject *>(
                    readedObject.get());

            ellipse->setPosition(cadEllipse->vertPosition);
            ellipse->setSMAxis(cadEllipse->vectSMAxis);
            ellipse->setAxisRatio(cadEllipse->dfAxisRatio);
            ellipse->setEndingAngle(cadEllipse->dfEndAngle);
            ellipse->setStartingAngle(cadEllipse->dfBegAngle);

            poGeometry = ellipse;
            break;
        }

        case CADObject::LINE:
        {
            CADLineObject * cadLine = static_cast<CADLineObject *>(
                    readedObject.get());

            CADPoint3D ptBeg(cadLine->vertStart, cadLine->dfThickness);
            CADPoint3D ptEnd(cadLine->vertEnd, cadLine->dfThickness);

            CADLine * line = new CADLine(ptBeg, ptEnd);

            poGeometry = line;
            break;
        }

        case CADObject::RAY:
        {
            CADRay       * ray    = new CADRay();
            CADRayObject * cadRay = static_cast<CADRayObject *>(
                    readedObject.get());

            ray->setVectVector(cadRay->vectVector);
            ray->setPosition(cadRay->vertPosition);

            poGeometry = ray;
            break;
        }

        case CADObject::SPLINE:
        {
            CADSpline       * spline    = new CADSpline();
            CADSplineObject * cadSpline = static_cast<CADSplineObject *>(
                    readedObject.get());

            spline->setScenario(cadSpline->dScenario);
            spline->setDegree(cadSpline->dDegree);
            if (spline->getScenario() == 2)
            {
                spline->setFitTollerance(cadSpline->dfFitTol);
            } else if (spline->getScenario() == 1)
            {
                spline->setRational(cadSpline->bRational);
                spline->setClosed(cadSpline->bClosed);
                spline->setWeight(cadSpline->bWeight);
            }
            for (double weight : cadSpline->adfCtrlPointsWeight)
                spline->addControlPointsWeight(weight);

            for (const CADVector& pt : cadSpline->averFitPoints)
                spline->addFitPoint(pt);

            for (const CADVector& pt : cadSpline->avertCtrlPoints)
                spline->addControlPoint(pt);

            poGeometry = spline;
            break;
        }

        case CADObject::TEXT:
        {
            CADText       * text    = new CADText();
            CADTextObject * cadText = static_cast<CADTextObject *>(
                    readedObject.get());

            text->setPosition(cadText->vertInsetionPoint);
            text->setTextValue(cadText->sTextValue);
            text->setRotationAngle(cadText->dfRotationAng);
            text->setObliqueAngle(cadText->dfObliqueAng);
            text->setThickness(cadText->dfThickness);
            text->setHeight(cadText->dfElevation);

            poGeometry = text;
            break;
        }

        case CADObject::SOLID:
        {
            CADSolid       * solid    = new CADSolid();
            CADSolidObject * cadSolid = static_cast<CADSolidObject *>(
                    readedObject.get());

            solid->setElevation(cadSolid->dfElevation);
            solid->setThickness(cadSolid->dfThickness);
            for (const CADVector& corner : cadSolid->avertCorners)
                solid->addCorner(corner);
            solid->setExtrusion(cadSolid->vectExtrusion);

            poGeometry = solid;
            break;
        }

        case CADObject::IMAGE:
        {
            CADImage       * image    = new CADImage();
            CADImageObject * cadImage = static_cast<CADImageObject *>(
                    readedObject.get());

            unique_ptr<CADImageDefObject> cadImageDef(static_cast<CADImageDefObject *>(
                                                               GetObject(cadImage->hImageDef.getAsLong())));


            image->setClippingBoundaryType(cadImage->dClipBoundaryType);
            image->setFilePath(cadImageDef->sFilePath);
            image->setVertInsertionPoint(cadImage->vertInsertion);
            CADVector imageSize(cadImage->dfSizeX, cadImage->dfSizeY);
            image->setImageSize(imageSize);
            CADVector imageSizeInPx(cadImageDef->dfXImageSizeInPx, cadImageDef->dfYImageSizeInPx);
            image->setImageSizeInPx(imageSizeInPx);
            CADVector pixelSizeInACADUnits(cadImageDef->dfXPixelSize, cadImageDef->dfYPixelSize);
            image->setPixelSizeInACADUnits(pixelSizeInACADUnits);
            image->setResolutionUnits((CADImage::ResolutionUnit) cadImageDef->dResUnits);
            image->setOptions(cadImage->dDisplayProps & 0x08, cadImage->bClipping, cadImage->dBrightness,
                               cadImage->dContrast);
            for (const CADVector& clipPt :  cadImage->avertClippingPolygonVertexes)
            {
                image->addClippingPoint(clipPt);
            }

            poGeometry = image;
            break;
        }

        case CADObject::MLINE:
        {
            CADMLine       * mline    = new CADMLine();
            CADMLineObject * cadmLine = static_cast<CADMLineObject *>(
                    readedObject.get());

            mline->setScale(cadmLine->dfScale);
            mline->setOpened(cadmLine->dOpenClosed == 1 ? true : false);
            for (const CADMLineVertex& vertex : cadmLine->avertVertexes)
                mline->addVertex(vertex.vertPosition);

            poGeometry = mline;
            break;
        }

        case CADObject::MTEXT:
        {
            CADMText       * mtext    = new CADMText();
            CADMTextObject * cadmText = static_cast<CADMTextObject *>(
                    readedObject.get());

            mtext->setTextValue(cadmText->sTextValue);
            mtext->setXAxisAng(cadmText->vectXAxisDir.getX()); //TODO: is this needed?

            mtext->setPosition(cadmText->vertInsertionPoint);
            mtext->setExtrusion(cadmText->vectExtrusion);

            mtext->setHeight(cadmText->dfTextHeight);
            mtext->setRectWidth(cadmText->dfRectWidth);
            mtext->setExtents(cadmText->dfExtents);
            mtext->setExtentsWidth(cadmText->dfExtentsWidth);

            poGeometry = mtext;
            break;
        }

        case CADObject::POLYLINE_PFACE:
        {
            CADPolylinePFace       * polyline                  = new CADPolylinePFace();
            CADPolylinePFaceObject * cadpolyPface              = static_cast<CADPolylinePFaceObject *>(
                    readedObject.get());

            // TODO: code can be much simplified if CADHandle will be used.
            // to do so, == and ++ operators should be implemented.
            unique_ptr<CADVertexPFaceObject> vertex;
            auto                             dCurrentEntHandle = cadpolyPface->hVertexes[0].getAsLong();
            auto                             dLastEntHandle    = cadpolyPface->hVertexes[1].getAsLong();
            while(true)
            {
                vertex.reset(static_cast<CADVertexPFaceObject *>(
                                      GetObject(dCurrentEntHandle)));
                /* TODO: this check is excessive, but if something goes wrong way -
             * some part of geometries will be parsed. */
                if (vertex == nullptr)
                    continue;

                polyline->addVertex(vertex->vertPosition);

                /* FIXME: somehow one more vertex which isnot presented is read.
             * so, checking the number of added vertexes */
                /*TODO: is this needed - check on real data
            if (polyline->hVertexes.size() == cadpolyPface->nNumVertexes)
            {
                delete(vertex);
                break;
            }*/

                if (vertex->stCed.bNoLinks)
                    ++dCurrentEntHandle;
                else
                    dCurrentEntHandle = vertex->stChed.hNextEntity.getAsLong(vertex->stCed.hObjectHandle);

                if (dCurrentEntHandle == dLastEntHandle)
                {
                    vertex.reset(static_cast<CADVertexPFaceObject *>(
                                          GetObject(dCurrentEntHandle)));
                    polyline->addVertex(vertex->vertPosition);
                    break;
                }
            }

            poGeometry = polyline;
            break;
        }

        case CADObject::XLINE:
        {
            CADXLine       * xline    = new CADXLine();
            CADXLineObject * cadxLine = static_cast<CADXLineObject *>(
                    readedObject.get());

            xline->setVectVector(cadxLine->vectVector);
            xline->setPosition(cadxLine->vertPosition);

            poGeometry = xline;
            break;
        }

        case CADObject::FACE3D:
        {
            CADFace3D       * face      = new CADFace3D();
            CAD3DFaceObject * cad3DFace = static_cast<CAD3DFaceObject *>(
                    readedObject.get());

            for (const CADVector& corner : cad3DFace->avertCorners)
                face->addCorner(corner);
            face->setInvisFlags(cad3DFace->dInvisFlags);

            poGeometry = face;
            break;
        }

        case CADObject::POLYLINE_MESH:
        case CADObject::VERTEX_MESH:
        case CADObject::VERTEX_PFACE_FACE:
        default:
            cerr << "Asked geometry has unsupported type." << endl;
            poGeometry = new CADUnknown();
            break;
    }

    if (poGeometry == nullptr)
        return nullptr;

    // Applying color
    if (readedObject->stCed.nCMColor == 256) // BYLAYER CASE
    {
        CADLayer& oCurrentLayer = this->GetLayer(iLayerIndex);
        poGeometry->setColor(CADACIColors[oCurrentLayer.getColor()]);
    }
    else if (readedObject->stCed.nCMColor <= 255 &&
             readedObject->stCed.nCMColor >= 0) // Excessive check until BYBLOCK case will not be implemented
    {
        poGeometry->setColor(CADACIColors[readedObject->stCed.nCMColor]);
    }

    // Applying EED
    // Casting object's EED to a vector of strings
    vector<string> asEED;
    for (auto      citer     = readedObject->stCed.aEED.cbegin(); citer != readedObject->stCed.aEED.cend(); ++citer)
    {
        string sEED = "";
        // Detect the type of EED entity
        switch (citer->acData[0])
        {
            case 0: // string
            {
                uint8_t nStrSize = citer->acData[1];
                // +2 = skip CodePage, no idea how to use it anyway
                for (size_t   i        = 0; i < nStrSize; ++i)
                {
                    sEED += citer->acData[i + 4];
                }
                break;
            }
            case 1: // invalid
            {
                DebugMsg("Error: EED obj type is 1, error in R2000::getGeometry()");
                break;
            }
            case 2: // { or }
            {
                sEED += citer->acData[1] == 0 ? '{' : '}';
                break;
            }
            case 3: // layer table ref
            {
                // FIXME: get CADHandle and return getAsLong() result.
                sEED += "Layer table ref (handle):";
                for (size_t i = 0; i < 8; ++i)
                {
                    sEED += citer->acData[i + 1];
                }
                break;
            }
            case 4: // binary chunk
            {
                uint8_t nChunkSize = citer->acData[1];
                sEED += "Binary chunk (chars):";
                for (size_t i = 0; i < nChunkSize; ++i)
                {
                    sEED += citer->acData[i + 2];
                }
                break;
            }
            case 5: // entity handle ref
            {
                // FIXME: get CADHandle and return getAsLong() result.
                sEED += "Entity handle ref (handle):";
                for (size_t i = 0; i < 8; ++i)
                {
                    sEED += citer->acData[i + 1];
                }
                break;
            }
            case 10:
            case 11:
            case 12:
            case 13:
            {
                sEED += "Point: {";
                double dfX = 0, dfY = 0, dfZ = 0;
                memcpy(& dfX, citer->acData.data() + 1, 8);
                memcpy(& dfY, citer->acData.data() + 9, 8);
                memcpy(& dfZ, citer->acData.data() + 17, 8);
                sEED += to_string(dfX);
                sEED += ';';
                sEED += to_string(dfY);
                sEED += ';';
                sEED += to_string(dfZ);
                sEED += '}';
                break;
            }
            case 40:
            case 41:
            case 42:
            {
                sEED += "Double:";
                double dfVal = 0;
                memcpy(& dfVal, citer->acData.data() + 1, 8);
                sEED += to_string(dfVal);
                break;
            }
            case 70:
            {
                sEED += "Short:";
                int16_t dVal = 0;
                memcpy(& dVal, citer->acData.data() + 1, 2);
                sEED += to_string(dVal);
                break;
            }
            case 71:
            {
                sEED += "Long Int:";
                int32_t dVal = 0;
                memcpy(& dVal, citer->acData.data() + 1, 4);
                sEED += to_string(dVal);
                break;
            }
            default:
            {
                DebugMsg("Error in parsing geometry EED: undefined typecode: %d", (int) citer->acData[0]);
            }
        }
        asEED.emplace_back(sEED);
    }

    // Getting block reference attributes.
    if (dBlockRefHandle != 0)
    {
        vector<CADAttrib>           blockRefAttributes;
        unique_ptr<CADInsertObject> spoBlockRef(static_cast<CADInsertObject *>(GetObject(dBlockRefHandle)));

        if (spoBlockRef->hAttribs.size() != 0)
        {
            int32_t dCurrentEntHandle = spoBlockRef->hAttribs[0].getAsLong();
            int32_t dLastEntHandle    = spoBlockRef->hAttribs[0].getAsLong();

            while(spoBlockRef->bHasAttribs)
            {
                // FIXME: memory leak, somewhere in CAD* destructor is a bug
                CADEntityObject * attDefObj = static_cast<CADEntityObject *>(
                        GetObject(dCurrentEntHandle, true));

                if (dCurrentEntHandle == dLastEntHandle)
                {
                    if (attDefObj == nullptr)
                        break;

                    CADAttrib * attrib = static_cast<CADAttrib *>(
                            GetGeometry(iLayerIndex, dCurrentEntHandle));

                    if (attrib)
                    {
                        blockRefAttributes.push_back(CADAttrib(* attrib));
                        delete attrib;
                    }
                    delete attDefObj;
                    break;
                }

                if (attDefObj != nullptr)
                {
                    if (attDefObj->stCed.bNoLinks)
                        ++dCurrentEntHandle;
                    else
                        dCurrentEntHandle = attDefObj->stChed.hNextEntity.getAsLong(attDefObj->stCed.hObjectHandle);

                    CADAttrib * attrib = static_cast<CADAttrib *>(
                            GetGeometry(iLayerIndex, dCurrentEntHandle));

                    if (attrib)
                    {
                        blockRefAttributes.push_back(CADAttrib(* attrib));
                        delete attrib;
                    }
                    delete attDefObj;
                } else
                {
                    assert (0);
                }
            }
            poGeometry->setBlockAttributes(blockRefAttributes);
        }
    }

    poGeometry->setEED(asEED);
    return poGeometry;
}

CADBlockObject * DWGFileR2000::getBlock(CADBitStreamReader& reader, int32_t objectSize, struct CADCommonED stCommonEntityData)
{
    CADBlockObject * pBlock = new CADBlockObject();

    pBlock->setSize(objectSize);
    pBlock->stCed = stCommonEntityData;

    pBlock->sBlockName = reader.ReadTv();

    fillCommonEntityHandleData(reader, pBlock);

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    pBlock->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG

    return pBlock;
}

CADEllipseObject * DWGFileR2000::getEllipse(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADEllipseObject * ellipse = new CADEllipseObject();

    ellipse->setSize(objectSize);
    ellipse->stCed = stCommonEntityData;

    CADVector vertPosition = reader.ReadVector();

    ellipse->vertPosition = vertPosition;

    CADVector vectSMAxis = reader.ReadVector();

    ellipse->vectSMAxis = vectSMAxis;

    CADVector vectExtrusion = reader.ReadVector();

    ellipse->vectExtrusion = vectExtrusion;

    ellipse->dfAxisRatio = reader.ReadBitDouble();
    ellipse->dfBegAngle  = reader.ReadBitDouble();
    ellipse->dfEndAngle  = reader.ReadBitDouble();

    fillCommonEntityHandleData(reader, ellipse);

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    ellipse->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG

    return ellipse;
}

CADSolidObject * DWGFileR2000::getSolid(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADSolidObject * solid = new CADSolidObject();

    solid->setSize(objectSize);
    solid->stCed = stCommonEntityData;

    solid->dfThickness = reader.ReadBit() ? 0.0f : reader.ReadBitDouble();

    solid->dfElevation = reader.ReadBitDouble();

    CADVector   oCorner;
    for (size_t i      = 0; i < 4; ++i)
    {
        oCorner.setX(reader.ReadRawDouble());
        oCorner.setY(reader.ReadRawDouble());
        solid->avertCorners.push_back(oCorner);
    }

    if (reader.ReadBit())
    {
        solid->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    }
    else
    {
        CADVector vectExtrusion = reader.ReadVector();
        solid->vectExtrusion = vectExtrusion;
    }

    fillCommonEntityHandleData(reader, solid);

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    solid->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG

    return solid;
}

CADPointObject * DWGFileR2000::getPoint(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADPointObject * point = new CADPointObject();

    point->setSize(objectSize);
    point->stCed = stCommonEntityData;

    CADVector vertPosition = reader.ReadVector();

    point->vertPosition = vertPosition;

    point->dfThickness = reader.ReadBit() ? 0.0f : reader.ReadBitDouble();

    if (reader.ReadBit())
    {
        point->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    } else
    {
        CADVector vectExtrusion = reader.ReadVector();
        point->vectExtrusion = vectExtrusion;
    }

    point->dfXAxisAng = reader.ReadBitDouble();

    fillCommonEntityHandleData(reader, point);

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    point->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG

    return point;
}

CADPolyline3DObject * DWGFileR2000::getPolyLine3D(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADPolyline3DObject * polyline = new CADPolyline3DObject();

    polyline->setSize(objectSize);
    polyline->stCed = stCommonEntityData;

	polyline->SplinedFlags = reader.ReadChar();
	if (polyline->SplinedFlags & 0x1 || polyline->SplinedFlags & 0x2) // quadratic or cubic spline fit 
		polyline->bSplined = true;
	else
		polyline->bSplined = false;

	polyline->ClosedFlags = reader.ReadChar();
	if (polyline->ClosedFlags & 0x1)
		polyline->bClosed = true;
	else
		polyline->bClosed = false;

    fillCommonEntityHandleData(reader, polyline);

    polyline->hVertexes.push_back(reader.ReadHandle()); // 1st vertex
    polyline->hVertexes.push_back(reader.ReadHandle()); // last vertex

    polyline->hSeqend = reader.ReadHandle();

    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    polyline->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG

    return polyline;
}

CADRayObject * DWGFileR2000::getRay(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADRayObject * ray = new CADRayObject();

    ray->setSize(objectSize);
    ray->stCed = stCommonEntityData;

    CADVector vertPosition = reader.ReadVector();

    ray->vertPosition = vertPosition;

    CADVector vectVector = reader.ReadVector();
    ray->vectVector = vectVector;

    fillCommonEntityHandleData(reader, ray);

    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    ray->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG

    return ray;
}

CADXLineObject * DWGFileR2000::getXLine(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADXLineObject * xline = new CADXLineObject();

    xline->setSize(objectSize);
    xline->stCed = stCommonEntityData;

    CADVector vertPosition = reader.ReadVector();

    xline->vertPosition = vertPosition;

    CADVector vectVector = reader.ReadVector();
    xline->vectVector = vectVector;

    fillCommonEntityHandleData(reader, xline);

    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    xline->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG

    return xline;
}

CADLineObject * DWGFileR2000::getLine(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADLineObject * line = new CADLineObject();

    line->setSize(objectSize);
    line->stCed = stCommonEntityData;

    bool bZsAreZeros = reader.ReadBit();

    CADVector vertStart, vertEnd;
    vertStart.setX(reader.ReadRawDouble());
    vertEnd.setX(reader.ReadBitDouble());
    vertStart.setY(reader.ReadRawDouble());
    vertEnd.setY(reader.ReadBitDouble());

    if (!bZsAreZeros)
    {
        vertStart.setZ(reader.ReadBitDouble());
        vertEnd.setZ(reader.ReadBitDouble());
    }

    line->vertStart = vertStart;
    line->vertEnd   = vertEnd;

    line->dfThickness = reader.ReadBit() ? 0.0f : reader.ReadBitDouble();

    if (reader.ReadBit())
    {
        line->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    } else
    {
        CADVector vectExtrusion = reader.ReadVector();
        line->vectExtrusion = vectExtrusion;
    }

    fillCommonEntityHandleData(reader, line);


    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    line->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG
    return line;
}

CADTextObject * DWGFileR2000::getText(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADTextObject * text = new CADTextObject();

    text->setSize(objectSize);
    text->stCed = stCommonEntityData;

    text->DataFlags = reader.ReadChar();

    if (!(text->DataFlags & 0x01))
        text->dfElevation = reader.ReadRawDouble();

    CADVector vertInsetionPoint = reader.ReadRawVector();

    text->vertInsetionPoint = vertInsetionPoint;

    if (!(text->DataFlags & 0x02))
    {
        double x, y;
        x = reader.ReadBitDouble();
        y = reader.ReadBitDouble();
        CADVector vertAlignmentPoint(x, y);
        text->vertAlignmentPoint = vertAlignmentPoint;
    }

    if (reader.ReadBit())
    {
        text->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    } else
    {
        CADVector vectExtrusion = reader.ReadVector();
        text->vectExtrusion = vectExtrusion;
    }

    text->dfThickness = reader.ReadBit() ? 0.0f : reader.ReadBitDouble();

    if (!(text->DataFlags & 0x04))
        text->dfObliqueAng  = reader.ReadRawDouble();
    if (!(text->DataFlags & 0x08))
        text->dfRotationAng = reader.ReadRawDouble();

    text->dfHeight = reader.ReadRawDouble();

    if (!(text->DataFlags & 0x10))
        text->dfWidthFactor = reader.ReadRawDouble();

    text->sTextValue = reader.ReadTv();

    if (!(text->DataFlags & 0x20))
        text->dGeneration = reader.ReadBitShort();
    if (!(text->DataFlags & 0x40))
        text->dHorizAlign = reader.ReadBitShort();
    if (!(text->DataFlags & 0x80))
        text->dVertAlign  = reader.ReadBitShort();

    fillCommonEntityHandleData(reader, text);

    text->hStyle = reader.ReadHandle();

    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    text->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG

    return text;
}

CADVertex2DObject * DWGFileR2000::getVertex2D(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
	CADVertex2DObject * vertex = new CADVertex2DObject();

	vertex->setSize(objectSize);
	vertex->stCed = stCommonEntityData;

	vertex->vFlags = reader.ReadChar();

	CADVector vertPosition = reader.ReadVector();
	vertex->vertPosition = vertPosition; // z must be taken from polyline elevation

	vertex->dfStartWidth = reader.ReadBitDouble();
	if (vertex->dfStartWidth < 0) // if negative, abs start value is applicable for both start and end
	{
		vertex->dfStartWidth = std::fabs(vertex->dfStartWidth);
		vertex->dfEndWidth = std::fabs(vertex->dfStartWidth);
	}
	else
		vertex->dfEndWidth = reader.ReadBitDouble();
	
	vertex->dfBulge = reader.ReadBitDouble();

	vertex->dfTangentDir = reader.ReadBitDouble();


	fillCommonEntityHandleData(reader, vertex);

	reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
	vertex->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
	if ((reader.GetOffset() / 8) != (objectSize + 4))
		DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
		(reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG
	return vertex;
}

CADVertex3DObject * DWGFileR2000::getVertex3D(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADVertex3DObject * vertex = new CADVertex3DObject();

    vertex->setSize(objectSize);
    vertex->stCed = stCommonEntityData;

    vertex->vFlags = reader.ReadChar();

    CADVector vertPosition = reader.ReadVector();;
    vertex->vertPosition = vertPosition;

    fillCommonEntityHandleData(reader, vertex);

    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    vertex->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG
    return vertex;
}

CADCircleObject * DWGFileR2000::getCircle(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADCircleObject * circle = new CADCircleObject();

    circle->setSize(objectSize);
    circle->stCed = stCommonEntityData;

    CADVector vertPosition = reader.ReadVector();
    circle->vertPosition = vertPosition;
    circle->dfRadius     = reader.ReadBitDouble();
    circle->dfThickness  = reader.ReadBit() ? 0.0f : reader.ReadBitDouble();

    if (reader.ReadBit())
    {
        circle->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    } else
    {
        CADVector vectExtrusion = reader.ReadVector();
        circle->vectExtrusion = vectExtrusion;
    }

    fillCommonEntityHandleData(reader, circle);

    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    circle->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG
    return circle;
}

CADEndblkObject * DWGFileR2000::getEndBlock(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADEndblkObject * endblk = new CADEndblkObject();

    endblk->setSize(objectSize);
    endblk->stCed = stCommonEntityData;

    fillCommonEntityHandleData(reader, endblk);

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    endblk->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return endblk;
}

CADPolyline2DObject * DWGFileR2000::getPolyline2D(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADPolyline2DObject * polyline = new CADPolyline2DObject();

    polyline->setSize(objectSize);
    polyline->stCed = stCommonEntityData;

    polyline->dFlags                = reader.ReadBitShort();
    polyline->dCurveNSmoothSurfType = reader.ReadBitShort();

	if (polyline->dFlags & 0x1)
		polyline->bClosed = true;
	else
		polyline->bClosed = false;

	if (polyline->dFlags & 0x4) // spline fit
		polyline->bSplined = true;
	else
		polyline->bSplined = false;


    polyline->dfStartWidth = reader.ReadBitDouble();
    polyline->dfEndWidth   = reader.ReadBitDouble();

    polyline->dfThickness = reader.ReadBit() ? 0.0f : reader.ReadBitDouble();

    polyline->dfElevation = reader.ReadBitDouble();

    if (reader.ReadBit())
    {
        polyline->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    } else
    {
        CADVector vectExtrusion = reader.ReadVector();
        polyline->vectExtrusion = vectExtrusion;
    }

    fillCommonEntityHandleData(reader, polyline);

    polyline->hVertexes.push_back(reader.ReadHandle()); // 1st vertex
    polyline->hVertexes.push_back(reader.ReadHandle()); // last vertex

    polyline->hSeqend = reader.ReadHandle();

    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    polyline->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG
    return polyline;
}

CADAttribObject * DWGFileR2000::getAttributes(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADAttribObject * attrib = new CADAttribObject();

    attrib->setSize(objectSize);
    attrib->stCed     = stCommonEntityData;
    attrib->DataFlags = reader.ReadChar();

    if (!(attrib->DataFlags & 0x01))
        attrib->dfElevation = reader.ReadRawDouble();

    double x, y;

    CADVector vertInsetionPoint = reader.ReadRawVector();
    attrib->vertInsetionPoint = vertInsetionPoint;

    if (!(attrib->DataFlags & 0x02))
    {
        x = reader.ReadBitDouble();
        y = reader.ReadBitDouble();
        CADVector vertAlignmentPoint(x, y);
        attrib->vertAlignmentPoint = vertAlignmentPoint;
    }

    if (reader.ReadBit())
    {
        attrib->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    } else
    {
        CADVector vectExtrusion = reader.ReadVector();
        attrib->vectExtrusion = vectExtrusion;
    }

    attrib->dfThickness = reader.ReadBit() ? 0.0f : reader.ReadBitDouble();

    if (!(attrib->DataFlags & 0x04))
        attrib->dfObliqueAng  = reader.ReadRawDouble();
    if (!(attrib->DataFlags & 0x08))
        attrib->dfRotationAng = reader.ReadRawDouble();
    attrib->dfHeight          = reader.ReadRawDouble();
    if (!(attrib->DataFlags & 0x10))
        attrib->dfWidthFactor = reader.ReadRawDouble();
    attrib->sTextValue        = reader.ReadTv();
    if (!(attrib->DataFlags & 0x20))
        attrib->dGeneration   = reader.ReadBitShort();
    if (!(attrib->DataFlags & 0x40))
        attrib->dHorizAlign   = reader.ReadBitShort();
    if (!(attrib->DataFlags & 0x80))
        attrib->dVertAlign    = reader.ReadBitShort();

    attrib->sTag         = reader.ReadTv();
    attrib->nFieldLength = reader.ReadBitShort();
    attrib->nFlags       = reader.ReadChar();

    fillCommonEntityHandleData(reader, attrib);

    attrib->hStyle = reader.ReadHandle();

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    attrib->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return attrib;
}

CADAttdefObject * DWGFileR2000::getAttributesDefn(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADAttdefObject * attdef = new CADAttdefObject();

    attdef->setSize(objectSize);
    attdef->stCed     = stCommonEntityData;
    attdef->DataFlags = reader.ReadChar();

    if (!(attdef->DataFlags & 0x01))
        attdef->dfElevation = reader.ReadRawDouble();

    CADVector vertInsetionPoint = reader.ReadRawVector();
    attdef->vertInsetionPoint = vertInsetionPoint;

    if (!(attdef->DataFlags & 0x02))
    {
        double    x = reader.ReadBitDouble();
        double    y = reader.ReadBitDouble();
        CADVector vertAlignmentPoint(x, y);
        attdef->vertAlignmentPoint = vertAlignmentPoint;
    }

    if (reader.ReadBit())
    {
        attdef->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    } else
    {
        CADVector vectExtrusion = reader.ReadVector();
        attdef->vectExtrusion = vectExtrusion;
    }

    attdef->dfThickness = reader.ReadBit() ? 0.0f : reader.ReadBitDouble();

    if (!(attdef->DataFlags & 0x04))
        attdef->dfObliqueAng  = reader.ReadRawDouble();
    if (!(attdef->DataFlags & 0x08))
        attdef->dfRotationAng = reader.ReadRawDouble();
    attdef->dfHeight          = reader.ReadRawDouble();
    if (!(attdef->DataFlags & 0x10))
        attdef->dfWidthFactor = reader.ReadRawDouble();
    attdef->sTextValue        = reader.ReadTv();
    if (!(attdef->DataFlags & 0x20))
        attdef->dGeneration   = reader.ReadBitShort();
    if (!(attdef->DataFlags & 0x40))
        attdef->dHorizAlign   = reader.ReadBitShort();
    if (!(attdef->DataFlags & 0x80))
        attdef->dVertAlign    = reader.ReadBitShort();

    attdef->sTag         = reader.ReadTv();
    attdef->nFieldLength = reader.ReadBitShort();
    attdef->nFlags       = reader.ReadChar();

    attdef->sPrompt = reader.ReadTv();

    fillCommonEntityHandleData(reader, attdef);

    attdef->hStyle = reader.ReadHandle();

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    attdef->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return attdef;
}

CADLWPolylineObject * DWGFileR2000::getLWPolyLine(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADLWPolylineObject * polyline = new CADLWPolylineObject();
    polyline->setSize(objectSize);
    polyline->stCed = stCommonEntityData;

    double x             = 0.0, y = 0.0;
    int    vertixesCount = 0, nBulges = 0, nNumWidths = 0;
    int16_t  dataFlag      = reader.ReadBitShort();
    if (dataFlag & 4)
        polyline->dfConstWidth = reader.ReadBitDouble();
    if (dataFlag & 8)
        polyline->dfElevation  = reader.ReadBitDouble();
    if (dataFlag & 2)
        polyline->dfThickness  = reader.ReadBitDouble();
    if (dataFlag & 1)
    {
        CADVector vectExtrusion = reader.ReadVector();
        polyline->vectExtrusion = vectExtrusion;
    }

    vertixesCount = reader.ReadBitLong();
    polyline->avertVertexes.reserve(vertixesCount);

    if (dataFlag & 16)
    {
        nBulges = reader.ReadBitLong();
        polyline->adfBulges.reserve(nBulges);
    }

    // TODO: tell ODA that R2000 contains nNumWidths flag
    if (dataFlag & 32)
    {
        nNumWidths = reader.ReadBitLong();
        polyline->astWidths.reserve(nNumWidths);
    }

    if (dataFlag & 512)
    {
        polyline->bClosed = true;
    } else
        polyline->bClosed = false;

    // First of all, read first vertex.
    CADVector vertex = reader.ReadRawVector();
    polyline->avertVertexes.push_back(vertex);

    // All the others are not raw doubles; bitdoubles with default instead,
    // where default is previous point coords.
    size_t   prev;
    for (int i       = 1; i < vertixesCount; ++i)
    {
        prev = size_t(i - 1);
        x    = reader.ReadBitDouble();
        y    = reader.ReadBitDouble();
        vertex.setX(x);
        vertex.setY(y);
        polyline->avertVertexes.push_back(vertex);
    }

    for (int i = 0; i < nBulges; ++i)
    {
        double dfBulgeValue = reader.ReadBitDouble();
        polyline->adfBulges.push_back(dfBulgeValue);
    }

    for (int i = 0; i < nNumWidths; ++i)
    {
        double dfStartWidth = reader.ReadBitDouble();
        double dfEndWidth   = reader.ReadBitDouble();
        polyline->astWidths.push_back(make_pair(dfStartWidth, dfEndWidth));
    }

    fillCommonEntityHandleData(reader, polyline);

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    polyline->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return polyline;
}

CADArcObject * DWGFileR2000::getArc(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADArcObject * arc = new CADArcObject();

    arc->setSize(objectSize);
    arc->stCed = stCommonEntityData;

    CADVector vertPosition = reader.ReadVector();
    arc->vertPosition = vertPosition;
    arc->dfRadius     = reader.ReadBitDouble();
    arc->dfThickness  = reader.ReadBit() ? 0.0f : reader.ReadBitDouble();

    if (reader.ReadBit())
    {
        arc->vectExtrusion = CADVector(0.0f, 0.0f, 1.0f);
    } else
    {
        CADVector vectExtrusion = reader.ReadVector();
        arc->vectExtrusion = vectExtrusion;
    }

    arc->dfStartAngle = reader.ReadBitDouble();
    arc->dfEndAngle   = reader.ReadBitDouble();

    fillCommonEntityHandleData(reader, arc);

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    arc->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG
    return arc;
}

CADSplineObject * DWGFileR2000::getSpline(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADSplineObject * spline = new CADSplineObject();
    spline->setSize(objectSize);
    spline->stCed     = stCommonEntityData;
    spline->dScenario = reader.ReadBitLong();
    spline->dDegree   = reader.ReadBitLong();

    if (spline->dScenario == 2)
    {
        spline->dfFitTol = reader.ReadBitDouble();
        CADVector vectBegTangDir = reader.ReadVector();
        spline->vectBegTangDir = vectBegTangDir;
        CADVector vectEndTangDir = reader.ReadVector();
        spline->vectEndTangDir = vectEndTangDir;

        spline->nNumFitPts = reader.ReadBitLong();
        spline->averFitPoints.reserve(spline->nNumFitPts);
    } else if (spline->dScenario == 1)
    {
        spline->bRational = reader.ReadBit();
        spline->bClosed   = reader.ReadBit();
        spline->bPeriodic = reader.ReadBit();
        spline->dfKnotTol = reader.ReadBitDouble();
        spline->dfCtrlTol = reader.ReadBitDouble();

        spline->nNumKnots = reader.ReadBitLong();
        spline->adfKnots.reserve(spline->nNumKnots);

        spline->nNumCtrlPts = reader.ReadBitLong();
        spline->avertCtrlPoints.reserve(spline->nNumCtrlPts);
        if (spline->bWeight) spline->adfCtrlPointsWeight.reserve(spline->nNumCtrlPts);

        spline->bWeight = reader.ReadBit();
    }
#ifdef _DEBUG
    else
    {
        DebugMsg("Spline scenario != {1,2} readed: error.");
    }
#endif
    for (int32_t i = 0; i < spline->nNumKnots; ++i)
        spline->adfKnots.push_back(reader.ReadBitDouble());
    for (int32_t i = 0; i < spline->nNumCtrlPts; ++i)
    {
        CADVector vertex = reader.ReadVector();
        spline->avertCtrlPoints.push_back(vertex);
        if (spline->bWeight)
            spline->adfCtrlPointsWeight.push_back(reader.ReadBitDouble());
    }
    for (int32_t i = 0; i < spline->nNumFitPts; ++i)
    {
        CADVector vertex = reader.ReadVector();
        spline->averFitPoints.push_back(vertex);
    }

    fillCommonEntityHandleData(reader, spline);

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    spline->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return spline;
}

CADEntityObject * DWGFileR2000::getEntity(CADBitStreamReader& reader, int objectType, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADEntityObject * entity = new CADEntityObject();

    entity->setType(static_cast<CADObject::ObjectType>(objectType));
    entity->setSize(objectSize);
    entity->stCed = stCommonEntityData;

    reader.SeekBits(entity->stCed.nObjectSizeInBits + 16);

    fillCommonEntityHandleData(reader, entity);

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    entity->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return entity;
}

CADInsertObject * DWGFileR2000::getInsert(CADBitStreamReader& reader, int32_t objectType, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADInsertObject * insert = new CADInsertObject();

    insert->setType(static_cast<CADObject::ObjectType>(objectType));
    insert->setSize(objectSize);
    insert->stCed = stCommonEntityData;

    insert->vertInsertionPoint = reader.ReadVector();
    uint8_t dataFlags = reader.Read2Bits();
    double        val41     = 1.0;
    double        val42     = 1.0;
    double        val43     = 1.0;
    if (dataFlags == 0)
    {
        val41 = reader.ReadRawDouble();
        val42 = reader.ReadBitDouble();
        val43 = reader.ReadBitDouble();
    } else if (dataFlags == 1)
    {
        val41 = 1.0;
        val42 = reader.ReadBitDouble();
        val43 = reader.ReadBitDouble();
    } else if (dataFlags == 2)
    {
        val41 = reader.ReadRawDouble();
        val42 = val41;
        val43 = val41;
    }
    insert->vertScales    = CADVector(val41, val42, val43);
    insert->dfRotation    = reader.ReadBitDouble();
    insert->vectExtrusion = reader.ReadVector();
    insert->bHasAttribs   = reader.ReadBit();

    fillCommonEntityHandleData(reader, insert);

    insert->hBlockHeader = reader.ReadHandle();
    if (insert->bHasAttribs)
    {
        insert->hAttribs.push_back(reader.ReadHandle());
        insert->hAttribs.push_back(reader.ReadHandle());
        insert->hSeqend = reader.ReadHandle();
    }

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    insert->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG

    return insert;
}

CADDictionaryObject * DWGFileR2000::getDictionary(CADBitStreamReader& reader, int32_t objectSize)
{
    /*
     * FIXME: ODA has a lot of mistypes in spec. for this objects,
     * it doesnt work for now (error begins in handles stream).
     * Nonetheless, dictionary->sItemNames is 100% array,
     * not a single obj as pointer by their docs.
     */
    CADDictionaryObject * dictionary = new CADDictionaryObject();

    dictionary->setSize(objectSize);
    dictionary->nObjectSizeInBits = reader.ReadRawLong();
    dictionary->hObjectHandle     = reader.ReadHandle();

    int16_t  eedSize = 0;
    CADEed dwgEed;
    while((eedSize = reader.ReadBitShort()) != 0)
    {
        dwgEed.dLength      = eedSize;
        dwgEed.hApplication = reader.ReadHandle();

        for (int16_t i = 0; i < eedSize; ++i)
        {
            dwgEed.acData.push_back(reader.ReadChar());
        }

        dictionary->aEED.push_back(dwgEed);
    }

    dictionary->nNumReactors   = reader.ReadBitShort();
    dictionary->nNumItems      = reader.ReadBitLong();
    dictionary->dCloningFlag   = reader.ReadBitShort();
    dictionary->dHardOwnerFlag = reader.ReadChar();

    for (int32_t i = 0; i < dictionary->nNumItems; ++i)
        dictionary->sItemNames.push_back(reader.ReadTv());

    dictionary->hParentHandle = reader.ReadHandle();

    for (int32_t i = 0; i < dictionary->nNumReactors; ++i)
        dictionary->hReactors.push_back(reader.ReadHandle());
    dictionary->hXDictionary = reader.ReadHandle();
    for (int32_t i = 0; i < dictionary->nNumItems; ++i)
        dictionary->hItemHandles.push_back(reader.ReadHandle());

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    dictionary->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif // _DEBUG
    return dictionary;
}

CADLayerObject * DWGFileR2000::getLayerObject(CADBitStreamReader& reader, int32_t objectSize)
{
    CADLayerObject * layer = new CADLayerObject();

    layer->setSize(objectSize);
    layer->nObjectSizeInBits = reader.ReadRawLong();
    layer->hObjectHandle     = reader.ReadHandle();

    int16_t  eedSize = 0;
    CADEed dwgEed;
    while((eedSize = reader.ReadBitShort()) != 0)
    {
        dwgEed.dLength      = eedSize;
        dwgEed.hApplication = reader.ReadHandle();

        for (int16_t i = 0; i < eedSize; ++i)
        {
            dwgEed.acData.push_back(reader.ReadChar());
        }

        layer->aEED.push_back(dwgEed);
    }

    layer->nNumReactors = reader.ReadBitLong();
    layer->sLayerName   = reader.ReadTv();
    layer->b64Flag      = reader.ReadBit();
    layer->dXRefIndex   = reader.ReadBitShort();
    layer->bXDep        = reader.ReadBit();

    int16_t dFlags = reader.ReadBitShort();
    layer->bFrozen           = dFlags & 0x01;
    layer->bOn               = !(dFlags & 0x02); // bit seems to indicate off, rather than on (as in ODA spec)
    layer->bFrozenInNewVPORT = dFlags & 0x04;
    layer->bLocked           = dFlags & 0x08;
    layer->bPlottingFlag     = dFlags & 0x10;
    layer->dLineWeight       = dFlags & 0x03E0; //
    layer->dCMColor          = reader.ReadBitShort();
    layer->hLayerControl     = reader.ReadHandle();
    for (int32_t i = 0; i < layer->nNumReactors; ++i)
        layer->hReactors.push_back(reader.ReadHandle());
    layer->hXDictionary            = reader.ReadHandle();
    layer->hExternalRefBlockHandle = reader.ReadHandle();
    layer->hPlotStyle              = reader.ReadHandle();
    layer->hLType                  = reader.ReadHandle();

    /*
     * FIXME: ODA says that this handle should be null hard pointer. It is not.
     * Also, after reading it objectSize is != actual readed structure's size.
     * Not used anyway, so no point to read it for now.
     * It also means that CRC cannot be computed correctly.
     */
// layer->hUnknownHandle = reader.ReadHandle() ;

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    layer->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return layer;
}

CADLayerControlObject * DWGFileR2000::getLayerControl(CADBitStreamReader& reader, int32_t objectSize)
{
    CADLayerControlObject * layerControl = new CADLayerControlObject();

    layerControl->setSize(objectSize);
    layerControl->nObjectSizeInBits = reader.ReadRawLong();
    layerControl->hObjectHandle     = reader.ReadHandle();

    int16_t  eedSize = 0;
    CADEed dwgEed;
    while((eedSize = reader.ReadBitShort()) != 0)
    {

        dwgEed.dLength      = eedSize;
        dwgEed.hApplication = reader.ReadHandle();

        for (int16_t i = 0; i < eedSize; ++i)
        {
            dwgEed.acData.push_back(reader.ReadChar());
        }

        layerControl->aEED.push_back(dwgEed);
    }

    layerControl->nNumReactors = reader.ReadBitLong();
    layerControl->nNumEntries  = reader.ReadBitLong();
    layerControl->hNull        = reader.ReadHandle();
    layerControl->hXDictionary = reader.ReadHandle();
    for (int32_t i = 0; i < layerControl->nNumEntries; ++i)
        layerControl->hLayers.push_back(reader.ReadHandle());

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    layerControl->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif
    return layerControl;
}

CADBlockControlObject * DWGFileR2000::getBlockControl(CADBitStreamReader& reader, int32_t objectSize)
{
    CADBlockControlObject * blockControl = new CADBlockControlObject();

    blockControl->setSize(objectSize);
    blockControl->nObjectSizeInBits = reader.ReadRawLong();
    blockControl->hObjectHandle     = reader.ReadHandle();

    int16_t  eedSize = 0;
    CADEed dwgEed;
    while((eedSize = reader.ReadBitShort()) != 0)
    {
        dwgEed.dLength      = eedSize;
        dwgEed.hApplication = reader.ReadHandle();

        for (int16_t i = 0; i < eedSize; ++i)
        {
            dwgEed.acData.push_back(reader.ReadChar());
        }

        blockControl->aEED.push_back(dwgEed);
    }

    blockControl->nNumReactors = reader.ReadBitLong();
    blockControl->nNumEntries  = reader.ReadBitLong();

    blockControl->hNull        = reader.ReadHandle();
    blockControl->hXDictionary = reader.ReadHandle();

    for (int32_t i = 0; i < blockControl->nNumEntries + 2; ++i)
    {
        blockControl->hBlocks.push_back(reader.ReadHandle());
    }

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    blockControl->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return blockControl;
}

CADBlockHeaderObject * DWGFileR2000::getBlockHeader(CADBitStreamReader& reader, int32_t objectSize)
{
    CADBlockHeaderObject * blockHeader = new CADBlockHeaderObject();

    blockHeader->setSize(objectSize);
    blockHeader->nObjectSizeInBits = reader.ReadRawLong();
    blockHeader->hObjectHandle     = reader.ReadHandle();

    int16_t  eedSize;
    CADEed dwgEed;
    while((eedSize = reader.ReadBitShort()) != 0)
    {
        dwgEed.dLength      = eedSize;
        dwgEed.hApplication = reader.ReadHandle();

        for (int16_t i = 0; i < eedSize; ++i)
        {
            dwgEed.acData.push_back(reader.ReadChar());
        }

        blockHeader->aEED.push_back(dwgEed);
    }

    blockHeader->nNumReactors  = reader.ReadBitLong();
    blockHeader->sEntryName    = reader.ReadTv();
    blockHeader->b64Flag       = reader.ReadBit();
    blockHeader->dXRefIndex    = reader.ReadBitShort();
    blockHeader->bXDep         = reader.ReadBit();
    blockHeader->bAnonymous    = reader.ReadBit();
    blockHeader->bHasAtts      = reader.ReadBit();
    blockHeader->bBlkisXRef    = reader.ReadBit();
    blockHeader->bXRefOverlaid = reader.ReadBit();
    blockHeader->bLoadedBit    = reader.ReadBit();

    CADVector vertBasePoint = reader.ReadVector();
    blockHeader->vertBasePoint = vertBasePoint;
    blockHeader->sXRefPName    = reader.ReadTv();
    uint8_t Tmp;
    do
    {
        Tmp = reader.ReadChar();
        blockHeader->adInsertCount.push_back(Tmp);
    } while(Tmp != 0);

    blockHeader->sBlockDescription  = reader.ReadTv();
    blockHeader->nSizeOfPreviewData = reader.ReadBitLong();
    for (int32_t i = 0; i < blockHeader->nSizeOfPreviewData; ++i)
        blockHeader->abyBinaryPreviewData.push_back(reader.ReadChar());

    blockHeader->hBlockControl = reader.ReadHandle();
    for (int32_t i = 0; i < blockHeader->nNumReactors; ++i)
        blockHeader->hReactors.push_back(reader.ReadHandle());
    blockHeader->hXDictionary = reader.ReadHandle();
    blockHeader->hNull        = reader.ReadHandle();
    blockHeader->hBlockEntity = reader.ReadHandle();
    if (!blockHeader->bBlkisXRef && !blockHeader->bXRefOverlaid)
    {
        blockHeader->hEntities.push_back(reader.ReadHandle()); // first
        blockHeader->hEntities.push_back(reader.ReadHandle()); // last
    }

    blockHeader->hEndBlk = reader.ReadHandle();
    for (size_t i = 0; i < blockHeader->adInsertCount.size() - 1; ++i)
        blockHeader->hInsertHandles.push_back(reader.ReadHandle());
    blockHeader->hLayout = reader.ReadHandle();

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    blockHeader->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return blockHeader;
}

CADLineTypeControlObject * DWGFileR2000::getLineTypeControl(CADBitStreamReader& reader, int32_t objectSize)
{
    CADLineTypeControlObject * ltypeControl = new CADLineTypeControlObject();
    ltypeControl->setSize(objectSize);
    ltypeControl->nObjectSizeInBits = reader.ReadRawLong();
    ltypeControl->hObjectHandle     = reader.ReadHandle();

    int16_t  eedSize = 0;
    CADEed dwgEed;
    while((eedSize = reader.ReadBitShort()) != 0)
    {
        dwgEed.dLength      = eedSize;
        dwgEed.hApplication = reader.ReadHandle();

        for (int16_t i = 0; i < eedSize; ++i)
        {
            dwgEed.acData.push_back(reader.ReadChar());
        }

        ltypeControl->aEED.push_back(dwgEed);
    }

    ltypeControl->nNumReactors = reader.ReadBitLong();
    ltypeControl->nNumEntries  = reader.ReadBitLong();

    ltypeControl->hNull        = reader.ReadHandle();
    ltypeControl->hXDictionary = reader.ReadHandle();

    // hLTypes ends with BYLAYER and BYBLOCK
    for (int32_t i = 0; i < ltypeControl->nNumEntries + 2; ++i)
        ltypeControl->hLTypes.push_back(reader.ReadHandle());

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    ltypeControl->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return ltypeControl;
}

CADLineTypeObject * DWGFileR2000::getLineType1(CADBitStreamReader& reader, int32_t objectSize)
{
    CADLineTypeObject * ltype = new CADLineTypeObject();

    ltype->setSize(objectSize);
    ltype->nObjectSizeInBits = reader.ReadRawLong();
    ltype->hObjectHandle     = reader.ReadHandle();
    int16_t  eedSize = 0;
    CADEed dwgEed;
    while((eedSize = reader.ReadBitShort()) != 0)
    {
        dwgEed.dLength      = eedSize;
        dwgEed.hApplication = reader.ReadHandle();

        for (int16_t i = 0; i < eedSize; ++i)
        {
            dwgEed.acData.push_back(reader.ReadChar());
        }

        ltype->aEED.push_back(dwgEed);
    }

    ltype->nNumReactors = reader.ReadBitLong();
    ltype->sEntryName   = reader.ReadTv();
    ltype->b64Flag      = reader.ReadBit();
    ltype->dXRefIndex   = reader.ReadBitShort();
    ltype->bXDep        = reader.ReadBit();
    ltype->sDescription = reader.ReadTv();
    ltype->dfPatternLen = reader.ReadBitDouble();
    ltype->dAlignment   = reader.ReadChar();
    ltype->nNumDashes   = reader.ReadChar();

    CADDash     dash;
    for (size_t i       = 0; i < ltype->nNumDashes; ++i)
    {
        dash.dfLength          = reader.ReadBitDouble();
        dash.dComplexShapecode = reader.ReadBitShort();
        dash.dfXOffset         = reader.ReadRawDouble();
        dash.dfYOffset         = reader.ReadRawDouble();
        dash.dfScale           = reader.ReadBitDouble();
        dash.dfRotation        = reader.ReadBitDouble();
        dash.dShapeflag        = reader.ReadBitShort(); // TODO: what to do with it?

        ltype->astDashes.push_back(dash);
    }

    for (int16_t i = 0; i < 256; ++i)
        ltype->abyTextArea.push_back(reader.ReadChar());

    ltype->hLTControl = reader.ReadHandle();

    for (int32_t i = 0; i < ltype->nNumReactors; ++i)
        ltype->hReactors.push_back(reader.ReadHandle());

    ltype->hXDictionary = reader.ReadHandle();
    ltype->hXRefBlock   = reader.ReadHandle();

    // TODO: shapefile for dash/shape (1 each). Does it mean that we have nNumDashes * 2 handles, or what?

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    ltype->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("[NOT IMPORTANT, CAUSE NOT IMPLEMENTATION NOT COMPLETED] "
                          "Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return ltype;
}

CADMLineObject * DWGFileR2000::getMLine(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADMLineObject * mline = new CADMLineObject();

    mline->setSize(objectSize);
    mline->stCed = stCommonEntityData;

    mline->dfScale = reader.ReadBitDouble();
    mline->dJust   = reader.ReadChar();

    CADVector vertBasePoint = reader.ReadVector();
    mline->vertBasePoint = vertBasePoint;

    CADVector vectExtrusion = reader.ReadVector();
    mline->vectExtrusion = vectExtrusion;
    mline->dOpenClosed   = reader.ReadBitShort();
    mline->nLinesInStyle = reader.ReadChar();
    mline->nNumVertexes  = reader.ReadBitShort();

    CADMLineVertex stVertex;
    CADLineStyle   stLStyle;
    for (int32_t      i     = 0; i < mline->nNumVertexes; ++i)
    {
        CADVector vertPosition = reader.ReadVector();
        stVertex.vertPosition = vertPosition;

        CADVector vectDirection = reader.ReadVector();
        stVertex.vectDirection = vectDirection;

        CADVector vectMIterDirection = reader.ReadVector();
        stVertex.vectMIterDirection = vectMIterDirection;
        for (size_t j = 0; j < mline->nLinesInStyle; ++j)
        {
            stLStyle.nNumSegParms = reader.ReadBitShort();
            for (int16_t k = 0; k < stLStyle.nNumSegParms; ++k)
                stLStyle.adfSegparms.push_back(reader.ReadBitDouble());
            stLStyle.nAreaFillParms = reader.ReadBitShort();
            for (int16_t k = 0; k < stLStyle.nAreaFillParms; ++k)
                stLStyle.adfAreaFillParameters.push_back(reader.ReadBitDouble());

            stVertex.astLStyles.push_back(stLStyle);
        }
        mline->avertVertexes.push_back(stVertex);
    }

    if (mline->stCed.bbEntMode == 0)
        mline->stChed.hOwner = reader.ReadHandle();

    for (int32_t i = 0; i < mline->stCed.nNumReactors; ++i)
        mline->stChed.hReactors.push_back(reader.ReadHandle());

    mline->stChed.hXDictionary = reader.ReadHandle();

    if (!mline->stCed.bNoLinks)
    {
        mline->stChed.hPrevEntity = reader.ReadHandle();
        mline->stChed.hNextEntity = reader.ReadHandle();
    }

    mline->stChed.hLayer = reader.ReadHandle();

    if (mline->stCed.bbLTypeFlags == 0x03)
        mline->stChed.hLType = reader.ReadHandle();

    if (mline->stCed.bbPlotStyleFlags == 0x03)
        mline->stChed.hPlotStyle = reader.ReadHandle();

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    mline->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return mline;
}

CADPolylinePFaceObject * DWGFileR2000::getPolylinePFace(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADPolylinePFaceObject * polyline = new CADPolylinePFaceObject();

    polyline->setSize(objectSize);
    polyline->stCed = stCommonEntityData;

    polyline->nNumVertexes = reader.ReadBitShort();
    polyline->nNumFaces    = reader.ReadBitShort();

    fillCommonEntityHandleData(reader, polyline);

    polyline->hVertexes.push_back(reader.ReadHandle()); // 1st vertex
    polyline->hVertexes.push_back(reader.ReadHandle()); // last vertex

    polyline->hSeqend = reader.ReadHandle();

    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    polyline->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return polyline;
}

CADImageObject * DWGFileR2000::getImage(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADImageObject * image = new CADImageObject();

    image->setSize(objectSize);
    image->stCed = stCommonEntityData;

    image->dClassVersion = reader.ReadBitLong();

    CADVector vertInsertion = reader.ReadVector();
    image->vertInsertion = vertInsertion;

    CADVector vectUDirection = reader.ReadVector();
    image->vectUDirection = vectUDirection;

    CADVector vectVDirection = reader.ReadVector();
    image->vectVDirection = vectVDirection;

    image->dfSizeX       = reader.ReadRawDouble();
    image->dfSizeY       = reader.ReadRawDouble();
    image->dDisplayProps = reader.ReadBitShort();

    image->bClipping         = reader.ReadBit();
    image->dBrightness       = reader.ReadChar();
    image->dContrast         = reader.ReadChar();
    image->dFade             = reader.ReadChar();
    image->dClipBoundaryType = reader.ReadBitShort();

    if (image->dClipBoundaryType == 1)
    {
        CADVector vertPoint1 = reader.ReadRawVector();
        image->avertClippingPolygonVertexes.push_back(vertPoint1);

        CADVector vertPoint2 = reader.ReadRawVector();
        image->avertClippingPolygonVertexes.push_back(vertPoint2);
    } else
    {
        image->nNumberVertexesInClipPolygon = reader.ReadBitLong();

        for (int32_t i = 0; i < image->nNumberVertexesInClipPolygon; ++i)
        {
            CADVector vertPoint = reader.ReadRawVector();
            image->avertClippingPolygonVertexes.push_back(vertPoint);
        }
    }

    fillCommonEntityHandleData(reader, image);

    image->hImageDef        = reader.ReadHandle();
    image->hImageDefReactor = reader.ReadHandle();

    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    image->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG

    return image;
}

CAD3DFaceObject * DWGFileR2000::get3DFace(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CAD3DFaceObject * face = new CAD3DFaceObject();

    face->setSize(objectSize);
    face->stCed = stCommonEntityData;

    face->bHasNoFlagInd = reader.ReadBit();
    face->bZZero        = reader.ReadBit();

    double x, y, z;

    CADVector vertex = reader.ReadRawVector();
    if (!face->bZZero)
    {
        z = reader.ReadRawDouble();
        vertex.setZ(z);
    }
    face->avertCorners.push_back(vertex);
    for (size_t i = 1; i < 4; ++i)
    {
        x = reader.ReadBitDouble();
        y = reader.ReadBitDouble();
        z = reader.ReadBitDouble();

        CADVector corner(x, y, z);
        face->avertCorners.push_back(corner);
    }

    if (!face->bHasNoFlagInd)
        face->dInvisFlags = reader.ReadBitShort();

    fillCommonEntityHandleData(reader, face);

    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    face->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif //_DEBUG
    return face;
}

CADVertexMeshObject * DWGFileR2000::getVertexMesh(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADVertexMeshObject * vertex = new CADVertexMeshObject();

    vertex->setSize(objectSize);
    vertex->stCed = stCommonEntityData;

    /*uint8_t Flags = */reader.ReadChar();
    CADVector vertPosition = reader.ReadVector();
    vertex->vertPosition = vertPosition;

    fillCommonEntityHandleData(reader, vertex);

    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    vertex->setCRC(reader.ReadRawShort());

#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif
    return vertex;
}

CADVertexPFaceObject * DWGFileR2000::getVertexPFace(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADVertexPFaceObject * vertex = new CADVertexPFaceObject();

    vertex->setSize(objectSize);
    vertex->stCed = stCommonEntityData;

    /*uint8_t Flags = */reader.ReadChar();
    CADVector vertPosition = reader.ReadVector();
    vertex->vertPosition = vertPosition;

    fillCommonEntityHandleData(reader, vertex);

    reader.SeekBits(8 - (reader.GetOffset() % 8)); // padding bits to next byte boundary
    vertex->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif
    return vertex;
}

CADMTextObject * DWGFileR2000::getMText(CADBitStreamReader& reader, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADMTextObject * text = new CADMTextObject();

    text->setSize(objectSize);
    text->stCed = stCommonEntityData;

    CADVector vertInsertionPoint = reader.ReadVector();
    text->vertInsertionPoint = vertInsertionPoint;

    CADVector vectExtrusion = reader.ReadVector();
    text->vectExtrusion = vectExtrusion;

    CADVector vectXAxisDir = reader.ReadVector();
    text->vectXAxisDir = vectXAxisDir;

    text->dfRectWidth        = reader.ReadBitDouble();
    text->dfTextHeight       = reader.ReadBitDouble();
    text->dAttachment        = reader.ReadBitShort();
    text->dDrawingDir        = reader.ReadBitShort();
    text->dfExtents          = reader.ReadBitDouble();
    text->dfExtentsWidth     = reader.ReadBitDouble();
    text->sTextValue         = reader.ReadTv();
    text->dLineSpacingStyle  = reader.ReadBitShort();
    text->dLineSpacingFactor = reader.ReadBitDouble();
    text->bUnknownBit        = reader.ReadBit();

    fillCommonEntityHandleData(reader, text);

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    text->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif
    return text;
}

CADDimensionObject * DWGFileR2000::getDimension(CADBitStreamReader& reader, int32_t objectType, int32_t objectSize, CADCommonED stCommonEntityData)
{
    CADCommonDimensionData stCDD;

    CADVector vectExtrusion = reader.ReadVector();
    stCDD.vectExtrusion = vectExtrusion;

    CADVector vertTextMidPt = reader.ReadRawVector();
    stCDD.vertTextMidPt = vertTextMidPt;

    stCDD.dfElevation = reader.ReadBitDouble();
    stCDD.dFlags      = reader.ReadChar();

    stCDD.sUserText      = reader.ReadTv();
    stCDD.dfTextRotation = reader.ReadBitDouble();
    stCDD.dfHorizDir     = reader.ReadBitDouble();

    stCDD.dfInsXScale   = reader.ReadBitDouble();
    stCDD.dfInsYScale   = reader.ReadBitDouble();
    stCDD.dfInsZScale   = reader.ReadBitDouble();
    stCDD.dfInsRotation = reader.ReadBitDouble();

    stCDD.dAttachmentPoint    = reader.ReadBitShort();
    stCDD.dLineSpacingStyle   = reader.ReadBitShort();
    stCDD.dfLineSpacingFactor = reader.ReadBitDouble();
    stCDD.dfActualMeasurement = reader.ReadBitDouble();

    CADVector vert12Pt = reader.ReadRawVector();
    stCDD.vert12Pt = vert12Pt;

    switch (objectType)
    {
        case CADObject::DIMENSION_ORDINATE:
        {
            CADDimensionOrdinateObject * dimension = new CADDimensionOrdinateObject();

            dimension->setSize(objectSize);
            dimension->stCed = stCommonEntityData;
            dimension->cdd   = stCDD;

            CADVector vert10pt = reader.ReadVector();
            dimension->vert10pt = vert10pt;

            CADVector vert13pt = reader.ReadVector();
            dimension->vert13pt = vert13pt;

            CADVector vert14pt = reader.ReadVector();
            dimension->vert14pt = vert14pt;

            dimension->Flags2 = reader.ReadChar();

            fillCommonEntityHandleData(reader, dimension);

            dimension->hDimstyle       = reader.ReadHandle();
            dimension->hAnonymousBlock = reader.ReadHandle();

            reader.SeekBits(8 - (reader.GetOffset() % 8));
            dimension->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
            if ((reader.GetOffset() / 8) != (objectSize + 4))
                DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                          (reader.GetOffset() / 8 - objectSize - 4));
#endif
            return dimension;
        }

        case CADObject::DIMENSION_LINEAR:
        {
            CADDimensionLinearObject * dimension = new CADDimensionLinearObject();

            dimension->setSize(objectSize);
            dimension->stCed = stCommonEntityData;
            dimension->cdd   = stCDD;

            CADVector vert13pt = reader.ReadVector();
            dimension->vert13pt = vert13pt;

            CADVector vert14pt = reader.ReadVector();
            dimension->vert14pt = vert14pt;

            CADVector vert10pt = reader.ReadVector();
            dimension->vert10pt = vert10pt;

            dimension->dfExtLnRot = reader.ReadBitDouble();
            dimension->dfDimRot   = reader.ReadBitDouble();

            fillCommonEntityHandleData(reader, dimension);

            dimension->hDimstyle       = reader.ReadHandle();
            dimension->hAnonymousBlock = reader.ReadHandle();

            reader.SeekBits(8 - (reader.GetOffset() % 8));
            dimension->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
            if ((reader.GetOffset() / 8) != (objectSize + 4))
                DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                          (reader.GetOffset() / 8 - objectSize - 4));
#endif
            return dimension;
        }

        case CADObject::DIMENSION_ALIGNED:
        {
            CADDimensionAlignedObject * dimension = new CADDimensionAlignedObject();

            dimension->setSize(objectSize);
            dimension->stCed = stCommonEntityData;
            dimension->cdd   = stCDD;

            CADVector vert13pt = reader.ReadVector();
            dimension->vert13pt = vert13pt;

            CADVector vert14pt = reader.ReadVector();
            dimension->vert14pt = vert14pt;

            CADVector vert10pt = reader.ReadVector();
            dimension->vert10pt = vert10pt;

            dimension->dfExtLnRot = reader.ReadBitDouble();

            fillCommonEntityHandleData(reader, dimension);

            dimension->hDimstyle       = reader.ReadHandle();
            dimension->hAnonymousBlock = reader.ReadHandle();

            reader.SeekBits(8 - (reader.GetOffset() % 8));
            dimension->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
            if ((reader.GetOffset() / 8) != (objectSize + 4))
                DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                          (reader.GetOffset() / 8 - objectSize - 4));
#endif
            return dimension;
        }

        case CADObject::DIMENSION_ANG_3PT:
        {
            CADDimensionAngular3PtObject * dimension = new CADDimensionAngular3PtObject();

            dimension->setSize(objectSize);
            dimension->stCed = stCommonEntityData;
            dimension->cdd   = stCDD;

            CADVector vert10pt = reader.ReadVector();
            dimension->vert10pt = vert10pt;

            CADVector vert13pt = reader.ReadVector();
            dimension->vert13pt = vert13pt;

            CADVector vert14pt = reader.ReadVector();
            dimension->vert14pt = vert14pt;

            CADVector vert15pt = reader.ReadVector();
            dimension->vert15pt = vert15pt;

            fillCommonEntityHandleData(reader, dimension);

            dimension->hDimstyle       = reader.ReadHandle();
            dimension->hAnonymousBlock = reader.ReadHandle();

            reader.SeekBits(8 - (reader.GetOffset() % 8));
            dimension->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
            if ((reader.GetOffset() / 8) != (objectSize + 4))
                DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                          (reader.GetOffset() / 8 - objectSize - 4));
#endif
            return dimension;
        }

        case CADObject::DIMENSION_ANG_2LN:
        {
            CADDimensionAngular2LnObject * dimension = new CADDimensionAngular2LnObject();

            dimension->setSize(objectSize);
            dimension->stCed = stCommonEntityData;
            dimension->cdd   = stCDD;

            CADVector vert16pt = reader.ReadVector();
            dimension->vert16pt = vert16pt;

            CADVector vert13pt = reader.ReadVector();
            dimension->vert13pt = vert13pt;

            CADVector vert14pt = reader.ReadVector();
            dimension->vert14pt = vert14pt;

            CADVector vert15pt = reader.ReadVector();
            dimension->vert15pt = vert15pt;

            CADVector vert10pt = reader.ReadVector();
            dimension->vert10pt = vert10pt;

            fillCommonEntityHandleData(reader, dimension);

            dimension->hDimstyle       = reader.ReadHandle();
            dimension->hAnonymousBlock = reader.ReadHandle();

            reader.SeekBits(8 - (reader.GetOffset() % 8));
            dimension->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
            if ((reader.GetOffset() / 8) != (objectSize + 4))
                DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                          (reader.GetOffset() / 8 - objectSize - 4));
#endif
            return dimension;
        }

        case CADObject::DIMENSION_RADIUS:
        {
            CADDimensionRadiusObject * dimension = new CADDimensionRadiusObject();

            dimension->setSize(objectSize);
            dimension->stCed = stCommonEntityData;
            dimension->cdd   = stCDD;

            CADVector vert10pt = reader.ReadVector();
            dimension->vert10pt = vert10pt;

            CADVector vert15pt = reader.ReadVector();
            dimension->vert15pt = vert15pt;

            dimension->dfLeaderLen = reader.ReadBitDouble();

            fillCommonEntityHandleData(reader, dimension);

            dimension->hDimstyle       = reader.ReadHandle();
            dimension->hAnonymousBlock = reader.ReadHandle();

            reader.SeekBits(8 - (reader.GetOffset() % 8));
            dimension->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
            if ((reader.GetOffset() / 8) != (objectSize + 4))
                DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                          (reader.GetOffset() / 8 - objectSize - 4));
#endif
            return dimension;
        }

        case CADObject::DIMENSION_DIAMETER:
        {
            CADDimensionDiameterObject * dimension = new CADDimensionDiameterObject();

            dimension->setSize(objectSize);
            dimension->stCed = stCommonEntityData;
            dimension->cdd   = stCDD;

            CADVector vert15pt = reader.ReadVector();
            dimension->vert15pt = vert15pt;

            CADVector vert10pt = reader.ReadVector();
            dimension->vert10pt = vert10pt;

            dimension->dfLeaderLen = reader.ReadBitDouble();

            fillCommonEntityHandleData(reader, dimension);

            dimension->hDimstyle       = reader.ReadHandle();
            dimension->hAnonymousBlock = reader.ReadHandle();

            reader.SeekBits(8 - (reader.GetOffset() % 8));
            dimension->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
            if ((reader.GetOffset() / 8) != (objectSize + 4))
                DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                          (reader.GetOffset() / 8 - objectSize - 4));
#endif
            return dimension;
        }
    }
    return nullptr;
}

CADImageDefObject * DWGFileR2000::getImageDef(CADBitStreamReader& reader, int32_t objectSize)
{
    CADImageDefObject * imagedef = new CADImageDefObject();

    imagedef->setSize(objectSize);
    imagedef->nObjectSizeInBits = reader.ReadRawLong();
    imagedef->hObjectHandle     = reader.ReadHandle8BitsLength();

    int16_t  eedSize = 0;
    CADEed dwgEed;
    while((eedSize = reader.ReadBitShort()) != 0)
    {
        dwgEed.dLength      = eedSize;
        dwgEed.hApplication = reader.ReadHandle();

        for (int16_t i = 0; i < eedSize; ++i)
        {
            dwgEed.acData.push_back(reader.ReadChar());
        }

        imagedef->aEED.push_back(dwgEed);
    }

    imagedef->nNumReactors  = reader.ReadBitLong();
    imagedef->dClassVersion = reader.ReadBitLong();

    imagedef->dfXImageSizeInPx = reader.ReadRawDouble();
    imagedef->dfYImageSizeInPx = reader.ReadRawDouble();

    imagedef->sFilePath = reader.ReadTv();
    imagedef->bIsLoaded = reader.ReadBit();

    imagedef->dResUnits = reader.ReadChar();

    imagedef->dfXPixelSize = reader.ReadRawDouble();
    imagedef->dfYPixelSize = reader.ReadRawDouble();

    imagedef->hParentHandle = reader.ReadHandle();

    for (int32_t i = 0; i < imagedef->nNumReactors; ++i)
        imagedef->hReactors.push_back(reader.ReadHandle());

    imagedef->hXDictionary = reader.ReadHandle();

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    imagedef->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif

    return imagedef;
}

CADImageDefReactorObject * DWGFileR2000::getImageDefReactor(CADBitStreamReader& reader, int32_t objectSize)
{
    CADImageDefReactorObject * imagedefreactor = new CADImageDefReactorObject();

    imagedefreactor->setSize(objectSize);
    imagedefreactor->nObjectSizeInBits = reader.ReadRawLong();
    imagedefreactor->hObjectHandle     = reader.ReadHandle8BitsLength();

    int16_t  eedSize = 0;
    CADEed dwgEed;
    while((eedSize = reader.ReadBitShort()) != 0)
    {
        dwgEed.dLength      = eedSize;
        dwgEed.hApplication = reader.ReadHandle();

        for (int16_t i = 0; i < eedSize; ++i)
        {
            dwgEed.acData.push_back(reader.ReadChar());
        }

        imagedefreactor->aEED.push_back(dwgEed);
    }

    imagedefreactor->nNumReactors  = reader.ReadBitLong();
    imagedefreactor->dClassVersion = reader.ReadBitLong();

    imagedefreactor->hParentHandle = reader.ReadHandle();

    for (int32_t i = 0; i < imagedefreactor->nNumReactors; ++i)
        imagedefreactor->hReactors.push_back(reader.ReadHandle());

    imagedefreactor->hXDictionary = reader.ReadHandle();

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    imagedefreactor->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif

    return imagedefreactor;
}

CADXRecordObject * DWGFileR2000::getXRecord(CADBitStreamReader& reader, int32_t objectSize)
{
    CADXRecordObject * xrecord = new CADXRecordObject();

    xrecord->setSize(objectSize);
    xrecord->nObjectSizeInBits = reader.ReadRawLong();
    xrecord->hObjectHandle     = reader.ReadHandle8BitsLength();

    int16_t  eedSize = 0;
    CADEed dwgEed;
    while((eedSize = reader.ReadBitShort()) != 0)
    {
        dwgEed.dLength      = eedSize;
        dwgEed.hApplication = reader.ReadHandle();

        for (int16_t i = 0; i < eedSize; ++i)
        {
            dwgEed.acData.push_back(reader.ReadChar());
        }

        xrecord->aEED.push_back(dwgEed);
    }

    xrecord->nNumReactors  = reader.ReadBitLong();
    xrecord->nNumDataBytes = reader.ReadBitLong();

    for (int32_t i = 0; i < xrecord->nNumDataBytes; ++i)
    {
        xrecord->abyDataBytes.push_back(reader.ReadChar());
    }

    xrecord->dCloningFlag = reader.ReadBitShort();

    int16_t dIndicatorNumber = reader.ReadRawShort();
    if (dIndicatorNumber == 1)
    {
        uint8_t nStringSize = reader.ReadChar();
        /* char dCodePage   =  */ reader.ReadChar();
        for (uint8_t i = 0; i < nStringSize; ++i)
        {
            reader.ReadChar();
        }
    } else if (dIndicatorNumber == 70)
    {
        reader.ReadRawShort();
    } else if (dIndicatorNumber == 10)
    {
        reader.ReadRawDouble();
        reader.ReadRawDouble();
        reader.ReadRawDouble();
    } else if (dIndicatorNumber == 40)
    {
        reader.ReadRawDouble();
    }

    xrecord->hParentHandle = reader.ReadHandle();

    for (int32_t i = 0; i < xrecord->nNumReactors; ++i)
        xrecord->hReactors.push_back(reader.ReadHandle());

    xrecord->hXDictionary = reader.ReadHandle();

    while(reader.GetOffset() / 8 < ((size_t) objectSize + 4))
    {
        xrecord->hObjIdHandles.push_back(reader.ReadHandle());
    }

    reader.SeekBits(8 - (reader.GetOffset() % 8));
    xrecord->setCRC(reader.ReadRawShort());
#ifdef _DEBUG
    if ((reader.GetOffset() / 8) != (objectSize + 4))
        DebugMsg("Assertion failed at %d in %s\nSize difference: %d\n", __LINE__, __FILE__,
                  (reader.GetOffset() / 8 - objectSize - 4));
#endif

    return xrecord;
}

void DWGFileR2000::fillCommonEntityHandleData(CADBitStreamReader& reader, CADEntityObject * pEnt)
{
    if (pEnt->stCed.bbEntMode == 0)
        pEnt->stChed.hOwner = reader.ReadHandle();

    for (int32_t i = 0; i < pEnt->stCed.nNumReactors; ++i)
        pEnt->stChed.hReactors.push_back(reader.ReadHandle());

    pEnt->stChed.hXDictionary = reader.ReadHandle();

    if (!pEnt->stCed.bNoLinks)
    {
        pEnt->stChed.hPrevEntity = reader.ReadHandle();
        pEnt->stChed.hNextEntity = reader.ReadHandle();
    }

    pEnt->stChed.hLayer = reader.ReadHandle();

    if (pEnt->stCed.bbLTypeFlags == 0x03)
        pEnt->stChed.hLType = reader.ReadHandle();

    if (pEnt->stCed.bbPlotStyleFlags == 0x03)
        pEnt->stChed.hPlotStyle = reader.ReadHandle();
}

DWGFileR2000::DWGFileR2000(CADFileIO * poFileIO) : CADFile(poFileIO)
{
    oHeader.addValue(CADHeader::OPENCADVER, CADVersions::DWG_R2000);
}

DWGFileR2000::~DWGFileR2000()
{
}

int DWGFileR2000::ReadSectionLocators()
{
    char  abyBuf[255];
    int   dImageSeeker, SLRecordsCount;
    int16_t dCodePage;

    pFileIO->Rewind();
    memset(abyBuf, 0, DWG_VERSION_STR_SIZE + 1);
    pFileIO->Read(abyBuf, DWG_VERSION_STR_SIZE);
    oHeader.addValue(CADHeader::ACADVER, abyBuf);
    memset(abyBuf, 0, 8);
    pFileIO->Read(abyBuf, 7);
    oHeader.addValue(CADHeader::ACADMAINTVER, abyBuf);
    // TODO: code can be much simplified if CADHandle will be used.
    pFileIO->Read(& dImageSeeker, 4);
    // to do so, == and ++ operators should be implemented.
    DebugMsg("Image seeker readed: %d\n", dImageSeeker);
    imageSeeker = dImageSeeker;

    pFileIO->Seek(2, CADFileIO::SeekOrigin::CUR); // 19
    pFileIO->Read(& dCodePage, 2);
    oHeader.addValue(CADHeader::DWGCODEPAGE, dCodePage);

    DebugMsg("DWG Code page: %d\n", dCodePage);

    pFileIO->Read(& SLRecordsCount, 4); // 21
    // Last vertex is reached. read it and break reading.
    DebugMsg("Section locator records count: %d\n", SLRecordsCount);

    for (size_t i = 0; i < static_cast<size_t>(SLRecordsCount); ++i)
    {
        SectionLocatorRecord readedRecord;
        pFileIO->Read(& readedRecord.byRecordNumber, 1);
        pFileIO->Read(& readedRecord.dSeeker, 4);
        pFileIO->Read(& readedRecord.dSize, 4);

        sectionLocatorRecords.push_back(readedRecord);
        DebugMsg("  Record #%d : %d %d\n", sectionLocatorRecords[i].byRecordNumber, sectionLocatorRecords[i].dSeeker,
                  sectionLocatorRecords[i].dSize);
    }

    return CADErrorCodes::SUCCESS;
}

CADDictionary DWGFileR2000::GetNOD()
{
    CADDictionary stNOD;

    unique_ptr<CADDictionaryObject> spoNamedDictObj(
            (CADDictionaryObject *) GetObject(oTables.GetTableHandle(CADTables::NamedObjectsDict).getAsLong()));

    for (size_t i = 0; i < spoNamedDictObj->sItemNames.size(); ++i)
    {
        unique_ptr<CADObject> spoDictRecord(GetObject(spoNamedDictObj->hItemHandles[i].getAsLong()));

        if (spoDictRecord == nullptr) continue; // skip unreaded objects

        if (spoDictRecord->getType() == CADObject::ObjectType::DICTIONARY)
        {
            // TODO: add implementation of DICTIONARY reading
        } else if (spoDictRecord->getType() == CADObject::ObjectType::XRECORD)
        {
            CADXRecord       * cadxRecord       = new CADXRecord();
            CADXRecordObject * cadxRecordObject = (CADXRecordObject *) spoDictRecord.get();

            string xRecordData(cadxRecordObject->abyDataBytes.begin(), cadxRecordObject->abyDataBytes.end());
            cadxRecord->setRecordData(xRecordData);

            stNOD.addRecord(make_pair(spoNamedDictObj->sItemNames[i], (CADDictionaryRecord *) cadxRecord));
        }
    }

    return stNOD;
}

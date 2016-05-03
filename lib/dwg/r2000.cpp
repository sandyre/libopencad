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
    int dImageSeeker, dSLRecords;
    short dCodePage;

    m_poFileIO->Rewind();
    memset(pabyBuf, 0, DWG_VERSION_STR_SIZE + 1);
    m_poFileIO->Read(pabyBuf, DWG_VERSION_STR_SIZE);
    m_poHeader->AddValue(CADHeader::ACADVER, pabyBuf);

    memset(pabyBuf, 0, 8);
    m_poFileIO->Read(pabyBuf, 7);
    m_poHeader->AddValue(CADHeader::ACADMAINTVER, pabyBuf);

    m_poFileIO->Read (&dImageSeeker, 4);

    DebugMsg("Image seeker readed: %d\n", dImageSeeker);

    m_poFileIO->Seek (2, CADFileIO::CUR); // 19
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

    m_poFileIO->Seek (file_header.SLRecords[0].dSeeker, CADFileIO::BEG);
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

    size_t bitOffsetFromStart = 0;
    pabyBuf = new char[dHeaderVarsSectionLength];
    m_poFileIO->Read ( pabyBuf, dHeaderVarsSectionLength + 2 );

    m_poHeader->AddValue(UNKNOWN1, ReadBITDOUBLE (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN2, ReadBITDOUBLE (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN3, ReadBITDOUBLE (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN4, ReadBITDOUBLE (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN5, ReadTV (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN6, ReadTV (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN7, ReadTV (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN8, ReadTV (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN9, ReadBITLONG (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(UNKNOWN10, ReadBITLONG (pabyBuf, bitOffsetFromStart));

    // TODO: support for handle
    header_variables.current_viewport_ent_header = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMASO, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DIMSHO, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PLINEGEN, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::ORTHOMODE, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::REGENMODE, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::FILLMODE, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::QTEXTMODE, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PSLTSCALE, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::LIMCHECK, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::USRTIMER, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SKPOLY, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::ANGDIR, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::SPLFRAME, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::MIRRTEXT, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::WORDLVIEW, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::TILEMODE, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PLIMCHECK, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::VISRETAIN, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::DISPSILH, ReadBIT (pabyBuf, bitOffsetFromStart));
    m_poHeader->AddValue(CADHeader::PELLIPSE, ReadBIT (pabyBuf, bitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::PROXYGRAPHICS  = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TREEDEPTH      = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LUNITS         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LUPREC         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::AUNITS         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::AUPREC         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::ATTMODE        = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PDMODE         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::USERI1         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::USERI2         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::USERI3         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::USERI4         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::USERI5         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::SPLINESEGS     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::SURFU          = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::SURFV          = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::SURFTYPE       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::SURFTAB1       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::SURFTAB2       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::SPLINETYPE     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::SHADEDGE       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::SHADEDIF       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UNITMODE       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::MAXACTVP       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::ISOLINES       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CMLJUST        = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TEXTQLTY       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::LTSCALE        = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TEXTSIZE       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TRACEWID       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::SKETCHINC      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::FILLETRAD      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::THICKNESS      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::ANGBASE        = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PDSIZE         = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PLINEWID       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::USERR1         = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::USERR2         = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::USERR3         = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::USERR4         = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::USERR5         = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CHAMFERA       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CHAMFERB       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CHAMFERC       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CHAMFERD       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::FACETRES       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CMLSCALE       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CELTSCALE      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::MENU, ReadTV (pabyBuf, bitOffsetFromStart));

    m_poHeader->AddValue(CADHeader::TDCREATE_DAY   = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TDCREATE_MS    = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TDUPDATE_DAY   = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TDUPDATE_MS    = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TDINDWG_DAY    = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TDINDWG_MS     = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TDUSRTIMER_DAY = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TDUSRTIMER_MS  = ReadBITLONG (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::CECOLOR        = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::HANDSEED       = ReadHANDLE8BLENGTH (pabyBuf, bitOffsetFromStart); // CHECK THIS CASE.

    m_poHeader->AddValue(CADHeader::CLAYER         = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TEXTSTYLE      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CELTYPE        = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMSTYLE       = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CMLSTYLE       = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::PSVPSCALE          = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::INSBASE_PSPACE.X   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::INSBASE_PSPACE.Y   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::INSBASE_PSPACE.Z   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMIN_PSPACE.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMIN_PSPACE.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMIN_PSPACE.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMAX_PSPACE.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMAX_PSPACE.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMAX_PSPACE.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::LIMMIN_PSPACE.X    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LIMMIN_PSPACE.Y    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LIMMAX_PSPACE.X    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LIMMAX_PSPACE.Y    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::ELEVATION_PSPACE   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORG_PSPACE.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORG_PSPACE.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORG_PSPACE.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSXDIR_PSPACE.X   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSXDIR_PSPACE.Y   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSXDIR_PSPACE.Z   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSYDIR_PSPACE.X   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSYDIR_PSPACE.Y   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSYDIR_PSPACE.Z   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::UCSNAME_PSPACE     = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORTHOREF       = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::PUCSORTHOVIEW      = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSBASE           = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::PUCSORGTOP.X       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGTOP.Y       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGTOP.Z       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGBOTTOM.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGBOTTOM.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGBOTTOM.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGLEFT.X      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGLEFT.Y      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGLEFT.Z      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGRIGHT.X     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGRIGHT.Y     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGRIGHT.Z     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGFRONT.X     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGFRONT.Y     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGFRONT.Z     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGBACK.X      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGBACK.Y      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORGBACK.Z      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::INSBASE_MSPACE.X   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::INSBASE_MSPACE.Y   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::INSBASE_MSPACE.Z   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMIN_MSPACE.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMIN_MSPACE.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMIN_MSPACE.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMAX_MSPACE.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMAX_MSPACE.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::EXTMAX_MSPACE.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::LIMMIN_MSPACE.X    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LIMMIN_MSPACE.Y    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LIMMAX_MSPACE.X    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LIMMAX_MSPACE.Y    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::ELEVATION_MSPACE   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORG_MSPACE.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORG_MSPACE.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORG_MSPACE.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSXDIR_MSPACE.X   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSXDIR_MSPACE.Y   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSXDIR_MSPACE.Z   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSYDIR_MSPACE.X   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSYDIR_MSPACE.Y   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSYDIR_MSPACE.Z   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::UCSNAME_MSPACE     = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::PUCSORTHOREF       = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::PUCSORTHOVIEW      = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::PUCSBASE           = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::UCSORGTOP.X        = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGTOP.Y        = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGTOP.Z        = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGBOTTOM.X     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGBOTTOM.Y     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGBOTTOM.Z     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGLEFT.X       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGLEFT.Y       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGLEFT.Z       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGRIGHT.X      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGRIGHT.Y      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGRIGHT.Z      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGFRONT.X      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGFRONT.Y      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGFRONT.Z      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGBACK.X       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGBACK.Y       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCSORGBACK.Z       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMPOST    = ReadTV (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMAPOST   = ReadTV (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMSCALE   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMASZ     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMEXO     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMDLI     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMEXE     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMRND     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMDLE     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMTP      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMTM      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMTOL     = ReadBIT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMLIM     = ReadBIT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMTIH     = ReadBIT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMTOH     = ReadBIT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMSE1     = ReadBIT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMSE2     = ReadBIT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMTAD     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMZIN     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMAZIN    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMTXT     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMCEN     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMTSZ     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMALTF    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMLFAC    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMTVP     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMTFAC    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMGAP     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMALTRND  = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMALT     = ReadBIT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMALTD    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMTOFL    = ReadBIT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMSAH     = ReadBIT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMTIX     = ReadBIT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMSOXD    = ReadBIT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMCLRD    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMCLRE    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMCLRT    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMADEC    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMDEC     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMTDEC    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMALTU    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMALTTD   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMAUNIT   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMFRAC    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMLUNIT   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMDSEP    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMTMOVE   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMJUST    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMSD1     = ReadBIT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMSD2     = ReadBIT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMTOLJ    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMTZIN    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMALTZ    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMALTTZ   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMUPT     = ReadBIT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMATFIT   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMTXSTY   = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMLDRBLK  = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMBLK     = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMBLK1    = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMBLK2    = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DIMLWD     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMLWE     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::BLOCK_CONTROL_OBJ      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LAYER_CONTROL_OBJ      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::STYLE_CONTROL_OBJ      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LINETYPE_CONTROL_OBJ   = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::VIEW_CONTROL_OBJ       = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UCS_CONTROL_OBJ        = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::VPORT_CONTROL_OBJ      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::APPID_CONTROL_OBJ      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DIMSTYLE               = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::VIEWPORT_ENT_HEADER_CONTROL_OBJ = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DICTIONARY_ACAD_GROUP      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DICTIONARY_ACAD_MLINESTYLE = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DICTIONARY_NAMED_OBJECTS   = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::TSTACKALIGN    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::TSTACKSIZE     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::HYPERLINKBASE  = ReadTV (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::STYLESHEET     = ReadTV (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::DICTIONARY_LAYOUTS         = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DICTIONARY_PLOTSETTINGS    = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::DICTIONARY_PLOTSTYLES      = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    int Flags               = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CELWEIGHT  = Flags & 0x001F;
    m_poHeader->AddValue(CADHeader::ENDCAPS    = Flags & 0x0060;
    m_poHeader->AddValue(CADHeader::JOINSTYLE  = Flags & 0x0180;
    m_poHeader->AddValue(CADHeader::LWDISPLAY  = !(Flags & 0x0200);
    m_poHeader->AddValue(CADHeader::XEDIT      = !(Flags & 0x0400);
    m_poHeader->AddValue(CADHeader::EXTNAMES   = Flags & 0x0800;
    m_poHeader->AddValue(CADHeader::PSTYLEMODE = Flags & 0x2000;
    m_poHeader->AddValue(CADHeader::OLESTARTUP = Flags & 0x4000;

    m_poHeader->AddValue(CADHeader::INSUNITS   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    short nCEPSNTYPE = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::CEPSNTYPE, nCEPSNTYPE);

    if ( nCEPSNTYPE == 3 )
        m_poHeader->AddValue(CADHeader::CPSNID = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::FINGERPRINTGUID = ReadTV (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::VERSIONGUID     = ReadTV (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::BLOCK_RECORD_PSPACE    = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::BLOCK_RECORD_MSPACE    = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LTYPE_BYLAYER          = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LTYPE_BYBLOCK          = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::LTYPE_CONTINUOUS       = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    m_poHeader->AddValue(CADHeader::UnknownShortInEnd1     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UnknownShortInEnd2     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UnknownShortInEnd3     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    m_poHeader->AddValue(CADHeader::UnknownShortInEnd4     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    short nCRC        = ReadRAWSHORT (pabyBuf, bitOffsetFromStart);
    uint16_t calculated_crc     = CalculateCRC8 ((unsigned short)0xC0C1, pabyBuf, dHeaderVarsSectionLength); // TODO: CRC is calculated wrong every time.

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

    m_poFileIO->Seek (file_header.SLRecords[1].dSeeker, CADFileIO::BEG);

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
        std::cout << "/////// CLASS INFO ////////" << std::endl;
        std::cout << "dClassNum: " << custom_classes[i].dClassNum << std::endl;
        std::cout << "VERSION: " << custom_classes[i].dVersion << std::endl;
        std::cout << "APPNAME: " << custom_classes[i].sAppName << std::endl;
        std::cout << "C++CLASSNAME: " << custom_classes[i].sCppClassName << std::endl;
        std::cout << "CLASSDXFNAME: " << custom_classes[i].sDXFClassName << std::endl;
        std::cout << "WASAZOMBIE: " << custom_classes[i].bWasAZombie << std::endl;
        std::cout << "ITEMCLASSID: " << std::hex << custom_classes[i].dItemClassID << std::dec << std::endl <<
        std::endl;
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
    uint16_t section_crc        = 0;
    uint16_t section_size       = 0;
    size_t   bitOffsetFromStart = 0;

    m_poFileIO->Seek (file_header.SLRecords[2].dSeeker, CADFileIO::BEG);

    int current_section = 0;
    while ( true )
    {
        bitOffsetFromStart = 0;
        std::vector <ObjHandleOffset> obj_map_section;
        m_poFileIO->Read (&section_size, 2);
        SwapEndianness (section_size, sizeof (section_size));

        std::cout << "OBJECT MAP SECTION SIZE: " << section_size << std::endl;

        if ( section_size == 2 ) break; // last section is empty.

        pabySectionContent = new char[section_size];
        m_poFileIO->Read (pabySectionContent, section_size);

        while ( ( bitOffsetFromStart / 8 ) < ( section_size - 2 ) )
        {
            ObjHandleOffset tmp;
            tmp.first  = ReadUMCHAR (pabySectionContent, bitOffsetFromStart);
            tmp.second = ReadMCHAR (pabySectionContent, bitOffsetFromStart);
            obj_map_section.push_back (tmp);
        }


        section_crc = ReadRAWSHORT (pabySectionContent, bitOffsetFromStart);
        SwapEndianness (section_crc, sizeof (section_crc));

        object_map_sections.push_back (obj_map_section);
        std::cout << "OBJECTS PARSED: " << object_map_sections[current_section].size () << std::endl;

        current_section++;
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
            bitOffsetFromStart = 0;
            m_poFileIO->Seek (object_map_sections[i][j].second, CADFileIO::BEG);
            m_poFileIO->Read (pabySectionContent, 400);

            DWG2000_CED ced;
            ced.dLength = ReadMSHORT (pabySectionContent, bitOffsetFromStart);
            ced.dType   = ReadBITSHORT (pabySectionContent, bitOffsetFromStart);

            try
            {
                DWG_OBJECT_NAMES.at (ced.dType);
                std::cout << "OBJECT TYPE: " << DWG_OBJECT_NAMES.at (ced.dType) << " HANDLE: " << object_map_sections[i][j].first << std::endl;

                if ( ced.dType == DWG_OBJECT_LAYER )
                    layer_map.push_back (object_map_sections[i][j]);
            }
            catch ( std::exception e )
            {
                std::cout << "OBJECT TYPE: " << custom_classes[ced.dType - 500].sCppClassName << std::endl;
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
    m_poFileIO->Seek (geometries_map[index].second, CADFileIO::BEG);
    m_poFileIO->Read (pabySectionSize, 8);
    uint32_t dGeometrySize = ReadMSHORT (pabySectionSize, bitOffsetFromStart);

    // And read whole data chunk into memory for future parsing.
    // +6 is because dGeometrySize does not cover ced.dLength length, and CRC length (so, ced.dLength can be
    // maximum 4 bytes long, and crc is 2 bytes long).
    char * pabySectionContent = new char[dGeometrySize + 6];
    bitOffsetFromStart = 0;
    // m_oFileStream.clear ();
    m_poFileIO->Seek (geometries_map[index].second, CADFileIO::BEG);
    m_poFileIO->Read (pabySectionContent, dGeometrySize + 6);

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

            DWG_HANDLE firstvertex = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            DWG_HANDLE lastvertex  = ReadHANDLE (pabySectionContent, bitOffsetFromStart);
            DWG_HANDLE seqend      = ReadHANDLE (pabySectionContent, bitOffsetFromStart);

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

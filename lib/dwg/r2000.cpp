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
#include "objects.h"
#include "cadgeometries.h"

#include <iostream>
#include <cstring>

void DWGFileR2000::ReadHeader ()
{
    char    *pabyBuf = new char[100];
    int32_t dImageSeeker, dSLRecords;
    int16_t dCodePage;

    m_oFileStream.seekg (6, std::ios_base::beg); // file version skipped.
    m_oFileStream.seekg (7, std::ios_base::cur); // meaningless data skipped.
    m_oFileStream.read (( char * ) & dImageSeeker, 4);

    printf ("Image seeker readed: %d\n", dImageSeeker);

    m_oFileStream.seekg (2, std::ios_base::cur);
    m_oFileStream.read (( char * ) & dCodePage, 2);

    printf ("DWG Code page: %d\n", dCodePage);

    m_oFileStream.read (( char * ) & dSLRecords, 4);

    printf ("Section-locator records count: %d\n", dSLRecords);

    for ( size_t i = 0; i < dSLRecords; ++i )
    {
        SLRecord readed_record;
        m_oFileStream.read (( char * ) & readed_record.byRecordNumber, 1);
        m_oFileStream.read (( char * ) & readed_record.dSeeker, 4);
        m_oFileStream.read (( char * ) & readed_record.dSize, 4);

        this->file_header.SLRecords.push_back (readed_record);
        printf ("SL Record #%d : %d %d\n", this->file_header.SLRecords[i].byRecordNumber,
                this->file_header.SLRecords[i].dSeeker,
                this->file_header.SLRecords[i].dSize);
    }

/*      READ HEADER VARIABLES        */
    size_t dHeaderVarsSectionLength = 0;

    m_oFileStream.seekg (this->file_header.SLRecords[0].dSeeker, std::ios_base::beg);

    m_oFileStream.read (pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH);
    if ( memcmp (pabyBuf, DWG_SENTINELS::HEADER_VARIABLES_START, DWG_SENTINELS::SENTINEL_LENGTH) )
    {
        printf ("File is corrupted (wrong pointer to HEADER_VARS section,"
                        "or HEADERVARS starting sentinel corrupted.)");

        delete[] pabyBuf;

        return;
    }

    delete[] pabyBuf;

    m_oFileStream.read (( char * ) & dHeaderVarsSectionLength, 4);
    printf ("Header variables section length: %ld\n", dHeaderVarsSectionLength);

    size_t bitOffsetFromStart = 0;
    pabyBuf = new char[dHeaderVarsSectionLength];
    m_oFileStream.read ( pabyBuf, dHeaderVarsSectionLength + 2 );

    header_variables.Unknown1   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.Unknown2   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.Unknown3   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.Unknown4   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.Unknown5   = ReadTV (pabyBuf, bitOffsetFromStart);
    header_variables.Unknown6   = ReadTV (pabyBuf, bitOffsetFromStart);
    header_variables.Unknown7   = ReadTV (pabyBuf, bitOffsetFromStart);
    header_variables.Unknown8   = ReadTV (pabyBuf, bitOffsetFromStart);
    header_variables.Unknown9   = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    header_variables.Unknown10  = ReadBITLONG (pabyBuf, bitOffsetFromStart);

    header_variables.current_viewport_ent_header = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    header_variables.DIMASO     = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMSHO     = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.PLINEGEN   = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.ORTHOMODE  = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.REGENMODE  = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.FILLMODE   = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.QTEXTMODE  = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.PSLTSCALE  = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.LIMCHECK   = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.USRTIMER   = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.SKPOLY     = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.ANGDIR     = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.SPLFRAME   = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.MIRRTEXT   = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.WORDLVIEW  = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.TILEMODE   = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.PLIMCHECK  = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.VISRETAIN  = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.DISPSILH   = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.PELLIPSE   = ReadBIT (pabyBuf, bitOffsetFromStart);

    header_variables.PROXYGRAPHICS  = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.TREEDEPTH      = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.LUNITS         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.LUPREC         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.AUNITS         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.AUPREC         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.ATTMODE        = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.PDMODE         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.USERI1         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.USERI2         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.USERI3         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.USERI4         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.USERI5         = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.SPLINESEGS     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.SURFU          = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.SURFV          = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.SURFTYPE       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.SURFTAB1       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.SURFTAB2       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.SPLINETYPE     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.SHADEDGE       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.SHADEDIF       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.UNITMODE       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.MAXACTVP       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.ISOLINES       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.CMLJUST        = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.TEXTQLTY       = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    header_variables.LTSCALE        = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.TEXTSIZE       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.TRACEWID       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.SKETCHINC      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.FILLETRAD      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.THICKNESS      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.ANGBASE        = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PDSIZE         = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PLINEWID       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.USERR1         = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.USERR2         = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.USERR3         = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.USERR4         = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.USERR5         = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.CHAMFERA       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.CHAMFERB       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.CHAMFERC       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.CHAMFERD       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.FACETRES       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.CMLSCALE       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.CELTSCALE      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    header_variables.MENUNAME       = ReadTV (pabyBuf, bitOffsetFromStart);

    header_variables.TDCREATE_DAY   = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    header_variables.TDCREATE_MS    = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    header_variables.TDUPDATE_DAY   = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    header_variables.TDUPDATE_MS    = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    header_variables.TDINDWG_DAY    = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    header_variables.TDINDWG_MS     = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    header_variables.TDUSRTIMER_DAY = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    header_variables.TDUSRTIMER_MS  = ReadBITLONG (pabyBuf, bitOffsetFromStart);

    header_variables.CECOLOR        = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    header_variables.HANDSEED       = ReadHANDLE8BLENGTH (pabyBuf, bitOffsetFromStart); // CHECK THIS CASE.

    header_variables.CLAYER         = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.TEXTSTYLE      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.CELTYPE        = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMSTYLE       = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.CMLSTYLE       = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    header_variables.PSVPSCALE          = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.INSBASE_PSPACE.X   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.INSBASE_PSPACE.Y   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.INSBASE_PSPACE.Z   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.EXTMIN_PSPACE.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.EXTMIN_PSPACE.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.EXTMIN_PSPACE.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.EXTMAX_PSPACE.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.EXTMAX_PSPACE.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.EXTMAX_PSPACE.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    header_variables.LIMMIN_PSPACE.X    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.LIMMIN_PSPACE.Y    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.LIMMAX_PSPACE.X    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.LIMMAX_PSPACE.Y    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);

    header_variables.ELEVATION_PSPACE   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORG_PSPACE.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORG_PSPACE.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORG_PSPACE.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSXDIR_PSPACE.X   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSXDIR_PSPACE.Y   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSXDIR_PSPACE.Z   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSYDIR_PSPACE.X   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSYDIR_PSPACE.Y   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSYDIR_PSPACE.Z   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    header_variables.UCSNAME_PSPACE     = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORTHOREF       = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    header_variables.PUCSORTHOVIEW      = ReadBITSHORT (pabyBuf, bitOffsetFromStart)
            ;
    header_variables.PUCSBASE           = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    header_variables.PUCSORGTOP.X       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGTOP.Y       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGTOP.Z       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGBOTTOM.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGBOTTOM.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGBOTTOM.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGLEFT.X      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGLEFT.Y      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGLEFT.Z      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGRIGHT.X     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGRIGHT.Y     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGRIGHT.Z     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGFRONT.X     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGFRONT.Y     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGFRONT.Z     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGBACK.X      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGBACK.Y      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORGBACK.Z      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    header_variables.INSBASE_MSPACE.X   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.INSBASE_MSPACE.Y   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.INSBASE_MSPACE.Z   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.EXTMIN_MSPACE.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.EXTMIN_MSPACE.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.EXTMIN_MSPACE.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.EXTMAX_MSPACE.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.EXTMAX_MSPACE.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.EXTMAX_MSPACE.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    header_variables.LIMMIN_MSPACE.X    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.LIMMIN_MSPACE.Y    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.LIMMAX_MSPACE.X    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.LIMMAX_MSPACE.Y    = ReadRAWDOUBLE (pabyBuf, bitOffsetFromStart);

    header_variables.ELEVATION_MSPACE   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORG_MSPACE.X    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORG_MSPACE.Y    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORG_MSPACE.Z    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSXDIR_MSPACE.X   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSXDIR_MSPACE.Y   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSXDIR_MSPACE.Z   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSYDIR_MSPACE.X   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSYDIR_MSPACE.Y   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSYDIR_MSPACE.Z   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    header_variables.UCSNAME_MSPACE     = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.PUCSORTHOREF       = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    header_variables.PUCSORTHOVIEW      = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    header_variables.PUCSBASE           = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    header_variables.UCSORGTOP.X        = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGTOP.Y        = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGTOP.Z        = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGBOTTOM.X     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGBOTTOM.Y     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGBOTTOM.Z     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGLEFT.X       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGLEFT.Y       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGLEFT.Z       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGRIGHT.X      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGRIGHT.Y      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGRIGHT.Z      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGFRONT.X      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGFRONT.Y      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGFRONT.Z      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGBACK.X       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGBACK.Y       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCSORGBACK.Z       = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    header_variables.DIMPOST    = ReadTV (pabyBuf, bitOffsetFromStart);
    header_variables.DIMAPOST   = ReadTV (pabyBuf, bitOffsetFromStart);

    header_variables.DIMSCALE   = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMASZ     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMEXO     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMDLI     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMEXE     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMRND     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMDLE     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMTP      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMTM      = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    header_variables.DIMTOL     = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMLIM     = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMTIH     = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMTOH     = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMSE1     = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMSE2     = ReadBIT (pabyBuf, bitOffsetFromStart);

    header_variables.DIMTAD     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMZIN     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMAZIN    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    header_variables.DIMTXT     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMCEN     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMTSZ     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMALTF    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMLFAC    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMTVP     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMTFAC    = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMGAP     = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMALTRND  = ReadBITDOUBLE (pabyBuf, bitOffsetFromStart);

    header_variables.DIMALT     = ReadBIT (pabyBuf, bitOffsetFromStart);

    header_variables.DIMALTD    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    header_variables.DIMTOFL    = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMSAH     = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMTIX     = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMSOXD    = ReadBIT (pabyBuf, bitOffsetFromStart);

    header_variables.DIMCLRD    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMCLRE    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMCLRT    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMADEC    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMDEC     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMTDEC    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMALTU    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMALTTD   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMAUNIT   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMFRAC    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMLUNIT   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMDSEP    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMTMOVE   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMJUST    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    header_variables.DIMSD1     = ReadBIT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMSD2     = ReadBIT (pabyBuf, bitOffsetFromStart);

    header_variables.DIMTOLJ    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMTZIN    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMALTZ    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMALTTZ   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    header_variables.DIMUPT     = ReadBIT (pabyBuf, bitOffsetFromStart);

    header_variables.DIMATFIT   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    header_variables.DIMTXSTY   = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMLDRBLK  = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMBLK     = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMBLK1    = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMBLK2    = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    header_variables.DIMLWD     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.DIMLWE     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    header_variables.BLOCK_CONTROL_OBJ      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.LAYER_CONTROL_OBJ      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.STYLE_CONTROL_OBJ      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.LINETYPE_CONTROL_OBJ   = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.VIEW_CONTROL_OBJ       = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.UCS_CONTROL_OBJ        = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.VPORT_CONTROL_OBJ      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.APPID_CONTROL_OBJ      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.DIMSTYLE               = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.VIEWPORT_ENT_HEADER_CONTROL_OBJ = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.DICTIONARY_ACAD_GROUP      = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.DICTIONARY_ACAD_MLINESTYLE = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.DICTIONARY_NAMED_OBJECTS   = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    header_variables.TSTACKALIGN    = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.TSTACKSIZE     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    header_variables.HYPERLINKBASE  = ReadTV (pabyBuf, bitOffsetFromStart);
    header_variables.STYLESHEET     = ReadTV (pabyBuf, bitOffsetFromStart);

    header_variables.DICTIONARY_LAYOUTS         = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.DICTIONARY_PLOTSETTINGS    = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.DICTIONARY_PLOTSTYLES      = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    int32_t Flags               = ReadBITLONG (pabyBuf, bitOffsetFromStart);
    header_variables.CELWEIGHT  = Flags & 0x001F;
    header_variables.ENDCAPS    = Flags & 0x0060;
    header_variables.JOINSTYLE  = Flags & 0x0180;
    header_variables.LWDISPLAY  = !(Flags & 0x0200);
    header_variables.XEDIT      = !(Flags & 0x0400);
    header_variables.EXTNAMES   = Flags & 0x0800;
    header_variables.PSTYLEMODE = Flags & 0x2000;
    header_variables.OLESTARTUP = Flags & 0x4000;

    header_variables.INSUNITS   = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.CEPSNTYPE  = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    if ( header_variables.CEPSNTYPE == 3 )
        header_variables.CPSNID = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    header_variables.FINGERPRINTGUID = ReadTV (pabyBuf, bitOffsetFromStart);
    header_variables.VERSIONGUID     = ReadTV (pabyBuf, bitOffsetFromStart);

    header_variables.BLOCK_RECORD_PSPACE    = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.BLOCK_RECORD_MSPACE    = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.LTYPE_BYLAYER          = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.LTYPE_BYBLOCK          = ReadHANDLE (pabyBuf, bitOffsetFromStart);
    header_variables.LTYPE_CONTINUOUS       = ReadHANDLE (pabyBuf, bitOffsetFromStart);

    header_variables.UnknownShortInEnd1     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.UnknownShortInEnd2     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.UnknownShortInEnd3     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);
    header_variables.UnknownShortInEnd4     = ReadBITSHORT (pabyBuf, bitOffsetFromStart);

    header_variables.CRC        = ReadRAWSHORT (pabyBuf, bitOffsetFromStart);
    uint16_t calculated_crc     = CalculateCRC8 ((unsigned short)0xC0C1, pabyBuf, dHeaderVarsSectionLength); // TODO: CRC is calculated wrong every time.

    m_oFileStream.read (pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH);
    if ( memcmp (pabyBuf, DWG_SENTINELS::HEADER_VARIABLES_END, DWG_SENTINELS::SENTINEL_LENGTH) )
    {
        printf ("File is corrupted (HEADERVARS section ending sentinel doesnt match.)");

        delete[] pabyBuf;

        return;
    }

    delete[] pabyBuf;

    return;
}

void DWGFileR2000::ReadClassesSection ()
{
    char    *pabySectionContent;
    char    *pabyBuf     = new char[100];
    int32_t section_size = 0;

    m_oFileStream.seekg (this->file_header.SLRecords[1].dSeeker, std::ios_base::beg);

    m_oFileStream.read (pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH);
    if ( memcmp (pabyBuf, DWG_SENTINELS::DS_CLASSES_START, DWG_SENTINELS::SENTINEL_LENGTH) )
    {
        std::cerr << "File is corrupted (wrong pointer to CLASSES section,"
                "or CLASSES starting sentinel corrupted.)\n";

        delete[] pabyBuf;

        return;
    }

    m_oFileStream.read (( char * ) & section_size, 4);
    printf ("Classes section length: %d\n", section_size);

    pabySectionContent = new char[section_size];
    m_oFileStream.read (pabySectionContent, section_size);

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

    m_oFileStream.read (pabyBuf, 2); // CLASSES CRC!. TODO: add CRC computing & checking feature.

    m_oFileStream.read (pabyBuf, DWG_SENTINELS::SENTINEL_LENGTH);
    if ( memcmp (pabyBuf, DWG_SENTINELS::DS_CLASSES_END, DWG_SENTINELS::SENTINEL_LENGTH) )
    {
        std::cerr << "File is corrupted (CLASSES section ending sentinel doesnt match.)\n";

        delete[] pabyBuf;

        return;
    }

    delete[] pabyBuf;

    return;
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

void DWGFileR2000::ReadObjectMap ()
{
    // Seems like ODA specification is completely awful. CRC is included in section size.
    char     *pabySectionContent;
    uint16_t section_crc        = 0;
    uint16_t section_size       = 0;
    size_t   bitOffsetFromStart = 0;

    m_oFileStream.seekg (this->file_header.SLRecords[2].dSeeker, std::ios_base::beg);

    int current_section = 0;
    while ( true )
    {
        bitOffsetFromStart = 0;
        std::vector <ObjHandleOffset> obj_map_section;
        m_oFileStream.read (( char * ) & section_size, 2);
        SwapEndianness (section_size, sizeof (section_size));

        std::cout << "OBJECT MAP SECTION SIZE: " << section_size << std::endl;

        if ( section_size == 2 ) break; // last section is empty.

        pabySectionContent = new char[section_size];
        m_oFileStream.read (pabySectionContent, section_size);

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
            m_oFileStream.seekg (object_map_sections[i][j].second, std::ios_base::beg);
            m_oFileStream.read (pabySectionContent, 400);

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

    return;
}

DWGObject * DWGFileR2000::getObject ( size_t section, size_t index )
{
    DWGObject * readed_object;

    char pabyObjectSize[8];
    size_t bitOffsetFromStart = 0;
    m_oFileStream.clear ();
    m_oFileStream.seekg (object_map_sections[section][index].second, std::ios_base::beg);
    m_oFileStream.read (pabyObjectSize, 8);
    uint32_t dObjectSize = ReadMSHORT (pabyObjectSize, bitOffsetFromStart);

    // And read whole data chunk into memory for future parsing.
    char * pabySectionContent = new char[dObjectSize];
    bitOffsetFromStart = 0;
    m_oFileStream.clear ();
    m_oFileStream.seekg (geometries_map[index].second, std::ios_base::beg);
    m_oFileStream.read (pabySectionContent, dObjectSize);

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

CADGeometry * DWGFileR2000::getGeometry ( size_t index )
{
    CADGeometry * readed_geometry;

    // Get geometric entity size in bytes.
    char   pabySectionSize[8];
    size_t bitOffsetFromStart = 0;
    m_oFileStream.clear ();
    m_oFileStream.seekg (geometries_map[index].second, std::ios_base::beg);
    m_oFileStream.read (pabySectionSize, 8);
    uint32_t dGeometrySize = ReadMSHORT (pabySectionSize, bitOffsetFromStart);

    // And read whole data chunk into memory for future parsing.
    // +6 is because dGeometrySize does not cover ced.dLength length, and CRC length (so, ced.dLength can be
    // maximum 4 bytes long, and crc is 2 bytes long).
    char * pabySectionContent = new char[dGeometrySize + 6];
    bitOffsetFromStart = 0;
    m_oFileStream.clear ();
    m_oFileStream.seekg (geometries_map[index].second, std::ios_base::beg);
    m_oFileStream.read (pabySectionContent, dGeometrySize + 6);

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

CADLayer * DWGFileR2000::getLayer ( size_t index )
{
    CADLayer * layer = new CADLayer();

    for ( size_t i = 0; i < getGeometriesCount (); ++i )
    {

    }

    return layer;
}

size_t DWGFileR2000::getGeometriesCount ()
{
    return geometries_map.size ();
}

size_t DWGFileR2000::getLayersCount ()
{
    return layer_map.size();
}

DWGFileR2000::DWGFileR2000(const char *pszFileName) :
    CADFile(pszFileName)
{

}

DWGFileR2000::~DWGFileR2000()
{

}

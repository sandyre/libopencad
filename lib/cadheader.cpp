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
#include <dwg/io.h>
#include "cadheader.h"
#include "opencad_api.h"

typedef struct
{
    short       nConstant;
    short       nGroupCode;
    const char *pszValueName;
} CADHeaderConstantDetail;

#define FillCADConstantDetail(x, y) {CADHeader::x, y, "$"#x}

static const CADHeaderConstantDetail CADHeaderConstantDetails[] {
    FillCADConstantDetail(ACADMAINTVER, 70),
    FillCADConstantDetail(ACADVER,       1),
    FillCADConstantDetail(ANGBASE,      50),
    FillCADConstantDetail(ANGDIR,       70),
    FillCADConstantDetail(ATTMODE,      70),
    FillCADConstantDetail(AUNITS,       70),
    FillCADConstantDetail(AUPREC,       70),
    FillCADConstantDetail(CECOLOR,      62),
    FillCADConstantDetail(CELTSCALE,    40),
    FillCADConstantDetail(CELTYPE,       6),
    FillCADConstantDetail(CELWEIGHT,   370),
    FillCADConstantDetail(CEPSNID,     390),
    FillCADConstantDetail(CEPSNTYPE,   380),
    FillCADConstantDetail(CHAMFERA,     40),
    FillCADConstantDetail(CHAMFERB,     40),
    FillCADConstantDetail(CHAMFERC,     40),
    FillCADConstantDetail(CHAMFERD,     40),
    FillCADConstantDetail(CLAYER,        8),
    FillCADConstantDetail(CMLJUST,      70),
    FillCADConstantDetail(CMLSCALE,     40),
    FillCADConstantDetail(CMLSTYLE,      2),
    FillCADConstantDetail(CSHADOW,     280),
    FillCADConstantDetail(DIMADEC,      70),
    FillCADConstantDetail(DIMALT,       70),
    FillCADConstantDetail(DIMALTD,      70),
    FillCADConstantDetail(DIMALTF,      40),
    FillCADConstantDetail(DIMALTRND,    40),
    FillCADConstantDetail(DIMALTTD,     70),
    FillCADConstantDetail(DIMALTTZ,     70),
    FillCADConstantDetail(DIMALTU,      70),
    FillCADConstantDetail(DIMALTZ,      70),
    FillCADConstantDetail(DIMAPOST,      1),
    FillCADConstantDetail(DIMASO,       70),
    FillCADConstantDetail(DIMASSOC,    280),
    FillCADConstantDetail(DIMASZ,       40),
    FillCADConstantDetail(DIMATFIT,     70),
    FillCADConstantDetail(DIMAUNIT,     70),
    FillCADConstantDetail(DIMAZIN,      70),
    FillCADConstantDetail(DIMBLK,        1),
    FillCADConstantDetail(DIMBLK1,       1),
    FillCADConstantDetail(DIMBLK2,       1),
    FillCADConstantDetail(DIMCEN,       40),
    FillCADConstantDetail(DIMCLRD,      70),
    FillCADConstantDetail(DIMCLRE,      70),
    FillCADConstantDetail(DIMCLRT,      70),
    FillCADConstantDetail(DIMDEC,       70),
    FillCADConstantDetail(DIMDLE,       40),
    FillCADConstantDetail(DIMDLI,       40),
    FillCADConstantDetail(DIMDSEP,      70),
    FillCADConstantDetail(DIMEXE,       40),
    FillCADConstantDetail(DIMEXO,       40),
    FillCADConstantDetail(DIMFAC,       40),
    FillCADConstantDetail(DIMGAP,       40),
    FillCADConstantDetail(DIMJUST,      70),
    FillCADConstantDetail(DIMLDRBLK,     1),
    FillCADConstantDetail(DIMLFAC,      40),
    FillCADConstantDetail(DIMLIM,       70),
    FillCADConstantDetail(DIMLUNIT,     70),
    FillCADConstantDetail(DIMLWD,       70),
    FillCADConstantDetail(DIMLWE,       70),
    FillCADConstantDetail(DIMPOST,       1),
    FillCADConstantDetail(DIMRND,       40),
    FillCADConstantDetail(DIMSAH,       70),
    FillCADConstantDetail(DIMSCALE,     40),
    FillCADConstantDetail(DIMSD1,       70),
    FillCADConstantDetail(DIMSD2,       70),
    FillCADConstantDetail(DIMSE1,       70),
    FillCADConstantDetail(DIMSE2,       70),
    FillCADConstantDetail(DIMSHO,       70),
    FillCADConstantDetail(DIMSOXD,      70),
    FillCADConstantDetail(DIMSTYLE,      2),
    FillCADConstantDetail(DIMTAD,       70),
    FillCADConstantDetail(DIMTDEC,      70),
    FillCADConstantDetail(DIMTFAC,      40),
    FillCADConstantDetail(DIMTIH,       70),
    FillCADConstantDetail(DIMTIX,       70),
    FillCADConstantDetail(DIMTM,        40),
    FillCADConstantDetail(DIMTMOVE,     70),
    FillCADConstantDetail(DIMTOFL,      70),
    FillCADConstantDetail(DIMTOH,       70),
    FillCADConstantDetail(DIMTOL,       70),
    FillCADConstantDetail(DIMTOLJ,      70),
    FillCADConstantDetail(DIMTP,        40),
    FillCADConstantDetail(DIMTSZ,       40),
    FillCADConstantDetail(DIMTVP,       40),
    FillCADConstantDetail(DIMTXSTY,      7),
    FillCADConstantDetail(DIMTXT,       40),
    FillCADConstantDetail(DIMTZIN,      70),
    FillCADConstantDetail(DIMUPT,       70),
    FillCADConstantDetail(DIMZIN,       70),
    FillCADConstantDetail(DISPSILH,     70),
    FillCADConstantDetail(DRAGVS,      349),
    FillCADConstantDetail(DWGCODEPAGE,   3),
    FillCADConstantDetail(ELEVATION,    40),
    FillCADConstantDetail(ENDCAPS,      280),
    FillCADConstantDetail(EXTMAX,    123),
    FillCADConstantDetail(EXTMIN,    123),
    FillCADConstantDetail(EXTNAMES,     290),
    FillCADConstantDetail(FILLETRAD,     40),
    FillCADConstantDetail(FILLMODE,      70),
    FillCADConstantDetail(FINGERPRINTGUID,2),
    FillCADConstantDetail(HALOGAP,      280),
    FillCADConstantDetail(HANDSEED,       5),
    FillCADConstantDetail(HIDETEXT,     290),
    FillCADConstantDetail(HYPERLINKBASE,  1),
    FillCADConstantDetail(INDEXCTL,     280),
    FillCADConstantDetail(INSBASE,   123),
    FillCADConstantDetail(INSUNITS,      70),
    FillCADConstantDetail(INTERFERECOLOR,62),
    FillCADConstantDetail(INTERFEREOBJVS,   345),
    FillCADConstantDetail(INTERFEREVPVS,    346),
    FillCADConstantDetail(INTERSECTIONCOLOR, 70),
    FillCADConstantDetail(INTERSECTIONDISPLAY, 290),
    FillCADConstantDetail(JOINSTYLE,    280),
    FillCADConstantDetail(LIMCHECK,      70),
    FillCADConstantDetail(LIMMAX,      1020),
    FillCADConstantDetail(LIMMIN,      1020),
    FillCADConstantDetail(LTSCALE,       40),
    FillCADConstantDetail(LUNITS,        70),
    FillCADConstantDetail(LUPREC,        70),
    FillCADConstantDetail(LWDISPLAY,    290),
    FillCADConstantDetail(MAXACTVP,      70),
    FillCADConstantDetail(MEASUREMENT,   70),
    FillCADConstantDetail(MENU,           1),
    FillCADConstantDetail(MIRRTEXT,      70),
    FillCADConstantDetail(OBSCOLOR,      70),
    FillCADConstantDetail(OBSLTYPE,     280),
    FillCADConstantDetail(ORTHOMODE,     70),
    FillCADConstantDetail(PDMODE,        70),
    FillCADConstantDetail(PDSIZE,        40),
    FillCADConstantDetail(PELEVATION,    40),
    FillCADConstantDetail(PEXTMAX,   123),
    FillCADConstantDetail(PEXTMIN,   123),
    FillCADConstantDetail(PINSBASE,  123),
    FillCADConstantDetail(PLIMCHECK,     70),
    FillCADConstantDetail(PLIMMAX,     1020),
    FillCADConstantDetail(PLIMMIN,     1020),
    FillCADConstantDetail(PLINEGEN,      70),
    FillCADConstantDetail(PLINEWID,      40),
    FillCADConstantDetail(PROJECTNAME,    1),
    FillCADConstantDetail(PROXYGRAPHICS, 70),
    FillCADConstantDetail(PSLTSCALE,     70),
    FillCADConstantDetail(PSTYLEMODE,   290),
    FillCADConstantDetail(PSVPSCALE,     40),
    FillCADConstantDetail(PUCSBASE,       2),
    FillCADConstantDetail(PUCSNAME,       2),
    FillCADConstantDetail(PUCSORG,   123),
    FillCADConstantDetail(PUCSORGBACK,   123),
    FillCADConstantDetail(PUCSORGBOTTOM, 123),
    FillCADConstantDetail(PUCSORGFRONT,  123),
    FillCADConstantDetail(PUCSORGLEFT,   123),
    FillCADConstantDetail(PUCSORGRIGHT,  123),
    FillCADConstantDetail(PUCSORGTOP,    123),
    FillCADConstantDetail(PUCSORTHOREF,       2),
    FillCADConstantDetail(PUCSORTHOVIEW,     70),
    FillCADConstantDetail(PUCSXDIR,      123),
    FillCADConstantDetail(PUCSYDIR,      123),
    FillCADConstantDetail(QTEXTMODE,         70),
    FillCADConstantDetail(REGENMODE,         70),
    FillCADConstantDetail(SHADEDGE,          70),
    FillCADConstantDetail(SHADEDIF,          70),
    FillCADConstantDetail(SHADOWPLANELOCATION, 40),
    FillCADConstantDetail(SKETCHINC,     40),
    FillCADConstantDetail(SKPOLY,        70),
    FillCADConstantDetail(SORTENTS,     280),
    FillCADConstantDetail(SPLINESEGS,    70),
    FillCADConstantDetail(SPLINETYPE,    70),
    FillCADConstantDetail(SURFTAB1,      70),
    FillCADConstantDetail(SURFTAB2,      70),
    FillCADConstantDetail(SURFTYPE,      70),
    FillCADConstantDetail(SURFU,         70),
    FillCADConstantDetail(SURFV,         70),
    FillCADConstantDetail(TDCREATE,      40),
    FillCADConstantDetail(TDINDWG,       40),
    FillCADConstantDetail(TDUCREATE,     40),
    FillCADConstantDetail(TDUPDATE,      40),
    FillCADConstantDetail(TDUSRTIMER,    40),
    FillCADConstantDetail(TDUUPDATE,     40),
    FillCADConstantDetail(TEXTSIZE,      40),
    FillCADConstantDetail(TEXTSTYLE,      7),
    FillCADConstantDetail(THICKNESS,     40),
    FillCADConstantDetail(TILEMODE,      70),
    FillCADConstantDetail(TRACEWID,      40),
    FillCADConstantDetail(TREEDEPTH,     70),
    FillCADConstantDetail(UCSBASE,        2),
    FillCADConstantDetail(UCSNAME,        2),
    FillCADConstantDetail(UCSORG,    123),
    FillCADConstantDetail(UCSORGBACK,123),
    FillCADConstantDetail(UCSORGBOTTOM, 123),
    FillCADConstantDetail(UCSORGFRONT,  123),
    FillCADConstantDetail(UCSORGLEFT,   123),
    FillCADConstantDetail(UCSORGRIGHT,  123),
    FillCADConstantDetail(UCSORGTOP,    123),
    FillCADConstantDetail(UCSORTHOREF,       2),
    FillCADConstantDetail(UCSORTHOVIEW,     70),
    FillCADConstantDetail(UCSXDIR,      123),
    FillCADConstantDetail(UCSYDIR,      123),
    FillCADConstantDetail(UNITMODE,         70),
    FillCADConstantDetail(USERI1,    70),
    FillCADConstantDetail(USERI2,    70),
    FillCADConstantDetail(USERI3,    70),
    FillCADConstantDetail(USERI4,    70),
    FillCADConstantDetail(USERI5,    70),
    FillCADConstantDetail(USERR1,    40),
    FillCADConstantDetail(USERR2,    40),
    FillCADConstantDetail(USERR3,    40),
    FillCADConstantDetail(USERR4,    40),
    FillCADConstantDetail(USERR5,    40),
    FillCADConstantDetail(USRTIMER,  70),
    FillCADConstantDetail(VERSIONGUID,2),
    FillCADConstantDetail(VISRETAIN, 70),
    FillCADConstantDetail(WORLDVIEW, 70),
    FillCADConstantDetail(XCLIPFRAME, 290),
    FillCADConstantDetail(XEDIT,      290),
    {-1, -1, NULL}
};

//------------------------------------------------------------------------------
// CADHandle
//------------------------------------------------------------------------------

CADHandle::CADHandle(char code)
{
    m_nCode = code;
}

void CADHandle::AddOffset(char val)
{
    m_HandleOrOffset.push_back(val);
}

long CADHandle::GetAsLong( CADHandle& ref_handle )
{
    long result = 0;
    switch ( m_nCode )
    {
        case 0x06:
        {
            memcpy ( &result, ref_handle.m_HandleOrOffset.data(), m_HandleOrOffset.size() );
            SwapEndianness ( result, m_HandleOrOffset.size() );
            return result + 1;
        }
        case 0x08:
        {
            memcpy ( &result, ref_handle.m_HandleOrOffset.data(), m_HandleOrOffset.size() );
            SwapEndianness ( result, m_HandleOrOffset.size() );
            return result - 1;
        }
        case 0x0A:
        {
            memcpy ( &result, ref_handle.m_HandleOrOffset.data(), m_HandleOrOffset.size() );
            SwapEndianness ( result, m_HandleOrOffset.size() );
            return result + this->GetAsLong ();
        }
        case 0x0C:
        {
            memcpy ( &result, ref_handle.m_HandleOrOffset.data(), m_HandleOrOffset.size() );
            SwapEndianness ( result, m_HandleOrOffset.size() );
            return result - this->GetAsLong ();
        }
    }

    return this->GetAsLong ();
}

long CADHandle::GetAsLong () const
{
    long result = 0;
    memcpy ( &result, m_HandleOrOffset.data(), m_HandleOrOffset.size() );
    SwapEndianness ( result, m_HandleOrOffset.size() );
    return result;
}

//------------------------------------------------------------------------------
// CADVariant
//------------------------------------------------------------------------------

CADVariant::CADVariant()
{
    m_eType = DataType::INVALID;
}

CADVariant::CADVariant(const char* val)
{
    m_eType = DataType::STRING;
    m_sString = std::string(val);
}

CADVariant::CADVariant(int val)
{
    m_eType = DataType::DECIMAL;
    m_nDecimal = val;
}

CADVariant::CADVariant(short val)
{
    m_eType = DataType::DECIMAL;
    m_nDecimal = val;
}

CADVariant::CADVariant(double val)
{
    m_eType = DataType::REAL;
    m_dX = val;
}

CADVariant::CADVariant(double x, double y, double z)
{
    m_eType = DataType::COORDINATES;
    m_dX = x;
    m_dY = y;
    m_dZ = z;
}

CADVariant::CADVariant(const std::string& val)
{
    m_eType = DataType::STRING;
    m_sString = val;
}

CADVariant::CADVariant(time_t val)
{
    m_eType = DataType::DATETIME;
    m_DateTime = val;
}

CADVariant::CADVariant(const CADHandle& val)
{
    m_eType = DataType::HANDLE;
    m_Handle = val;
}

CADVariant::CADVariant(const CADVariant& orig)
{
    m_eType = orig.m_eType;
    m_sString = orig.m_sString;
    m_nDecimal = orig.m_nDecimal;
    m_dX = orig.m_dX;
}

CADVariant& CADVariant::operator = (const CADVariant& orig)
{
    if (this == &orig)
        return *this;
    m_eType = orig.m_eType;
    m_sString = orig.m_sString;
    m_nDecimal = orig.m_nDecimal;
    m_dX = orig.m_dX;
    return *this;
}

long CADVariant::GetDecimal() const
{
    return m_nDecimal;
}

double CADVariant::GetReal() const
{
    return m_dX;
}

const std::string &CADVariant::GetString() const
{
    return m_sString;
}

CADVariant::DataType CADVariant::GetType() const
{
    return m_eType;
}

double CADVariant::GetX() const
{
    return m_dX;
}

double CADVariant::GetY() const
{
    return m_dY;
}

double CADVariant::GetZ() const
{
    return m_dZ;
}

const CADHandle &CADVariant::GetHandle() const
{
    return m_Handle;
}

//------------------------------------------------------------------------------
// CADHeader
//------------------------------------------------------------------------------

CADHeader::CADHeader()
{

}

int CADHeader::AddValue(short code, const CADVariant &val)
{
    if(m_moValues.find(code) != m_moValues.end())
        return CADErrorCodes::VALUE_EXISTS;

    m_moValues[code] = val;
    return CADErrorCodes::SUCCESS;
}

int CADHeader::AddValue(short code, const char* val)
{
    return AddValue(code, CADVariant(val));
}

int CADHeader::AddValue(short code, int val)
{
    return AddValue(code, CADVariant(val));
}

int CADHeader::AddValue(short code, short val)
{
    return AddValue(code, CADVariant(val));
}

int CADHeader::AddValue(short code, double val)
{
    return AddValue(code, CADVariant(val));
}

int CADHeader::AddValue(short code, const std::string& val)
{
    return AddValue(code, CADVariant(val));
}

int CADHeader::AddValue(short code, bool val)
{
    return AddValue(code, CADVariant(val ? 1 : 0));
}

int CADHeader::AddValue(short code, double x, double y, double z)
{
    return AddValue(code, CADVariant(x, y, z));
}

int CADHeader::AddValue(short code, long julianday, long milliseconds)
{
    // unix -> julian        return ( unixSecs / 86400.0 ) + 2440587.5;
    // julian -> unix        return (julian - 2440587.5) * 86400.0

    double seconds = double(milliseconds) / 1000;
    double unix = (double(julianday) - 2440587.5) * 86400.0;
    time_t fullSeconds = unix + seconds;
    return AddValue(code, CADVariant(fullSeconds));
}

int CADHeader::GetGroupCode(short code) const
{
    for(CADHeaderConstantDetail detail : CADHeaderConstantDetails)
    {
        if(detail.nConstant == code)
            return detail.nGroupCode;
    }

    return -1;
}

const CADVariant &CADHeader::GetValue(short code, const CADVariant& val) const
{
    auto it = m_moValues.find(code);
    if(it != m_moValues.end())
        return it->second;
    else
        return val;
}

const char *CADHeader::GetValueName(short code) const
{
    for(CADHeaderConstantDetail detail : CADHeaderConstantDetails)
    {
        if(detail.nConstant == code)
            return detail.pszValueName;
    }
    return nullptr;
}



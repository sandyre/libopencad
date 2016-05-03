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
    FillCADConstantDetail(ACADVER,      1),
    FillCADConstantDetail(ANGBASE,      50),
    FillCADConstantDetail(ANGDIR,       70),
    {-1, -1, NULL}
};

//------------------------------------------------------------------------------
// CADVariant
//------------------------------------------------------------------------------

CADVariant::CADVariant()
{
    m_eType = INVALID;
}

CADVariant::CADVariant(const char* val)
{
    m_eType = STRING;
    m_sString = std::string(val);
}

CADVariant::CADVariant(int val)
{
    m_eType = DECIMAL;
    m_nDecimal = val;
}

CADVariant::CADVariant(short val)
{
    m_eType = DECIMAL;
    m_nDecimal = val;
}

CADVariant::CADVariant(double val)
{
    m_eType = REAL;
    m_dX = val;
}

CADVariant::CADVariant(double x, double y, double z)
{
    m_eType = COORDINATES;
    m_dX = x;
    m_dY = y;
    m_dZ = z;
}

CADVariant::CADVariant(const std::string& val)
{
    m_eType = STRING;
    m_sString = val;
}

CADVariant::CADVariant(const struct CADHandle& val)
{
    m_eType = HANDLE;
    m_nDecimal = val.pHandleOrOffset;
    m_nCode = val.nCode;
    m_nCounter = val.nCounter;
}

CADVariant::CADVariant(const CADVariant& orig)
{
    m_eType = orig.m_eType;
    m_sString = orig.m_sString;
    m_nDecimal = orig.m_nDecimal;
    m_dReal = orig.m_dReal;
}

CADVariant& CADVariant::operator = (const CADVariant& orig)
{
    if (this == &orig)
        return *this;
    m_eType = orig.m_eType;
    m_sString = orig.m_sString;
    m_nDecimal = orig.m_nDecimal;
    m_dReal = orig.m_dReal;
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

const std::__cxx11::string &CADVariant::GetString() const
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

struct CADHandle CADVariant::GetHandle() const
{
    return {m_nCode, m_nCounter, (char*)m_nDecimal};
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

int CADHeader::GetGroupCode(short code) const
{
    for(int i = 0; CADHeaderConstantDetails[i].nConstant != -1; ++i)
    {
        if(CADHeaderConstantDetails[i].nConstant == code)
            return CADHeaderConstantDetails[i].nGroupCode;
    }
    return -1;
}

const CADVariant &CADHeader::GetValue(short code, const CADVariant& val) const
{
    std::map<short, CADVariant>::const_iterator it = m_moValues.find(code);
    if(it != m_moValues.end())
        return it->second;
    else
        return val;
}

const char *CADHeader::GetValueName(short code) const
{
    for(int i = 0; CADHeaderConstantDetails[i].nConstant != -1; ++i)
    {
        if(CADHeaderConstantDetails[i].nConstant == code)
            return CADHeaderConstantDetails[i].pszValueName;
    }
    return NULL;
}



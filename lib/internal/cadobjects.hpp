/*******************************************************************************
 *  Project: libopencad
 *  Purpose: OpenSource CAD formats support library
 *  Author: Alexandr Borzykh  mush3d at gmail.com
 *  Language: C++
 *******************************************************************************
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2017 Alexandr Borzykh
 *
 *  Permission is hereby granted  free of charge  to any person obtaining a copy
 *  of this software and associated documentation files (the "Software")  to deal
 *  in the Software without restriction  including without limitation the rights
 *  to use  copy  modify  merge  publish  distribute  sublicense  and/or sell
 *  copies of the Software  and to permit persons to whom the Software is
 *  furnished to do so  subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS"  WITHOUT WARRANTY OF ANY KIND  EXPRESS OR
 *  IMPLIED  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY  
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM  DAMAGES OR OTHER
 *  LIABILITY  WHETHER IN AN ACTION OF CONTRACT  TORT OR OTHERWISE  ARISING FROM  
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *******************************************************************************/
#ifndef LIBOPENCAD_INTERNAL_OBJECTS_CADOBJECT_HPP
#define LIBOPENCAD_INTERNAL_OBJECTS_CADOBJECT_HPP

namespace libopencad
{

    struct CADObject
    {
        enum Type
        {
            UNUSED               = 0x0   // 0
            TEXT                 = 0x1   // 1
            ATTRIB               = 0x2   // 2
            ATTDEF               = 0x3   // 3
            BLOCK                = 0x4   // 4
            ENDBLK               = 0x5   // 5
            SEQEND               = 0x6   // 6
            INSERT               = 0x7   // 7
            MINSERT1             = 0x8   // 8
            MINSERT2             = 0x9   // 9
            VERTEX2D             = 0x0A  // 10
            VERTEX3D             = 0x0B  // 11
            VERTEX_MESH          = 0x0C  // 12
            VERTEX_PFACE         = 0x0D  // 13
            VERTEX_PFACE_FACE    = 0x0E  // 14
            POLYLINE2D           = 0x0F  // 15
            POLYLINE3D           = 0x10  // 16
            ARC                  = 0x11  // 17
            CIRCLE               = 0x12  // 18
            LINE                 = 0x13  // 19
            DIMENSION_ORDINATE   = 0x14  // 20
            DIMENSION_LINEAR     = 0x15  // 21
            DIMENSION_ALIGNED    = 0x16  // 22
            DIMENSION_ANG_3PT    = 0x17  // 23
            DIMENSION_ANG_2LN    = 0x18  // 24
            DIMENSION_RADIUS     = 0x19  // 25
            DIMENSION_DIAMETER   = 0x1A  // 26
            POINT                = 0x1B  // 27
            FACE3D               = 0x1C  // 28
            POLYLINE_PFACE       = 0x1D  // 29
            POLYLINE_MESH        = 0x1E  // 30
            SOLID                = 0x1F  // 31
            TRACE                = 0x20  // 32
            SHAPE                = 0x21  // 33
            VIEWPORT             = 0x22  // 34
            ELLIPSE              = 0x23  // 35
            SPLINE               = 0x24  // 36
            REGION               = 0x25  // 37
            SOLID3D              = 0x26  // 38
            BODY                 = 0x27  // 39
            RAY                  = 0x28  // 40
            XLINE                = 0x29  // 41
            DICTIONARY           = 0x2A  // 42
            OLEFRAME             = 0x2B  // 43
            MTEXT                = 0x2C  // 44
            LEADER               = 0x2D  // 45
            TOLERANCE            = 0x2E  // 46
            MLINE                = 0x2F  // 47
            BLOCK_CONTROL_OBJ    = 0x30  // 48
            BLOCK_HEADER         = 0x31  // 49
            LAYER_CONTROL_OBJ    = 0x32  // 50
            LAYER                = 0x33  // 51
            STYLE_CONTROL_OBJ    = 0x34  // 52
            STYLE1               = 0x35  // 53
            STYLE2               = 0x36  // 54
            STYLE3               = 0x37  // 55
            LTYPE_CONTROL_OBJ    = 0x38  // 56
            LTYPE1               = 0x39  // 57
            LTYPE2               = 0x3A  // 58
            LTYPE3               = 0x3B  // 59
            VIEW_CONTROL_OBJ     = 0x3C  // 60
            VIEW                 = 0x3D  // 61
            UCS_CONTROL_OBJ      = 0x3E  // 62
            UCS                  = 0x3F  // 63
            VPORT_CONTROL_OBJ    = 0x40  // 64
            VPORT                = 0x41  // 65
            APPID_CONTROL_OBJ    = 0x42  // 66
            APPID                = 0x43  // 67
            DIMSTYLE_CONTROL_OBJ = 0x44  // 68
            DIMSTYLE             = 0x45  // 69
            VP_ENT_HDR_CTRL_OBJ  = 0x46  // 70
            VP_ENT_HDR           = 0x47  // 71
            GROUP                = 0x48  // 72
            MLINESTYLE           = 0x49  // 73
            OLE2FRAME            = 0x4A  // 74
            DUMMY                = 0x4B  // 75
            LONG_TRANSACTION     = 0x4C  // 76
            LWPOLYLINE           = 0x4D  // 77
            HATCH                = 0x4E  // 78
            XRECORD              = 0x4F  // 79
            ACDBPLACEHOLDER      = 0x50  // 80
            VBA_PROJECT          = 0x51  // 81
            LAYOUT               = 0x52  // 82
            // Codes below arent fixed  libopencad uses it for reading  in writing it will be different!
            CELLSTYLEMAP         = 0x53  // 83
            DBCOLOR              = 0x54  // 84
            DICTIONARYVAR        = 0x55  // 85
            DICTIONARYWDFLT      = 0x56  // 86
            FIELD                = 0x57  // 87
            GROUP_UNFIXED        = 0x58  // 88
            HATCH_UNFIXED        = 0x59  // 89
            IDBUFFER             = 0x5A  // 90
            IMAGE                = 0x5B  // 91
            IMAGEDEF             = 0x5C  // 92
            IMAGEDEFREACTOR      = 0x5D  // 93
            LAYER_INDEX          = 0x5E  // 94
            LAYOUT_UNFIXED       = 0x5F  // 95
            LWPOLYLINE_UNFIXED   = 0x60  // 96
            MATERIAL             = 0x61  // 97
            MLEADER              = 0x62  // 98
            MLEADERSTYLE         = 0x63  // 99
            OLE2FRAME_UNFIXED    = 0x64  // 100
            PLACEHOLDER          = 0x65  // 101
            PLOTSETTINGS         = 0x66  // 102
            RASTERVARIABLES      = 0x67  // 103
            SCALE                = 0x68  // 104
            SORTENTSTABLE        = 0x69  // 105
            SPATIAL_FILTER       = 0x6A  // 106
            SPATIAL_INDEX        = 0x6B  // 107
            TABLEGEOMETRY        = 0x6C  // 108
            TABLESTYLES          = 0x6D  // 109
            VBA_PROJECT_UNFIXED  = 0x6E  // 110
            VISUALSTYLE          = 0x6F  // 111
            WIPEOUTVARIABLE      = 0x70  // 112
            XRECORD_UNFIXED      = 0x71  // 113
            WIPEOUT              = 0x72  // 114
        };

        Type        _type;
        int32_t     _size;
        int16_t     _crc;
    };

}

#endi
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

#ifndef CADFILE_H
#define CADFILE_H

#include "cadtables.h"
#include "cadclasses.h"
#include "cadfileio.h"

#include "cadobjects.h"


class Layer;
class CADGeometry;

/**
 * @brief The abstact CAD file class
 */
class OCAD_EXTERN CADFile
{
    friend class Layer;
public:
    /**
     * @brief The CAD file open options enum
     */
    enum OpenOptions
    {
        READ_ALL,       /**< read all available information */
        FAST_READ,      /**< read some methadata */
        FASTEST_READ    /**< read only geometry and layers */
    };

public:
    CADFile (CADFileIO* poFileIO);
    virtual ~CADFile();

public:
    const CADHeader& GetHeader() const;
    const CADClasses& GetClasses() const;
    const CADTables& GetTables() const;

public:
    virtual int ParseFile(enum OpenOptions eOptions);
    virtual size_t GetLayersCount();
    virtual size_t GetBlocksCount();
    virtual Layer * GetLayer( size_t index );
    virtual CADBlock * GetBlock( size_t index );

protected:
    virtual CADObject * GetObject( size_t index );
    virtual CADGeometry * GetGeometry( size_t layer_index, size_t index );

    /**
     * @brief Read header from CAD file
     * @return CADErrorCodes::SUCCESS if OK, or error code
     */
    virtual int ReadHeader() = 0;

    /**
     * @brief Read classes from CAD file
     * @return CADErrorCodes::SUCCESS if OK, or error code
     */
    virtual int ReadClasses() = 0;

    /**
     * @brief Create the file map for fast access to CAD objects
     * @return CADErrorCodes::SUCCESS if OK, or error code
     */
    virtual int CreateFileMap() = 0;

    /**
     * @brief Read tables from CAD file
     * @param eOptions Read options
     * @return CADErrorCodes::SUCCESS if OK, or error code
     */
    virtual int ReadTables(enum OpenOptions eOptions) = 0;

protected:
    CADFileIO* m_poFileIO;
    CADHeader m_oHeader;
    CADClasses m_oClasses;
    CADTables m_oTables;

protected:
    std::map<long, long> m_mdObjectsMap; // object index <-> file offset
};


#endif // CADFILE_H

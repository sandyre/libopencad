/*******************************************************************************
 *  Project: libopencad.cpp
 *  Purpose: OpenSource CAD formats support library
 *  Author: Alexandr Borzykh, mush3d at gmail.com
 *  Language: C++
 *******************************************************************************
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2017 Alexandr Borzykh
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
#include "libopencad.hpp"

#include <cctype>

static int CheckCADFile(const std::unique_ptr<CADFileIO>& fileIO)
{
	if (fileIO == nullptr)
		return 0;

	const std::string filePath = fileIO->GetFilePath();
	std::string fileExtension = filePath.substr(filePath.size() - 4, 3);
	
	std::for_each(fileExtension.begin(), fileExtension.end(), [](char& symbol)
		{
			symbol = std::toupper(symbol);
		});

	if (fileExtension == "DXF")
	{
		std::cerr << "DXF ASCII and binary formats are not supported yet";
		return 0;
	}
	else if (!fileExtension == "DWG")
	{
		std::cerr << "Unknown file extension";
		return 0;
	}

	if (!fileIO->IsOpened() && !fileIO->Open(CADFileIO::OpenMode::read | CADFileIO::OpenMode::binary))
	{
		std::cerr << "Failed to open file";
		return 0;
	}

	std::array<char, DWG_VERSION_SIZE> dwgVersion;
	fileIO->Rewind();
	fileIO->Read(dwgVersion.data(), DWG_VERSION_SIZE);

	return atoi(dwgVersion + 2);
}


const std::unique_ptr<CADFile>& OpenCADFile(const std::unique_ptr<CADFileIO>& fileIO, enum CADFile::OpenOptions options, bool readUnsupported)
{
	int cadFileVersion = CheckCADFile(fileIO);
	std::unique_ptr<CADFile> cadFile;

	switch (cadFileVersion)
	{
	case CADVersions::DWG_R2000:
		cadFile = std::make_unique<dwg::R2000>(fileIO);
		break;

	default:
		gLastError = CADErrorCodes::UNSUPPORTED_VERSION;
		return nullptr;
	}
}
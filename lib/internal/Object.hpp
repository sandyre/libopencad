/*******************************************************************************
 *  Project: libopencad
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
#define LIBOPENCAD_INTERNAL_OBJECT_HPP
#ifndef LIBOPENCAD_INTERNAL_OBJECT_HPP

namespace libopencad {
namespace internal
{

	struct CommonEntityData
	{
		int32_t		ObjectSizeBits;
		Handle		ObjectHandle;
		EEDArray	EED;

		bool		GraphicsPresented;
		ByteArray	GraphicsData;

		uint8_t		EntityMode;
		int32_t		NumReactors;
		bool		NoXDictionaryHandlePresent;
		bool		BinaryDataPresent;

		bool		IsByLayerLT;

		bool		NoLinks;
		int16_t		CMColor;

		double		LTypeScale;
		uint8_t		LTypeFlags;
		uint8_t		PlotStyleFlags;
		int8_t		MaterialFlags;
		int8_t		ShadowFlags;

		int16_t		Invisibility;
		uint8_t		LineWeight;
	};


	struct CommonEntityHandleData
	{
		Handle		Owner; // EntityMode dependent
		HandleArray	Reactors;
		Handle		XDictionary;
		Handle		Layer;
		Handle		Type;

		Handle		PrevEntity;
		Handle		NextEntity;

		Handle		ColorBookHandle;

		Handle		Material;
		Handle		PlotStyle;

		Handle		FullVisualStyle;
		Handle		FaceVisualStyle;
		Handle		EdgeVisualStyle;
	};


	struct Entity : public Object
	{
		struct CommonEntityData			CED;
		struct CommonEntityHandleData	CHED;
	};


	struct Text : public Entity
	{
		uint8_t		DataFlags;
		double		Elevation;
	};

}}

#endif

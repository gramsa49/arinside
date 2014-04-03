//Copyright (C) 2014 John Luthgers | jls17
//
//This file is part of ARInside.
//
//    ARInside is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    ARInside is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with ARInside.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "ImageTag.h"
#include "../core/ARServerObject.h"
#include "../output/RootPath.h"

namespace OUTPUT
{
	struct ImageDimensions
	{
		unsigned short x;
		unsigned short y;
	};

	// the following function maps the imageId to a file name
	// make sure new images are added here
	const char* GetImageName(int imageId)
	{
		switch (imageId)
		{
		case ImageTag::NoImage: return "";
		case ImageTag::Schema: return "schema.gif";
		case ImageTag::Server: return "server.gif";
		}
		// always throw an assert here, in case a undefined image is used!
		assert(false);
		return "";
	}

	ImageDimensions DefaultImageDimmensions = { 16, 16 };

	// the following provides the image dimensions (width and height)
	// for a particular imageId. If new images don't use the default
	// width and height of 16x16, make sure the correct dimension is
	// returned here.
	ImageDimensions GetImageDimensions(ImageTag::ImageEnum image)
	{
		switch (image)
		{
		}
		return DefaultImageDimmensions;
	}

	// ################################################################
	// ## ImageTag class definition
	ImageTag::ImageTag(OUTPUT::ImageTag::ImageEnum image, int currentRootLevel)
	{
		rootLevel = currentRootLevel;
		imageId = image;
	}

	ostream& ImageTag::ToStream(std::ostream &strm) const
	{
		if (imageId != NoImage)
		{
			const char* imageName = GetImageName(imageId);
			ImageDimensions imageDim = GetImageDimensions(imageId);

			strm << "<img ";
			strm << "src=\"" << RootPath(rootLevel) << "img/" << imageName << "\" ";
			strm << "width=\"" << imageDim.x << "\" height=\"" << imageDim.y << "\" alt=\"" << imageName << "\" />";
		}
		return strm;
	}

	ostream& operator <<(ostream &strm, OUTPUT::ImageTag::ImageEnum image)
	{
		ImageTag img(image, 0);
		return img.ToStream(strm);
	}

	ostream& operator <<(ostream &strm, const OUTPUT::ImageTag &image)
	{
		return image.ToStream(strm);
	}
}; // end namespace OUTPUT

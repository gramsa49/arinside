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

namespace OUTPUT
{
	const char* GetImageName(int imageId)
	{
		switch (imageId)
		{
		case ImageTag::Schema: return "schema.gif";
		}
		return "";
	}

	struct ImageDimensions
	{
		unsigned short x;
		unsigned short y;
	};

	ImageDimensions DefaultImageDimmensions = { 16, 16 };

	ImageDimensions GetImageDimensions(ImageTag::ImageEnum image)
	{
		switch (image)
		{
		}
		return DefaultImageDimmensions;
	}

	ImageTag::ImageTag(OUTPUT::ImageTag::ImageEnum image)
	{
		imageId = image;
	}

	ostream& ImageTag::ToStream(std::ostream &strm)
	{
		if (imageId != NoImage)
		{
			const char* imageName = GetImageName(imageId);
			ImageDimensions &imageDim = GetImageDimensions(imageId);

			strm << "<img ";
			strm << "src=\"" << "img/" << imageName << "\" ";
			strm << "width=\"" << imageDim.x << "\" height=\"" << imageDim.y << "\" alt=\"" << imageName << "\" />";
		}
		return strm;
	}
}; // end namespace OUTPUT

ostream& operator <<(ostream &strm, OUTPUT::ImageTag::ImageEnum image)
{
	ImageTag img(image);
	return img.ToStream(strm);
}

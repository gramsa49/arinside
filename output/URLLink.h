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

#pragma once

class CPageParams;

namespace OUTPUT
{
	class ImageTag;

	class URLLink
	{
	public:
		enum LinkTargetMode {
			TARGET_MODE_SELF = 0,
			TARGET_MODE_PARENT = 1,
			TARGET_MODE_TOP = 2,
			TARGET_MODE_BLANK = 3,
		};

		URLLink(const std::string &caption, const CPageParams &linkToPage, const OUTPUT::ImageTag &image, int rootLevel, bool validate, OUTPUT::URLLink::LinkTargetMode target);
		URLLink(const std::string &caption, const CPageParams &linkToPage, const OUTPUT::ImageTag &image, int rootLevel, OUTPUT::URLLink::LinkTargetMode target);	

		std::ostream& ToStream(std::ostream &strm) const;
	private:
		std::string link;

	private:
		void Init(const std::string &caption, const CPageParams &linkToPage, const OUTPUT::ImageTag &image, int rootLevel, bool validate, OUTPUT::URLLink::LinkTargetMode target);
		static void AddCaption(std::ostream &strm, const std::string &caption, bool validate);
	};

	std::ostream& operator<<(std::ostream &stream, const OUTPUT::URLLink &link);
};

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

namespace OUTPUT {

	class RootLevel
	{
	public:
		RootLevel(int rootLevel);
		const char * GetRootPath() const;

	private:
		int rootLevel;
	};

}; // end namespace OUTPUT

std::ostream& operator <<(std::ostream &stream, OUTPUT::RootLevel &rootLevelObj);

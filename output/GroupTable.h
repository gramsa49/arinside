//Copyright (C) 2009 Stefan Nerlich | stefan.nerlich@hotmail.com
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
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
#include "../ARInside.h"
#include "ObjectTable.h"

namespace OUTPUT
{
	class CGroupTable:
		public CObjectTable
	{
	public:
		CGroupTable(CARInside &arIn);
		~CGroupTable(void);

		void AddRow(string appRefName, int id, int rootLevel);
	private:
		void AddGroupRow(string appRefName, int groupId, int rootLevel);
		void AddRoleRow(string appRefName, int roleId, int rootLevel);
	};
}

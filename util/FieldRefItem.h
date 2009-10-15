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

class CFieldRefItem
{
public:
	CFieldRefItem();
	CFieldRefItem(int arsStructItemType, string fromName, string description, int fieldInsideId, int schemaInsideId);
	~CFieldRefItem(void);

	int arsStructItemType;
	string fromName;
	int fromFieldId;	// use this to create a reference between two field (e.g. a field used as column in a table); fromName should hold the schema name in this case
	string description;
	int fieldInsideId;
	int schemaInsideId;
};

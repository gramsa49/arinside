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
#include "DocBase.h"
#include "../core/ARProplistHelper.h"

class CDocCharMenuDetails :
	public CDocBase
{
public:
	CDocCharMenuDetails(unsigned int menuInsideId);
	~CDocCharMenuDetails(void);

	void Documentation();
private:
	CARCharMenu menu;

	string CharMenuDetails();
	string SearchMenuDetails();
	string FileMenuDetails();
	string SqlMenuDetails();
	string DataDictMenuDetails();
	string GetFieldTypes(unsigned int fieldMask);
	string RelatedFields();
	string RelatedActiveLinks();

	string GetSQLLabelList(const ARCharMenuSQLStruct* sqlMenu);
};

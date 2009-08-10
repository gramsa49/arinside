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
#include ".\docbase.h"
#include ".\docfilteractionstruct.h"
#include "..\output\containertable.h"
#include "..\output\grouptable.h"
#include "..\core\arproplisthelper.h"

class CDocEscalationDetails :
	public CDocBase
{
public:
	CDocEscalationDetails(CARInside &arInside, CAREscalation &arEscalation, string path, int rootLevel);
	~CDocEscalationDetails(void);

	void Documentation();
private:
	CAREscalation *pEscal;

	string CreateSpecific(string schemaName);
	string ContainerReferences();
};

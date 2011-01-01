//Copyright (C) 2011 John Luthgers | jls17
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

#include "stdafx.h"
#include "ScanMain.h"
#include "../ARInside.h"
#include "ScanSchema.h"
#include "ScanActiveLinks.h"
#include "ScanFilters.h"
#include "ScanEscalations.h"
#include "ScanContainers.h"

CScanMain::CScanMain(void)
{
}

CScanMain::~CScanMain(void)
{
}

void CScanMain::BuildReferences()
{
	CScanSchema::Start();
	CScanActiveLinks::Start();
	CScanFilters::Start();
	CScanEscalations::Start();
	CScanContainers::Start();

	SortReferences();
}

void CScanMain::SortReferences()
{
	CARInside::GetInstance()->schemaList.SortReferences();
}


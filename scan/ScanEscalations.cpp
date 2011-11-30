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
#include "ScanEscalations.h"
#include "../ARInside.h"

CScanEscalations::CScanEscalations(CAREscalation& escalation)
: escal(escalation)
{
}

CScanEscalations::~CScanEscalations(void)
{
}

void CScanEscalations::Start()
{
	cout << "Checking escalation references";

	CARInside* pInside = CARInside::GetInstance();
	unsigned int escalCount = pInside->escalationList.GetCount();
	for (unsigned int escalIndex = 0; escalIndex < escalCount; ++escalIndex)
	{
		CAREscalation	escal(escalIndex);
		if (pInside->appConfig.bOverlaySupport && !IsVisibleOverlay(escal))
			continue;

		CScanEscalations scanEsc(escal);
		scanEsc.Scan();
	}

	cout << endl;
}

void CScanEscalations::Scan()
{
	const ARWorkflowConnectStruct& connectList = escal.GetSchemaList();

	if (connectList.type == AR_WORKFLOW_CONN_SCHEMA_LIST)
	{
		for (unsigned int connectIndex = 0; connectIndex < connectList.u.schemaList->numItems; ++connectIndex)
		{
			CARSchema schema(connectList.u.schemaList->nameList[connectIndex]);
			if (schema.Exists())
			{
				schema.AddEscalation(escal);
			}
		}
	}
}


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
#include "ScanActiveLinks.h"
#include "../ARInside.h"
#include "../core/ARSchema.h"
#include "../core/ARActiveLink.h"


CScanActiveLinks::CScanActiveLinks(CARActiveLink& actlink)
:al(actlink)
{
}

CScanActiveLinks::~CScanActiveLinks(void)
{
}

void CScanActiveLinks::Start()
{
	cout << "Checking active link references";

	unsigned int actlinkCount = CARInside::GetInstance()->alList.GetCount();
	for (unsigned int actlinkIndex = 0; actlinkIndex < actlinkCount; actlinkIndex++)
	{
		CARActiveLink al(actlinkIndex);
		CScanActiveLinks scanAl(al);
		scanAl.Scan();
	}

	cout << endl;
}

void CScanActiveLinks::Scan()
{
	const ARWorkflowConnectStruct& connectList = al.GetSchemaList();

	// scan connected schemas
	if (connectList.type == AR_WORKFLOW_CONN_SCHEMA_LIST)
	{
		for (unsigned int connectIndex = 0; connectIndex < connectList.u.schemaList->numItems; ++connectIndex)
		{
			CARSchema schema(connectList.u.schemaList->nameList[connectIndex]);
			if (schema.Exists())
			{
				// the following list is later used for action-parsing to add a reference to each connected schema
				schemaList.push_back(schema.GetInsideId());
				schema.AddActiveLink(al);
			}
		}
	}

	// scan if and else actions
	ScanActions(al, al.GetIfActions(), IES_IF);
	ScanActions(al, al.GetElseActions(), IES_ELSE);
}

void CScanActiveLinks::ScanActions(CARActiveLink& al, const ARActiveLinkActionList& actions, IfElseState ifElse)
{
	for (unsigned int actionIndex = 0; actionIndex < actions.numItems; ++actionIndex)
	{
		switch (actions.actionList[actionIndex].action)
		{
		case AR_ACTIVE_LINK_ACTION_SET_CHAR:
			{
				const ARFieldCharacteristics& action = actions.actionList[actionIndex].u.characteristics;

				// create a reference to the changed field or is a field changed by a fields value?
				int msgId = (action.option == AR_FIELD_CHAR_OPTION_REFERENCE ? REFM_CHANGEFIELD_OF_FIELDS_VALUE : REFM_CHANGEFIELD);
				CRefItem refItem(al, ifElse, actionIndex, msgId);
				AddReferenceForEachSchema(action.fieldId, refItem);

				if (action.charMenu[0] != 0 && strcmp(action.charMenu, "$NULL$") != 0)
				{
					// add a reference to the menu or save as a missing menu reference
					CRefItem refItem(al, ifElse, actionIndex, REFM_CHANGEFIELD);
					CARInside::GetInstance()->AddMenuReference(action.charMenu, refItem);
				}
			}
			break;
		}
	}
}

void CScanActiveLinks::AddReferenceForEachSchema(int fieldId, const CRefItem &refItem)
{
	SchemaList::iterator curIt = schemaList.begin();
	SchemaList::iterator endIt = schemaList.end();

	for (; curIt != endIt; ++curIt)
	{
		CARInside::GetInstance()->AddFieldReference(*curIt, fieldId, refItem);
	}
}

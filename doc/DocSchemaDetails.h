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
#include "../output/Table.h"
#include "../output/TableRow.h"
#include "../output/FilterTable.h"
#include "../output/AlTable.h"
#include "../output/EscalTable.h"
#include "../output/ContainerTable.h"
#include "../output/MenuTable.h"
#include "../output/SchemaTable.h"

class CDocSchemaDetails :
	public CDocBase
{
public:
	CDocSchemaDetails(CARInside &arInside, CARSchema &schema, string path, int rootLevel);
	~CDocSchemaDetails(void);

	void Documentation();
private:
	CARSchema *pSchema;

	void IndexDoc();
	void ResultListDoc();
	void SortListDoc();
	void VuiListDoc();
	void SchemaFilterDoc();
	void SchemaAlDoc();
	void SchemaEscalDoc();
	void SchemaPermissionDoc();
	void SchemaSubadminDoc();
	void WorkflowDoc();


	string AllFields(string fName);
	void AllFieldsCsv(string fName);
	string AllFieldsJoin(string fName);
	void AllFieldsJoinCsv(string fName);

	string TypeDetails();
	string ContainerReferences();
	string TableFieldReferences();
	string AlPushFieldsReferences();
	string FilterPushFieldsReferences();
	string AlWindowOpenReferences();
	string JoinFormReferences();
	string SearchMenuReferences();
	
	string ShowProperties();
	string ShowBasicProperties(CARProplistHelper* propIndex);
	string ShowAuditProperties();
	string ShowArchiveProperties();

	string SchemaNavigation();	
	string FormPageHeader(string description);

	bool InAlList(string objName);
	bool InFilterList(string objName);
	bool InEscalList(string objName);
	list<string> uniqueAlList;
	list<string> uniqueFilterList;
	list<string> uniqueEscalList;
	CTableRow UniqueTblRow(string name, int itemType);
};

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
	CDocSchemaDetails(unsigned int schemaInsideId, int rootLevel);
	~CDocSchemaDetails(void);

	void Documentation();
private:
	CARSchema schema;
	int overlayType;

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


	string AllFields();
	void AllFieldsCsv();
	string AllFieldsSpecial();
	void AllFieldsSpecialCsv();

	string TypeDetails();
	string ContainerReferences();
	string TableFieldReferences();
	string AlPushFieldsReferences();
	string FilterPushFieldsReferences();
	string AlWindowOpenReferences();
	string JoinFormReferences();
	string SearchMenuReferences();
	
	string ShowProperties();
	void ShowBasicProperties(std::ostream& strm, CARProplistHelper* propIndex);
	void ShowAuditProperties(std::ostream& strm);
	void ShowArchiveProperties(std::ostream& strm);

	string SchemaNavigation();	
	string FormPageHeader(string description);

	bool InAlList(string objName);
	bool InFilterList(string objName);
	bool InEscalList(string objName);
	bool IsSchemaInWFConnectStruct(const ARWorkflowConnectStruct& wfCS);
	list<string> uniqueAlList;
	list<string> uniqueFilterList;
	list<string> uniqueEscalList;

	void AddTableRow(CTable& tbl, CARActiveLink& al);
	void AddTableRow(CTable& tbl, CARFilter& flt);
	void AddTableRow(CTable& tbl, CAREscalation& esc);
	void AddTableRow(CTable& tbl, CARContainer& cont);
};

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

#include "stdafx.h"
#include "DocSchemaDetails.h"
#include "DocFieldDetails.h"
#include "../output/CsvPage.h"
#include "../core/ARDayStructHelper.h"
#include "DocActionOpenWindowHelper.h"

CDocSchemaDetails::CDocSchemaDetails(unsigned int schemaInsideId, int rootLevel)
: schema(schemaInsideId)
{
	this->uniqueAlList.clear();
	this->uniqueEscalList.clear();
	this->uniqueFilterList.clear();
	this->overlayType = 0;
}

CDocSchemaDetails::~CDocSchemaDetails(void)
{
}

string CDocSchemaDetails::FormPageHeader(string description)
{
	//ContentHead informations
	stringstream contHeadStrm;
	contHeadStrm.str("");
	contHeadStrm << CWebUtil::LinkToSchemaIndex(this->rootLevel) << endl;
	contHeadStrm << MenuSeparator <<this->pInside->LinkToSchemaTypeList(this->schema.GetCompound().schemaType, rootLevel) << endl;
	contHeadStrm << MenuSeparator << CWebUtil::Link(this->schema.GetName(), CPageParams(PAGE_DETAILS, &schema), "", rootLevel);
	contHeadStrm << CAREnum::GetOverlayTypeString(overlayType);
	contHeadStrm << MenuSeparator << CWebUtil::ObjName(description) << endl;
	return contHeadStrm.str();
}

void CDocSchemaDetails::Documentation()
{
	try
	{
		CPageParams file(PAGE_DETAILS, &this->schema);
		rootLevel = file->GetRootLevel();
		string path = file->GetPath();

		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(path)>=0)
		{
			stringstream pgStrm;	
			CWebPage webPage(file->GetFileName(), this->schema.GetName(), rootLevel, this->pInside->appConfig);
			
			const ARCompoundSchema& compSchema = this->schema.GetCompound();
			overlayType = schema.GetOverlayType();

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << CWebUtil::LinkToSchemaIndex(rootLevel) << endl;
			contHeadStrm << MenuSeparator << this->pInside->LinkToSchemaTypeList(compSchema.schemaType, rootLevel) << endl;
			contHeadStrm << MenuSeparator << CWebUtil::ObjName(this->schema.GetName());
			contHeadStrm << CAREnum::GetOverlayTypeString(overlayType);

			CARSchema overlayObj;

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport && overlayType > 0)
			{
				string correspondingName;
				switch (overlayType)
				{
				case AR_OVERLAID_OBJECT:
					correspondingName = schema.GetOverlayName();
					break;
				case AR_OVERLAY_OBJECT:
					correspondingName = schema.GetOverlayBaseName();
					break;
				}

				CARSchema correspondingObject(correspondingName);
				overlayObj = correspondingObject;	
			}
#endif

			if(!this->schema.GetAppRefName().empty())
				contHeadStrm << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->schema.GetAppRefName(), rootLevel);

			webPage.AddContentHead(contHeadStrm.str(), PlaceOverlayLink(overlayType, overlayObj));

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->overlayMode == 1 && overlayType == AR_OVERLAID_OBJECT)
				pgStrm << pInside->PlaceOverlaidNotice(overlayObj, rootLevel);
#endif

			//Add schema navigation menu	
			webPage.SetNavigation(this->SchemaNavigation());


			//Schema Properties
			stringstream strmTmp;
			CTable tblObjProp("objProperties", "TblObjectList");
			tblObjProp.AddColumn(0, "Property");
			tblObjProp.AddColumn(0, "Value");

			//Schema type informations
			CTableRow row("cssStdRow");	
			CTableCell cellProp("");				
			CTableCell cellPropValue("");    


			if(compSchema.schemaType == AR_SCHEMA_JOIN
				|| compSchema.schemaType == AR_SCHEMA_VIEW
				|| compSchema.schemaType == AR_SCHEMA_VENDOR)
			{
				cellProp.content = "Schema Type Details";				
				cellPropValue.content = this->TypeDetails();    
				row.AddCell(cellProp);
				row.AddCell(cellPropValue);
				tblObjProp.AddRow(row);	
			}

			//Container References
			row.ClearCells();
			cellProp.content = "Container References";
			cellPropValue.content = this->ContainerReferences();
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);			

			//TableField References
			row.ClearCells();
			cellProp.content = "Table Fields datasource form";
			cellPropValue.content = this->TableFieldReferences();
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//AL window open references
			row.ClearCells();
			cellProp.content = "Active Link \"Open Window Action\"";
			cellPropValue.content = this->AlWindowOpenReferences();
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);

			//ActiveLink PushFields References
			row.ClearCells();
			cellProp.content = "Active Link \"Push Field Action\" to this form";
			cellPropValue.content = this->AlPushFieldsReferences();
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//Filter PushFields References
			row.ClearCells();
			cellProp.content = "Filter \"Push Field Action\" to this form";
			cellPropValue.content = this->FilterPushFieldsReferences();
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);

			//Join References
			row.ClearCells();
			cellProp.content = "Join Form References";
			cellPropValue.content = this->JoinFormReferences();
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);

			//Menu References
			row.ClearCells();
			cellProp.content = "Search Menu References";
			cellPropValue.content = this->SearchMenuReferences();
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);

			//Add table to page
			pgStrm << tblObjProp;
			tblObjProp.Clear();

			//Fields
			switch(compSchema.schemaType)
			{
			case AR_SCHEMA_JOIN:
			case AR_SCHEMA_VIEW:
			case AR_SCHEMA_VENDOR:
				pgStrm << this->AllFieldsSpecial();
				break;
			default:	
				pgStrm << this->AllFields();
				break;
			}

			//indexes
			this->IndexDoc();

			//sortlist
			this->SortListDoc();

			//resultlist link
			this->ResultListDoc();

			//views
			this->VuiListDoc();

			//permissions
			this->SchemaPermissionDoc();

			//Workflow
			this->WorkflowDoc();

			//subadmins
			this->SchemaSubadminDoc();

			//active links
			this->SchemaAlDoc();


			//filter
			this->SchemaFilterDoc();

			//escalation
			this->SchemaEscalDoc();


			webPage.AddContent(pgStrm.str());

			// Basics / Entry Points
			webPage.AddContent(ShowProperties());

			//Properties
			//webPage.AddContent(CARProplistHelper::GetList(*this->pInside, this->pSchema->objPropList));

			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(&this->schema, rootLevel));

			webPage.SaveInFolder(path);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema details documentation of '"<< this->schema.GetName() << "': " << e.what() << endl;
	}
}

string CDocSchemaDetails::SchemaNavigation()
{	
	CUList uList(this->rootLevel, "");

	try
	{
		//Views
		uList.AddItem(CUListItem(CWebUtil::Link("Views", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_VUI, &this->schema), "", rootLevel)));

		//Workflow		
		uList.AddItem(CUListItem(CWebUtil::Link("Workflow", CPageParams(PAGE_SCHEMA_WORKFLOW, &this->schema), "", rootLevel)));

		//Active Links
		uList.AddItem(CUListItem(CWebUtil::Link("Active Link", CPageParams(PAGE_SCHEMA_ACTIVELINKS, &this->schema), "", rootLevel)));

		//Filters
		uList.AddItem(CUListItem(CWebUtil::Link("Filter", CPageParams(PAGE_SCHEMA_FILTERS, &this->schema), "", rootLevel)));

		//Escalations
		uList.AddItem(CUListItem(CWebUtil::Link("Escalation", CPageParams(PAGE_SCHEMA_ESCALATIONS, &this->schema), "", rootLevel)));

		//ResultList
		uList.AddItem(CUListItem(CWebUtil::Link("Resultlist", CPageParams(PAGE_SCHEMA_RESULTLIST, &this->schema), "", rootLevel)));

		//Indexes
		uList.AddItem(CUListItem(CWebUtil::Link("Indexes", CPageParams(PAGE_SCHEMA_INDEXES, &this->schema), "", rootLevel)));

		//SortList
		uList.AddItem(CUListItem(CWebUtil::Link("Sortlist", CPageParams(PAGE_SCHEMA_SORTLIST, &this->schema), "", rootLevel)));

		//Permissions
		uList.AddItem(CUListItem(CWebUtil::Link("Permission", CPageParams(PAGE_SCHEMA_PERMISSIONS, &this->schema), "", rootLevel)));

		//Subadministrator
		uList.AddItem(CUListItem(CWebUtil::Link("Subadministrator", CPageParams(PAGE_SCHEMA_SUBADMINS, &this->schema), "", rootLevel)));
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema navigation of '" << this->schema.GetName() << "': " << e.what() << endl;
	}	

	return "<ul><span class=\"objName\">" + this->schema.GetURL(rootLevel, true) + "</span>"+ uList.ToXHtml("", true) + "</ul>";
}


//Create a page with all fields of this form
string CDocSchemaDetails::AllFields()
{		
	CTable tbl("fieldListAll", "TblObjectList");
	tbl.AddColumn(40, "Field Name");
	tbl.AddColumn(10, "Field ID");	
	tbl.AddColumn(10, "Datatype");
	tbl.AddColumn(10, "In Views");
	tbl.AddColumn(10, "Modified");
	tbl.AddColumn(20, "By");

	try
	{
		CARFieldList* fields = schema.GetFields();
		unsigned int fieldCount = fields->GetCount();
		for (unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
		{
			CARField field(schema.GetInsideId(), 0, fieldIndex);

			CTableRow row("");

			CTableCell cellName(field.GetURL(rootLevel), "");
			CTableCell cellFieldId(field.GetFieldId(), "");
			CTableCell cellDataType(CAREnum::DataType(field.GetDataType()), "");

			stringstream strmTmp;
			if(field.GetDisplayInstances().numItems == 0 && field.GetFieldId() != 15)				
				strmTmp << "<span class=\"fieldInNoView\">" << field.GetDisplayInstances().numItems << "</span" << endl;
			else
				strmTmp << field.GetDisplayInstances().numItems;			
			CTableCell cellNumViews(strmTmp.str(), "");			

			CTableCell cellTimestamp(CUtil::DateTimeToHTMLString(field.GetTimestamp()), "");
			CTableCell cellLastChanged(this->pInside->LinkToUser(field.GetLastChanged(), rootLevel), "");

			row.AddCell(cellName);
			row.AddCell(cellFieldId);
			row.AddCell(cellDataType);
			row.AddCell(cellNumViews);
			row.AddCell(cellTimestamp);
			row.AddCell(cellLastChanged);

			tbl.AddRow(row);
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << tbl.NumRows() << " fields (" << CWebUtil::Link("data", CPageParams(PAGE_SCHEMA_FIELDS_CSV, &this->schema), "", rootLevel) << ")" << endl;
		tbl.description = tblDesc.str();

		AllFieldsCsv();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema all fields of '"<< this->schema.GetName() << "': " << e.what() << endl;
	}

	return tbl.ToXHtml();	
}

void CDocSchemaDetails::AllFieldsCsv()
{		
	CTable tbl("fieldListAll", "TblObjectList");
	tbl.AddColumn(40, "Field Name");
	tbl.AddColumn(10, "Field ID");	
	tbl.AddColumn(10, "Datatype");
	tbl.AddColumn(10, "In Views");
	tbl.AddColumn(10, "Modified");
	tbl.AddColumn(20, "By");

	try
	{
		CARFieldList* fields = schema.GetFields();
		unsigned int fieldCount = fields->GetCount();
		for (unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
		{
			CARField field(schema.GetInsideId(), 0, fieldIndex);

			CTableRow row("");
			CTableCell cellName(field.GetName(), "");
			CTableCell cellFieldId(field.GetFieldId(), "");
			CTableCell cellDataType(CAREnum::DataType(field.GetDataType()), "");			
			CTableCell cellNumViews(field.GetDisplayInstances().numItems, "");	
			CTableCell cellTimestamp(CUtil::DateTimeToString(field.GetTimestamp()), "");
			CTableCell cellLastChanged(field.GetLastChanged(), "");

			row.AddCell(cellName);		
			row.AddCell(cellFieldId);
			row.AddCell(cellDataType);
			row.AddCell(cellNumViews);
			row.AddCell(cellTimestamp);
			row.AddCell(cellLastChanged);

			tbl.AddRow(row);
		}

		//Save field information to ccs
		CPageParams file(PAGE_SCHEMA_FIELDS_CSV, &this->schema);
		CCsvPage csvPage(file->GetFileName(), this->pInside->appConfig);
		csvPage.SaveInFolder(file->GetPath(), tbl.ToCsv());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema all fields csv of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}


//Create a page with all fields of a joinform
string CDocSchemaDetails::AllFieldsSpecial()
{		
	CTable tbl("fieldListAll", "TblObjectList");
	tbl.AddColumn(20, "Field Name");
	tbl.AddColumn(5, "Field ID");
	tbl.AddColumn(5, "Datatype");
	tbl.AddColumn(30, "Real Field");
	tbl.AddColumn(10, "In Views");
	tbl.AddColumn(10, "Modified");
	tbl.AddColumn(20, "By");

	try
	{
		CARFieldList* fields = schema.GetFields();
		unsigned int fieldCount = fields->GetCount();
		for (unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
		{
			CARField field(schema.GetInsideId(), 0, fieldIndex);

			CTableRow row("");
			CTableCell cellName(field.GetURL(rootLevel), "");
			CTableCell cellFieldId(field.GetFieldId(), "");

			stringstream strmTmp;
			if(field.GetDisplayInstances().numItems == 0 && field.GetFieldId() != 15)				
				strmTmp << "<span class=\"fieldInNoView\">" << field.GetDisplayInstances().numItems << "</span" << endl;
			else
				strmTmp << field.GetDisplayInstances().numItems;			
			
			CTableCell cellNumViews(strmTmp.str(), "");	
			CTableCell cellDataType(CAREnum::DataType(field.GetDataType()), "");

			strmTmp.str("");			
			switch (field.GetMapping().fieldType)
			{
			case AR_FIELD_JOIN:
				{
					if(field.GetFieldId() == 1) // RequestID 1 in Joinform = ReqId1 | ReqId2
					{
						const ARCompoundSchema& compSchema = this->schema.GetCompound();
						strmTmp << this->pInside->LinkToField(compSchema.u.join.memberA, 1, rootLevel) << "&nbsp;" << MenuSeparator << "&nbsp;" << this->pInside->LinkToSchema(compSchema.u.join.memberA, rootLevel);
						strmTmp << "<br/>" << endl;
						strmTmp << this->pInside->LinkToField(compSchema.u.join.memberB, 1, rootLevel) << "&nbsp;" << MenuSeparator << "&nbsp;" << this->pInside->LinkToSchema(compSchema.u.join.memberB, rootLevel);
					}
					else
					{
						if(field.GetMapping().u.join.realId > 0)
						{
							string tmpBaseSchema = this->schema.GetCompound().u.join.memberA;
							if(field.GetMapping().u.join.schemaIndex > 0)
								tmpBaseSchema = this->schema.GetCompound().u.join.memberB;

							strmTmp << this->pInside->LinkToField(tmpBaseSchema, field.GetMapping().u.join.realId, rootLevel) << "&nbsp;" << MenuSeparator << "&nbsp;" << this->pInside->LinkToSchema(tmpBaseSchema, rootLevel);
						}
						else
							strmTmp << "&nbsp;";
					}
				}
				break;
			case AR_FIELD_VIEW:
				strmTmp << field.GetMapping().u.view.fieldName;
				break;
			case AR_FIELD_VENDOR:
				strmTmp << field.GetMapping().u.vendor.fieldName;
				break;
			}

			CTableCell cellFieldRealId(strmTmp.str(), "");
			CTableCell cellTimestamp(CUtil::DateTimeToHTMLString(field.GetTimestamp()), "");
			CTableCell cellLastChanged(this->pInside->LinkToUser(field.GetLastChanged(), rootLevel), "");

			row.AddCell(cellName);
			row.AddCell(cellFieldId);
			row.AddCell(cellDataType);
			row.AddCell(cellFieldRealId);
			row.AddCell(cellNumViews);
			row.AddCell(cellTimestamp);
			row.AddCell(cellLastChanged);

			tbl.AddRow(row);
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << tbl.NumRows() << " fields (" << CWebUtil::Link("data", CPageParams(PAGE_SCHEMA_FIELDS_CSV, &this->schema), "", rootLevel) << ")" << endl;
		tbl.description = tblDesc.str();

		AllFieldsSpecialCsv();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema all fields join of '"<< this->schema.GetName() << "': " << e.what() << endl;
	}

	return tbl.ToXHtml();	
}

void CDocSchemaDetails::AllFieldsSpecialCsv()
{		
	CTable tbl("fieldListAll", "TblObjectList");
	tbl.AddColumn(20, "Field Name");
	tbl.AddColumn(5, "Field ID");
	tbl.AddColumn(5, "Datatype");
	tbl.AddColumn(30, "Real Field");
	tbl.AddColumn(10, "In Views");
	tbl.AddColumn(10, "Modified");
	tbl.AddColumn(20, "By");

	try
	{
		CARFieldList* fields = schema.GetFields();
		unsigned int fieldCount = fields->GetCount();
		for (unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
		{
			CARField field(schema.GetInsideId(), 0, fieldIndex);

			CTableRow row("");
			CTableCell cellName(field.GetName(), "");
			CTableCell cellFieldId(field.GetFieldId(), "");
			CTableCell cellNumViews(field.GetDisplayInstances().numItems, "");
			CTableCell cellDataType(CAREnum::DataType(field.GetDataType()), "");

			stringstream strmTmp;
			switch (field.GetMapping().fieldType)
			{
			case AR_FIELD_JOIN:
				{
					if(field.GetFieldId() == 1) // RequestID 1 in Joinform = ReqId1 | ReqId2
					{
						strmTmp << 1;
					}
					else
					{
						if(field.GetMapping().u.join.realId > 0)
						{
							strmTmp << field.GetMapping().u.join.realId;
						}
					}
				}
				break;
			case AR_FIELD_VIEW:
				{
					strmTmp << field.GetMapping().u.view.fieldName;
				}
				break;
			case AR_FIELD_VENDOR:
				{
					strmTmp << field.GetMapping().u.vendor.fieldName;
				}
				break;			
			}

			CTableCell cellFieldRealId(strmTmp.str());
			CTableCell cellTimestamp(CUtil::DateTimeToString(field.GetTimestamp()), "");
			CTableCell cellLastChanged(field.GetLastChanged(), "");

			row.AddCell(cellName);
			row.AddCell(cellFieldId);
			row.AddCell(cellDataType);
			row.AddCell(cellFieldRealId);
			row.AddCell(cellNumViews);
			row.AddCell(cellTimestamp);
			row.AddCell(cellLastChanged);

			tbl.AddRow(row);
		}

		//Save field information to csv
		CPageParams file(PAGE_SCHEMA_FIELDS_CSV, &this->schema);
		CCsvPage csvPage(file->GetFileName(), this->pInside->appConfig);
		csvPage.SaveInFolder(file->GetPath(), tbl.ToCsv());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema all fields join csv of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

bool CDocSchemaDetails::InAlList(string objName)
{
	list<string>::iterator listIter;		
	for ( listIter = this->uniqueAlList.begin(); listIter != this->uniqueAlList.end(); listIter++ )
	{
		string *listItemName = &(*listIter);
		if(strcmp(objName.c_str(), listItemName->c_str())==0)
		{
			return true;
		}
	}
	return false;
}

bool CDocSchemaDetails::InFilterList(string objName)
{
	list<string>::iterator listIter;		
	for ( listIter = this->uniqueFilterList.begin(); listIter != this->uniqueFilterList.end(); listIter++ )
	{
		string *listItemName = &(*listIter);
		if(strcmp(objName.c_str(), listItemName->c_str())==0)
		{
			return true;
		}
	}
	return false;
}

bool CDocSchemaDetails::InEscalList(string objName)
{
	list<string>::iterator listIter;		
	for ( listIter = this->uniqueEscalList.begin(); listIter != this->uniqueEscalList.end(); listIter++ )
	{
		string *listItemName = &(*listIter);
		if(strcmp(objName.c_str(), listItemName->c_str())==0)
		{
			return true;
		}
	}
	return false;
}

bool CDocSchemaDetails::IsSchemaInWFConnectStruct(const ARWorkflowConnectStruct& wfCS)
{
	if (wfCS.type == AR_WORKFLOW_CONN_SCHEMA_LIST && wfCS.u.schemaList != NULL)
	{
		for (unsigned int connectIndex = 0; connectIndex < wfCS.u.schemaList->numItems; ++connectIndex)
		{
			if (schema.GetName() == wfCS.u.schemaList->nameList[connectIndex])
			{
				return true;
			}
		}
	}
	return false;
}

void CDocSchemaDetails::AddTableRow(CTable& tbl, CARActiveLink& al)
{
	unsigned int enabled = al.GetEnabled();
	CARProplistHelper alProps(&al.GetPropList());

	CTableRow row("cssStdRow");
	row.AddCell(CAREnum::XmlStructItem(al.GetServerObjectTypeXML()));
	row.AddCell(al.GetURL(rootLevel));
	row.AddCell(CTableCell(CAREnum::ObjectEnable(enabled), (enabled == 0 ? "objStatusDisabled" : "")));
	row.AddCell(CTableCell(al.GetOrder()));
	row.AddCell(al.GetExecuteOn(true, &alProps));
	row.AddCell(CTableCell(al.GetIfActions().numItems));
	row.AddCell(CTableCell(al.GetElseActions().numItems));
	row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(al.GetTimestamp())));
	row.AddCell(CTableCell(this->pInside->LinkToUser(al.GetLastChanged(), rootLevel)));
	tbl.AddRow(row);
}

void CDocSchemaDetails::AddTableRow(CTable& tbl, CARFilter& flt)
{
	unsigned int enabled = flt.GetEnabled();

	CTableRow row("cssStdRow");
	row.AddCell(CAREnum::XmlStructItem(flt.GetServerObjectTypeXML()));
	row.AddCell(flt.GetURL(rootLevel));
	row.AddCell(CTableCell(CAREnum::ObjectEnable(enabled), (enabled == 0 ? "objStatusDisabled" : "")));
	row.AddCell(CTableCell(flt.GetOrder()));
	row.AddCell(flt.GetExecuteOn(true));
	row.AddCell(CTableCell(flt.GetIfActions().numItems));
	row.AddCell(CTableCell(flt.GetElseActions().numItems));
	row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(flt.GetTimestamp())));
	row.AddCell(CTableCell(this->pInside->LinkToUser(flt.GetLastChanged(), rootLevel)));
	tbl.AddRow(row);
}

void CDocSchemaDetails::AddTableRow(CTable& tbl, CAREscalation& esc)
{
	unsigned int enabled = esc.GetEnabled();

	CTableRow row("cssStdRow");
	row.AddCell(CAREnum::XmlStructItem(esc.GetServerObjectTypeXML()));
	row.AddCell(esc.GetURL(rootLevel));
	row.AddCell(CTableCell(CAREnum::ObjectEnable(enabled), (enabled == 0 ? "objStatusDisabled" : "")));
	row.AddCell(""); // order
	row.AddCell(esc.GetExecuteOn());
	row.AddCell(CTableCell(esc.GetIfActions().numItems));
	row.AddCell(CTableCell(esc.GetElseActions().numItems));
	row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(esc.GetTimestamp())));
	row.AddCell(CTableCell(this->pInside->LinkToUser(esc.GetLastChanged(), rootLevel)));
	tbl.AddRow(row);
}

void CDocSchemaDetails::AddTableRow(CTable& tbl, CARContainer& cont)
{
	CTableRow row("cssStdRow");
	row.AddCell(CAREnum::ContainerType(cont.GetType()));
	row.AddCell(cont.GetURL(rootLevel));
	row.AddCell(""); // enabled column
	row.AddCell(""); // order column
	row.AddCell(""); // execute on column
	row.AddCell(""); // if actions column
	row.AddCell(""); // else actions column
	row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(cont.GetTimestamp())));
	row.AddCell(CTableCell(this->pInside->LinkToUser(cont.GetLastChanged(), rootLevel)));
	tbl.AddRow(row);
}

void CDocSchemaDetails::WorkflowDoc()
{
	try
	{
		CPageParams file(PAGE_SCHEMA_WORKFLOW, &this->schema);
		int rootLevel = file->GetRootLevel();

		string title = "Schema " + this->schema.GetName() + " (Workflow)";
		CWebPage webPage(file->GetFileName(), title, rootLevel, this->pInside->appConfig);

		//ContentHead informations		
		webPage.AddContentHead(this->FormPageHeader("Workflow"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());


		//Field references
		CTable tblRef("referenceList", "TblObjectList");
		tblRef.AddColumn(05, "Type");
		tblRef.AddColumn(20, "Server object");
		tblRef.AddColumn(05, "Enabled");
		tblRef.AddColumn(05, "Order");
		tblRef.AddColumn(10, "Execute On");
		tblRef.AddColumn(05, "If");
		tblRef.AddColumn(05, "Else");
		tblRef.AddColumn(10, "Changed");
		tblRef.AddColumn(10, "By");

		const CARSchemaList::ObjectRefList& alList = this->schema.GetActiveLinks();
		const CARSchemaList::ObjectRefList& fltList = this->schema.GetFilters();
		const CARSchemaList::ObjectRefList& escList = this->schema.GetEscalations();
		const CARSchemaList::ObjectRefList& algList = this->schema.GetActLinkGuides();
		const CARSchemaList::ObjectRefList& flgList = this->schema.GetFilterGuides();
		const CARSchemaList::ObjectRefList& wbsList = this->schema.GetWebservices();

		// Add all active links
		CARSchemaList::ObjectRefList::const_iterator curIt = alList.begin();
		CARSchemaList::ObjectRefList::const_iterator endIt = alList.end();
		for (; curIt != endIt; ++curIt)
		{
			CARActiveLink al(*curIt);
			AddTableRow(tblRef, al);
		}

		// Add all filters
		curIt = fltList.begin();
		endIt = fltList.end();
		for (; curIt != endIt; ++curIt)
		{
			CARFilter flt(*curIt);
			AddTableRow(tblRef, flt);
		}

		// Add all escalations
		curIt = escList.begin();
		endIt = escList.end();
		for (; curIt != endIt; ++curIt)
		{
			CAREscalation esc(*curIt);
			AddTableRow(tblRef, esc);
		}

		// add all active link guides
		curIt = algList.begin();
		endIt = algList.end();
		for (; curIt != endIt; ++curIt)
		{
			CARContainer alg(*curIt);
			AddTableRow(tblRef, alg);
		}

		// add all filter guides
		curIt = flgList.begin();
		endIt = flgList.end();
		for (; curIt != endIt; ++curIt)
		{
			CARContainer flg(*curIt);
			AddTableRow(tblRef, flg);
		}

		// add all webservices
		curIt = wbsList.begin();
		endIt = wbsList.end();
		for (; curIt != endIt; ++curIt)
		{
			CARContainer ws(*curIt);
			AddTableRow(tblRef, ws);
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Workflow Reference:";

		tblRef.description = tblDesc.str();

		webPage.AddContent(tblRef.ToXHtml());
		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema workflow doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//list of groups with access rights
void CDocSchemaDetails::SchemaPermissionDoc()
{
	try
	{
		CPageParams file(PAGE_SCHEMA_PERMISSIONS, &this->schema);
		int rootLevel = file->GetRootLevel();

		string title = "Schema " +this->schema.GetName() +" (Indexes)";
		CWebPage webPage(file->GetFileName(), title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Permissions"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());

		CTable tbl("permissionList", "TblObjectList");
		tbl.AddColumn(5, "Permission");
		tbl.AddColumn(10, "Description");
		tbl.AddColumn(75, "Group Name");
		tbl.AddColumn(10, "Group Id");

		const ARPermissionList& perms = this->schema.GetPermissions();
		for(unsigned int i = 0; i < perms.numItems; ++i)
		{
			if(this->pInside->ValidateGroup(this->schema.GetAppRefName(), perms.permissionList[i].groupId))
			{
				CTableRow row("");

				string img;

				if(perms.permissionList[i].permissions == AR_PERMISSIONS_HIDDEN)
					img = CWebUtil::ImageTag("hidden.gif", rootLevel);
				else
					img = CWebUtil::ImageTag("visible.gif", rootLevel);

				row.AddCell(CTableCell(img));
				row.AddCell(CTableCell(CAREnum::ObjectPermission(perms.permissionList[i].permissions)));
				row.AddCell(CTableCell(this->pInside->LinkToGroup(this->schema.GetAppRefName(), perms.permissionList[i].groupId, rootLevel)));
				row.AddCell(CTableCell(perms.permissionList[i].groupId));
				tbl.AddRow(row);
			}
		}	    
		webPage.AddContent(tbl.ToXHtml());


		//Show all fields with all group permissions
		CTable fieldTbl("fieldList", "TblObjectList");

		fieldTbl.AddColumn(40, "Field Name");
		fieldTbl.AddColumn(10, "Field ID");	
		fieldTbl.AddColumn(10, "Datatype");		
		fieldTbl.AddColumn(40, "Permissions");


		CARFieldList* fields = schema.GetFields();
		unsigned int fieldCount = fields->GetCount();
		for (unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
		{
			CARField field(schema.GetInsideId(), 0, fieldIndex);

			//Field Permissions
			stringstream strmFieldPermissions;
			strmFieldPermissions.str("");

			const ARPermissionList& fldPerms = field.GetPermissions();
			if(fldPerms.numItems > 0)
			{
				CTable tblFieldPermissionDetails("PermissionFieldList", "TblHidden");
				tblFieldPermissionDetails.AddColumn(5, "Permission");
				tblFieldPermissionDetails.AddColumn(20, "Description");
				tblFieldPermissionDetails.AddColumn(75, "Group Name");
				tblFieldPermissionDetails.DisableHeader();

				for(unsigned int i = 0; i < fldPerms.numItems; ++i)
				{
					string img;
					
					if(fldPerms.permissionList[i].permissions == AR_PERMISSIONS_CHANGE)
						img = CWebUtil::ImageTag("edit.gif", rootLevel);
					else
						img = CWebUtil::ImageTag("visible.gif", rootLevel);

					CTableRow row("");
					row.AddCell(CTableCell(img));
					row.AddCell(CTableCell(CAREnum::FieldPermission(fldPerms.permissionList[i].permissions)));
					row.AddCell(CTableCell(this->pInside->LinkToGroup(schema.GetAppRefName(), fldPerms.permissionList[i].groupId, rootLevel)));
					tblFieldPermissionDetails.AddRow(row);
				}

				strmFieldPermissions << tblFieldPermissionDetails;
			}
			else
			{
				strmFieldPermissions << EmptyValue << endl;
			}


			CTableRow row("");			
			row.AddCell(CTableCell(field.GetURL(rootLevel)));
			row.AddCell(CTableCell(field.GetFieldId()));
			row.AddCell(CTableCell(CAREnum::DataType(field.GetDataType())));
			row.AddCell(CTableCell(strmFieldPermissions.str()));
			fieldTbl.AddRow(row);
		}

		webPage.AddContent(fieldTbl.ToXHtml());	
		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema permission doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//list of groups with subadministrator access to this form
void CDocSchemaDetails::SchemaSubadminDoc()
{
	try
	{
		CPageParams file(PAGE_SCHEMA_SUBADMINS, &this->schema);
		int rootLevel = file->GetRootLevel();

		string title = "Schema " +this->schema.GetName() +" (Indexes)";
		CWebPage webPage(file->GetFileName(), title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Subadministrator Permission"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());


		CTable tbl("fieldListAll", "TblObjectList");
		tbl.AddColumn(10, "Group Id");
		tbl.AddColumn(90, "Group Name");

		const ARInternalIdList& subAdmins = schema.GetSubadmins();
		for(unsigned int i = 0; i < subAdmins.numItems; ++i)
		{
			CTableRow row("");					
			row.AddCell(CTableCell(subAdmins.internalIdList[i]));
			row.AddCell(CTableCell(this->pInside->LinkToGroup(this->schema.GetAppRefName(), subAdmins.internalIdList[i], rootLevel)));		
			tbl.AddRow(row);
		}

		webPage.AddContent(tbl.ToXHtml());
		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema subadmin doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//Create a page that shows a list of all indexes
void CDocSchemaDetails::IndexDoc()
{
	try
	{
		CPageParams file(PAGE_SCHEMA_INDEXES, &this->schema);
		int rootLevel = file->GetRootLevel();

		string title = "Schema " +this->schema.GetName() +" (Indexes)";
		CWebPage webPage(file->GetFileName(), title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Indexes"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());

		const ARIndexList& indexList = this->schema.GetIndexList();
		for(unsigned int nIndex=0; nIndex < indexList.numItems; ++nIndex)
		{		
			CTable tbl("fieldListAll", "TblObjectList");
			tbl.AddColumn(0, "Field Name");
			tbl.AddColumn(0, "Field ID");
			tbl.AddColumn(0, "Datatype");
			tbl.AddColumn(0, "Modified");
			tbl.AddColumn(0, "By");

			stringstream tblDesc;
			if(indexList.indexList[nIndex].unique)
				tblDesc << "Unique Index :" << CWebUtil::ObjName(indexList.indexList[nIndex].indexName);
			else
				tblDesc << "Index: " << CWebUtil::ObjName(indexList.indexList[nIndex].indexName);

			tbl.description = tblDesc.str();

			for(unsigned int nField=0; nField < indexList.indexList[nIndex].numFields; ++nField)
			{
				CARField field(schema.GetInsideId(), indexList.indexList[nIndex].fieldIds[nField]);

				if (field.Exists())
				{
					CTableRow row("");
					row.AddCell( CTableCell(field.GetURL(rootLevel)));
					row.AddCell( CTableCell(field.GetFieldId()));
					row.AddCell( CTableCell(CAREnum::DataType(field.GetDataType())));
					row.AddCell( CTableCell(CUtil::DateTimeToHTMLString(field.GetDataType())));
					row.AddCell( CTableCell(this->pInside->LinkToUser(field.GetLastChanged(), rootLevel)));
					tbl.AddRow(row);

					//Add a reference
					CRefItem refItem(this->schema, 0, nIndex, REFM_SCHEMA_INDEX);
					pInside->AddFieldReference(schema.GetInsideId(), field.GetInsideId(), refItem);
				}
			}

			webPage.AddContent(tbl.ToXHtml());
		}

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema index doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//Create a page that shows the form resultlist
void CDocSchemaDetails::ResultListDoc()
{
	try
	{
		CPageParams file(PAGE_SCHEMA_RESULTLIST, &this->schema);
		int rootLevel = file->GetRootLevel();

		string title = "Schema " +this->schema.GetName() +" (Results Lists Fields)";
		CWebPage webPage("form_result_list", title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Resultlist"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());


		CTable tbl("fieldListIndex", "TblObjectList");
		tbl.AddColumn(25, "Field Name");
		tbl.AddColumn(10, "Field Id");
		tbl.AddColumn(10, "Datatype");
		tbl.AddColumn(5, "Width");
		tbl.AddColumn(10, "Separator");
		tbl.AddColumn(20, "Modified");
		tbl.AddColumn(20, "By");

		const AREntryListFieldList& resultFields = schema.GetResultFields();
		for(unsigned int i = 0; i < resultFields.numItems; ++i)
		{
			CARField field(schema.GetInsideId(), resultFields.fieldsList[i].fieldId);

			if (field.Exists())
			{
				CTableRow row("");
				row.AddCell( CTableCell(field.GetURL(rootLevel)));
				row.AddCell( CTableCell(field.GetFieldId()));
				row.AddCell( CTableCell(CAREnum::DataType(field.GetDataType())));
				row.AddCell( CTableCell(resultFields.fieldsList[i].columnWidth));
				row.AddCell( CTableCell(resultFields.fieldsList[i].separator));
				row.AddCell( CTableCell(CUtil::DateTimeToHTMLString(field.GetTimestamp())));
				row.AddCell( CTableCell(this->pInside->LinkToUser(field.GetLastChanged(), rootLevel)));
				tbl.AddRow(row);

				//Add a reference
				CRefItem refItem(this->schema, REFM_SCHEMA_RESULTLIST);
				pInside->AddFieldReference(schema.GetInsideId(), field.GetInsideId(), refItem);
			}
		}

		webPage.AddContent(tbl.ToXHtml());

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema resultlist doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//Create a page that shows the page sortslist
void CDocSchemaDetails::SortListDoc()
{
	try
	{
		CPageParams file(PAGE_SCHEMA_SORTLIST, &this->schema);
		int rootLevel = file->GetRootLevel();

		string title = "Schema " +this->schema.GetName() +" (Results Lists Fields)";
		CWebPage webPage(file->GetFileName(), title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Sortlist"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());


		CTable tbl("fieldListIndex", "TblObjectList");
		tbl.AddColumn(10, "Sort Order");
		tbl.AddColumn(30, "Field Name");
		tbl.AddColumn(10, "Field Id");
		tbl.AddColumn(10, "Datatype");
		tbl.AddColumn(20, "Modified");
		tbl.AddColumn(20, "By");


		const ARSortList& sorting = schema.GetSortList();
		for(unsigned int i = 0; i < sorting.numItems; ++i)
		{
			CARField field(schema.GetInsideId(), sorting.sortList[i].fieldId);

			if (field.Exists())
			{
				CTableRow row("");

				string sortImage;
				if(sorting.sortList[i].sortOrder == AR_SORT_DESCENDING)
					sortImage = CWebUtil::ImageTag("sort_desc.gif", rootLevel);
				else
					sortImage = CWebUtil::ImageTag("sort_asc.gif", rootLevel);

				row.AddCell( CTableCell(sortImage));
				row.AddCell( CTableCell(field.GetURL(rootLevel)));
				row.AddCell( CTableCell(field.GetFieldId()));
				row.AddCell( CTableCell(CAREnum::DataType(field.GetDataType())));				
				row.AddCell( CTableCell(CUtil::DateTimeToHTMLString(field.GetTimestamp())));
				row.AddCell( CTableCell(this->pInside->LinkToUser(field.GetLastChanged(), rootLevel)));

				tbl.AddRow(row);

				//Add a reference
				CRefItem refItem(this->schema, REFM_SCHEMA_SORTLIST);
				pInside->AddFieldReference(schema.GetInsideId(), field.GetInsideId(), refItem);
			}
		}

		webPage.AddContent(tbl.ToXHtml());

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema sortlist doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//Create a page with a list of all views
void CDocSchemaDetails::VuiListDoc()
{	
	try
	{
		CPageParams file(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_VUI, &this->schema);
		int rootLevel = file->GetRootLevel();

		string title = "Schema " +this->schema.GetName() +" (Views)";
		CWebPage webPage(file->GetFileName(), title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Views"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());

		CTable tbl("vuiList", "TblObjectList");
		tbl.AddColumn(30, "Vui Name");
		tbl.AddColumn(20, "Label");
		tbl.AddColumn(10, "Web Alias");
		tbl.AddColumn(10, "Type");
		tbl.AddColumn(15, "Modified");
		tbl.AddColumn(15, "By");

		unsigned int vuiCount = schema.GetVUIs()->GetCount();
		for (unsigned int vuiIndex = 0; vuiIndex < vuiCount; ++vuiIndex)
		{
			CARVui vui(schema.GetInsideId(), 0, vuiIndex);

			CTableRow row("");			
			row.AddCell( CTableCell(vui.GetURL(rootLevel)));
			row.AddCell( CTableCell(vui.Label()));
			row.AddCell( CTableCell(vui.webAlias()));
			row.AddCell( CTableCell(CAREnum::VuiType(vui.GetType())));
			row.AddCell( CTableCell(CUtil::DateTimeToHTMLString(vui.GetTimestamp())));
			row.AddCell( CTableCell(this->pInside->LinkToUser(vui.GetLastChanged(), rootLevel)));
			tbl.AddRow(row);
		}

		webPage.AddContent(tbl.ToXHtml());

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema vuilist doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//Create a list with all filters related to thisform
void CDocSchemaDetails::SchemaFilterDoc()
{
	try
	{
		CPageParams file(PAGE_SCHEMA_FILTERS, &this->schema);
		int rootLevel = file->GetRootLevel();

		string title = "Schema " +this->schema.GetName() +" (Filter)";
		CWebPage webPage(file->GetFileName(), title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Filter"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());

		CFilterTable *tbl = new CFilterTable(*this->pInside);

		unsigned int filterCount = pInside->filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex)
		{
			CARFilter filter(filterIndex);

			const ARWorkflowConnectStruct &schemas = filter.GetSchemaList();
			if (schemas.u.schemaList != NULL)
			{
				for(unsigned int i=0; i < schemas.u.schemaList->numItems; i++)
				{
					// using schemaList.SchemaGetName instead of schema.GetName() avoids converting ARNameType to string object
					if(strcmp(schemas.u.schemaList->nameList[i], this->pInside->schemaList.SchemaGetName(schema.GetInsideId())) == 0)
					{
						tbl->AddRow(filterIndex, rootLevel);
					}
				}
			}
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("filter.gif", rootLevel) << tbl->NumRows() << " ";
		tblDesc << CWebUtil::Link("Filter", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_FILTER), "", rootLevel);
		tbl->SetDescription(tblDesc.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema filter doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//Create a list with all active links related to this form
void CDocSchemaDetails::SchemaAlDoc()
{
	try
	{
		CPageParams file(PAGE_SCHEMA_ACTIVELINKS, &this->schema);
		int rootLevel = file->GetRootLevel();

		string title = "Schema " + this->schema.GetName() +" (ActiveLinks)";
		CWebPage webPage(file->GetFileName(), title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Active Links"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());

		CAlTable *tbl = new CAlTable(*this->pInside);

		unsigned int alCount = pInside->alList.GetCount();
		for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex)
		{
			CARActiveLink enumObj(alIndex);
			
			ARNameList* alSchemas = enumObj.GetSchemaList().u.schemaList;

			if(alSchemas != NULL)
			{
				for(unsigned int i=0; i < alSchemas->numItems; i++)
				{
					if(strcmp(alSchemas->nameList[i], this->pInside->schemaList.SchemaGetName(schema.GetInsideId())) == 0)
					{
						tbl->AddRow(alIndex, rootLevel);
					}
				}
			}
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("active_link.gif", rootLevel) << tbl->NumRows() << " ";
		// TODO: move to single CWebUtil call
		tblDesc << CWebUtil::Link("ActiveLinks", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_ACTIVE_LINK), "", rootLevel);
		tbl->SetDescription(tblDesc.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema active link doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//Create a list with all escalations related to this form
void CDocSchemaDetails::SchemaEscalDoc()
{
	try
	{
		CPageParams file(PAGE_SCHEMA_ESCALATIONS, &this->schema);
		int rootLevel = file->GetRootLevel();

		string title = "Schema " +this->schema.GetName() +" (Escalations)";
		CWebPage webPage(file->GetFileName(), title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Escalations"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());

		CEscalTable *tbl = new CEscalTable(*this->pInside);

		unsigned int escalCount = pInside->escalationList.GetCount();
		for (unsigned int escalIndex = 0; escalIndex < escalCount; ++escalIndex)
		{
			CAREscalation escalation(escalIndex);

			const ARWorkflowConnectStruct &schemas = escalation.GetSchemaList();
			if(schemas.u.schemaList != NULL)
			{
				for(unsigned int i=0; i < schemas.u.schemaList->numItems; i++)
				{
					if(strcmp(schemas.u.schemaList->nameList[i], this->pInside->schemaList.SchemaGetName(schema.GetInsideId())) == 0)
					{
						tbl->AddRow(escalIndex, rootLevel);
					}
				}
			}
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("escalation.gif", rootLevel) << tbl->NumRows() << " ";
		tblDesc << CWebUtil::Link("Escalations", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_ESCALATION), "", rootLevel);
		tbl->SetDescription(tblDesc.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema escalation doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

string CDocSchemaDetails::TypeDetails()
{
	stringstream strm;
	strm.str("");

	try
	{
		const ARCompoundSchema& compSchema = schema.GetCompound();
		switch(compSchema.schemaType)
		{
		case AR_SCHEMA_JOIN:
			{
				strm << "Primary Form: " << this->pInside->LinkToSchema(compSchema.u.join.memberA, rootLevel) << " <-> Secondary Form: " << this->pInside->LinkToSchema(compSchema.u.join.memberB, rootLevel) << "<br/>";

				if(compSchema.u.join.option == 0)
					strm << "Join Type: Inner" << "<br/>" << endl;
				else
					strm << "Join Type: Outer" << "<br/>" << endl;

				if(compSchema.u.join.joinQual.operation != AR_COND_OP_NONE)
				{
					stringstream strmQuery;
					CRefItem refItem(this->schema, REFM_SCHEMA_JOIN_QUALIFICATION);
					CARQualification arQual(*this->pInside);

					int pFormId = this->pInside->SchemaGetInsideId(compSchema.u.join.memberA);
					int sFormId = this->pInside->SchemaGetInsideId(compSchema.u.join.memberB);

					arQual.CheckQuery(&compSchema.u.join.joinQual, refItem, 0, pFormId, sFormId, strmQuery, rootLevel);

					strm << "Join Qualification: " << strmQuery.str();
				}
			}
			break;
		case AR_SCHEMA_VIEW:
			{
				strm << "Table Name: " << compSchema.u.view.tableName << "<br/>" << endl;
				strm << "Key Field: " << compSchema.u.view.keyField << "<br/>" << endl;
			}
			break;			
		case AR_SCHEMA_VENDOR:
			{
				strm << "Vendor Name: " << compSchema.u.vendor.vendorName << "<br/>";
				strm << "Table Name: " << compSchema.u.vendor.tableName << "<br/>" << endl;
			}
			break;
		default:
			{
				strm << EmptyValue;
			}
			break;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating regular schema type information of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
	return strm.str();
}

string CDocSchemaDetails::ContainerReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		//// Active Link Guides ////
		const CARSchemaList::ObjectRefList& alg = schema.GetActLinkGuides();
		CARSchemaList::ObjectRefList::const_iterator curIt = alg.begin();
		CARSchemaList::ObjectRefList::const_iterator endIt = alg.end();

		CContainerTable *contTable = new CContainerTable(*this->pInside);

		for (; curIt != endIt; ++curIt)
		{
			CARContainer cont(*curIt);
			contTable->AddRow(cont, rootLevel);
		}

		//// Filter Guides ////
		const CARSchemaList::ObjectRefList& flg = schema.GetFilterGuides();
		curIt = flg.begin();
		endIt = flg.end();

		for (; curIt != endIt; ++curIt)
		{
			CARContainer cont(*curIt);
			contTable->AddRow(cont, rootLevel);
		}

		//// Packing Lists ////
		const CARSchemaList::ObjectRefList& pkl = schema.GetPackingLists();
		curIt = pkl.begin();
		endIt = pkl.end();

		for (; curIt != endIt; ++curIt)
		{
			CARContainer cont(*curIt);
			contTable->AddRow(cont, rootLevel);
		}

		const CARSchemaList::ObjectRefList& ws = schema.GetWebservices();
		curIt = ws.begin();
		endIt = ws.end();

		for (; curIt != endIt; ++curIt)
		{
			CARContainer cont(*curIt);
			contTable->AddRow(cont, rootLevel);
		}

		//// create output ////
		if(contTable->NumRows() > 0)
			strm << *contTable;
		else
			strm << EmptyValue;

		delete contTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating container references in schema '" << this->schema.GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

string CDocSchemaDetails::JoinFormReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		const ARNameType& schemaName = this->schema.GetARName();
		unsigned int schemaCount = this->pInside->schemaList.GetCount();
		for (unsigned int schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex)
		{
			CARSchema schema(schemaIndex);
			const ARCompoundSchema& compSchema = schema.GetCompound();

			if(compSchema.schemaType == AR_SCHEMA_JOIN)
			{
				if(strcmp(schemaName, compSchema.u.join.memberA) == 0)
				{
					strm << "Primary Form in: " << schema.GetURL(rootLevel) << "<br/>" << endl;
				}

				if(strcmp(schemaName, compSchema.u.join.memberB)==0)					
				{
					strm << "Secondary Form in: " << schema.GetURL(rootLevel) << "<br/>" << endl;
				}
			}
		}

		if(strm.str().size() == 0)
			strm << EmptyValue;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating join form references of '" << this->schema.GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}


string CDocSchemaDetails::TableFieldReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		// TODO: there is a function that checks for field references between table-field and column-field
		// before the documentation starts. Move this function over to this scanning routing and store
		// "table field to form" references in a list within the CARSchemaList
		// ===> move scaning over to CARInside::CustomFieldReferences
		bool bFound = false;

		unsigned int schemaCount = pInside->schemaList.GetCount();
		for (unsigned int schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex)
		{			
			CARSchema searchSchema(schemaIndex);
			
			unsigned int fieldCount = searchSchema.GetFields()->GetCount();
			for (unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
			{
				CARField field(schemaIndex, 0, fieldIndex);
				
				if(field.GetDataType() == AR_DATA_TYPE_TABLE && field.GetLimits().dataType == AR_DATA_TYPE_TABLE)
				{
					const ARTableLimitsStruct &limit = field.GetLimits().u.tableLimits;					
					const char *tableSchema = limit.schema;

					if (limit.schema[0] == '$')
						tableSchema = limit.sampleSchema;

					if(schema.GetName().compare(tableSchema) == 0)
					{
						strm << "Table: " << field.GetURL(rootLevel);
						strm << " in form: " << searchSchema.GetURL(rootLevel) << "<br/>" << endl;

						bFound = true;
					}
				}
			}
		}

		if(!bFound)
			strm << EmptyValue;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating table references in schema '" << this->schema.GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

string CDocSchemaDetails::SearchMenuReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		CMenuTable *menuTable = new CMenuTable(*this->pInside);

		unsigned int menuCount = this->pInside->menuList.GetCount();
		for ( unsigned int menuIndex = 0; menuIndex < menuCount; ++menuIndex )
		{
			CARCharMenu menu(menuIndex);
			const ARCharMenuStruct& menuDefn = menu.GetDefinition();
			if(menuDefn.menuType == AR_CHAR_MENU_QUERY)
			{
				if(strcmp(menuDefn.u.menuQuery.schema, schema.GetARName()) == 0 && 
				   strcmp(menuDefn.u.menuQuery.server, AR_CURRENT_SERVER_TAG)==0)
				{
					menuTable->AddRow(menu, rootLevel);
				}
			}
		}		


		if(menuTable->NumRows() > 0)
			strm << *menuTable;
		else
			strm << EmptyValue;

		delete menuTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating search menu references in schema '" << this->schema.GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

string CDocSchemaDetails::AlPushFieldsReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		CAlTable *alTable = new CAlTable(*this->pInside);

		unsigned int alCount = pInside->alList.GetCount();
		for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex)
		{
			CARActiveLink al(alIndex);

			bool bPushToForm = false;

			//If-Actions
			for(unsigned int i = 0; i < al.GetIfActions().numItems; ++i)
			{
				if(al.GetIfActions().actionList[i].action == AR_ACTIVE_LINK_ACTION_FIELDP)
				{
					ARPushFieldsActionStruct &action = al.GetIfActions().actionList[i].u.pushFields;
					if (action.pushFieldsList.numItems > 0)
					{
						char *pushSchema;
						if (action.pushFieldsList.pushFieldsList[0].field.schema[0] == '$')
							pushSchema = action.sampleSchema;
						else
							pushSchema = action.pushFieldsList.pushFieldsList[0].field.schema;

						if ((pushSchema[0] == '@' && IsSchemaInWFConnectStruct(al.GetSchemaList())) || strcmp(schema.GetARName(),pushSchema) == 0)
						{
							bPushToForm = true;
							break;
						}
					}
				}
			}

			//Else Actions
			if(bPushToForm == false) // Only search the else actions if the al is still false
			{
				for(unsigned int i = 0; i < al.GetElseActions().numItems; ++i)
				{
					if(al.GetElseActions().actionList[i].action == AR_ACTIVE_LINK_ACTION_FIELDP)
					{
						ARPushFieldsActionStruct &action = al.GetElseActions().actionList[i].u.pushFields;

						if (action.pushFieldsList.numItems > 0)
						{
							char *pushSchema;
							if (action.pushFieldsList.pushFieldsList[0].field.schema[0] == '$')
								pushSchema = action.sampleSchema;
							else
								pushSchema = action.pushFieldsList.pushFieldsList[0].field.schema;

							if ((pushSchema[0] == '@' && IsSchemaInWFConnectStruct(al.GetSchemaList())) || strcmp(schema.GetARName(),pushSchema) == 0)
							{
								bPushToForm = true;
								break;
							}
						}
					}
				}
			}

			if(bPushToForm == true)
			{
				alTable->AddRow(alIndex, rootLevel);
			}
		}

		if(alTable->NumRows() > 0)
			strm << *alTable;
		else
			strm << EmptyValue;

		delete alTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating active link push fields references in schema '" << this->schema.GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

string CDocSchemaDetails::AlWindowOpenReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		CAlTable *alTable = new CAlTable(*this->pInside);

		unsigned int alCount = pInside->alList.GetCount();
		for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex)
		{
			CARActiveLink al(alIndex);

			bool bPushToForm = false;

			//If-Actions
			for(unsigned int i = 0; i < al.GetIfActions().numItems; i++)
			{
				if(al.GetIfActions().actionList[i].action == AR_ACTIVE_LINK_ACTION_OPENDLG)
				{
					AROpenDlgStruct &action = al.GetIfActions().actionList[i].u.openDlg;                    
					
					string openWindowSchema;
					if (action.schemaName[0] == '$' )
					{
						string sampleServer;
						CDocActionOpenWindowHelper::GetSampleData(al, IES_IF, i,	sampleServer, openWindowSchema);
					}
					else
						openWindowSchema = action.schemaName;

					if ((openWindowSchema.compare(AR_CURRENT_SCHEMA_TAG) == 0 /*&& IsSchemaInWFConnectStruct(al->schemaList)*/) ||
						openWindowSchema.compare(schema.GetARName()) == 0)
					{
						bPushToForm = true;
						break;
					}

				}
			}	

			//Else Actions
			if(bPushToForm == false) // Only search the else actions if the al is still false
			{
				for(unsigned int i = 0; i < al.GetElseActions().numItems; i++)
				{
					if(al.GetElseActions().actionList[i].action == AR_ACTIVE_LINK_ACTION_OPENDLG)
					{
						AROpenDlgStruct &action = al.GetElseActions().actionList[i].u.openDlg;

						string openWindowSchema;
						if (action.schemaName[0] == '$' )
						{
							string sampleServer;
							CDocActionOpenWindowHelper::GetSampleData(al, IES_ELSE, i,	sampleServer, openWindowSchema);
						}
						else
							openWindowSchema = action.schemaName;

						if ((openWindowSchema.compare(AR_CURRENT_SCHEMA_TAG) == 0 /*&& IsSchemaInWFConnectStruct(al->schemaList)*/) ||
						openWindowSchema.compare(schema.GetARName()) == 0)
						{
							bPushToForm = true;
							break;
						}

					}
				}
			}

			if(bPushToForm == true)
			{
				alTable->AddRow(alIndex, rootLevel);
			}
		}

		if(alTable->NumRows() > 0)
			strm << *alTable;
		else
			strm << EmptyValue;

		delete alTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating window open references in schema '" << this->schema.GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

string CDocSchemaDetails::FilterPushFieldsReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		CFilterTable *filterTable = new CFilterTable(*this->pInside);


		unsigned int filterCount = pInside->filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex )
		{
			CARFilter filter(filterIndex);

			bool bPushToForm = false;

			//If-Actions
			for(unsigned int i = 0; i < filter.GetIfActions().numItems; ++i)
			{
				if(filter.GetIfActions().actionList[i].action == AR_FILTER_ACTION_FIELDP)
				{
					ARPushFieldsActionStruct &action = filter.GetIfActions().actionList[i].u.pushFields;
					if (action.pushFieldsList.numItems > 0)
					{
						char *pushSchema;
						if (action.pushFieldsList.pushFieldsList[0].field.schema[0] == '$')
							pushSchema = action.sampleSchema;
						else
							pushSchema = action.pushFieldsList.pushFieldsList[0].field.schema;

						if ((pushSchema[0] == '@' && IsSchemaInWFConnectStruct(filter.GetSchemaList())) || strcmp(schema.GetARName(), pushSchema) == 0)
						{
							bPushToForm = true;
							break;
						}
					}
				}
			}	

			//Else Actions
			if(bPushToForm == false) // Only search the else actions if the al is still false
			{
				for(unsigned int i = 0; i < filter.GetElseActions().numItems; ++i)
				{
					if(filter.GetElseActions().actionList[i].action == AR_FILTER_ACTION_FIELDP)
					{
						ARPushFieldsActionStruct &action = filter.GetElseActions().actionList[i].u.pushFields;

						if (action.pushFieldsList.numItems > 0)
						{
							char *pushSchema;
							if (action.pushFieldsList.pushFieldsList[0].field.schema[0] == '$')
								pushSchema = action.sampleSchema;
							else
								pushSchema = action.pushFieldsList.pushFieldsList[0].field.schema;

							if ((pushSchema[0] == '@' && IsSchemaInWFConnectStruct(filter.GetSchemaList())) || strcmp(schema.GetARName(), pushSchema) == 0)
							{
								bPushToForm = true;
								break;
							}
						}
					}
				}
			}

			if(bPushToForm == true)
			{
				filterTable->AddRow(filterIndex, rootLevel);
			}
		}

		if(filterTable->NumRows() > 0)
			strm << *filterTable;
		else
			strm << EmptyValue;

		delete filterTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating filter push fields references in schema '" << this->schema.GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

string CDocSchemaDetails::ShowProperties()
{
	stringstream strm;
	strm.str("");
	map<ARLong32,ARValueStruct*> propIdx;

	try
	{
		const ARPropList& propList = this->schema.GetPropList();
		CARProplistHelper propIdx(&propList);

		// doc basic properties
		ShowBasicProperties(strm, &propIdx);

		if (this->schema.GetCompound().schemaType != AR_SCHEMA_DIALOG)
		{
			// doc archive properties
			ShowArchiveProperties(strm);

			if (pInside->CompareServerVersion(7,0) >= 0) 
			{
				// doc audit properties
				ShowAuditProperties(strm);
			}
		}

		// doc properties left
		propIdx.UnusedPropertiesToHTML(strm);
	}
	catch(exception& e)
	{
		cerr << "EXCEPTION enumerating properties: " << e.what() << endl;
	}
	return strm.str();
}

void CDocSchemaDetails::ShowBasicProperties(std::ostream& strm, CARProplistHelper* propIndex)
{
	ARValueStruct* propVal;

	try
	{
		CTable tbl("displayPropList", "TblObjectList");
		tbl.AddColumn(20, "Description");
		tbl.AddColumn(80, "Value");

		// entry points
		ARLong32 entryPointNewOrder = -1;
		ARLong32 entryPointSearchOrder = -1;

		propVal = propIndex->GetAndUseValue(AR_SMOPROP_ENTRYPOINT_DEFAULT_NEW_ORDER);
		if (propVal != NULL) 
			if (propVal->dataType == AR_DATA_TYPE_INTEGER)
				entryPointNewOrder = propVal->u.intVal;

		propVal = propIndex->GetAndUseValue(AR_SMOPROP_ENTRYPOINT_DEFAULT_SEARCH_ORDER);
		if (propVal != NULL) 
			if (propVal->dataType == AR_DATA_TYPE_INTEGER)
				entryPointSearchOrder = propVal->u.intVal;

		stringstream tmpStrm; tmpStrm.str("");
		tmpStrm << "New Mode: " << (entryPointNewOrder >= 0 ? "Enabled" : "Disabled") << "<br/>";
		tmpStrm << "Application List Order: "; if (entryPointNewOrder >=0) tmpStrm << entryPointNewOrder; tmpStrm << "<br/><br/>";

		tmpStrm << "Search Mode: " << (entryPointSearchOrder >= 0 ? "Enabled":"Disabled") << "<br/>";
		tmpStrm << "Application List Order: "; if (entryPointSearchOrder >= 0) tmpStrm << entryPointSearchOrder; tmpStrm << endl;
		
		CTableRow rowEP("");
		rowEP.AddCell(CTableCell("Entry Points"));
		rowEP.AddCell(tmpStrm.str());
		tbl.AddRow(rowEP);

		// entry id block size
		ARLong32 nextIDBlockSize = -1;

		propVal = propIndex->GetAndUseValue(AR_OPROP_NEXT_ID_BLOCK_SIZE);
		if (propVal != NULL)
			if (propVal->dataType == AR_DATA_TYPE_INTEGER)
				nextIDBlockSize = propVal->u.intVal;

		tmpStrm.str("");
		tmpStrm << (nextIDBlockSize>0?"Enabled":"Disabled") << "<br/>";
		if (nextIDBlockSize>0)
			tmpStrm << "Size: " << nextIDBlockSize;

		CTableRow rowBS("");
		rowBS.AddCell("Next Request ID Block");
		rowBS.AddCell(tmpStrm.str());
		tbl.AddRow(rowBS);

#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
		// cache properties
		ARLong32 cacheDisplayProps = -1;

		propVal = propIndex->GetAndUseValue(AR_OPROP_CACHE_DISP_PROP);
		if (propVal != NULL) 
			if (propVal->dataType == AR_DATA_TYPE_INTEGER)
				cacheDisplayProps = propVal->u.intVal;

		tmpStrm.str("");
		tmpStrm << "Override Server Settings: " << (cacheDisplayProps>=0?"Enabled":"Disabled");
		if (cacheDisplayProps>=0)
		{
			tmpStrm << "<br/>";
			tmpStrm << CWebUtil::ChkBoxInput("disableVUICaching",(cacheDisplayProps & AR_CACHE_DPROP_VUI) > 0) << "Disable VUI Display Property Caching<br/>" ;
			tmpStrm << CWebUtil::ChkBoxInput("disableFieldCaching",(cacheDisplayProps & AR_CACHE_DPROP_FIELD) > 0) << "Disable Field Display Property Caching" ;
		}

		CTableRow rowOC("");
		rowOC.AddCell("Display Property Caching");
		rowOC.AddCell(tmpStrm.str());
		tbl.AddRow(rowOC);

		ARLong32 disableStatusHistory = 0;
		propVal = propIndex->GetAndUseValue(AR_OPROP_CORE_FIELDS_OPTION_MASK);
		if (propVal != NULL)
			if (propVal->dataType == AR_DATA_TYPE_INTEGER)
				disableStatusHistory = propVal->u.intVal;

		CTableRow disSH("");
		disSH.AddCell("Disable Status History");
		disSH.AddCell((disableStatusHistory & AR_CORE_FIELDS_OPTION_DISABLE_STATUS_HISTORY ? "Yes" : "No"));
		tbl.AddRow(disSH);
#endif

		// now write the table
		tbl.description = CWebUtil::ImageTag("doc.gif", rootLevel) + "Basic / EntryPoints:";
		strm << tbl;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating basic properties: " << e.what() << endl;
	}
}

void CDocSchemaDetails::ShowAuditProperties(std::ostream& strm)
{
	try
	{
		const ARAuditInfoStruct& audit = this->schema.GetAuditInfo();
		CTable tbl("displayPropList", "TblObjectList");
		tbl.AddColumn(20, "Description");
		tbl.AddColumn(80, "Values");

		CTableRow row("");
		row.AddCell("Audit Style");
		row.AddCell(CAREnum::AuditStyle(audit.style));
		tbl.AddRow(row);

		row.ClearCells();
		row.AddCell("Audit Enabled");
		row.AddCell((audit.enable==0?"No":"Yes"));
		tbl.AddRow(row);

#if AR_CURRENT_API_VERSION > 13 // Version 7.5 and higher
		row.ClearCells();
		row.AddCell("Audit Only Changed Fields");
		row.AddCell(CAREnum::AuditChangedFields(audit.auditMask));
		tbl.AddRow(row);
#endif

		row.ClearCells();
		switch (audit.style)
		{
			case AR_AUDIT_NONE:
				{
					if (audit.formName[0] != 0)
					{
						row.AddCell("Audited From Form");
						row.AddCell(this->pInside->LinkToSchema(this->pInside->SchemaGetInsideId(audit.formName),rootLevel));
						tbl.AddRow(row);
					}
				}
				break;
			case AR_AUDIT_COPY:
				{
					row.AddCell("Audit Form");
					row.AddCell(this->pInside->LinkToSchema(this->pInside->SchemaGetInsideId(audit.formName),rootLevel));
					tbl.AddRow(row);
				}
				break;
			case AR_AUDIT_LOG:
				{
					row.AddCell("Audit Log Form");
					row.AddCell(this->pInside->LinkToSchema(this->pInside->SchemaGetInsideId(audit.formName),rootLevel));
					tbl.AddRow(row);
				}
				break;
		}

		// audit qualification
		stringstream qualStrm; qualStrm.str("");
		int pFormId = schema.GetInsideId();

		if (audit.query.operation != AR_COND_OP_NONE)
		{
			CRefItem refItem(this->schema, REFM_SCHEMA_AUDIT_QUALIFICATION);
			CARQualification arQual(*this->pInside);
			arQual.CheckQuery(&audit.query, refItem, 0, pFormId, pFormId, qualStrm, rootLevel);
		}
		else
		{
			qualStrm << EmptyRunIf << endl;
		}

		row.ClearCells();
		row.AddCell("Qualification");
		row.AddCell(qualStrm.str());
		tbl.AddRow(row);

		tbl.description = CWebUtil::ImageTag("doc.gif", rootLevel) + "Audit Settings:";
		strm << tbl;
	}
	catch (...)
	{
		cerr << "EXCEPTION audit properties" << endl;
	}
}

void CDocSchemaDetails::ShowArchiveProperties(std::ostream& strm)
{
	try
	{
		const ARArchiveInfoStruct& archive = this->schema.GetArchiveInfo();
		bool archiveToForm = (archive.archiveType & AR_ARCHIVE_FORM)>0;
		bool deleteSource = (archive.archiveType & AR_ARCHIVE_DELETE)>0;
		bool archiveToFile = (archive.archiveType & (AR_ARCHIVE_FILE_ARX | AR_ARCHIVE_FILE_XML))>0;
		unsigned int fileFormat = (archive.archiveType & (AR_ARCHIVE_FILE_ARX | AR_ARCHIVE_FILE_XML)) << 2;
		bool noAttachments = (archive.archiveType & AR_ARCHIVE_NO_ATTACHMENTS)>0;
		bool noDiary = (archive.archiveType & AR_ARCHIVE_NO_DIARY)>0;

		CTable tbl("displayPropList", "TblObjectList");
		tbl.AddColumn(20, "Description");
		tbl.AddColumn(80, "Values");

		// archive type
		stringstream typeStrm; typeStrm.str("");
		if (!archiveToForm && !archiveToFile)
		{
			typeStrm << "None";
		}
		else if (archiveToForm || deleteSource)
		{
			if (archiveToForm) typeStrm << "Copy to Archive";
			if (deleteSource && archiveToForm) typeStrm << " and ";
			if (deleteSource) typeStrm << "Delete From Source";
		}

		CTableRow row("");
		row.AddCell("Archive Type");
		row.AddCell(typeStrm.str());
		tbl.AddRow(row);

		row.ClearCells();
		row.AddCell("Archive State");
		row.AddCell((archive.enable==0?"Disabled":"Enabled"));
		tbl.AddRow(row);		

		row.ClearCells();
		if (archiveToFile)
		{
			// archive to file is not supported by admin/developer tool; but maybe some day...
			row.AddCell("Archive to File");
			row.AddCell(archive.u.dirPath);
			tbl.AddRow(row);
		}
		else if (archiveToForm)
		{
			stringstream tmpStrm; tmpStrm.str("");
			tmpStrm << this->pInside->LinkToSchema(this->pInside->SchemaGetInsideId(archive.u.formName),rootLevel) << "<br/>" << endl;
			tmpStrm << CWebUtil::ChkBoxInput("noAttachments",noAttachments) << "No Attachments&nbsp;&nbsp;";
			tmpStrm << CWebUtil::ChkBoxInput("noDiary",noDiary) << "No Diary" << endl;

			row.AddCell("Archive to Form");
			row.AddCell(tmpStrm.str());
			tbl.AddRow(row);
		}

		if (archive.archiveFrom[0] != 0)
		{
			row.ClearCells();
			row.AddCell("Archive From Form");
			row.AddCell(this->pInside->LinkToSchema(this->pInside->SchemaGetInsideId(archive.archiveFrom), rootLevel));
			tbl.AddRow(row);
		}

		stringstream qualStrm; qualStrm.str("");
		int pFormId = this->schema.GetInsideId(); //this->pInside->SchemaGetInsideId(this->schema.GetARName());

		if (archive.query.operation != AR_COND_OP_NONE)
		{
			CRefItem refItem(this->schema, REFM_SCHEMA_ARCHIVE_QUALIFICATION);
			CARQualification arQual(*this->pInside);
			arQual.CheckQuery(&archive.query, refItem, 0, pFormId, pFormId, qualStrm, rootLevel);
		}
		else
		{
			qualStrm << EmptyRunIf << endl;
		}

		row.ClearCells();
		row.AddCell("Times");
		row.AddCell(CARDayStructHelper::DayStructToHTMLString(&archive.archiveTime));
		tbl.AddRow(row);

		row.ClearCells();
		row.AddCell("Qualification");
		row.AddCell(qualStrm.str());
		tbl.AddRow(row);

		tbl.description = CWebUtil::ImageTag("doc.gif", rootLevel) + "Archive Settings:";
		strm << tbl;
	}
	catch (...)
	{
		cerr << "EXCEPTION archive properties" << endl;
	}
}

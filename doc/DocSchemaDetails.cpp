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

CDocSchemaDetails::CDocSchemaDetails(CARInside &arInside, CARSchema &schema, string path, int rootLevel)
{
	this->pInside = &arInside;
	this->pSchema = &schema;
	this->path = path;
	this->rootLevel = rootLevel;

	this->uniqueAlList.clear();
	this->uniqueEscalList.clear();
	this->uniqueFilterList.clear();
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
	contHeadStrm << MenuSeparator <<this->pInside->LinkToSchemaTypeList(this->pSchema->schema.schemaType, rootLevel) << endl;
	contHeadStrm << MenuSeparator << CWebUtil::Link(this->pSchema->name, CWebUtil::DocName("index"), "", rootLevel);
	contHeadStrm << MenuSeparator << CWebUtil::ObjName(description) << endl;
	return contHeadStrm.str();
}

void CDocSchemaDetails::Documentation()
{
	try
	{
		//Create a directory for each form
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStrm;	
			CWebPage webPage("index", this->pSchema->name, rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << CWebUtil::LinkToSchemaIndex(this->rootLevel) << endl;
			contHeadStrm << MenuSeparator << this->pInside->LinkToSchemaTypeList(this->pSchema->schema.schemaType, rootLevel) << endl;
			contHeadStrm << MenuSeparator << CWebUtil::ObjName(this->pSchema->name) << endl;

			if(this->pSchema->appRefName.c_str() != NULL && this->pSchema->appRefName.size() > 0)
				contHeadStrm << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->pSchema->appRefName, this->rootLevel);

			pgStrm << contHeadStrm.str();

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


			if(this->pSchema->schema.schemaType == AR_SCHEMA_JOIN
				|| this->pSchema->schema.schemaType == AR_SCHEMA_VIEW
				|| this->pSchema->schema.schemaType == AR_SCHEMA_VENDOR)
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
			pgStrm << tblObjProp.ToXHtml();	
			tblObjProp.Clear();

			//Fields
			switch(this->pSchema->schema.schemaType)
			{
			case AR_SCHEMA_JOIN:
				pgStrm << this->AllFieldsJoin(this->pSchema->FileID());
				break;
			default:	
				pgStrm << this->AllFields(this->pSchema->FileID());
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
			webPage.AddContent(this->pInside->ServerObjectHistory(this->pSchema, this->rootLevel));

			webPage.SaveInFolder(this->path);
			pgStrm.str("");
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema details documentation of '"<< this->pSchema->name << "': " << e.what() << endl;
	}
}

string CDocSchemaDetails::SchemaNavigation()
{	
	CUList uList(this->rootLevel, "");

	try
	{
		//Permissions
		uList.AddItem(CUListItem(CWebUtil::Link("Permission", CWebUtil::DocName("form_permission_list"), "", rootLevel)));

		//Workflow		
		uList.AddItem(CUListItem(CWebUtil::Link("Workflow", CWebUtil::DocName("form_workflow"), "", rootLevel)));

		//Indexes
		uList.AddItem(CUListItem(CWebUtil::Link("Indexes", CWebUtil::DocName("form_index_list"), "", rootLevel)));

		//SortList
		uList.AddItem(CUListItem(CWebUtil::Link("Sortlist", CWebUtil::DocName("form_sort_list"), "", rootLevel)));

		//ResultList
		uList.AddItem(CUListItem(CWebUtil::Link("Resultlist", CWebUtil::DocName("form_result_list"), "", rootLevel)));

		//Views
		uList.AddItem(CUListItem(CWebUtil::Link("Views", CWebUtil::DocName("form_vui_list"), "", rootLevel)));

		//Subadministrator
		uList.AddItem(CUListItem(CWebUtil::Link("Subadministrator", CWebUtil::DocName("form_subadmin_list"), "", rootLevel)));

		//Active Links
		uList.AddItem(CUListItem(CWebUtil::Link("Active Link", CWebUtil::DocName("form_al_list"), "", rootLevel)));

		//Filters
		uList.AddItem(CUListItem(CWebUtil::Link("Filter", CWebUtil::DocName("form_filter_list"), "", rootLevel)));

		//Escalations
		uList.AddItem(CUListItem(CWebUtil::Link("Escalation", CWebUtil::DocName("form_escal_list"), "", rootLevel)));
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema navigation of '" << this->pSchema->name << "': " << e.what() << endl;
	}	

	return uList.ToXHtml(CWebUtil::Link("&nbsp;Form&nbsp;Property&nbsp;Navigation&nbsp;", CWebUtil::DocName("index"), "", rootLevel, false), true);
}


//Create a page with all fields of this form
string CDocSchemaDetails::AllFields(string fName)
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
		list<CARField>::iterator listIter;
		CARField *field;
		for ( listIter = this->pSchema->fieldList.begin(); listIter != this->pSchema->fieldList.end(); listIter++ )
		{
			field = &(*listIter);

			CTableRow row("");


			CTableCell cellName(field->GetURL(rootLevel), "");
			CTableCell cellFieldId(field->fieldId, "");
			CTableCell cellDataType(CAREnum::DataType(field->dataType), "");

			stringstream strmTmp;
			if(field->dInstanceList.numItems == 0 && field->fieldId != 15)				
				strmTmp << "<span class=\"fieldInNoView\">" << field->dInstanceList.numItems << "</span" << endl;
			else
				strmTmp << field->dInstanceList.numItems;			
			CTableCell cellNumViews(strmTmp.str(), "");			

			CTableCell cellTimestamp(CUtil::DateTimeToHTMLString(field->timestamp), "");
			CTableCell cellLastChanged(this->pInside->LinkToUser(field->lastChanged, 2), "");

			row.AddCell(cellName);		
			row.AddCell(cellFieldId);
			row.AddCell(cellDataType);
			row.AddCell(cellNumViews);
			row.AddCell(cellTimestamp);
			row.AddCell(cellLastChanged);

			tbl.AddRow(row);
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << tbl.NumRows() << " fields (" << CWebUtil::Link("data", CWebUtil::CsvDocName(fName+"_fields"), "", 0) << ")" << endl;
		tbl.description = tblDesc.str();

		AllFieldsCsv(fName);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema all fields of '"<< this->pSchema->name << "': " << e.what() << endl;
	}

	return tbl.ToXHtml();	
}

void CDocSchemaDetails::AllFieldsCsv(string fName)
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
		list<CARField>::iterator listIter;
		CARField *field;
		for ( listIter = this->pSchema->fieldList.begin(); listIter != this->pSchema->fieldList.end(); listIter++ )
		{
			field = &(*listIter);

			CTableRow row("");
			CTableCell cellName(field->name, "");
			CTableCell cellFieldId(field->fieldId, "");
			CTableCell cellDataType(CAREnum::DataType(field->dataType), "");			
			CTableCell cellNumViews(field->dInstanceList.numItems, "");	
			CTableCell cellTimestamp(CUtil::DateTimeToString(field->timestamp), "");
			CTableCell cellLastChanged(field->lastChanged, "");

			row.AddCell(cellName);		
			row.AddCell(cellFieldId);
			row.AddCell(cellDataType);
			row.AddCell(cellNumViews);
			row.AddCell(cellTimestamp);
			row.AddCell(cellLastChanged);

			tbl.AddRow(row);
		}

		//Save field information to ccs
		CCsvPage csvPage(fName+"_fields", this->pInside->appConfig);
		csvPage.SaveInFolder(path, tbl.ToCsv());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema all fields csv of '" << this->pSchema->name << "': " << e.what() << endl;
	}
}


//Create a page with all fields of a joinform
string CDocSchemaDetails::AllFieldsJoin(string fName)
{		
	CTable tbl("fieldListAll", "TblObjectList");
	tbl.AddColumn(20, "Field Name");
	tbl.AddColumn(5, "Field ID");
	tbl.AddColumn(10, "In Views");
	tbl.AddColumn(5, "Datatype");
	tbl.AddColumn(30, "Real Field ID");
	tbl.AddColumn(10, "Modified");
	tbl.AddColumn(20, "By");

	try
	{
		list<CARField>::iterator listIter;		
		for ( listIter = this->pSchema->fieldList.begin(); listIter != this->pSchema->fieldList.end(); listIter++ )
		{
			CARField *field = &(*listIter);

			CTableRow row("");
			CTableCell cellName(field->GetURL(rootLevel), "");
			CTableCell cellFieldId(field->fieldId, "");

			stringstream strmTmp;
			if(field->dInstanceList.numItems == 0 && field->fieldId != 15)				
				strmTmp << "<span class=\"fieldInNoView\">" << field->dInstanceList.numItems << "</span" << endl;
			else
				strmTmp << field->dInstanceList.numItems;			
			CTableCell cellNumViews(strmTmp.str(), "");	

			CTableCell cellDataType(CAREnum::DataType(field->dataType), "");

			strmTmp.str("");			
			if(field->fieldId == 1) // RequestID 1 in Joinform = ReqId1 | ReqId2
			{
				strmTmp.str("");
				strmTmp << this->pInside->LinkToField(this->pSchema->schema.u.join.memberA, 1, rootLevel) << "&nbsp;" << MenuSeparator << "&nbsp;" << this->pInside->LinkToSchema(this->pSchema->schema.u.join.memberA, rootLevel);
				strmTmp << "<br/>" << endl;
				strmTmp << this->pInside->LinkToField(this->pSchema->schema.u.join.memberB, 1, rootLevel) << "&nbsp;" << MenuSeparator << "&nbsp;" << this->pInside->LinkToSchema(this->pSchema->schema.u.join.memberB, rootLevel);
			}
			else
			{
				strmTmp.str("");
				if(field->fieldMap.u.join.realId > 0)
				{			
					string tmpBaseSchema = this->pSchema->schema.u.join.memberA;
					if(field->fieldMap.u.join.schemaIndex > 0)
						tmpBaseSchema = this->pSchema->schema.u.join.memberB;

					strmTmp << this->pInside->LinkToField(tmpBaseSchema, field->fieldMap.u.join.realId, rootLevel) << "&nbsp;" << MenuSeparator << "&nbsp;" << this->pInside->LinkToSchema(tmpBaseSchema, rootLevel);
				}
				else
					strmTmp << "&nbsp;";
			}			

			CTableCell cellFieldRealId(strmTmp.str(), "");
			CTableCell cellTimestamp(CUtil::DateTimeToHTMLString(field->timestamp), "");
			CTableCell cellLastChanged(this->pInside->LinkToUser(field->lastChanged, rootLevel), "");

			row.AddCell(cellName);
			row.AddCell(cellFieldId);
			row.AddCell(cellNumViews);
			row.AddCell(cellDataType);
			row.AddCell(cellFieldRealId);
			row.AddCell(cellTimestamp);
			row.AddCell(cellLastChanged);

			tbl.AddRow(row);
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << tbl.NumRows() << " fields (" << CWebUtil::Link("data", CWebUtil::CsvDocName(fName+"_fields"), "", 0) << ")" << endl;
		tbl.description = tblDesc.str();

		AllFieldsJoinCsv(fName);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema all fields join of '"<< this->pSchema->name << "': " << e.what() << endl;
	}

	return tbl.ToXHtml();	
}

void CDocSchemaDetails::AllFieldsJoinCsv(string fName)
{		
	CTable tbl("fieldListAll", "TblObjectList");
	tbl.AddColumn(20, "Field Name");
	tbl.AddColumn(5, "Field ID");
	tbl.AddColumn(10, "In Views");
	tbl.AddColumn(5, "Datatype");
	tbl.AddColumn(30, "Real Field ID");
	tbl.AddColumn(10, "Modified");
	tbl.AddColumn(20, "By");

	try
	{
		list<CARField>::iterator listIter;		
		for ( listIter = this->pSchema->fieldList.begin(); listIter != this->pSchema->fieldList.end(); listIter++ )
		{
			CARField *field = &(*listIter);

			CTableRow row("");
			CTableCell cellName(field->name, "");
			CTableCell cellFieldId(field->fieldId, "");
			CTableCell cellNumViews(field->dInstanceList.numItems, "");
			CTableCell cellDataType(CAREnum::DataType(field->dataType), "");

			int nFieldRealId = 0;		
			if(field->fieldId == 1) // RequestID 1 in Joinform = ReqId1 | ReqId2
			{
				nFieldRealId = 1;
			}
			else
			{
				if(field->fieldMap.u.join.realId > 0)
				{			
					nFieldRealId = field->fieldMap.u.join.realId;
				}
			}			

			CTableCell cellFieldRealId(nFieldRealId, "");
			CTableCell cellTimestamp(CUtil::DateTimeToString(field->timestamp), "");
			CTableCell cellLastChanged(field->lastChanged, "");

			row.AddCell(cellName);
			row.AddCell(cellFieldId);
			row.AddCell(cellNumViews);
			row.AddCell(cellDataType);
			row.AddCell(cellFieldRealId);
			row.AddCell(cellTimestamp);
			row.AddCell(cellLastChanged);

			tbl.AddRow(row);
		}

		//Save field information to ccs
		CCsvPage csvPage(fName+"_fields", this->pInside->appConfig);
		csvPage.SaveInFolder(path, tbl.ToCsv());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema all fields join csv of '" << this->pSchema->name << "': " << e.what() << endl;
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

CTableRow CDocSchemaDetails::UniqueTblRow(string listItemName, int itemType)
{			
	CTableRow row("cssStdRow");		
	row.AddCell(CAREnum::XmlStructItem(itemType));				
	row.AddCell(this->pInside->LinkToXmlObjType(itemType, listItemName, rootLevel));

	string tmpEnabled = this->pInside->XmlObjEnabled(itemType, listItemName);
	string tmpCssEnabled = "";

	if(strcmp(tmpEnabled.c_str(), "Disabled")==0)
		tmpCssEnabled = "objStatusDisabled";

	row.AddCell(CTableCell(tmpEnabled, tmpCssEnabled));
	return row;
}

void CDocSchemaDetails::WorkflowDoc()
{
	try
	{
		string title = "Schema " +this->pSchema->name +" (Workflow)";
		CWebPage webPage("form_workflow", title, rootLevel, this->pInside->appConfig);

		//ContentHead informations		
		webPage.AddContentHead(this->FormPageHeader("Workflow"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());


		//Field references
		CTable tblRef("referenceList", "TblObjectList");
		tblRef.AddColumn(15, "Type");
		tblRef.AddColumn(25, "Server object");
		tblRef.AddColumn(10, "Enabled");

		list<CFieldRefItem>::iterator iter;
		for ( iter = this->pInside->listFieldRefItem.begin(); iter != this->pInside->listFieldRefItem.end(); iter++ )
		{	
			CFieldRefItem *item = &(*iter);
			if(item->schemaInsideId == this->pSchema->GetInsideId() 
				&& ( item->arsStructItemType == AR_STRUCT_ITEM_XML_FILTER
				|| item->arsStructItemType == AR_STRUCT_ITEM_XML_ACTIVE_LINK
				|| item->arsStructItemType == AR_STRUCT_ITEM_XML_ESCALATION))
			{			
				if(strcmp(item->fromName.c_str(), EmptyValue)!=0)
				{
					if(item->arsStructItemType == AR_STRUCT_ITEM_XML_ACTIVE_LINK)
					{
						if(!this->InAlList(item->fromName))
						{
							this->uniqueAlList.insert(this->uniqueAlList.end(), item->fromName);
							tblRef.AddRow(UniqueTblRow(item->fromName, AR_STRUCT_ITEM_XML_ACTIVE_LINK));
						}
					}

					if(item->arsStructItemType == AR_STRUCT_ITEM_XML_FILTER)
					{
						if(!this->InFilterList(item->fromName))
						{
							this->uniqueFilterList.insert(this->uniqueFilterList.end(), item->fromName);
							tblRef.AddRow(UniqueTblRow(item->fromName, AR_STRUCT_ITEM_XML_FILTER));
						}
					}

					if(item->arsStructItemType == AR_STRUCT_ITEM_XML_ESCALATION)
					{
						if(!this->InEscalList(item->fromName))
						{
							this->uniqueEscalList.insert(this->uniqueEscalList.end(), item->fromName);
							tblRef.AddRow(UniqueTblRow(item->fromName, AR_STRUCT_ITEM_XML_ESCALATION));
						}
					}
				}
			}
		}


		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Workflow Reference:";

		tblRef.description = tblDesc.str();

		webPage.AddContent(tblRef.ToXHtml());
		webPage.SaveInFolder(this->path);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema workflow doc of '" << this->pSchema->name << "': " << e.what() << endl;
	}
}

//list of groups with access rights
void CDocSchemaDetails::SchemaPermissionDoc()
{
	try
	{
		string title = "Schema " +this->pSchema->name +" (Indexes)";
		CWebPage webPage("form_permission_list", title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Permissions"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());

		CTable tbl("permissionList", "TblObjectList");
		tbl.AddColumn(5, "Permission");
		tbl.AddColumn(10, "Description");
		tbl.AddColumn(75, "Group Name");
		tbl.AddColumn(10, "Group Id");

		for(unsigned int i=0; i< this->pSchema->groupList.numItems; i++)
		{
			if(this->pInside->ValidateGroup(this->pSchema->appRefName, this->pSchema->groupList.permissionList[i].groupId))
			{
				CTableRow row("");

				string img = CWebUtil::ImageTag("visible.gif", rootLevel);

				if(this->pSchema->groupList.permissionList[i].permissions == AR_PERMISSIONS_HIDDEN)
					img = CWebUtil::ImageTag("hidden.gif", rootLevel);

				row.AddCell(CTableCell(img));
				row.AddCell(CTableCell(CAREnum::ObjectPermission(this->pSchema->groupList.permissionList[i].permissions)));
				row.AddCell(CTableCell(this->pInside->LinkToGroup(this->pSchema->appRefName, this->pSchema->groupList.permissionList[i].groupId, rootLevel)));
				row.AddCell(CTableCell(this->pSchema->groupList.permissionList[i].groupId));
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


		list<CARField>::iterator listIter;
		CARField *field;
		for ( listIter = this->pSchema->fieldList.begin(); listIter != this->pSchema->fieldList.end(); listIter++ )
		{
			field = &(*listIter);

			//Field Permissions
			stringstream strmFieldPermissions;
			strmFieldPermissions.str("");

			if(field->permissions.numItems > 0)
			{
				CTable tblFieldPermissionDetails("PermissionFieldList", "TblHidden");
				tblFieldPermissionDetails.AddColumn(5, "Permission");
				tblFieldPermissionDetails.AddColumn(20, "Description");
				tblFieldPermissionDetails.AddColumn(75, "Group Name");

				for(unsigned int i=0; i<field->permissions.numItems; i++)
				{
					string img = CWebUtil::ImageTag("visible.gif", rootLevel);
					if(field->permissions.permissionList[i].permissions == AR_PERMISSIONS_CHANGE)
						img = CWebUtil::ImageTag("edit.gif", rootLevel);

					CTableRow row("");
					row.AddCell(CTableCell(img));
					row.AddCell(CTableCell(CAREnum::FieldPermission(field->permissions.permissionList[i].permissions)));
					row.AddCell(CTableCell(this->pInside->LinkToGroup(field->appRefName, field->permissions.permissionList[i].groupId, rootLevel)));
					tblFieldPermissionDetails.AddRow(row);
				}

				strmFieldPermissions << tblFieldPermissionDetails.ToXHtmlNoHeader();
			}
			else
			{
				strmFieldPermissions << EmptyValue << endl;
			}


			CTableRow row("");			
			row.AddCell(CTableCell(field->GetURL(rootLevel)));
			row.AddCell(CTableCell(field->fieldId));
			row.AddCell(CTableCell(CAREnum::DataType(field->dataType)));
			row.AddCell(CTableCell(strmFieldPermissions.str()));
			fieldTbl.AddRow(row);
		}

		webPage.AddContent(fieldTbl.ToXHtml());	
		webPage.SaveInFolder(this->path);	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema permission doc of '" << this->pSchema->name << "': " << e.what() << endl;
	}
}

//list of groups with subadministrator access to this form
void CDocSchemaDetails::SchemaSubadminDoc()
{
	try
	{
		string title = "Schema " +this->pSchema->name +" (Indexes)";
		CWebPage webPage("form_subadmin_list", title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Subadministrator Permission"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());


		CTable tbl("fieldListAll", "TblObjectList");
		tbl.AddColumn(10, "Group Id");
		tbl.AddColumn(90, "Group Name");

		for(unsigned int i=0; i< this->pSchema->admingrpList.numItems; i++)
		{
			CTableRow row("");					
			row.AddCell(CTableCell(this->pSchema->admingrpList.internalIdList[i]));
			row.AddCell(CTableCell(this->pInside->LinkToGroup(this->pSchema->appRefName, this->pSchema->admingrpList.internalIdList[i], rootLevel)));		
			tbl.AddRow(row);
		}

		webPage.AddContent(tbl.ToXHtml());
		webPage.SaveInFolder(this->path);	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema subadmin doc of '" << this->pSchema->name << "': " << e.what() << endl;
	}
}

//Create a page that shows a list of all indexes
void CDocSchemaDetails::IndexDoc()
{
	try
	{
		string title = "Schema " +this->pSchema->name +" (Indexes)";
		CWebPage webPage("form_index_list", title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Indexes"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());

		for(unsigned int nIndex=0; nIndex < this->pSchema->indexList.numItems; nIndex++)
		{		
			CTable tbl("fieldListAll", "TblObjectList");
			tbl.AddColumn(0, "Field Name");
			tbl.AddColumn(0, "Field ID");
			tbl.AddColumn(0, "Datatype");
			tbl.AddColumn(0, "Modified");
			tbl.AddColumn(0, "By");

			stringstream tblDesc;
			if(this->pSchema->indexList.indexList[nIndex].unique)
				tblDesc << "Unique Index :" <<  CWebUtil::ObjName(this->pSchema->indexList.indexList[nIndex].indexName);
			else
				tblDesc << "Index: " << CWebUtil::ObjName(this->pSchema->indexList.indexList[nIndex].indexName);

			tbl.description = tblDesc.str();

			for(unsigned int nField=0; nField < this->pSchema->indexList.indexList[nIndex].numFields; nField++)
			{
				list<CARField>::iterator listIter;			
				CARField *field;
				for ( listIter = this->pSchema->fieldList.begin(); listIter != this->pSchema->fieldList.end(); listIter++ )
				{
					field = &(*listIter);

					if(field->fieldId == this->pSchema->indexList.indexList[nIndex].fieldIds[nField])
					{
						CTableRow row("");
						row.AddCell( CTableCell(field->GetURL(rootLevel)));
						row.AddCell( CTableCell(field->fieldId));
						row.AddCell( CTableCell(CAREnum::DataType(field->dataType)));
						row.AddCell( CTableCell(CUtil::DateTimeToHTMLString(field->timestamp)));
						row.AddCell( CTableCell(this->pInside->LinkToUser(field->lastChanged, rootLevel)));
						tbl.AddRow(row);

						//Add a reference
						CFieldRefItem *refItem = new CFieldRefItem();
						refItem->arsStructItemType = AR_STRUCT_ITEM_XML_SCHEMA;

						string tmp = CWebUtil::Link(this->pSchema->indexList.indexList[nIndex].indexName, CWebUtil::DocName("form_index_list"), "", 2);
						refItem->description = "Field in "+tmp;
						refItem->fromName = this->pSchema->name;
						refItem->fieldInsideId = field->GetInsideId();
						refItem->schemaInsideId = this->pSchema->GetInsideId();
						this->pInside->AddReferenceItem(refItem);
						delete refItem;
					}
				}
			}

			webPage.AddContent(tbl.ToXHtml());
			tbl.Clear();
		}

		webPage.SaveInFolder(this->path);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema index doc of '" << this->pSchema->name << "': " << e.what() << endl;
	}
}

//Create a page that shows the form resultlist
void CDocSchemaDetails::ResultListDoc()
{
	try
	{
		string title = "Schema " +this->pSchema->name +" (Results Lists Fields)";
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

		for(unsigned int i=0; i< this->pSchema->getListFields.numItems; i++)
		{
			AREntryListFieldStruct fList = this->pSchema->getListFields.fieldsList[i];

			list<CARField>::iterator listIter;						
			for ( listIter = this->pSchema->fieldList.begin(); listIter != this->pSchema->fieldList.end(); listIter++ )
			{
				CARField *field = &(*listIter);

				if(field->fieldId == fList.fieldId)
				{
					CTableRow row("");
					row.AddCell( CTableCell( field->GetURL(rootLevel)));
					row.AddCell( CTableCell(field->fieldId));
					row.AddCell( CTableCell(CAREnum::DataType(field->dataType)));
					row.AddCell( CTableCell(fList.columnWidth));
					row.AddCell( CTableCell(fList.separator));
					row.AddCell( CTableCell(CUtil::DateTimeToHTMLString(field->timestamp)));
					row.AddCell( CTableCell(this->pInside->LinkToUser(field->lastChanged, rootLevel)));
					tbl.AddRow(row);

					//Add a reference
					CFieldRefItem *refItem = new CFieldRefItem();
					refItem->arsStructItemType = AR_STRUCT_ITEM_XML_SCHEMA;

					string tmp = CWebUtil::Link("ResultList", CWebUtil::DocName("form_result_list"), "", 2);
					refItem->description = "Field in "+tmp;

					refItem->fromName = this->pSchema->name;
					refItem->fieldInsideId = field->GetInsideId();
					refItem->schemaInsideId = this->pSchema->GetInsideId();
					this->pInside->AddReferenceItem(refItem);
					delete refItem;				
				}
			}
		}

		webPage.AddContent(tbl.ToXHtml());
		tbl.Clear();

		webPage.SaveInFolder(this->path);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema resultlist doc of '" << this->pSchema->name << "': " << e.what() << endl;
	}
}

//Create a page that shows the page sortslist
void CDocSchemaDetails::SortListDoc()
{
	try
	{
		string title = "Schema " +this->pSchema->name +" (Results Lists Fields)";
		CWebPage webPage("form_sort_list", title, rootLevel, this->pInside->appConfig);

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


		for(unsigned int i=0; i< this->pSchema->sortList.numItems; i++)
		{
			ARSortStruct sList = this->pSchema->sortList.sortList[i];

			list<CARField>::iterator listIter;			
			CARField *field;
			for ( listIter = this->pSchema->fieldList.begin(); listIter != this->pSchema->fieldList.end(); listIter++ )
			{
				field = &(*listIter);

				if(field->fieldId == sList.fieldId)
				{
					CTableRow row("");

					string sortImage = CWebUtil::ImageTag("sort_asc.gif", rootLevel);
					if(sList.sortOrder == AR_SORT_DESCENDING)
						sortImage = CWebUtil::ImageTag("sort_desc.gif", rootLevel);

					row.AddCell( CTableCell(sortImage));
					row.AddCell( CTableCell(field->GetURL(rootLevel)));
					row.AddCell( CTableCell(field->fieldId));
					row.AddCell( CTableCell(CAREnum::DataType(field->dataType)));				
					row.AddCell( CTableCell(CUtil::DateTimeToHTMLString(field->timestamp)));
					row.AddCell( CTableCell(this->pInside->LinkToUser(field->lastChanged, rootLevel)));

					tbl.AddRow(row);

					//Add a reference
					CFieldRefItem *refItem = new CFieldRefItem();
					refItem->arsStructItemType = AR_STRUCT_ITEM_XML_SCHEMA;
					refItem->description = "SortList";
					refItem->fromName = this->pSchema->name;
					refItem->fieldInsideId = field->GetInsideId();
					refItem->schemaInsideId = this->pSchema->GetInsideId();
					this->pInside->AddReferenceItem(refItem);
					delete refItem;
				}
			}
		}

		webPage.AddContent(tbl.ToXHtml());
		tbl.Clear();

		webPage.SaveInFolder(this->path);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema sortlist doc of '" << this->pSchema->name << "': " << e.what() << endl;
	}
}

//Create a page with a list of all views
void CDocSchemaDetails::VuiListDoc()
{	
	try
	{
		string title = "Schema " +this->pSchema->name +" (Views)";
		CWebPage webPage("form_vui_list", title, rootLevel, this->pInside->appConfig);

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

		list<CARVui>::iterator vuiIter;			
		CARVui *vui;
		for ( vuiIter = this->pSchema->vuiList.begin(); vuiIter != this->pSchema->vuiList.end(); vuiIter++ )
		{
			vui = &(*vuiIter);

			CTableRow row("");			
			row.AddCell( CTableCell(vui->GetURL(rootLevel)));
			row.AddCell( CTableCell(vui->Label()));
			row.AddCell( CTableCell(vui->webAlias()));
			row.AddCell( CTableCell(CAREnum::VuiType(vui->vuiType)));
			row.AddCell( CTableCell(CUtil::DateTimeToHTMLString(vui->timestamp)));
			row.AddCell( CTableCell(this->pInside->LinkToUser(vui->lastChanged, rootLevel)));
			tbl.AddRow(row);
		}

		webPage.AddContent(tbl.ToXHtml());
		tbl.Clear();

		webPage.SaveInFolder(this->path);	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema vuilist doc of '" << this->pSchema->name << "': " << e.what() << endl;
	}
}

//Create a list with all filters related to thisform
void CDocSchemaDetails::SchemaFilterDoc()
{
	try
	{
		string title = "Schema " +this->pSchema->name +" (Filter)";
		CWebPage webPage("form_filter_list", title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Filter"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());

		CFilterTable *tbl = new CFilterTable(*this->pInside);

		list<CARFilter>::iterator iter;			
		CARFilter *enumObj;
		for (iter = this->pInside->filterList.begin(); iter != this->pInside->filterList.end(); iter++)
		{
			enumObj = &(*iter);

			if(enumObj->schemaList.u.schemaList != NULL)
			{
				for(unsigned int i=0; i < enumObj->schemaList.u.schemaList->numItems; i++)
				{
					if(enumObj->schemaList.u.schemaList->nameList[i] == this->pSchema->name)
					{
						tbl->AddRow(*enumObj, rootLevel);
					}
				}
			}
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("filter.gif", rootLevel) << tbl->NumRows() << " ";
		tblDesc << CWebUtil::Link("Filter", CWebUtil::DocName("../../filter/index"), "", rootLevel);
		tbl->SetDescription(tblDesc.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder(this->path);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema filter doc of '" << this->pSchema->name << "': " << e.what() << endl;
	}
}

//Create a list with all active links related to this form
void CDocSchemaDetails::SchemaAlDoc()
{
	try
	{
		string title = "Schema " +this->pSchema->name +" (ActiveLinks)";
		CWebPage webPage("form_al_list", title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Active Links"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());

		CAlTable *tbl = new CAlTable(*this->pInside);

		list<CARActiveLink>::iterator iter;			
		CARActiveLink *enumObj;
		for (iter = this->pInside->alList.begin(); iter != this->pInside->alList.end(); iter++)
		{
			enumObj = &(*iter);

			if(enumObj->schemaList.u.schemaList != NULL)
			{
				for(unsigned int i=0; i < enumObj->schemaList.u.schemaList->numItems; i++)
				{
					if(enumObj->schemaList.u.schemaList->nameList[i] == this->pSchema->name)
					{
						tbl->AddRow(*enumObj, rootLevel);
					}
				}
			}
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("active_link.gif", rootLevel) << tbl->NumRows() << " ";
		tblDesc << CWebUtil::Link("ActiveLinks", CWebUtil::DocName("../../active_link/index"), "", rootLevel);
		tbl->SetDescription(tblDesc.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder(this->path);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema active link doc of '" << this->pSchema->name << "': " << e.what() << endl;
	}
}

//Create a list with all escalations related to this form
void CDocSchemaDetails::SchemaEscalDoc()
{
	try
	{
		string title = "Schema " +this->pSchema->name +" (Escalations)";
		CWebPage webPage("form_escal_list", title, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(this->FormPageHeader("Escalations"));

		//Add schema navigation menu	
		webPage.SetNavigation(this->SchemaNavigation());

		CEscalTable *tbl = new CEscalTable(*this->pInside);

		list<CAREscalation>::iterator iter;			
		CAREscalation *enumObj;
		for (iter = this->pInside->escalList.begin(); iter != this->pInside->escalList.end(); iter++)
		{
			enumObj = &(*iter);

			if(enumObj->schemaList.u.schemaList != NULL)
			{
				for(unsigned int i=0; i < enumObj->schemaList.u.schemaList->numItems; i++)
				{
					if(enumObj->schemaList.u.schemaList->nameList[i] == this->pSchema->name)
					{
						tbl->AddRow(*enumObj, rootLevel);
					}
				}
			}
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("escalation.gif", rootLevel) << tbl->NumRows() << " ";
		tblDesc << CWebUtil::Link("Escalations", CWebUtil::DocName("../../escalation/index"), "", rootLevel);
		tbl->SetDescription(tblDesc.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder(this->path);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema escalation doc of '" << this->pSchema->name << "': " << e.what() << endl;
	}
}

string CDocSchemaDetails::TypeDetails()
{
	stringstream strm;
	strm.str("");

	try
	{
		switch(this->pSchema->schema.schemaType)
		{
		case AR_SCHEMA_JOIN:
			{
				strm << "Primary Form: " << this->pInside->LinkToSchema(this->pSchema->schema.u.join.memberA, rootLevel) << " <-> Secondary Form: " << this->pInside->LinkToSchema(this->pSchema->schema.u.join.memberB, rootLevel) << "<br/>";

				if(this->pSchema->schema.u.join.option == 0)
					strm << "Join Type: Inner" << "<br/>" << endl;
				else
					strm << "Join Type: Outer" << "<br/>" << endl;

				if(this->pSchema->schema.u.join.joinQual.operation != NULL)
				{
					CFieldRefItem *refItem = new CFieldRefItem();
					refItem->arsStructItemType = AR_STRUCT_ITEM_XML_SCHEMA;
					refItem->description = "Join Qualification";
					refItem->fromName = this->pSchema->name;

					stringstream strmQuery;		

					CARQualification arQual(*this->pInside);

					int pFormId = this->pInside->SchemaGetInsideId(this->pSchema->schema.u.join.memberA);
					int sFormId = this->pInside->SchemaGetInsideId(this->pSchema->schema.u.join.memberB);
					arQual.CheckQuery(&this->pSchema->schema.u.join.joinQual, *refItem, 0, pFormId, sFormId, strmQuery, rootLevel);

					strm << "Join Qualification: " << strmQuery.str();
					delete refItem;
				}
			}
			break;
		case AR_SCHEMA_VIEW:
			{
				strm << "Table Name: " << this->pSchema->schema.u.view.tableName << "<br/>" << endl;
				strm << "Key Field: " << this->pSchema->schema.u.view.keyField << "<br/>" << endl;
			}
			break;			
		case AR_SCHEMA_VENDOR:
			{
				strm << "Vendor Name: " << this->pSchema->schema.u.vendor.vendorName << "<br/>";
				strm << "Table Name: " << this->pSchema->schema.u.vendor.tableName << "<br/>" << endl;
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
		cout << "EXCEPTION enumerating regular schema type information of '" << this->pSchema->name << "': " << e.what() << endl;
	}
	return strm.str();
}

string CDocSchemaDetails::ContainerReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		CContainerTable *contTable = new CContainerTable(*this->pInside);

		list<CARContainer>::iterator listContIter;		
		for ( listContIter = this->pInside->containerList.begin();  listContIter != this->pInside->containerList.end(); listContIter++ )
		{
			CARContainer *cont = &(*listContIter);
			for(unsigned int nCnt = 0; nCnt < cont->references.numItems; nCnt++)
			{
				if(cont->type != ARCON_APP)
				{
					if(cont->references.referenceList[nCnt].reference.u.name != NULL)
					{
						if(strcmp(cont->references.referenceList[nCnt].reference.u.name, this->pSchema->name.c_str())==0
							&& cont->references.referenceList[nCnt].type == ARREF_SCHEMA)
						{
							contTable->AddRow(*cont, rootLevel);
						}
					}
				}
			}
		}		


		if(contTable->NumRows() > 0)
			strm << contTable->Print();
		else
			strm << EmptyValue;

		delete contTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating container references in schema '" << this->pSchema->name << "': " << e.what() << endl;
	}

	return strm.str();
}

string CDocSchemaDetails::JoinFormReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		list<CARSchema>::iterator schemaIter;		
		CARSchema *schema;
		for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
		{			
			schema = &(*schemaIter);
			if(schema->schema.schemaType == AR_SCHEMA_JOIN)
			{
				if(strcmp(schema->schema.u.join.memberA, this->pSchema->name.c_str())==0)					
				{
					strm << "Primary Form in: " << schema->GetURL(rootLevel) << "<br/>" << endl;
				}

				if(strcmp(schema->schema.u.join.memberB, this->pSchema->name.c_str())==0)					
				{
					strm << "Secondary Form in: " << schema->GetURL(rootLevel) << "<br/>" << endl;
				}
			}
		}

		if(strm.str().size() == 0)
			strm << EmptyValue;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating join form references of '" << this->pSchema->name << "': " << e.what() << endl;
	}

	return strm.str();
}


string CDocSchemaDetails::TableFieldReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		bool bFound = false;

		list<CARSchema>::iterator schemaIter;		
		for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
		{			
			CARSchema *schema = &(*schemaIter);

			list<CARField>::iterator fieldIter;		
			for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
			{
				CARField *field = &(*fieldIter);
				if(field->dataType == AR_DATA_TYPE_TABLE && field->limit.dataType == AR_DATA_TYPE_TABLE)
				{
					ARTableLimitsStruct &limit = field->limit.u.tableLimits;
					
					char *tableSchema = limit.schema;

					if (limit.schema[0] == '$')
						tableSchema = limit.sampleSchema;

					if(pSchema->name.compare(tableSchema) == 0)
					{
						strm << "Table: " << field->GetURL(rootLevel);
						strm << " in form: " << schema->GetURL(rootLevel) << "<br/>" << endl;

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
		cout << "EXCEPTION enumerating table references in schema '" << this->pSchema->name << "': " << e.what() << endl;
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

		list<CARCharMenu>::iterator listMenuIter;		
		CARCharMenu *menu;
		for ( listMenuIter = this->pInside->menuList.begin();  listMenuIter != this->pInside->menuList.end(); listMenuIter++ )
		{
			menu = &(*listMenuIter);
			if(menu->menuDefn.menuType == AR_CHAR_MENU_QUERY)
			{
				if(strcmp(menu->menuDefn.u.menuQuery.schema, this->pSchema->name.c_str())==0 && strcmp(menu->menuDefn.u.menuQuery.server ,AR_CURRENT_SERVER_TAG)==0)
				{
					menuTable->AddRow(*menu, rootLevel);
				}
			}
		}		


		if(menuTable->NumRows() > 0)
			strm << menuTable->Print();
		else
			strm << EmptyValue;

		delete menuTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating search menu references in schema '" << this->pSchema->name << "': " << e.what() << endl;
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

		list<CARActiveLink>::iterator listIter;		
		CARActiveLink *al;
		for ( listIter = this->pInside->alList.begin(); listIter != this->pInside->alList.end(); listIter++ )
		{
			al = &(*listIter);

			bool bPushToForm = false;

			//If-Actions
			for(unsigned int i = 0; i < al->actionList.numItems; ++i)
			{
				if(al->actionList.actionList[i].action == AR_ACTIVE_LINK_ACTION_FIELDP)
				{
					ARPushFieldsActionStruct &action = al->actionList.actionList[i].u.pushFields;
					if (action.pushFieldsList.numItems > 0)
					{
						char *pushSchema;
						if (action.pushFieldsList.pushFieldsList[0].field.schema[0] == '$')
							pushSchema = action.sampleSchema;
						else
							pushSchema = action.pushFieldsList.pushFieldsList[0].field.schema;

						if (pushSchema[0] == '@' || pSchema->name.compare(pushSchema) == 0)
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
				for(unsigned int i = 0; i < al->elseList.numItems; ++i)
				{
					if(al->elseList.actionList[i].action == AR_ACTIVE_LINK_ACTION_FIELDP)
					{
						ARPushFieldsActionStruct &action = al->elseList.actionList[i].u.pushFields;

						if (action.pushFieldsList.numItems > 0)
						{
							char *pushSchema;
							if (action.pushFieldsList.pushFieldsList[0].field.schema[0] == '$')
								pushSchema = action.sampleSchema;
							else
								pushSchema = action.pushFieldsList.pushFieldsList[0].field.schema;

							if (pushSchema[0] == '@' || pSchema->name.compare(pushSchema) == 0)
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
				alTable->AddRow(*al, rootLevel);
			}
		}

		if(alTable->NumRows() > 0)
			strm << alTable->Print();
		else
			strm << EmptyValue;

		delete alTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating active link push fields references in schema '" << this->pSchema->name << "': " << e.what() << endl;
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

		list<CARActiveLink>::iterator listIter;		
		CARActiveLink *al;
		for ( listIter = this->pInside->alList.begin(); listIter != this->pInside->alList.end(); listIter++ )
		{
			al = &(*listIter);

			bool bPushToForm = false;

			//If-Actions
			for(unsigned int i = 0; i < al->actionList.numItems; i++)
			{
				if(al->actionList.actionList[i].action == AR_ACTIVE_LINK_ACTION_OPENDLG)
				{
					AROpenDlgStruct &action = al->actionList.actionList[i].u.openDlg;                    
					
					string openWindowSchema;
					if (action.schemaName[0] == '$' )
					{
						string sampleServer;
						CDocActionOpenWindowHelper::GetSampleData(*al, "If", i,	sampleServer, openWindowSchema);
					}
					else
						openWindowSchema = action.schemaName;

					if (!openWindowSchema.empty() && openWindowSchema.compare(AR_CURRENT_SCHEMA_TAG) == 0)
						openWindowSchema.clear();

					if(!openWindowSchema.empty() && openWindowSchema.compare(pSchema->name) == 0)
						bPushToForm = true;
				}
			}	

			//Else Actions
			if(bPushToForm == false) // Only search the else actions if the al is still false
			{
				for(unsigned int i = 0; i < al->elseList.numItems; i++)
				{
					if(al->elseList.actionList[i].action == AR_ACTIVE_LINK_ACTION_OPENDLG)
					{
						AROpenDlgStruct &action = al->elseList.actionList[i].u.openDlg;

						string openWindowSchema;
						if (action.schemaName[0] == '$' )
						{
							string sampleServer;
							CDocActionOpenWindowHelper::GetSampleData(*al, "Else", i,	sampleServer, openWindowSchema);
						}
						else
							openWindowSchema = action.schemaName;

						if (openWindowSchema.compare(AR_CURRENT_SCHEMA_TAG) == 0)
							openWindowSchema.clear();

						if(!openWindowSchema.empty() && openWindowSchema.compare(pSchema->name) == 0)
							bPushToForm = true;
					}
				}
			}

			if(bPushToForm == true)
			{
				alTable->AddRow(*al, rootLevel);
			}
		}

		if(alTable->NumRows() > 0)
			strm << alTable->Print();
		else
			strm << EmptyValue;

		delete alTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating window open references in schema '" << this->pSchema->name << "': " << e.what() << endl;
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

		list<CARFilter>::iterator listIter;		
		CARFilter *filter;
		for ( listIter = this->pInside->filterList.begin(); listIter != this->pInside->filterList.end(); listIter++ )
		{
			filter = &(*listIter);

			bool bPushToForm = false;

			//If-Actions
			for(unsigned int i = 0; i < filter->actionList.numItems; ++i)
			{
				if(filter->actionList.actionList[i].action == AR_FILTER_ACTION_FIELDP)
				{
					ARPushFieldsActionStruct &action = filter->actionList.actionList[i].u.pushFields;

					if (action.pushFieldsList.numItems > 0)
					{
						char *pushSchema;
						if (action.pushFieldsList.pushFieldsList[0].field.schema[0] == '$')
							pushSchema = action.sampleSchema;
						else
							pushSchema = action.pushFieldsList.pushFieldsList[0].field.schema;

						if (pushSchema[0] == '@' || pSchema->name.compare(pushSchema) == 0)
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
				for(unsigned int i = 0; i < filter->elseList.numItems; i++)
				{
					if(filter->elseList.actionList[i].action == AR_FILTER_ACTION_FIELDP)
					{
						ARPushFieldsActionStruct &action = filter->elseList.actionList[i].u.pushFields;

						if (action.pushFieldsList.numItems > 0)
						{
							char *pushSchema;
							if (action.pushFieldsList.pushFieldsList[0].field.schema[0] == '$')
								pushSchema = action.sampleSchema;
							else
								pushSchema = action.pushFieldsList.pushFieldsList[0].field.schema;

							if (pushSchema[0] == '@' || pSchema->name.compare(pushSchema) == 0)
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
				filterTable->AddRow(*filter, rootLevel);
			}
		}

		if(filterTable->NumRows() > 0)
			strm << filterTable->Print();
		else
			strm << EmptyValue;

		delete filterTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating filter push fields references in schema '" << this->pSchema->name << "': " << e.what() << endl;
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
		ARPropList* propList = &this->pSchema->objPropList;
		CARProplistHelper propIdx(propList);

		//for (unsigned int i=0; i < propList->numItems; ++i)
		//{
		//	propIdx[propList->props[i].prop] = &propList->props[i].value;
		//}

		// doc basic properties
		strm << ShowBasicProperties(&propIdx);

		if (this->pSchema->schema.schemaType != AR_SCHEMA_DIALOG)
		{
			// doc archive properties
			strm << ShowArchiveProperties();

			// doc audit properties
			strm << ShowAuditProperties();
		}

		// doc properties left
		//strm << ShowPropertiesLeft(&propIdx);
		strm << propIdx.UnusedPropertiesToHTML();
	}
	catch(exception& e)
	{
		cerr << "EXCEPTION enumerating properties: " << e.what() << endl;
	}
	return strm.str();
}

string CDocSchemaDetails::ShowBasicProperties(CARProplistHelper* propIndex)
{
	stringstream strm;
	strm.str("");
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
#endif

		// now write the table
		tbl.description = CWebUtil::ImageTag("doc.gif", rootLevel) + "Basic / EntryPoints:";
		strm << tbl.ToXHtml();
		tbl.Clear();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating basic properties: " << e.what() << endl;
	}

	return strm.str();
}

string CDocSchemaDetails::ShowAuditProperties()
{
	stringstream strm; strm.str("");

	try
	{
		CTable tbl("displayPropList", "TblObjectList");
		tbl.AddColumn(20, "Description");
		tbl.AddColumn(80, "Values");

		CTableRow row("");
		row.AddCell("Audit Style");
		row.AddCell(CAREnum::AuditStyle(this->pSchema->auditInfo.style));
		tbl.AddRow(row);

		row.ClearCells();
		row.AddCell("Audit Enabled");
		row.AddCell((this->pSchema->auditInfo.enable==0?"No":"Yes"));
		tbl.AddRow(row);

#if AR_CURRENT_API_VERSION > 13 // Version 7.5 and higher
		row.ClearCells();
		row.AddCell("Audit Only Changed Fields");
		row.AddCell(CAREnum::AuditChangedFields(this->pSchema->auditInfo.auditMask));
		tbl.AddRow(row);
#endif

		row.ClearCells();
		switch (this->pSchema->auditInfo.style)
		{
			case AR_AUDIT_NONE:
				{
					if (this->pSchema->auditInfo.formName[0] != NULL)
					{
						row.AddCell("Audited From Form");
						row.AddCell(this->pInside->LinkToSchema(this->pInside->SchemaGetInsideId(this->pSchema->auditInfo.formName),rootLevel));
						tbl.AddRow(row);
					}
				}
				break;
			case AR_AUDIT_COPY:
				{
					row.AddCell("Audit Form");
					row.AddCell(this->pInside->LinkToSchema(this->pInside->SchemaGetInsideId(this->pSchema->auditInfo.formName),rootLevel));
					tbl.AddRow(row);
				}
				break;
			case AR_AUDIT_LOG:
				{
					row.AddCell("Audit Log Form");
					row.AddCell(this->pInside->LinkToSchema(this->pInside->SchemaGetInsideId(this->pSchema->auditInfo.formName),rootLevel));
					tbl.AddRow(row);
				}
				break;
		}

		// audit qualification
		stringstream qualStrm; qualStrm.str("");
		int pFormId = this->pInside->SchemaGetInsideId(this->pSchema->name);

		CFieldRefItem refItem(AR_STRUCT_ITEM_XML_SCHEMA, this->pSchema->name, "Audit Qualification", 0, 0);

		if (this->pSchema->auditInfo.query.operation != AR_COND_OP_NONE)
		{
			CARQualification arQual(*this->pInside);
			arQual.CheckQuery(&this->pSchema->auditInfo.query, refItem, 0, pFormId, pFormId, qualStrm, rootLevel);
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
		strm << tbl.ToXHtml();
		tbl.Clear();
	}
	catch (...)
	{
		cerr << "EXCEPTION audit properties" << endl;
	}
	return strm.str();
}

string CDocSchemaDetails::ShowArchiveProperties()
{
	stringstream strm; strm.str("");

	try
	{
		bool archiveToForm = (pSchema->archiveInfo.archiveType & AR_ARCHIVE_FORM)>0;
		bool deleteSource = (pSchema->archiveInfo.archiveType & AR_ARCHIVE_DELETE)>0;
		bool archiveToFile = (pSchema->archiveInfo.archiveType & (AR_ARCHIVE_FILE_ARX | AR_ARCHIVE_FILE_XML))>0;
		unsigned int fileFormat = (pSchema->archiveInfo.archiveType & (AR_ARCHIVE_FILE_ARX | AR_ARCHIVE_FILE_XML)) << 2;
		bool noAttachments = (pSchema->archiveInfo.archiveType & AR_ARCHIVE_NO_ATTACHMENTS)>0;
		bool noDiary = (pSchema->archiveInfo.archiveType & AR_ARCHIVE_NO_DIARY)>0;

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
		row.AddCell((this->pSchema->archiveInfo.enable==0?"Disabled":"Enabled"));
		tbl.AddRow(row);		

		row.ClearCells();
		if (archiveToFile)
		{
			// archive to file is not supported by admin/developer tool; but maybe some day...
			row.AddCell("Archive to File");
			row.AddCell(pSchema->archiveInfo.u.dirPath);
			tbl.AddRow(row);
		}
		else if (archiveToForm)
		{
			stringstream tmpStrm; tmpStrm.str("");
			tmpStrm << this->pInside->LinkToSchema(this->pInside->SchemaGetInsideId(pSchema->archiveInfo.u.formName),rootLevel) << "<br/>" << endl;
			tmpStrm << CWebUtil::ChkBoxInput("noAttachments",noAttachments) << "No Attachments&nbsp;&nbsp;";
			tmpStrm << CWebUtil::ChkBoxInput("noDiary",noDiary) << "No Diary" << endl;

			row.AddCell("Archive to Form");
			row.AddCell(tmpStrm.str());
			tbl.AddRow(row);
		}

		if (pSchema->archiveInfo.archiveFrom[0] != NULL)
		{
			row.ClearCells();
			row.AddCell("Archive From Form");
			row.AddCell(this->pInside->LinkToSchema(this->pInside->SchemaGetInsideId(pSchema->archiveInfo.archiveFrom), rootLevel));
			tbl.AddRow(row);
		}

		stringstream qualStrm; qualStrm.str("");
		int pFormId = this->pInside->SchemaGetInsideId(this->pSchema->name);
		CFieldRefItem refItem(AR_STRUCT_ITEM_XML_SCHEMA, this->pSchema->name, "Archive Qualification", 0, 0);

		if (this->pSchema->archiveInfo.query.operation != AR_COND_OP_NONE)
		{
			CARQualification arQual(*this->pInside);
			arQual.CheckQuery(&this->pSchema->archiveInfo.query, refItem, 0, pFormId, pFormId, qualStrm, rootLevel);
		}
		else
		{
			qualStrm << EmptyRunIf << endl;
		}

		row.ClearCells();
		row.AddCell("Times");
		row.AddCell(CARDayStructHelper::DayStructToHTMLString(&pSchema->archiveInfo.archiveTime));
		tbl.AddRow(row);

		row.ClearCells();
		row.AddCell("Qualification");
		row.AddCell(qualStrm.str());
		tbl.AddRow(row);

		tbl.description = CWebUtil::ImageTag("doc.gif", rootLevel) + "Archive Settings:";
		strm << tbl.ToXHtml();
		tbl.Clear();
	}
	catch (...)
	{
		cerr << "EXCEPTION archive properties" << endl;
	}
	return strm.str();
}

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
#include "../output/TabControl.h"
#include "../core/ARDayStructHelper.h"
#include "DocActionOpenWindowHelper.h"
#include "../core/ARHandle.h"
#include "DocOverlayHelper.h"

#include "rapidjson/document.h"
#include "rapidjson/genericwritestream.h"
#include "rapidjson/writer.h"

using namespace rapidjson;

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

		FileSystemUtil fsUtil(this->pInside->appConfig);
		if(fsUtil.CreateSubDirectory(path)>=0)
		{
			//stringstream pgStrm;	
			CWebPage webPage(file->GetFileName(), this->schema.GetName(), rootLevel, this->pInside->appConfig);
			CDocOverlayHelper overlayHelper(schema, rootLevel);
			
			const ARCompoundSchema& compSchema = this->schema.GetCompound();
			overlayType = schema.GetOverlayType();

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << CWebUtil::LinkToSchemaIndex(rootLevel) << endl;
			contHeadStrm << MenuSeparator << this->pInside->LinkToSchemaTypeList(compSchema.schemaType, rootLevel) << endl;
			contHeadStrm << MenuSeparator << CWebUtil::ObjName(this->schema.GetName());
			contHeadStrm << CAREnum::GetOverlayTypeString(overlayType);

			if(!this->schema.GetAppRefName().empty())
				contHeadStrm << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->schema.GetAppRefName(), rootLevel);

			webPage.AddContentHead(contHeadStrm.str(), overlayHelper.PlaceOverlayLink());
			webPage.AddContent(overlayHelper.PlaceOverlaidNotice());

			//Add schema navigation menu	
			webPage.SetNavigation(this->SchemaNavigation());

			// add the javascript we need for this page to display correctly
			webPage.GetReferenceManager()
				.AddScriptReference("img/schema_page.js")
				.AddScriptReference("img/jquery.timers.js")
				.AddScriptReference("img/jquery.address.min.js");

			// now the content
			CTabControl tabControl;

			// Add general schema informations to the page
			tabControl.AddTab("General", ShowGeneralInfo());

			// Add list of all fields to the page
			tabControl.AddTab("Fields", (IsJoinViewOrVendorForm() ? this->AllFieldsSpecial() : this->AllFields()) );

			// Add list of all views to the page
			tabControl.AddTab("Views", this->ShowVuiList());

			// Add schemas properties (like Entrypoint-, Archiv- and Audit-settings) to the page
			tabControl.AddTab("Properties", "");

			// Add a section which lists workflow thats reading data from this form
			tabControl.AddTab("Workflow", this->SetFieldReferences());

			// Add table with all references to the page
			tabControl.AddTab("References", GenerateReferencesTable(compSchema));

			webPage.AddContent(tabControl.ToXHtml());

			//pgStrm << GenerateReferencesTable(compSchema);

			//// Add a section which lists workflow thats reading data from this form
			//pgStrm << CWebUtil::ImageTag("doc.gif", rootLevel) << "Workflow reading data from this form" << endl;
			//pgStrm << this->SetFieldReferences();

			////Fields
			//switch(compSchema.schemaType)
			//{
			//case AR_SCHEMA_JOIN:
			//case AR_SCHEMA_VIEW:
			//case AR_SCHEMA_VENDOR:
			//	pgStrm << this->AllFieldsSpecial();
			//	break;
			//default:	
			//	pgStrm << this->AllFields();
			//	break;
			//}

			//indexes
			//this->IndexDoc();

			//sortlist
			//this->SortListDoc();

			//resultlist link
			//this->ResultListDoc();

			//views
			//this->VuiListDoc();

			//permissions
			//this->SchemaPermissionDoc();

			//Workflow
			this->WorkflowDoc();

			//subadmins
			//this->SchemaSubadminDoc();

			//active links
			this->SchemaAlDoc();


			//filter
			this->SchemaFilterDoc();

			//escalation
			this->SchemaEscalDoc();


			//webPage.AddContent(pgStrm.str());

			//// Basics / Entry Points
			//webPage.AddContent(ShowProperties());

			//Properties
			//webPage.AddContent(CARProplistHelper::GetList(*this->pInside, this->pSchema->objPropList));

			////History
			//webPage.AddContent(this->pInside->ServerObjectHistory(&this->schema, rootLevel));

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
		//Workflow		
		uList.AddItem(CUListItem(CWebUtil::Link("Workflow", CPageParams(PAGE_SCHEMA_WORKFLOW, &this->schema), "", rootLevel)));

		//Active Links
		uList.AddItem(CUListItem(CWebUtil::Link("Active Link", CPageParams(PAGE_SCHEMA_ACTIVELINKS, &this->schema), "", rootLevel)));

		//Filters
		uList.AddItem(CUListItem(CWebUtil::Link("Filter", CPageParams(PAGE_SCHEMA_FILTERS, &this->schema), "", rootLevel)));

		//Escalations
		uList.AddItem(CUListItem(CWebUtil::Link("Escalation", CPageParams(PAGE_SCHEMA_ESCALATIONS, &this->schema), "", rootLevel)));
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

		tbl.description = GenerateFieldTableDescription(tbl);

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

void CDocSchemaDetails::AllFieldsJson(std::ostream &out)
{
	try
	{
		Document document;
		Document::AllocatorType &alloc = document.GetAllocator();
		document.SetArray();

		CARFieldList* fields = schema.GetFields();
		unsigned int fieldCount = fields->GetCount();
		bool isSpecialForm = IsJoinViewOrVendorForm();

		for (unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
		{
			CARField field(schema.GetInsideId(), 0, fieldIndex);
			CPageParams fieldLink(PAGE_DETAILS, &field);

			Value item;
			item.SetArray();

			string strName = field.GetName();
			string strDate = CUtil::DateTimeToString(field.GetTimestamp());
			string strLink = CWebUtil::DocName(fieldLink->GetFileName());

			Value name(strName.c_str(), strName.size(), alloc);
			Value modified(strDate.c_str(), strDate.size(), alloc);
			Value link(strLink.c_str(), strLink.size(), alloc);

			item.PushBack(static_cast<int>(field.GetFieldId()), alloc);
			item.PushBack(name, alloc);
			item.PushBack(field.GetDataType(), alloc);
			item.PushBack(field.GetDisplayInstances().numItems, alloc);
			item.PushBack(modified, alloc);
			item.PushBack(field.GetLastChanged(), alloc);
			item.PushBack(link, alloc);
			
			// for special forms, we need some more details for the real-field column
			if (isSpecialForm)
			{
				switch (field.GetMapping().fieldType)
				{
				case AR_FIELD_JOIN:
					{
						if(field.GetFieldId() == 1) // RequestID 1 in Joinform = ReqId1 | ReqId2
						{
							const ARCompoundSchema& compSchema = this->schema.GetCompound();
							CARSchema schemaLeft(compSchema.u.join.memberA);
							CARSchema schemaRight(compSchema.u.join.memberB);

							CARField fldLeft(schemaLeft.GetInsideId(), 1);
							CARField fldRight(schemaRight.GetInsideId(), 1);

							CPageParams schemaLeftLink(PAGE_DETAILS, &schemaLeft);
							CPageParams schemaRightLink(PAGE_DETAILS, &schemaRight);
							CPageParams fldLeftLink(PAGE_DETAILS, &fldLeft);
							CPageParams fldRightLink(PAGE_DETAILS, &fldRight);

							string leftFieldName = fldLeft.GetName();
							string leftFieldLink = CWebUtil::GetRelativeURL(rootLevel, fldLeftLink);
							string leftSchemaName = schemaLeft.GetName();
							string leftSchemaLink = CWebUtil::GetRelativeURL(rootLevel, schemaLeftLink);
							
							string rightFieldName = fldRight.GetName();
							string rightFieldLink = CWebUtil::GetRelativeURL(rootLevel, fldRightLink);
							string rightSchemaName = schemaRight.GetName();
							string rightSchemaLink = CWebUtil::GetRelativeURL(rootLevel, schemaRightLink);

							Value leftNameVal(leftFieldName.c_str(), leftFieldName.size(), alloc);
							Value leftLinkVal(leftFieldLink.c_str(), leftFieldLink.size(), alloc);
							Value leftSchemaVal(leftSchemaName.c_str(), leftSchemaName.size(), alloc);
							Value leftSchemaLinkVal(leftSchemaLink.c_str(), leftSchemaLink.size(), alloc);

							Value rightNameVal(rightFieldName.c_str(), rightFieldName.size(), alloc);
							Value rightLinkVal(rightFieldLink.c_str(), rightFieldLink.size(), alloc);
							Value rightSchemaVal(rightSchemaName.c_str(), rightSchemaName.size(), alloc);
							Value rightSchemaLinkVal(rightSchemaLink.c_str(), rightSchemaLink.size(), alloc);

							item.PushBack(leftNameVal, alloc);
							item.PushBack(leftLinkVal, alloc);
							item.PushBack(leftSchemaVal, alloc);
							item.PushBack(leftSchemaLinkVal, alloc);

							item.PushBack(rightNameVal, alloc);
							item.PushBack(rightLinkVal, alloc);
							item.PushBack(rightSchemaVal, alloc);
							item.PushBack(rightSchemaLinkVal, alloc);
						}
						else
						{
							if(field.GetMapping().u.join.realId > 0)
							{
								const ARCompoundSchema& compSchema = this->schema.GetCompound();

								string baseSchemaName = (field.GetMapping().u.join.schemaIndex == 0 ? compSchema.u.join.memberA : compSchema.u.join.memberB);
								
								CARSchema baseSchema(baseSchemaName);
								CPageParams baseSchemaPage(PAGE_DETAILS, &baseSchema);

								CARField baseField(baseSchema.GetInsideId(), field.GetMapping().u.join.realId);
								CPageParams baseFieldPage(PAGE_DETAILS, &baseField);

								string baseFieldName = baseField.GetName();
								string baseFieldLink = CWebUtil::GetRelativeURL(rootLevel, baseFieldPage);
								string baseSchemaLink = CWebUtil::GetRelativeURL(rootLevel, baseSchemaPage);
								
								Value baseFileNameVal(baseFieldName.c_str(), baseFieldName.size(), alloc);
								Value baseFileLinkVal(baseFieldLink.c_str(), baseFieldLink.size(), alloc);
								Value baseSchemaNameVal(baseSchemaName.c_str(), baseSchemaName.size(), alloc);
								Value baseSchemaLinkVal(baseSchemaLink.c_str(), baseSchemaLink.size(), alloc);

								item.PushBack(baseFileNameVal, alloc);
								item.PushBack(baseFileLinkVal, alloc);
								item.PushBack(baseSchemaNameVal, alloc);
								item.PushBack(baseSchemaLinkVal, alloc);

								//strmTmp << this->pInside->LinkToField(tmpBaseSchema, field.GetMapping().u.join.realId, rootLevel) << "&nbsp;" << MenuSeparator << "&nbsp;" << this->pInside->LinkToSchema(tmpBaseSchema, rootLevel);
							}
							//else
							//	strmTmp << "&nbsp;";
						}
					}
					break;
				case AR_FIELD_VIEW:
					{
						string val = field.GetMapping().u.view.fieldName;
						Value itemVal(val.c_str(), val.size(), alloc);
						item.PushBack(itemVal, alloc);
					}
					break;
				case AR_FIELD_VENDOR:
					{
						string val = field.GetMapping().u.vendor.fieldName;
						Value itemVal(val.c_str(), val.size(), alloc);
						item.PushBack(itemVal, alloc);
					}
					break;
				}

			}

			document.PushBack(item, alloc);
		}

		GenericWriteStream output(out);
		Writer<GenericWriteStream> writer(output);

		out << endl << "<script type=\"text/javascript\">" << endl;
		out << "var schemaFieldList = "; document.Accept(writer); out << ";";
		if (isSpecialForm)
		{
			string type;
			switch (this->schema.GetCompound().schemaType)
			{
			case AR_SCHEMA_VIEW:
				type = "view";
				break;
			case AR_SCHEMA_VENDOR:
				type = "vendor";
				break;
			case AR_SCHEMA_JOIN:
				type = "join";
				break;
			}
			if (!type.empty())
			{
				out << "schemaFieldManager.setRenderer(\"" << type << "\");";
			}
		}
		out << endl;
		out << "</script>" << endl;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema all fields json of '" << this->schema.GetName() << "': " << e.what() << endl;
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

		tbl.description = GenerateFieldTableDescription(tbl);

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

string CDocSchemaDetails::GenerateFieldTableDescription(CTable &tbl)
{
	stringstream outputStrm;
	AllFieldsJson(outputStrm);
	outputStrm << "<div><span class='clearable'><input type=\"text\" class='data_field' id=\"fieldNameFilter\"/></span><button id=\"execFieldFilter\">Filter</button></div>" << endl;
	outputStrm << CWebUtil::ImageTag("doc.gif", rootLevel) << "<span id='fieldListFilterResultCount'></span>" << tbl.NumRows() << " fields (" << CWebUtil::Link("data", CPageParams(PAGE_SCHEMA_FIELDS_CSV, &this->schema), "", rootLevel) << ")" << endl;
	outputStrm << "<div id=\"result\"></div>";
	return outputStrm.str();
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
void CDocSchemaDetails::ShowPermissionProperties(std::ostream &strm, CARProplistHelper *propIndex)
{
	try
	{
		//CPageParams file(PAGE_SCHEMA_PERMISSIONS, &this->schema);

		CTable tbl("permissionList", "TblObjectList");
		tbl.AddColumn(60, "Group Name");
		tbl.AddColumn(20, "Group Id");
		tbl.AddColumn(20, "Permission");

		const ARPermissionList& perms = this->schema.GetPermissions();
		for(unsigned int i = 0; i < perms.numItems; ++i)
		{
			if(this->pInside->ValidateGroup(this->schema.GetAppRefName(), perms.permissionList[i].groupId))
			{
				CTableRow row("");
				stringstream perm;

				if(perms.permissionList[i].permissions == AR_PERMISSIONS_HIDDEN)
					perm << CWebUtil::ImageTag("hidden.gif", rootLevel);
				else
					perm << CWebUtil::ImageTag("visible.gif", rootLevel);

				perm << CAREnum::ObjectPermission(perms.permissionList[i].permissions);

				row.AddCell(CTableCell(this->pInside->LinkToGroup(this->schema.GetAppRefName(), perms.permissionList[i].groupId, rootLevel)));
				row.AddCell(CTableCell(perms.permissionList[i].groupId));
				row.AddCell(perm.str());
				tbl.AddRow(row);
			}
		}

		//CPageParams file(PAGE_SCHEMA_SUBADMINS, &this->schema); //////////////////////////////////
		CTable tblSubAdm("subadminPerms", "TblObjectList");
		tblSubAdm.AddColumn(90, "Group Name");
		tblSubAdm.AddColumn(10, "Group Id");

		const ARInternalIdList& subAdmins = schema.GetSubadmins();
		for(unsigned int i = 0; i < subAdmins.numItems; ++i)
		{
			CTableRow row("");
			row.AddCell(this->pInside->LinkToGroup(this->schema.GetAppRefName(), subAdmins.internalIdList[i], rootLevel));
			row.AddCell(subAdmins.internalIdList[i]);
			tblSubAdm.AddRow(row);
		}

		//Show all fields with all group permissions
		CTable tblFieldPerm("fieldPerms", "TblObjectList");

		tblFieldPerm.AddColumn(40, "Field Name");
		tblFieldPerm.AddColumn(10, "Field ID");	
		tblFieldPerm.AddColumn(10, "Datatype");		
		tblFieldPerm.AddColumn(40, "Permissions");


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
				tblFieldPermissionDetails.AddColumn(50, "Group Name");
				tblFieldPermissionDetails.AddColumn(50, "Permission");
				tblFieldPermissionDetails.DisableHeader();

				for(unsigned int i = 0; i < fldPerms.numItems; ++i)
				{
					stringstream perm;
					
					if(fldPerms.permissionList[i].permissions == AR_PERMISSIONS_CHANGE)
						perm << CWebUtil::ImageTag("edit.gif", rootLevel);
					else
						perm << CWebUtil::ImageTag("visible.gif", rootLevel);

					perm << CAREnum::FieldPermission(fldPerms.permissionList[i].permissions);

					CTableRow row("");
					row.AddCell(this->pInside->LinkToGroup(schema.GetAppRefName(), fldPerms.permissionList[i].groupId, rootLevel));
					row.AddCell(perm.str());
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
			tblFieldPerm.AddRow(row);
		}

		strm << "<h2>" << CWebUtil::ImageTag("doc.gif", rootLevel) << "Permissions" << "</h2>";
		strm << "<div id='schemaPermissions'>" 
		         << "<h2>" << "Group Permissions" << "</h2>" << "<div>" << tbl << "</div>"
             << "<h2>" << "Subadministrator Permissions" << "</h2>" << "<div>" << tblSubAdm << "</div>"
             << "<h2>" << "Field Permissions" << "</h2>" << "<div>" << tblFieldPerm << "</div>"
		     << "</div>";
	}
	catch(exception& e)
	{
		cout << "EXCEPTION schema permission doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//Create a page that shows a list of all indexes
void CDocSchemaDetails::ShowIndexProperties(std::ostream &strm, CARProplistHelper *propIndex)
{
	try
	{
		//CPageParams file(PAGE_SCHEMA_INDEXES, &this->schema);

		strm << "<h2>" << CWebUtil::ImageTag("doc.gif", rootLevel) << "Indexes" << "</h2>";
		strm << "<div>";

		const ARIndexList& indexList = this->schema.GetIndexList();
		for(unsigned int nIndex=0; nIndex < indexList.numItems; ++nIndex)
		{		
			CTable tbl("indexTbl", "TblObjectList");
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

			strm << tbl;
		}

		strm << "</div>";
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema index doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//Create a page that shows the form resultlist
void CDocSchemaDetails::ShowResultListProperties(std::ostream &strm, CARProplistHelper *propIndex)
{
	try
	{
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

		strm << "<h2>" << CWebUtil::ImageTag("doc.gif", rootLevel) << "Result List Fields" << "</h2>";
		strm << "<div>" << tbl << "</div>";
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema resultlist doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//Create a page that shows the page sortslist
void CDocSchemaDetails::ShowSortListProperties(std::ostream &strm, CARProplistHelper *propIndex)
{
	try
	{
		//CPageParams file(PAGE_SCHEMA_SORTLIST, &this->schema);

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

		strm << "<h2>" << CWebUtil::ImageTag("doc.gif", rootLevel) << "Sort" << "</h2>";
		strm << "<div>" << tbl << "</div>";
	}
	catch(exception& e)
	{
		cout << "EXCEPTION creating schema sortlist doc of '" << this->schema.GetName() << "': " << e.what() << endl;
	}
}

//Create a page with a list of all views
string CDocSchemaDetails::ShowVuiList()
{	
	try
	{
		//CPageParams file(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_VUI, &this->schema);

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

		stringstream strm;
		strm << tbl;
		return strm.str();
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

		const CARSchemaList::ObjectRefList& fltList = this->schema.GetFilters();
		CARSchemaList::ObjectRefList::const_iterator curIt = fltList.begin();
		CARSchemaList::ObjectRefList::const_iterator endIt = fltList.end();

		for (; curIt != endIt; ++curIt)
		{
			CARFilter filter(*curIt);
			tbl->AddRow(*curIt, rootLevel);
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

		const CARSchemaList::ObjectRefList& alList = this->schema.GetActiveLinks();
		CARSchemaList::ObjectRefList::const_iterator curIt = alList.begin();
		CARSchemaList::ObjectRefList::const_iterator endIt = alList.end();

		for (; curIt != endIt; ++curIt)
		{
			CARActiveLink enumObj(*curIt);
			tbl->AddRow(*curIt, rootLevel);
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

		const CARSchemaList::ObjectRefList& escList = this->schema.GetEscalations();
		CARSchemaList::ObjectRefList::const_iterator curIt = escList.begin();
		CARSchemaList::ObjectRefList::const_iterator endIt = escList.end();

		for (; curIt != endIt; ++curIt)
		{
			CAREscalation escalation(*curIt);
			tbl->AddRow(*curIt, rootLevel);
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
				strm << "(" << this->pInside->LinkToSchema(compSchema.u.join.memberA, rootLevel) << " <-> " << this->pInside->LinkToSchema(compSchema.u.join.memberB, rootLevel) << ")" << "<br/>";

				if(compSchema.u.join.joinQual.operation != AR_COND_OP_NONE)
				{
					stringstream strmQuery;
					CRefItem refItem(this->schema, REFM_SCHEMA_JOIN_QUALIFICATION);
					CARQualification arQual(*this->pInside);

					int pFormId = this->pInside->SchemaGetInsideId(compSchema.u.join.memberA);
					int sFormId = this->pInside->SchemaGetInsideId(compSchema.u.join.memberB);

					arQual.CheckQuery(&compSchema.u.join.joinQual, refItem, 0, pFormId, sFormId, strmQuery, rootLevel);

					strm << "Qualification: " << strmQuery.str();
				}
			}
			break;
		case AR_SCHEMA_VIEW:
			{
				strm << "<span class='additionalInfo'>" << "(" << "Table&nbsp;Name: " << compSchema.u.view.tableName << " &nbsp;&nbsp;&nbsp; "
				     << "Key&nbsp;Field: " << compSchema.u.view.keyField << ")" << "</span>";
			}
			break;			
		case AR_SCHEMA_VENDOR:
			{
				strm << "<span class='additionalInfo'>" << "(" << "Plugin:&nbsp;" << compSchema.u.vendor.vendorName << " &nbsp;&nbsp;&nbsp; ";
				strm << "Table: " << compSchema.u.vendor.tableName << ")" << "</span>";
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

			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(schema))
				continue;

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
			
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(searchSchema))
				continue;

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

		const CARSchema::ReferenceList &refList = schema.GetReferences();
		CARSchema::ReferenceList::const_iterator curIt = refList.begin();
		CARSchema::ReferenceList::const_iterator endIt = refList.end();

		for (; curIt != endIt; ++curIt)
		{
			if (curIt->GetMessageId() == REFM_CHARMENU_FORM && curIt->GetObjectType() == AR_STRUCT_ITEM_XML_CHAR_MENU)
			{
				CARHandle<CARCharMenu> hMenu(*curIt);
				menuTable->AddRow(*hMenu, rootLevel);
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

			// skip this object in case it's overlaid (hidden)
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(al))
				continue;

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

			// skip this object in case it's overlaid (hidden)
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(al))
				continue;

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

			// skip this object in case it's overlaid (hidden)
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(filter))
				continue;

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

string CDocSchemaDetails::EscalationPushFieldsReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		CEscalTable *escalTable = new CEscalTable(*this->pInside);

		unsigned int escalCount = pInside->escalationList.GetCount();
		for (unsigned int escalIndex = 0; escalIndex < escalCount; ++escalIndex )
		{
			CAREscalation escalation(escalIndex);

			// skip this object in case it's overlaid (hidden)
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(escalation))
				continue;

			bool bPushToForm = false;

			//If-Actions
			for(unsigned int i = 0; i < escalation.GetIfActions().numItems; ++i)
			{
				if(escalation.GetIfActions().actionList[i].action == AR_FILTER_ACTION_FIELDP)
				{
					ARPushFieldsActionStruct &action = escalation.GetIfActions().actionList[i].u.pushFields;
					if (action.pushFieldsList.numItems > 0)
					{
						char *pushSchema;
						if (action.pushFieldsList.pushFieldsList[0].field.schema[0] == '$')
							pushSchema = action.sampleSchema;
						else
							pushSchema = action.pushFieldsList.pushFieldsList[0].field.schema;

						if ((pushSchema[0] == '@' && IsSchemaInWFConnectStruct(escalation.GetSchemaList())) || strcmp(schema.GetARName(), pushSchema) == 0)
						{
							bPushToForm = true;
							break;
						}
					}
				}
			}	

			//Else Actions
			if(bPushToForm == false) // Only search the else actions if the escalation is still false
			{
				for(unsigned int i = 0; i < escalation.GetElseActions().numItems; ++i)
				{
					if(escalation.GetElseActions().actionList[i].action == AR_FILTER_ACTION_FIELDP)
					{
						ARPushFieldsActionStruct &action = escalation.GetElseActions().actionList[i].u.pushFields;

						if (action.pushFieldsList.numItems > 0)
						{
							char *pushSchema;
							if (action.pushFieldsList.pushFieldsList[0].field.schema[0] == '$')
								pushSchema = action.sampleSchema;
							else
								pushSchema = action.pushFieldsList.pushFieldsList[0].field.schema;

							if ((pushSchema[0] == '@' && IsSchemaInWFConnectStruct(escalation.GetSchemaList())) || strcmp(schema.GetARName(), pushSchema) == 0)
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
				escalTable->AddRow(escalIndex, rootLevel);
			}
		}

		if(escalTable->NumRows() > 0)
			strm << *escalTable;
		else
			strm << EmptyValue;

		delete escalTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating escalation push fields references in schema '" << this->schema.GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

string CDocSchemaDetails::ShowProperties()
{
	stringstream strm;
	strm.str("");
	map<ARLong32,ARValueStruct*> propIdx;
	int schemaType = this->schema.GetCompound().schemaType;
	
	try
	{
		const ARPropList& propList = this->schema.GetPropList();
		CARProplistHelper propIdx(&propList);

		strm << "<div id='schemaProperties'>";

		// doc basic properties
		ShowBasicProperties(strm, &propIdx);
		ShowEntryPointProperties(strm, &propIdx);

		if (schemaType != AR_SCHEMA_DIALOG)
		{
			ShowResultListProperties(strm, &propIdx);
			ShowSortListProperties(strm, &propIdx);
			ShowArchiveProperties(strm);

			if (pInside->CompareServerVersion(7,0) >= 0) 
			{
				ShowAuditProperties(strm);
			}
			
			if (schemaType != AR_SCHEMA_VENDOR && schemaType != AR_SCHEMA_VIEW)
				ShowIndexProperties(strm, &propIdx);

			ShowFTSMTSProperties(strm, &propIdx);
		}

		ShowPermissionProperties(strm, &propIdx);

		propIdx.UnusedPropertiesToHTML(strm);

		strm << "</div>";
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
		stringstream tmpStrm;
		CTable tbl("basicProps", "TblNoBorder");
		tbl.AddColumn(20, "Description");
		tbl.AddColumn(80, "Value");
		tbl.DisableHeader();

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

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_710
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

		/////////////////////////////////////////////////////////////////////////////////////

		ARLong32 disableStatusHistory = 0;
		propVal = propIndex->GetAndUseValue(AR_OPROP_CORE_FIELDS_OPTION_MASK);
		if (propVal != NULL)
			if (propVal->dataType == AR_DATA_TYPE_INTEGER)
				disableStatusHistory = propVal->u.intVal;

		CTableRow disSH("");
		disSH.AddCell("Disable Status History");
		disSH.AddCell((disableStatusHistory & AR_CORE_FIELDS_OPTION_DISABLE_STATUS_HISTORY ? "Yes" : "No"));
		tbl.AddRow(disSH);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
		ARLong32 allowDelete = 0;
		propVal = propIndex->GetAndUseValue(AR_OPROP_FORM_ALLOW_DELETE);
		if (propVal != NULL)
			if (propVal->dataType == AR_DATA_TYPE_INTEGER)
				allowDelete = propVal->u.intVal;

		CTableRow row("");
		row.AddCell("Allow Delete");
		row.AddCell((allowDelete == 1 ? "Yes" : "No"));
		tbl.AddRow(row);

		/////////////////////////////////////////////////////////////////////////////////////

		ARLong32 allowDrillDownWebReports = 0;
		propVal = propIndex->GetAndUseValue(AR_OPROP_FORM_ALLOW_DELETE);
		if (propVal != NULL)
			if (propVal->dataType == AR_DATA_TYPE_INTEGER)
				allowDrillDownWebReports = propVal->u.intVal;

		row.ClearCells();
		row.AddCell("Allow Drill Down in Web Reports");
		row.AddCell((allowDrillDownWebReports == 1 ? "Yes" : "No"));
		tbl.AddRow(row);

		/////////////////////////////////////////////////////////////////////////////////////

		ARLong32 value = 0;
		propVal = propIndex->GetAndUseValue(AR_OPROP_LOCALIZE_FORM_VIEWS);
		if (propVal != NULL && propVal->dataType == AR_DATA_TYPE_INTEGER)
		{
			value = propVal->u.intVal;
		}
		else
			value = AR_LOCALIZE_FORM_VIEWS_ALL;	// default, if property isn't present

		char* strState = "Disabled";
		if (value == AR_LOCALIZE_FORM_VIEWS_ALL) strState = "All";
		if (value == AR_LOCALIZE_FORM_VIEWS_ALIASES) strState = "Only for selection field aliases";

		row.ClearCells();
		row.AddCell(CAREnum::FieldPropertiesLabel(AR_OPROP_LOCALIZE_FORM_VIEWS));
		row.AddCell(strState);
		tbl.AddRow(row);

		/////////////////////////////////////////////////////////////////////////////////////

		value = 0;
		propVal = propIndex->GetAndUseValue(AR_OPROP_LOCALIZE_FORM_DATA);
		if (propVal != NULL && propVal->dataType == AR_DATA_TYPE_INTEGER)
		{
			value = propVal->u.intVal;
		}
		else
			value = AR_LOCALIZE_FORM_DATA_ALL; // default, if property isn't present

		row.ClearCells();
		row.AddCell(CAREnum::FieldPropertiesLabel(AR_OPROP_LOCALIZE_FORM_DATA));
		row.AddCell((value == 1 ? "Yes" : "No"));
		tbl.AddRow(row);

#endif //AR_CURRENT_API_VERSION >= AR_API_VERSION_763
#endif //AR_CURRENT_API_VERSION >= AR_API_VERSION_710

		// now write the table
		strm << "<h2>" << CWebUtil::ImageTag("doc.gif", rootLevel) << "Basic" << "</h2>";
		strm << "<div>" << tbl << "</div>";
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating basic properties: " << e.what() << endl;
	}
}

void CDocSchemaDetails::ShowEntryPointProperties(std::ostream &strm, CARProplistHelper *propIndex)
{
	ARValueStruct* propVal;

	try
	{
		stringstream tmpStrm;
		CTable tbl("entryPoints", "TblNoBorder");
		tbl.AddColumn(20, "Description");
		tbl.AddColumn(80, "Value");
		tbl.DisableHeader();

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


		tmpStrm << CWebUtil::ChkBoxInput("" ,(entryPointNewOrder >= 0)) << "Enable Entry Point" << "<br/>";
		tmpStrm << "Application List Display Order: "; if (entryPointNewOrder >=0) tmpStrm << entryPointNewOrder; tmpStrm << "<br/>" << "<br/>";

		CTableRow row;
		row.AddCell("New Mode");
		row.AddCell(tmpStrm.str());
		tbl.AddRow(row);
		
		tmpStrm.str("");
		tmpStrm << CWebUtil::ChkBoxInput("", (entryPointSearchOrder >= 0)) << "Enable Entry Point" << "<br/>";
		tmpStrm << "Application List Display Order: "; if (entryPointSearchOrder >= 0) tmpStrm << entryPointSearchOrder; tmpStrm << endl;

		row.ClearCells();
		row.AddCell("Search Mode");
		row.AddCell(tmpStrm.str());
		tbl.AddRow(row);
		
		// now write the table
		strm << "<h2>" << CWebUtil::ImageTag("doc.gif", rootLevel) << "EntryPoints" << "</h2>";
		strm << "<div>" << tbl << "</div>";
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

		strm << "<h2>" << CWebUtil::ImageTag("doc.gif", rootLevel) + "Audit Settings:" << "</h2>";
		strm << "<div>" << tbl << "</div>";
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

		strm << "<h2>" << CWebUtil::ImageTag("doc.gif", rootLevel) + "Archive Settings:" << "</h2>";
		strm << "<div>" << tbl << "</div>";
	}
	catch (...)
	{
		cerr << "EXCEPTION archive properties" << endl;
	}
}

string CDocSchemaDetails::SetFieldReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		CTable tab("setFieldReferences", "TblObjectList");
		tab.AddColumn(50, "Object Name");
		tab.AddColumn(10, "Enabled");
		tab.AddColumn(20, "Execute On");
		tab.AddColumn(20, "Description");

		const CARSchema::ReferenceList &refList = schema.GetReferences();
		CARSchema::ReferenceList::const_iterator curIt = refList.begin();
		CARSchema::ReferenceList::const_iterator endIt = refList.end();

		for (; curIt != endIt; ++curIt)
		{
			if (curIt->GetMessageId() == REFM_SETFIELDS_FORM)
			{
				bool hasEnabledFlag;
				string tmpEnabled = "";
				string tmpCssEnabled = "";

				unsigned int enabled = curIt->GetObjectEnabled(hasEnabledFlag);

				if (hasEnabledFlag)
				{
					tmpEnabled = CAREnum::ObjectEnable(enabled);
					if (!enabled) { tmpCssEnabled = "objStatusDisabled"; }
				}

				CTableRow row;
				row.AddCell(pInside->LinkToObjByRefItem(*curIt, rootLevel));
				row.AddCell(CTableCell(tmpEnabled, tmpCssEnabled)); // Enabled?
				row.AddCell(curIt->GetObjectExecuteOn()); // Exceute On
				row.AddCell(curIt->GetDescription(rootLevel));
				tab.AddRow(row);
			}
		}

		strm << CWebUtil::ImageTag("doc.gif", rootLevel) << "Workflow reading data from this form" << endl;
		tab.ToXHtml(strm);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating filter push fields references in schema '" << this->schema.GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

string CDocSchemaDetails::GenerateReferencesTable(const ARCompoundSchema &compSchema)
{
	//Schema Properties
	stringstream strmTmp;
	CTable tblObjProp("objProperties", "TblObjectList");
	tblObjProp.AddColumn(0, "Property");
	tblObjProp.AddColumn(0, "Value");

	//Schema type informations
	CTableRow row("cssStdRow");	
	CTableCell cellProp("");				
	CTableCell cellPropValue("");    

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

	//Escalation PushFIelds References
	row.ClearCells();
	cellProp.content = "Escalation \"Push Field Action\" to this form";
	cellPropValue.content = this->EscalationPushFieldsReferences();
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

	return tblObjProp.ToXHtml();
}

bool CDocSchemaDetails::IsJoinViewOrVendorForm()
{
	switch(this->schema.GetCompound().schemaType)
	{
	case AR_SCHEMA_JOIN:
	case AR_SCHEMA_VIEW:
	case AR_SCHEMA_VENDOR:
		return true;
	default:	
		return false;
	}
}

string CDocSchemaDetails::ShowGeneralInfo()
{
	const ARCompoundSchema &compSchema = this->schema.GetCompound();
	stringstream strm;

	CTable tbl("general", "TblNoBorder");
	tbl.AddColumn(20, "");
	tbl.AddColumn(80, "");
	tbl.DisableHeader();

	CTableRow row;
	row.AddCell("Name");
	row.AddCell(this->schema.GetName());
	tbl.AddRow(row);	
	row.ClearCells();

	// generate the schema type row (and with additional details for special form types like join, view and vendor)
	if (compSchema.schemaType == AR_SCHEMA_JOIN)
	{
		strm << CAREnum::JoinType(compSchema.u.join.option) << "-";
	}
	strm << CAREnum::SchemaType(compSchema.schemaType);

	if(compSchema.schemaType == AR_SCHEMA_JOIN
		|| compSchema.schemaType == AR_SCHEMA_VIEW
		|| compSchema.schemaType == AR_SCHEMA_VENDOR)
	{
		strm << " " << this->TypeDetails();
	}

	row.AddCell("Type");
	row.AddCell(strm.str());
	tbl.AddRow(row);
	row.ClearCells();
	strm.str("");

	// search for the default vui
	CARVui defaultVUI(this->schema.GetInsideId(), this->schema.GetDefaultVUI());

	row.AddCell("Default View");
	row.AddCell((defaultVUI.Exists() ? defaultVUI.GetURL(rootLevel) : this->schema.GetDefaultVUI()));
	tbl.AddRow(row);
	row.ClearCells();

	strm << tbl.ToXHtml();
	tbl.ClearRows();

	if (pInside->CompareServerVersion(7,1) >= 0)
	{
		stringstream numberConverter;
		if (schema.GetDbTableId() > 0)
			numberConverter << schema.GetDbTableId();

		row.AddCell("DB Table ID");
		row.AddCell(numberConverter.str());
		tbl.AddRow(row);
		row.ClearCells();

		row.AddCell("DB Table View");
		row.AddCell(schema.GetDbViewName());
		tbl.AddRow(row);
		row.ClearCells();

		row.AddCell("DB Table SH Views");
		row.AddCell(schema.GetDbShViewName());
		tbl.AddRow(row);
		row.ClearCells();

		strm << tbl.ToXHtml();
	}

	strm << "<hr/>" << endl;

	strm << ShowProperties();

	strm << "<hr/>" << endl;

	strm << this->pInside->ServerObjectHistory(&this->schema, rootLevel);

	return strm.str();
}

void CDocSchemaDetails::ShowFTSMTSProperties(std::ostream& strm, CARProplistHelper *propIndex)
{
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	if (pInside->CompareServerVersion(7,6,3) < 0)
		return;

	ARValueStruct* propVal;
	int iValue;

	try
	{
		stringstream tmpStrm;
		CTable tbl("ftsProps", "TblNoBorder");
		tbl.AddColumn(20, "Description");
		tbl.AddColumn(80, "Value");
		tbl.DisableHeader();

		///////////////////////////////////////////////////////////////////////////////////////

		CTableRow	row("");
		iValue = 0;
		propVal = propIndex->GetAndUseValue(AR_OPROP_MFS_OPTION_MASK);
		if (propVal != NULL && propVal->dataType == AR_DATA_TYPE_INTEGER)
			iValue = propVal->u.intVal;
		
		row.AddCell("Exclude from multi-form search");
		row.AddCell((iValue & AR_MULTI_FORM_SEARCH_OPTION_EXCLUDE ? "Yes" : "No"));		
		tbl.AddRow(row);

		///////////////////////////////////////////////////////////////////////////////////////

		row.ClearCells();
		char* strValue = NULL;
		vector<int> weightedRelevancyFields; weightedRelevancyFields.resize(4);

		propVal = propIndex->GetAndUseValue(AR_OPROP_MFS_WEIGHTED_RELEVANCY_FIELDS);
		if (propVal != NULL && propVal->dataType == AR_DATA_TYPE_CHAR)
			strValue = propVal->u.charVal;


		if (strValue != NULL && strValue[0] != 0)
		{
			int numFields = atoi(strValue);
			char* newPos = strchr(strValue, ';');
			if (newPos)
			{
				strValue = newPos + 1;
				for (int fieldPos = 0; fieldPos < numFields; fieldPos++)
				{
					int fieldTag = atoi(strValue);
					
					newPos = strchr(strValue, ';');
					if (newPos)
					{
						strValue = newPos + 1;
						int fieldId = atoi(strValue);
						
						if (fieldTag > 0 && fieldTag <= AR_MFS_WEIGHTED_RELEVANCY_KEYWORDS_FIELD_TAG)
						{
							weightedRelevancyFields[fieldTag-1] = fieldId;
						}
					}
				}
			}
		}

		CTable weightedRelFields("WRFields", "TblNoBorder");
		weightedRelFields.AddColumn(0, "");
		weightedRelFields.AddColumn(80, "");
		weightedRelFields.DisableHeader();

		for (unsigned int wrfPos = 0; wrfPos < AR_MFS_WEIGHTED_RELEVANCY_KEYWORDS_FIELD_TAG; wrfPos++)
		{
			int fieldId = weightedRelevancyFields[wrfPos];

			CTableRow wrfRow("");
			wrfRow.AddCell(CAREnum::WeightedRelevancyFieldType(wrfPos+1));
			wrfRow.AddCell((fieldId > 0 ? pInside->LinkToField(this->schema.GetInsideId(), fieldId, rootLevel) : ""));
			weightedRelFields.AddRow(wrfRow);

			if (fieldId > 0)
			{
				CRefItem ref(schema, REFM_SCHEMA_FTS_WEIGHTED_RELEVANCY_FIELD);
				pInside->AddFieldReference(schema.GetInsideId(), fieldId, ref);
			}
		}

		row.AddCell("Weighted relevancy fields");
		row.AddCell(weightedRelFields.ToXHtml());
		tbl.AddRow(row);

		///////////////////////////////////////////////////////////////////////////////////////

		row.ClearCells();
		ARDayStruct timeValue;
		unsigned int intervalValue = 0;

		ARZeroMemory(&timeValue);

		propVal = propIndex->GetAndUseValue(AR_OPROP_FT_SCAN_TIME_MONTH_MASK);
		if (propVal != NULL && propVal->dataType == AR_DATA_TYPE_INTEGER)
			timeValue.monthday = propVal->u.intVal;

		propVal = propIndex->GetAndUseValue(AR_OPROP_FT_SCAN_TIME_WEEKDAY_MASK);
		if (propVal != NULL && propVal->dataType == AR_DATA_TYPE_INTEGER)
			timeValue.weekday = propVal->u.intVal;

		propVal = propIndex->GetAndUseValue(AR_OPROP_FT_SCAN_TIME_HOUR_MASK);
		if (propVal != NULL && propVal->dataType == AR_DATA_TYPE_INTEGER)
			timeValue.hourmask = propVal->u.intVal;
		
		propVal = propIndex->GetAndUseValue(AR_OPROP_FT_SCAN_TIME_MINUTE);
		if (propVal != NULL && propVal->dataType == AR_DATA_TYPE_INTEGER)
			timeValue.minute = propVal->u.intVal;

		propVal = propIndex->GetAndUseValue(AR_OPROP_FT_SCAN_TIME_INTERVAL);
		if (propVal != NULL && (propVal->dataType == AR_DATA_TYPE_INTEGER || propVal->dataType == AR_DATA_TYPE_ULONG))
			intervalValue = propVal->u.ulongVal;
		
		row.AddCell("FT-indexed field updates");
		if (timeValue.monthday != 0 || timeValue.weekday != 0 || timeValue.hourmask != 0 || timeValue.minute != 0)
		{
			row.AddCell(CARDayStructHelper::DayStructToHTMLString(&timeValue));
		}
		else if (intervalValue != 0)
		{
			unsigned int days, hours, minutes, seconds;
			CARDayStructHelper::SplitInterval(intervalValue, days, hours, minutes, seconds);

			tmpStrm.str("");
			tmpStrm << days << " Days " << hours << " Hours " << minutes << " Minutes";
			row.AddCell(tmpStrm.str());
		}
		else
		{
			row.AddCell("No Time specified");
		}
		tbl.AddRow(row);

		///////////////////////////////////////////////////////////////////////////////////////

		strm << "<h2>" << CWebUtil::ImageTag("doc.gif", rootLevel) + "Full Text Search:" << "</h2>";
		strm << "<div>" << tbl << "</div>";
	}
	catch (exception &e)
	{
		cerr << "EXCEPTION in ShowFTSMTSProperties: " << e.what() << endl;
	}
#endif
}

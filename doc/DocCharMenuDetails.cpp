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
#include "DocCharMenuDetails.h"
#include "../output/ContainerTable.h"

const char* MenuDefinitionText = "Menu Definition";

CDocCharMenuDetails::CDocCharMenuDetails(unsigned int menuInsideId)
: menu(menuInsideId)
{
	CPageParams file(PAGE_DETAILS, &menu);

	this->path = file->GetPath();
	this->rootLevel = file->GetRootLevel();
}

CDocCharMenuDetails::~CDocCharMenuDetails(void)
{
}

void CDocCharMenuDetails::Documentation()
{
	CPageParams file(PAGE_DETAILS, &menu);

	try
	{
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			CWebPage webPage(file->GetFileName(), this->menu.GetName(), this->rootLevel, this->pInside->appConfig);

			const ARCharMenuStruct& menuDef = menu.GetDefinition();
			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");
			int overlayType = this->menu.GetOverlayType();

			strmHead << CWebUtil::LinkToMenuIndex(this->rootLevel) << MenuSeparator << CWebUtil::ObjName(this->menu.GetName()) 
				<< " (" << CAREnum::MenuType(menuDef.menuType) << ")" << CAREnum::GetOverlayTypeString(overlayType);

			CARCharMenu overlayObj;

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport && overlayType > 0)
			{
				string correspondingName;
				switch (overlayType)
				{
				case AR_OVERLAID_OBJECT:
					correspondingName = this->menu.GetOverlayName();
					break;
				case AR_OVERLAY_OBJECT:
					correspondingName = this->menu.GetOverlayBaseName();
					break;
				}

				CARCharMenu correspondingObject(correspondingName);
				overlayObj = correspondingObject;	
			}
#endif

			if(!this->menu.GetAppRefName().empty())
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->menu.GetAppRefName(), this->rootLevel);

			webPage.AddContentHead(strmHead.str(), PlaceOverlayLink(overlayType, overlayObj));

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->overlayMode == 1 && overlayType == AR_OVERLAID_OBJECT)
				webPage.AddContent(pInside->PlaceOverlaidNotice(overlayObj, rootLevel));
#endif

			//ActiveLink Properties
			stringstream strmTmp;
			CTable tblObjProp("objProperties", "TblObjectList");
			tblObjProp.AddColumn(30, "Property");	
			tblObjProp.AddColumn(70, "Value");

			//Status
			CTableRow row("cssStdRow");		
			CTableCell cellProp("Refresh", "");				
			CTableCell cellPropValue(CAREnum::MenuRefresh(this->menu.GetRefreshCode()), "");
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//Menuspecific
			switch (menuDef.menuType)
			{		
			case AR_CHAR_MENU_LIST: 
				CharMenuDetails(tblObjProp);
				break;
			case AR_CHAR_MENU_QUERY:
				SearchMenuDetails(tblObjProp);
				break;
			case AR_CHAR_MENU_FILE:
				FileMenuDetails(tblObjProp);
				break;
			case AR_CHAR_MENU_SQL:
				SqlMenuDetails(tblObjProp);
				break;
			case AR_CHAR_MENU_DATA_DICTIONARY:
				DataDictMenuDetails(tblObjProp); 
				break;
			default:
				row.ClearCells();
				row.AddCell(MenuDefinitionText);
				row.AddCell(EnumDefault);
				tblObjProp.AddRow(row);	
				break;
			}		

			//RelatedFields
			row.ClearCells();
			cellProp.content = "Related Fields";
			cellPropValue.content = this->RelatedFields();
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//RelatedActiveLinks
			row.ClearCells();
			cellProp.content = "Related ActiveLinks (Change Field)";
			cellPropValue.content = this->RelatedActiveLinks();
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//used in container?
			row.ClearCells();
			cellProp.content = "Container References";
			cellPropValue.content = this->ContainerReferences();				
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);

			//Add table to page
			webPage.AddContent(tblObjProp.ToXHtml());
			tblObjProp.Clear();

			//Properties
			webPage.AddContent(CARProplistHelper::GetList(this->menu.GetPropList()));

			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(&menu, this->rootLevel));

			webPage.SaveInFolder(this->path);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION Menu doc of '" << this->menu.GetName() << "': " << e.what() << endl;
	}
}

void CDocCharMenuDetails::CharMenuDetails(CTable& table)
{
	try
	{
		CTable tbl("menuItems", "TblObjectList");
		tbl.AddColumn(20, "Type");	
		tbl.AddColumn(40, "Label");
		tbl.AddColumn(40, "Value");

		const ARCharMenuList& menu = this->menu.GetDefinition().u.menuList;
		for(unsigned int i=0; i< menu.numItems; i++)
		{
			CTableRow row("cssStdRow");		
			CTableCell cellItemType(CAREnum::MenuItemType(menu.charMenuList[i].menuType), "");				
			CTableCell cellItemLabel(menu.charMenuList[i].menuLabel, "");

			string mValue = "";
			if(menu.charMenuList[i].menuType == AR_MENU_TYPE_VALUE)
				mValue = menu.charMenuList[i].u.menuValue;
			CTableCell cellItemValue(mValue, "");

			row.AddCell(cellItemType);
			row.AddCell(cellItemLabel);
			row.AddCell(cellItemValue);
			tbl.AddRow(row);		
		}
		
		CTableRow row;
		row.AddCell(MenuDefinitionText);
		row.AddCell(tbl.ToXHtml());
		table.AddRow(row);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION Menu details doc of '" << this->menu.GetName() << "': " << e.what() << endl;
	}
}

void CDocCharMenuDetails::FileMenuDetails(CTable& table)
{
	try
	{
		stringstream strm;

		const ARCharMenuFileStruct& menu = this->menu.GetDefinition().u.menuFile;
		strm << "File Name: " << menu.filename << "<br/>";
		strm << "Location: " << CAREnum::MenuFileLocation(menu.fileLocation) << endl;

		CTableRow row;
		row.AddCell(MenuDefinitionText);
		row.AddCell(strm.str());
		table.AddRow(row);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in FieldMenuDetails: " << e.what() << endl; 
	}
}

void CDocCharMenuDetails::SqlMenuDetails(CTable& table)
{

	try
	{
		vector<int> attachedToSchemaList;
		BuildUniqueSchemaList(attachedToSchemaList);

		// if the menu isnt attached to any form, add a dummy to the list, so the definition is generated at least once
		if (attachedToSchemaList.empty()) attachedToSchemaList.push_back(-1);

		vector<int>::iterator curIt = attachedToSchemaList.begin();
		vector<int>::iterator endIt = attachedToSchemaList.end();

		for (; curIt != endIt; ++curIt)
		{
			CARSchema schema(*curIt);

			stringstream strm;
			const ARCharMenuSQLStruct& menu = this->menu.GetDefinition().u.menuSQL;

			// in case a field in placed within the SQL command
			CRefItem refItem(this->menu, REFM_CHARMENU_SQL);

			strm << "Server: " << this->pInside->LinkToServerInfo(menu.server, rootLevel) << "<br/>" << endl;
			strm << "Label Index List: " << GetSQLLabelList(&menu) << "<br/>" << endl;
			strm << "Value Index: " << menu.valueIndex << "<br/><br/>" << endl;
			strm << "SQL Command: " << pInside->TextFindFields(menu.sqlCommand, "$", *curIt, rootLevel, true, &refItem) << endl;

			// add the table row now
			CTableRow row;
			row.AddCell( (schema.Exists() ? schema.GetURL(rootLevel) : MenuDefinitionText) );
			row.AddCell( strm.str() );
			table.AddRow(row);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in SqlMenuDetails: " << e.what() << endl; 
	}
}

string CDocCharMenuDetails::GetFieldTypes(unsigned int fieldMask)
{		
	stringstream strm;
	strm.str("");

	try
	{
		unsigned int bitmask[9] = { 1, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7, 1<<8 };
		char fieldType[9][30] = { "Data", "Trim", "Control", "Page", "Page Holder", "Table", "Column", "Attachment", "Attachment Pool" };

		for (unsigned int k= 0; k < 9; k++)
		{
			if ( (fieldMask & bitmask[k]) != 0)
			{				
				strm << fieldType[k] << "<br/>" << endl;			
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in DDCharMenu GetFieldType(): " << e.what() << endl; 
	}
	return strm.str();
}

void CDocCharMenuDetails::DataDictMenuDetails(CTable& table)
{
	try
	{
		vector<int> attachedToSchemaList;
		BuildUniqueSchemaList(attachedToSchemaList);

		// if the menu isnt attached to any form, add a dummy to the list, so the definition is generated at least once
		if (attachedToSchemaList.empty()) attachedToSchemaList.push_back(-1);

		vector<int>::iterator curIt = attachedToSchemaList.begin();
		vector<int>::iterator endIt = attachedToSchemaList.end();

		for (; curIt != endIt; ++curIt)
		{
			CARSchema schema(*curIt);

			stringstream strm;
			const ARCharMenuDDStruct& menu = this->menu.GetDefinition().u.menuDD;

			CRefItem refItemServer(this->menu, REFM_CHARMENU_SERVER);

			strm << "Server: " << ( menu.server[0] == '$' ? pInside->TextFindFields(menu.server, "$", *curIt, rootLevel, true, &refItemServer) : this->pInside->LinkToServerInfo(menu.server, rootLevel)) << "<br/>" << endl;
			strm << "Label Format: " << CAREnum::MenuDDLabelFormat(menu.nameType) << "<br/>" << endl;
			strm << "Value Format: " << CAREnum::MenuDDValueFormat(menu.valueFormat) << "<br/>" << endl;

			switch(menu.structType)
			{
			case AR_CHAR_MENU_DD_FORM:
				{
					strm << "Object Type: Form<br/>" << endl;

					string schemaType = "All";
					if(menu.u.formDefn.schemaType > 0)
						schemaType = CAREnum::SchemaType(menu.u.formDefn.schemaType);

					strm << "Form Type: " << schemaType << "<br/>" << endl;

					string showHiddenForms = "Yes";
					if(!menu.u.formDefn.includeHidden)
						showHiddenForms = "No";

					strm << "Show Hidden Forms: " << showHiddenForms << "<br/>" << endl;
				}
				break;
			case AR_CHAR_MENU_DD_FIELD:
				{
					CRefItem refItemForm(this->menu, REFM_CHARMENU_FORM);

					strm << "Object Type: Field<br/>" << endl;
					strm << "Form Name: " << (menu.u.fieldDefn.schema[0] == '$' ? pInside->TextFindFields(menu.u.fieldDefn.schema, "$", *curIt, rootLevel, true, &refItemForm) : CWebUtil::Validate(menu.u.fieldDefn.schema)) << "<br/><br/>" << endl;
					strm << "Field Type:<br/>" << GetFieldTypes(menu.u.fieldDefn.fieldType) << "<br/>" << endl;
				}
				break;
			}

			// add the table row
			CTableRow row;
			row.AddCell((schema.Exists() ? schema.GetURL(rootLevel) : MenuDefinitionText));
			row.AddCell(strm.str());
			table.AddRow(row);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in DDMenuDetails: " << e.what() << endl; 
	}
}

void CDocCharMenuDetails::SearchMenuDetails(CTable& table)
{
	try
	{
		vector<int> attachedToSchemaList;
		BuildUniqueSchemaList(attachedToSchemaList);

		// if the menu isnt attached to any form, add a dummy to the list, so the definition is generated at least once
		if (attachedToSchemaList.empty()) attachedToSchemaList.push_back(-1);

		vector<int>::iterator curIt = attachedToSchemaList.begin();
		vector<int>::iterator endIt = attachedToSchemaList.end();

		for(; curIt != endIt; ++curIt)
		{
			stringstream strm;
			CARSchema schema(*curIt);

			const ARCharMenuQueryStruct& menu = this->menu.GetDefinition().u.menuQuery;
			strm << "Server: " << this->pInside->LinkToServerInfo(menu.server, rootLevel) << "<br/>" << endl;
			strm << "Schema: " << this->pInside->LinkToSchema(menu.schema, rootLevel) << "<br/>" << endl;	

			//Label Fields
			for(int i=0; i< 5; i++)
			{
				if(menu.labelField[i] != 0)
				{
					strm << "Label Field (" << i << ") : " << this->pInside->LinkToField(menu.schema, menu.labelField[i], rootLevel);
					strm << " (FieldId: " << menu.labelField[i] << ")<br/>" << endl;

					CRefItem refItem(this->menu, REFM_CHARMENU_LABELFIELD);
					pInside->AddFieldReference(pInside->SchemaGetInsideId(menu.schema), menu.labelField[i], refItem);
				}
			}

			//Sort On Label
			string sortOnLabel = "Yes";
			if(!menu.sortOnLabel)
				sortOnLabel = "No";
			strm << "Sort On Label: " << sortOnLabel << "<br/>" << endl;


			//Value Field
			strm << "Value Field: " << this->pInside->LinkToField(menu.schema, menu.valueField, rootLevel) << "<br/>";
			CRefItem refItemValue(this->menu, REFM_CHARMENU_VALUE);
			pInside->AddFieldReference(pInside->SchemaGetInsideId(menu.schema), menu.valueField, refItemValue);

			//Query
			stringstream strmQuery;
			strmQuery.str("");

			CRefItem refItemQuery(this->menu, REFM_CHARMENU_QUALIFICATION);

			CARQualification arQual(*this->pInside);
			int pFormId = this->pInside->SchemaGetInsideId(menu.schema);
			int sFormId = this->pInside->SchemaGetInsideId(menu.schema);
			arQual.arsStructItemType = AR_STRUCT_ITEM_XML_CHAR_MENU;

			arQual.CheckQuery(&menu.qualifier, refItemQuery, 0, schema.GetInsideId(), sFormId, strmQuery, rootLevel);

			if(strmQuery.str().length() > 0)
			{
				strm << "Qualification:<br/>" << strmQuery.str() << endl;
			}
			else
			{
				strm << "Qualification: " << EmptyValue << "<br/>" << endl;
			}

			// now add the row to the table
			CTableRow row;
			row.AddCell((schema.Exists() ? schema.GetURL(rootLevel) : MenuDefinitionText));
			row.AddCell(strm.str());
			table.AddRow(row);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in SearchMenuDetails: " << e.what() << endl; 
	}
}

string CDocCharMenuDetails::RelatedFields()
{	
	CTable tbl("relatedFields", "TblObjectList");
	tbl.AddColumn(40, "Field Name");	
	tbl.AddColumn(20, "Field Id");
	tbl.AddColumn(40, "Form");

	try
	{
		const CARCharMenu::ReferenceList& refs = this->menu.GetReferences();
		CARCharMenu::ReferenceList::const_iterator curIt = refs.begin();
		CARCharMenu::ReferenceList::const_iterator endIt = refs.end();

		for (; curIt != endIt; ++curIt)
		{
			if (curIt->GetObjectType() == AR_STRUCT_ITEM_XML_FIELD && curIt->GetMessageId() == REFM_FIELD_CHARMENU)
			{
				CARField field(curIt->GetObjectId(), curIt->GetSubObjectId());
				
				CTableRow row("cssStdRow");		
				row.AddCell(field.GetURL(rootLevel));
				row.AddCell(CTableCell(field.GetInsideId()));
				row.AddCell(field.GetSchema().GetURL(rootLevel));
				tbl.AddRow(row);		
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in CharMenu related fields: " << e.what() << endl; 
	}

	return tbl.ToXHtml();
}

string CDocCharMenuDetails::RelatedActiveLinks()
{
	CTable tbl("relatedActiveLinks", "TblObjectList");
	tbl.AddColumn(100, "Active Link");

	try
	{
		const CARCharMenu::ReferenceList& refs = this->menu.GetReferences();
		CARCharMenu::ReferenceList::const_iterator curIt = refs.begin();
		CARCharMenu::ReferenceList::const_iterator endIt = refs.end();

		for (; curIt != endIt; ++curIt)
		{
			if (curIt->GetObjectType() == AR_STRUCT_ITEM_XML_ACTIVE_LINK)
			{
				CARActiveLink al(curIt->GetObjectId());

				CTableRow row("cssStdRow");

				stringstream tmp;
				tmp << curIt->IfElse() << "-Action " << curIt->ActionIndex() << " " << al.GetURL(rootLevel);

				CTableCell cellActiveLink(tmp.str(), "");						
				row.AddCell(cellActiveLink);
				tbl.AddRow(row);	
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in CharMenu related active links: " << e.what() << endl; 
	}

	return tbl.ToXHtml();
}

string CDocCharMenuDetails::ContainerReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		CContainerTable contTable(*this->pInside);
		unsigned int cntCount = this->pInside->containerList.GetCount();
		for ( unsigned int cntIndex = 0; cntIndex < cntCount; ++cntIndex )
		{
			CARContainer cont(cntIndex);

			if(cont.GetType() != ARCON_APP)
			{
				const ARReferenceList& refs = cont.GetReferences();
				for(unsigned int nCnt = 0; nCnt < refs.numItems; nCnt++)
				{
					if(refs.referenceList[nCnt].type == ARREF_CHAR_MENU)
					{
						if(refs.referenceList[nCnt].reference.u.name[0] != 0 &&
						   strcmp(this->menu.GetARName(), refs.referenceList[nCnt].reference.u.name) == 0)
						{
							contTable.AddRow(cont, rootLevel);
						}
					}
				}
			}
		}

		strm << contTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating char menu container references of '" << this->menu.GetARName() << "': " << e.what() << endl;
	}

	return strm.str();
}

string CDocCharMenuDetails::GetSQLLabelList(const ARCharMenuSQLStruct *sqlMenu)
{
	stringstream strm;
	char buffer[16];

	if (sqlMenu == NULL) return "";

	for (int k=0; k<5; ++k)
	{
		if (sqlMenu->labelIndex[k] == 0) break;
		
		if (k > 0) strm << ",";
		sprintf(buffer, "%d", sqlMenu->labelIndex[k]);
		//_itoa(sqlMenu->labelIndex[k], buffer, 16);
		strm << buffer;
	}

	return strm.str();
}

void CDocCharMenuDetails::BuildUniqueSchemaList(std::vector<int>& schemaList)
{
	const CARCharMenu::ReferenceList& refs = this->menu.GetReferences();
	CARCharMenu::ReferenceList::const_iterator curIt = refs.begin();
	CARCharMenu::ReferenceList::const_iterator endIt = refs.end();

	for (; curIt != endIt; ++curIt)
	{
		switch (curIt->GetObjectType())
		{
		case AR_STRUCT_ITEM_XML_FIELD:
			{
				// the objectId of a field reference contains the schema id ... add it
				schemaList.push_back(curIt->GetObjectId());
			}
			break;
		case AR_STRUCT_ITEM_XML_ACTIVE_LINK:
			{
				CARActiveLink al(curIt->GetObjectId());
				const ARWorkflowConnectStruct& conStruct = al.GetSchemaList();
				if (conStruct.type == AR_WORKFLOW_CONN_SCHEMA_LIST)
				{
					int conCount = conStruct.u.schemaList->numItems;
					for (int conIdx = 0; conIdx < conCount; ++conIdx)
					{
						CARSchema schema(conStruct.u.schemaList->nameList[conIdx]);
						
						if (schema.Exists())
						{
							// each schema the referenced active link is attached to, is a candidate... add it
							schemaList.push_back(schema.GetInsideId());
						}
					}
				}
			}
			break;
		}
	}

	SortAndRemoveDuplicates(schemaList);
}

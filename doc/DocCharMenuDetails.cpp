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

#include "StdAfx.h"
#include ".\doccharmenudetails.h"

CDocCharMenuDetails::CDocCharMenuDetails(CARInside &arInside, CARCharMenu &arCharMenu, string path, int rootLevel)
{
	this->pInside = &arInside;
	this->pMenu = &arCharMenu;
	this->path = path;
	this->rootLevel = rootLevel;
}

CDocCharMenuDetails::~CDocCharMenuDetails(void)
{
}

void CDocCharMenuDetails::Documentation()
{
	try
	{
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			CWebPage webPage("index", this->pMenu->name, this->rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");

			strmHead << CWebUtil::LinkToMenuIndex(this->rootLevel) + MenuSeparator + CWebUtil::ObjName(this->pMenu->name) + " (" + CAREnum::MenuType(this->pMenu->menuDefn.menuType) + ")";
			if(this->pMenu->appRefName.c_str() != NULL && this->pMenu->appRefName.size() > 0)
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->pMenu->appRefName, this->rootLevel);

			webPage.AddContentHead(strmHead.str());

			//ActiveLink Properties
			stringstream strmTmp;
			CTable tblObjProp("objProperties", "TblObjectList");
			tblObjProp.AddColumn(30, "Property");	
			tblObjProp.AddColumn(70, "Value");

			//Status
			CTableRow row("cssStdRow");		
			CTableCell cellProp("Refresh", "");				
			CTableCell cellPropValue(CAREnum::MenuRefresh(this->pMenu->refreshCode), "");    
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//Menuspecific
			row.ClearCells();
			cellProp.content = "Menu Definition";
			cellPropValue.content = "";
			switch (this->pMenu->menuDefn.menuType)
			{		
			case AR_CHAR_MENU_LIST: 
				cellPropValue.content = CharMenuDetails(); 
				break;
			case AR_CHAR_MENU_QUERY:
				cellPropValue.content = SearchMenuDetails(); 
				break;
			case AR_CHAR_MENU_FILE:
				cellPropValue.content = FileMenuDetails(); 
				break;
			case AR_CHAR_MENU_SQL:
				cellPropValue.content = SqlMenuDetails(); 
				break;
			case AR_CHAR_MENU_DATA_DICTIONARY:
				cellPropValue.content = DataDictMenuDetails(); 
				break;
			}		
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

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

			//Add table to page
			webPage.AddContent(tblObjProp.ToXHtml());
			tblObjProp.Clear();

			//Properties
			webPage.AddContent(CARProplistHelper::GetList(this->pMenu->objPropList));

			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(this->pMenu, this->rootLevel));

			webPage.SaveInFolder(this->path);
		}
	}
	catch(...)
	{
		cout << "EXCEPTION Menu doc: " << this->pMenu->name << endl;
	}
}

string CDocCharMenuDetails::CharMenuDetails()
{
	stringstream strm;
	strm.str("");

	try
	{
		CTable tbl("menuItems", "TblObjectList");
		tbl.AddColumn(20, "Type");	
		tbl.AddColumn(40, "Label");
		tbl.AddColumn(40, "Value");

		ARCharMenuList menu = this->pMenu->menuDefn.u.menuList;
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
		strm << tbl.ToXHtml();
		tbl.Clear();
	}
	catch(...)
	{
		cout << "EXCEPTION Menu details doc: " << this->pMenu->name << endl;
	}


	return strm.str();
}

string CDocCharMenuDetails::FileMenuDetails()
{
	stringstream strm;
	strm.str("");

	try
	{
		ARCharMenuFileStruct menu = this->pMenu->menuDefn.u.menuFile;
		strm << "File Name: " << menu.filename << " (" << CAREnum::MenuFileLocation(menu.fileLocation) << ")" << endl;
	}
	catch(...)
	{
		cout << "EXCEPTION in FieldMenuDetails" << endl; 
	}

	return strm.str();
}

string CDocCharMenuDetails::SqlMenuDetails()
{
	stringstream strm;
	strm.str("");

	try
	{
		ARCharMenuSQLStruct *menu = &this->pMenu->menuDefn.u.menuSQL;

		strm << "Server: " << this->pInside->LinkToServerInfo(menu->server, rootLevel) << "<br/>" << endl;
		strm << "Label Index List: " << GetSQLLabelList(menu) << "<br/>" << endl;
		strm << "Value Index: " << menu->valueIndex << "<br/><br/>" << endl;
		strm << "SQL Command: " << menu->sqlCommand << endl;
	}
	catch(...)
	{
		cout << "EXCEPTION in SqlMenuDetails" << endl; 
	}

	return strm.str();
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
	catch(...)
	{
		cout << "EXCEPTION in DDCharMenu GetFieldType()" << endl; 
	}
	return strm.str();
}

string CDocCharMenuDetails::DataDictMenuDetails()
{
	stringstream strm;
	strm.str("");

	try
	{
		ARCharMenuDDStruct menu = this->pMenu->menuDefn.u.menuDD;

		strm << "Server: " << this->pInside->LinkToServerInfo(menu.server, rootLevel) << "<br/>" << endl;
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
				strm << "Object Type: Field<br/>" << endl;
				strm << "Form Name: " << menu.u.fieldDefn.schema << "<br/><br/>" << endl;	
				strm << "Field Type:<br/>" << GetFieldTypes(menu.u.fieldDefn.fieldType) << "<br/>" << endl;
			}
			break;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in DDMenuDetails" << endl; 
	}

	return strm.str();
}

string CDocCharMenuDetails::SearchMenuDetails()
{
	stringstream strm;
	strm.str("");

	try
	{
		ARCharMenuQueryStruct menu = this->pMenu->menuDefn.u.menuQuery;
		strm << "Server: " << this->pInside->LinkToServerInfo(menu.server, rootLevel) << "<br/>" << endl;
		strm << "Schema: " << this->pInside->LinkToSchema(menu.schema, rootLevel) << "<br/>" << endl;	

		//Label Fields
		for(int i=0; i< 5; i++)
		{
			if(menu.labelField[i] != NULL)
			{
				strm << "Label Field (" << i << ") : " << this->pInside->LinkToField(menu.schema, menu.labelField[i], rootLevel);
				strm << " (FieldId: " << menu.labelField[i] << ")<br/>" << endl;

				CFieldRefItem *refItem = new CFieldRefItem();
				refItem->arsStructItemType = AR_STRUCT_ITEM_XML_CHAR_MENU;
				refItem->description = "Menu Label Field";
				refItem->fromName = this->pMenu->name;
				refItem->fieldInsideId = menu.labelField[i];
				refItem->schemaInsideId = this->pInside->SchemaGetInsideId(menu.schema);
				this->pInside->AddReferenceItem(refItem);
				delete refItem;
			}
		}

		//Sort On Label
		string sortOnLabel = "Yes";
		if(!menu.sortOnLabel)
			sortOnLabel = "No";
		strm << "Sort On Label: " << sortOnLabel << "<br/>" << endl;


		//Value Field
		strm << "Value Field: " << this->pInside->LinkToField(menu.schema, menu.valueField, rootLevel);
		CFieldRefItem *refItem = new CFieldRefItem();
		refItem->arsStructItemType = AR_STRUCT_ITEM_XML_CHAR_MENU;
		refItem->description = "Menu Value Field";
		refItem->fromName = this->pMenu->name;
		refItem->fieldInsideId = menu.valueField;
		refItem->schemaInsideId = this->pInside->SchemaGetInsideId(menu.schema);
		this->pInside->AddReferenceItem(refItem);
		delete refItem;
		strm << " (FieldId: " << menu.valueField << ")<br/>" << endl;


		//Query
		stringstream strmQuery;
		strmQuery.str("");

		CFieldRefItem *refItemQuery = new CFieldRefItem();
		refItemQuery->arsStructItemType = AR_STRUCT_ITEM_XML_CHAR_MENU;
		refItemQuery->description = "Search Menu Qualification";
		refItemQuery->fromName = this->pMenu->name;
		refItemQuery->schemaInsideId = this->pInside->SchemaGetInsideId(menu.schema);


		CARQualification arQual(*this->pInside);
		int pFormId = this->pInside->SchemaGetInsideId(menu.schema);
		int sFormId = this->pInside->SchemaGetInsideId(menu.schema);
		arQual.arsStructItemType = AR_STRUCT_ITEM_XML_CHAR_MENU;

		arQual.CheckQuery(&menu.qualifier, *refItemQuery, 0, pFormId, sFormId, strmQuery, rootLevel);
		delete refItemQuery;

		if(strmQuery.str().length() > 0)
		{
			strm << "Qualification:<br/>" << strmQuery.str() << endl;
		}
		else
		{
			strm << "Qualification: " << EmptyValue << "<br/>" << endl;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in SearchMenuDetails" << endl; 
	}

	return strm.str();
}

string CDocCharMenuDetails::RelatedFields()
{	
	CTable tbl("relatedFields", "TblObjectList");
	tbl.AddColumn(40, "Field Name");	
	tbl.AddColumn(20, "Field Id");
	tbl.AddColumn(40, "Form");

	try
	{
		list<CARSchema>::iterator schemaIter;				
		for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
		{			
			CARSchema *schema = &(*schemaIter);

			list<CARField>::iterator fieldIter;
			for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
			{
				CARField *field = &(*fieldIter);

				if(field->dataType == AR_DATA_TYPE_CHAR)
				{
					if(strcmp(field->limit.u.charLimits.charMenu, this->pMenu->name.c_str())==0)
					{
						CTableRow row("cssStdRow");		
						CTableCell cellFieldName(field->GetURL(rootLevel), "");				
						CTableCell cellFieldId(field->insideId, "");
						CTableCell cellForm(schema->GetURL(rootLevel), "");

						row.AddCell(cellFieldName);
						row.AddCell(cellFieldId);
						row.AddCell(cellForm);
						tbl.AddRow(row);		

					}
				}
			}		
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in CharMenu related fields" << endl; 
	}

	return tbl.ToXHtml();
}

string CDocCharMenuDetails::RelatedActiveLinks()
{
	CTable tbl("relatedActiveLinks", "TblObjectList");
	tbl.AddColumn(100, "Active Link");

	try
	{
		list<CARActiveLink>::iterator alIter;		
		CARActiveLink *al;
		for ( alIter = this->pInside->alList.begin(); alIter != this->pInside->alList.end(); alIter++ )
		{			
			al = &(*alIter);
			for(unsigned int nAction = 0; nAction < al->actionList.numItems; nAction++)
			{
				ARActiveLinkActionStruct action = al->actionList.actionList[nAction];
				if(action.action == AR_ACTIVE_LINK_ACTION_SET_CHAR)
				{
					if(action.u.characteristics.charMenu != NULL)
					{
						if(strcmp(action.u.characteristics.charMenu, this->pMenu->name.c_str())==0)
						{
							CTableRow row("cssStdRow");				

							stringstream tmp;
							tmp << "If-Action " << nAction << " " << al->GetURL(rootLevel);

							CTableCell cellActiveLink(tmp.str(), "");						
							row.AddCell(cellActiveLink);
							tbl.AddRow(row);	
						}
					}
				}
			}		

			for(unsigned int nAction = 0; nAction < al->elseList.numItems; nAction++)
			{
				ARActiveLinkActionStruct action = al->elseList.actionList[nAction];
				if(action.action == AR_ACTIVE_LINK_ACTION_SET_CHAR)
				{
					if(action.u.characteristics.charMenu != NULL)
					{
						if(strcmp(action.u.characteristics.charMenu, this->pMenu->name.c_str())==0)
						{
							CTableRow row("cssStdRow");				

							stringstream tmp;
							tmp << "Else-Action " << nAction << " " << al->GetURL(rootLevel);

							row.AddCell(CTableCell(tmp.str()));
							tbl.AddRow(row);
						}
					}
				}
			}		
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in CharMenu related active links" << endl; 
	}

	return tbl.ToXHtml();
}

string CDocCharMenuDetails::GetSQLLabelList(ARCharMenuSQLStruct *sqlMenu)
{
	stringstream strm;
	char buffer[16];

	if (sqlMenu == NULL) return "";

	for (int k=0; k<5; ++k)
	{
		if (sqlMenu->labelIndex[k] == 0) break;
		
		if (k > 0) strm << ",";
		_itoa(sqlMenu->labelIndex[k], buffer, 16);
		strm << buffer;
	}

	return strm.str();
}
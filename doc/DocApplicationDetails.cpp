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
#include "DocApplicationDetails.h"

CDocApplicationDetails::CDocApplicationDetails(CARInside &arIn, CARContainer &application)
{
	this->pInside = &arIn;
	this->pApp = &application;
	this->rootLevel = 2;
}

CDocApplicationDetails::~CDocApplicationDetails(void)
{
}

void CDocApplicationDetails::Documentation()
{
	try
	{
		string dir = CAREnum::ContainerDir(ARCON_APP)+"/"+ this->pApp->FileID();

		CWindowsUtil winUtil(pInside->appConfig);
		if(winUtil.CreateSubDirectory(dir)>=0)
		{
			CWebPage webPage("index", this->pApp->name, rootLevel, pInside->appConfig);

			//ContentHead informations
			webPage.AddContentHead(CWebUtil::LinkToApplicationIndex(this->rootLevel) + MenuSeparator + CWebUtil::ObjName(this->pApp->name));

			//Container Base Informations
			CDocContainerHelper *contHelper = new CDocContainerHelper(*this->pInside, *this->pApp, this->rootLevel);
			webPage.AddContent(contHelper->BaseInfo());
			delete contHelper;

			//Object specific documentation
			webPage.AddContent(ApplicationInformation());

			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(this->pApp, this->rootLevel));

			//Save File
			webPage.SaveInFolder(dir);
		}
		else
			cout << "[Skipped] Directory '" << dir << "' already exists." << endl;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ApplicationDetails_Documentation: " << e.what() << endl; 
	}
}

string CDocApplicationDetails::GetPrimaryForm()
{
	try
	{
		for(unsigned int i=0; i< this->pApp->references.numItems; i++)
		{
			switch(this->pApp->references.referenceList[i].type)
			{						
				//case ARREF_APPLICATION_PRIMARY_FORM:
			case ARREF_SCHEMA:
				{
					if(InList(this->pApp->references.referenceList[i].reference.u.name, ARREF_SCHEMA))
					{
						return this->pApp->references.referenceList[i].reference.u.name;
					}
				}
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ApplicationDetails_GetPrimaryForm: " << e.what() << endl; 
	}

	return EmptyValue;
}

bool CDocApplicationDetails::InList(string searchName, int nType)
{
	try
	{
		for(unsigned int i=0; i< this->pApp->references.numItems; i++)
		{
			if(this->pApp->references.referenceList[i].type == nType)
			{	
				if(strcmp(this->pApp->references.referenceList[i].reference.u.name, searchName.c_str())==0)
				{
					return true;
				}
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ApplicationDetails_InList: " << e.what() << endl; 
	}

	return false;
}

string CDocApplicationDetails::ApplicationInformation()
{
	//Container specific properties
	CTable tblProp("specificPropList", "TblObjectList");
	tblProp.AddColumn(20, "Type");
	tblProp.AddColumn(80, "Server Object");

	try
	{
		CTableRow row("");		

		//Get the primary form of this application		
		row.ClearCells();
		CTableCell cellSrvType("Primary Form", "");
		CTableCell cellSrvObj(this->pInside->LinkToSchema(this->GetPrimaryForm(), this->rootLevel), "");
		if(cellSrvObj.content.size()==0)
			cellSrvObj.content = EmptyValue;

		row.AddCell(cellSrvType);
		row.AddCell(cellSrvObj);
		tblProp.AddRow(row);


		int nResult = 0;
		string strResult = "";

		//Forms in Applications
		nResult = 0;
		strResult = SearchForms(nResult);

		row.ClearCells();
		cellSrvType.content = CWebUtil::LinkToSchemaIndex(nResult, this->rootLevel);
		cellSrvObj.content = strResult;
		if(cellSrvObj.content.size()==0)
			cellSrvObj.content = EmptyValue;

		row.AddCell(cellSrvType);
		row.AddCell(cellSrvObj);
		tblProp.AddRow(row);


		//Search related ActiveLinks		
		strResult = SearchActiveLinks(nResult);
		row.ClearCells();
		cellSrvType.content = CWebUtil::LinkToActiveLinkIndex(nResult,this->rootLevel);
		cellSrvObj.content = strResult;
		if(cellSrvObj.content.size()==0)
			cellSrvObj.content = EmptyValue;

		row.AddCell(cellSrvType);
		row.AddCell(cellSrvObj);
		tblProp.AddRow(row);

		//Search related Filters
		strResult = SearchFilters(nResult);
		row.ClearCells();
		cellSrvType.content = CWebUtil::LinkToFilterIndex(nResult, this->rootLevel);
		cellSrvObj.content = strResult;
		if(cellSrvObj.content.size()==0)
			cellSrvObj.content = EmptyValue;

		row.AddCell(cellSrvType);
		row.AddCell(cellSrvObj);
		tblProp.AddRow(row);

		//Search related Escalations
		strResult = SearchEscalations(nResult);
		row.ClearCells();
		cellSrvType.content = CWebUtil::LinkToEscalationIndex(nResult, this->rootLevel);
		cellSrvObj.content = strResult;
		if(cellSrvObj.content.size()==0)
			cellSrvObj.content = EmptyValue;

		row.AddCell(cellSrvType);
		row.AddCell(cellSrvObj);
		tblProp.AddRow(row);


		//Related containers
		for(int nType=1; nType<6; nType++)
		{			
			strResult = SearchContainer(nResult, nType);
			row.ClearCells();
			cellSrvType.content = CWebUtil::LinkToContainer(nResult, this->rootLevel, nType);
			cellSrvObj.content = strResult;
			if(cellSrvObj.content.size()==0)
				cellSrvObj.content = EmptyValue;

			if(nType != ARCON_APP) //Application cannot be embedded in another app
			{
				row.AddCell(cellSrvType);
				row.AddCell(cellSrvObj);
				tblProp.AddRow(row);
			}
		}

		//Search CharMenus
		strResult = SearchMenus(nResult);
		row.ClearCells();
		cellSrvType.content = CWebUtil::LinkToMenuIndex(nResult, this->rootLevel);
		cellSrvObj.content = strResult;
		if(cellSrvObj.content.size()==0)
			cellSrvObj.content = EmptyValue;

		row.AddCell(cellSrvType);
		row.AddCell(cellSrvObj);
		tblProp.AddRow(row);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ApplicationDetails_BaseDocumentation: " << e.what() << endl; 
	}

	tblProp.description = "Application Properties";
	return tblProp.ToXHtml();
}

string CDocApplicationDetails::SearchForms(int &nResult)
{
	stringstream strmResult;
	strmResult.str("");

	nResult = 0;

	try
	{
		//Update the schema informations
		list<CARSchema>::iterator schemaIter;				
		for ( schemaIter = pInside->schemaList.begin(); schemaIter != pInside->schemaList.end(); schemaIter++ )
		{	
			CARSchema *schema = &(*schemaIter);
			if(this->InList(schema->name.c_str(), ARREF_SCHEMA))
			{
				nResult++;	
				strmResult << schema->GetURL(this->rootLevel) << "<br/>" << endl;

				schema->appRefName = this->pApp->name;

				list<CARField>::iterator fieldIter;
				for(fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
				{
					CARField *field = &(*fieldIter);
					field->appRefName = schema->appRefName;
				}
			}
		}	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ApplicationDetails_SearchForms: " << e.what() << endl; 
	}

	return strmResult.str();
}

string CDocApplicationDetails::SearchActiveLinks(int &nResult)
{
	stringstream strmResult;
	strmResult.str("");

	nResult = 0;

	try
	{	
		list<CARActiveLink>::iterator objIter;		

		for ( objIter = pInside->alList.begin(); objIter != pInside->alList.end(); objIter++ )
		{				
			CARActiveLink *obj = &(*objIter);
			bool bInsert = false;

			for(unsigned int i=0; i< obj->schemaList.u.schemaList->numItems; i++)
			{
				if(InList(obj->schemaList.u.schemaList->nameList[i], ARREF_SCHEMA))
				{
					bInsert = true;
				}
			}

			if(InList(obj->name.c_str(), ARREF_ACTLINK))
				bInsert = true;

			if(bInsert)
			{
				nResult++;
				strmResult << obj->GetURL(rootLevel) << "<br/>" << endl;
				obj->appRefName = this->pApp->name;
			}
		}		
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ApplicationDetails_SearchActiveLinks: " << e.what() << endl; 
	}

	return strmResult.str();
}

string CDocApplicationDetails::SearchFilters(int &nResult)
{
	stringstream strmResult;
	strmResult.str("");

	nResult = 0;

	try
	{
		list<CARFilter>::iterator objIter;
		for ( objIter = pInside->filterList.begin(); objIter != pInside->filterList.end(); objIter++ )
		{	
			CARFilter *obj = &(*objIter);
			bool bInsert = false;

			for(unsigned int i=0; i< obj->schemaList.u.schemaList->numItems; i++)
			{
				if(InList(obj->schemaList.u.schemaList->nameList[i], ARREF_SCHEMA))
				{
					bInsert = true;
				}
			}

			if(InList(obj->name.c_str(), ARREF_FILTER))
				bInsert = true;

			if(bInsert)
			{
				nResult++;
				strmResult << obj->GetURL(rootLevel) << "<br/>" << endl;
				obj->appRefName = this->pApp->name;
			}
		}		
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ApplicationDetails_SearchFilters: " << e.what() << endl; 
	}

	return strmResult.str();
}

string CDocApplicationDetails::SearchEscalations(int &nResult)
{
	stringstream strmResult;
	strmResult.str("");

	nResult = 0;

	try
	{
		list<CAREscalation>::iterator objIter;		
		for ( objIter = pInside->escalList.begin(); objIter != pInside->escalList.end(); objIter++ )
		{	
			CAREscalation *obj = &(*objIter);
			bool bInsert = false;

			for(unsigned int i=0; i< obj->schemaList.u.schemaList->numItems; i++)
			{
				if(InList(obj->schemaList.u.schemaList->nameList[i], ARREF_SCHEMA))
				{
					bInsert = true;
				}
			}

			if(InList(obj->name.c_str(), ARREF_ESCALATION))
				bInsert = true;

			if(bInsert)
			{
				nResult++;
				strmResult << obj->GetURL(rootLevel) << "<br/>" << endl;
				obj->appRefName = this->pApp->name;
			}
		}		
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ApplicationDetails_SearchEscalations: " << e.what() << endl; 
	}

	return strmResult.str();
}


string CDocApplicationDetails::SearchContainer(int &nResult, int nType)
{
	stringstream strmResult;
	strmResult.str("");

	nResult = 0;

	try
	{
		list<CARContainer>::iterator objIter;				
		for ( objIter = pInside->containerList.begin(); objIter != pInside->containerList.end(); objIter++ )
		{	
			bool bInsert = false;
			CARContainer *obj = &(*objIter);	

			if(obj->type == nType)
			{	
				if(this->InList(obj->name, ARREF_CONTAINER))
				{
					bInsert = true;
				}

				CARContainer *obj = &(*objIter);			
				if(obj->type == nType)
				{
					for(unsigned int i=0; i< obj->ownerObjList.numItems; i++)
					{
						if(this->InList(obj->ownerObjList.ownerObjList[i].ownerName, ARREF_SCHEMA))
						{
							bInsert = true;
						}
					}
				}

				if(bInsert)
				{
					nResult++;
					strmResult << obj->GetURL(rootLevel) << "<br/>" << endl;

					obj->appRefName = this->pApp->name;
				}
			}
		}	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ApplicationDetails_SearchContainer:" << e.what() << endl; 
	}

	return strmResult.str();
}

string CDocApplicationDetails::SearchMenus(int &nResult)
{
	stringstream strmResult;
	strmResult.str("");

	nResult = 0;

	try
	{
		list<CARCharMenu>::iterator objIter;				
		for ( objIter = pInside->menuList.begin(); objIter != pInside->menuList.end(); objIter++ )
		{	
			bool bInsert = false;
			CARCharMenu *obj = &(*objIter);	

			list<CARSchema>::iterator schemaIter;				
			for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
			{			
				CARSchema *schema = &(*schemaIter);

				if(InList(schema->name, ARREF_SCHEMA))
				{
					list<CARField>::iterator fieldIter;
					for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
					{
						CARField *field = &(*fieldIter);
						if(field->dataType == AR_DATA_TYPE_CHAR)
						{
							if(strcmp(field->limit.u.charLimits.charMenu, obj->name.c_str())==0)
							{
								bInsert = true;
							}
						}
					}
				}
			}

			if(InList(obj->name.c_str(), ARREF_CHAR_MENU))
				bInsert = true;

			if(bInsert)
			{
				nResult++;
				strmResult << obj->GetURL(rootLevel) << "<br/>" << endl;

				obj->appRefName = this->pApp->name;
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ApplicationDetails_SearchMenu: " << e.what() << endl; 
	}

	return strmResult.str();
}

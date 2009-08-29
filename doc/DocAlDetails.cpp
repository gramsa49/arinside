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
#include "docaldetails.h"

CDocAlDetails::CDocAlDetails(CARInside &arInside, CARActiveLink &arActiveLink, string path, int rootLevel)
{
	this->pInside = &arInside;
	this->pAl = &arActiveLink;
	this->path = path;
	this->rootLevel = rootLevel;
	this->props = NULL;
}

CDocAlDetails::~CDocAlDetails(void)
{
}

void CDocAlDetails::Documentation()
{
	try
	{
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage("index", this->pAl->name, this->rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");

			// temp list to output only infos that arent already displayed
			props = new CARProplistHelper(&pAl->objPropList);

			strmHead << CWebUtil::LinkToActiveLinkIndex(this->rootLevel) + MenuSeparator + CWebUtil::ObjName(this->pAl->name);
			if(this->pAl->appRefName.c_str() != NULL && this->pAl->appRefName.size() > 0)
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->pAl->appRefName, this->rootLevel);

			webPage.AddContentHead(strmHead.str());

			//ActiveLink Properties
			stringstream strmTmp;
			CTable tblObjProp("objProperties", "TblObjectList");
			tblObjProp.AddColumn(30, "Property");	
			tblObjProp.AddColumn(70, "Value");

			//Status
			CTableRow row("cssStdRow");		
			CTableCell cellProp("Status", "");				
			CTableCell cellPropValue(CAREnum::ObjectEnable(this->pAl->enable), "");    
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//Execution Order
			row.ClearCells();
			strmTmp << this->pAl->order;

			cellProp.content = "Execution Order";
			cellPropValue.content = strmTmp.str();
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//Execute On
			row.ClearCells();
			cellProp.content = "Execute On";
			cellPropValue.content = pAl->GetExecuteOn(false, props);
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//Workflow	
			if(this->pAl->schemaList.u.schemaList->numItems > 0)
			{		
				for(unsigned int i=0; i< this->pAl->schemaList.u.schemaList->numItems; i++)
				{
					//Workflowlink to each page	
					strmTmp.str("");
					row.ClearCells();
					cellProp.content = this->pInside->LinkToSchema(this->pAl->schemaList.u.schemaList->nameList[i], rootLevel);

					cellPropValue.content = this->CreateSpecific(this->pAl->schemaList.u.schemaList->nameList[i]);
					row.AddCell(cellProp);
					row.AddCell(cellPropValue);
					tblObjProp.AddRow(row);	
				}

			}
			else // AL is not related to any form
			{
				strmTmp.str("");
				row.ClearCells();
				cellProp.content = "No schema specified";

				cellPropValue.content = this->CreateSpecific("");
				row.AddCell(cellProp);
				row.AddCell(cellPropValue);
				tblObjProp.AddRow(row);	
			}	

			//Permissions
			row.ClearCells();
			cellProp.content = "Permissions";
			cellPropValue.content = this->Permissions();				
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//Al used in container?
			row.ClearCells();
			cellProp.content = "Container References";
			cellPropValue.content = this->ContainerReferences();				
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);

			//Table description
			stringstream tblDesc;
			tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Active Link Properties";
			tblObjProp.description = tblDesc.str();

			//Add table to page
			webPage.AddContent(tblObjProp.ToXHtml());
			tblObjProp.Clear();

			//Properties
			webPage.AddContent(props->UnusedPropertiesToHTML());
			//webPage.AddContent(CARProplistHelper::GetList(*this->pInside, this->pAl->objPropList));

			//History		
			webPage.AddContent(this->pInside->ServerObjectHistory(this->pAl, this->rootLevel));

			webPage.SaveInFolder(path);

			delete props; props = NULL;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION enumerating common active link informations: " << this->pAl->name << endl;
	}
}

string CDocAlDetails::Permissions()
{
	stringstream strm;
	strm.str("");
	try
	{
		CGroupTable *grpTbl = new CGroupTable(*this->pInside);
		for(unsigned int i=0; i< this->pAl->groupList.numItems; i++)
		{			
			grpTbl->AddRow(this->pAl->appRefName, this->pAl->groupList.internalIdList[i], this->rootLevel);
		}

		strm << grpTbl->Print();
		delete grpTbl;

	}
	catch(...)
	{
		cout << "EXCEPTION enumerating active link permissions: " << this->pAl->name << endl;
	}

	return strm.str();
}

string CDocAlDetails::ContainerReferences()
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
						if(strcmp(cont->references.referenceList[nCnt].reference.u.name, this->pAl->name.c_str())==0
							&& cont->references.referenceList[nCnt].type == ARREF_ACTLINK)
						{
							contTable->AddRow(*cont, rootLevel);
						}
					}
				}
			}
		}		

		strm << contTable->Print();
		delete contTable;
	}
	catch(...)
	{
		cout << "EXCEPTION enumerating active link container references: " << this->pAl->name << endl;
	}

	return strm.str();
}


string CDocAlDetails::CreateSpecific(string schemaName)
{
	stringstream pgStrm;
	pgStrm.str("");

	try
	{	
		//Control Field
		stringstream strmTmp;
		strmTmp.str("");

		if(this->pAl->controlField > 0)
		{
			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = AR_STRUCT_ITEM_XML_ACTIVE_LINK;
			refItem->description = "Control Field";
			refItem->fromName = this->pAl->name;	
			refItem->fieldInsideId = this->pAl->controlField;
			refItem->schemaInsideId = this->pInside->SchemaGetInsideId(schemaName);
			pgStrm << "Control Field: " << this->pInside->LinkToField(schemaName, this->pAl->controlField, rootLevel) << "<br/><br/>" << endl; 
			this->pInside->AddReferenceItem(refItem);
			delete refItem;
		}


		if(this->pAl->focusField > 0)
		{
			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = AR_STRUCT_ITEM_XML_ACTIVE_LINK;
			refItem->description = "Focus Field";
			refItem->fromName = this->pAl->name;	
			refItem->fieldInsideId = this->pAl->focusField;
			refItem->schemaInsideId = this->pInside->SchemaGetInsideId(schemaName);
			pgStrm << "Focus Field: " << this->pInside->LinkToField(schemaName, this->pAl->focusField, rootLevel) << "<br/><br/>" << endl;
			this->pInside->AddReferenceItem(refItem);
			delete refItem;
		}

		//Query
		strmTmp.str("");
		if(this->pAl->query.operation != NULL)
		{		
			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = AR_STRUCT_ITEM_XML_ACTIVE_LINK;
			refItem->description = "Run If";
			refItem->fromName = this->pAl->name;

			CARQualification arQual(*this->pInside);
			int pFormId = this->pInside->SchemaGetInsideId(schemaName.c_str());
			int sFormId = pFormId;
			arQual.CheckQuery(&this->pAl->query, *refItem, 0, pFormId, sFormId, strmTmp, rootLevel);

			delete refItem;
		}
		else
		{
			strmTmp << EmptyRunIf << endl;
		}

		pgStrm << "Run If Qualification: <br/>" << strmTmp.str();

		//If-Actions		
		CDocAlActionStruct actionStruct(*this->pInside, *this->pAl, schemaName, this->path, this->rootLevel);
		pgStrm << actionStruct.Get("If", this->pAl->actionList);

		//Else-Actions
		pgStrm << actionStruct.Get("Else", this->pAl->elseList);
	}
	catch(...)
	{
		cout << "EXCEPTION enumerating active link details: " << this->pAl->name << endl;
	}

	return pgStrm.str();
}


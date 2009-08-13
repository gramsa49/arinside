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
#include ".\docfilterdetails.h"

CDocFilterDetails::CDocFilterDetails(CARInside &arInside, CARFilter &arFilter, string path, int rootLevel)
{	
	this->pInside = &arInside;
	this->pFilter = &arFilter;
	this->path = path;
	this->rootLevel = rootLevel;
}

CDocFilterDetails::~CDocFilterDetails(void)
{
}

void CDocFilterDetails::Documentation()
{	
	try
	{
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage("index", this->pFilter->name, this->rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");

			strmHead << CWebUtil::LinkToFilterIndex(this->rootLevel) + MenuSeparator + CWebUtil::ObjName(this->pFilter->name);
			if(this->pFilter->appRefName.c_str() != NULL && this->pFilter->appRefName.size() > 0)
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->pFilter->appRefName, this->rootLevel);

			webPage.AddContentHead(strmHead.str());

			//Filter Properties
			stringstream strmTmp;
			CTable tblObjProp("objProperties", "TblObjectList");
			tblObjProp.AddColumn(30, "Property");	
			tblObjProp.AddColumn(70, "Value");

			//Status
			CTableRow tblRow("");
			tblRow.AddCellList(CTableCell("Status"), CTableCell(CAREnum::ObjectEnable(this->pFilter->enable)));
			tblObjProp.AddRow(tblRow);

			//Execution Order		
			strmTmp << this->pFilter->order;

			tblRow.AddCellList(CTableCell("Execution Order"), CTableCell(strmTmp.str()));
			tblObjProp.AddRow(tblRow);

			//Execute On
			tblRow.AddCellList(CTableCell("Execute On"), CTableCell(this->pFilter->GetExecuteOn()));
			tblObjProp.AddRow(tblRow);

			// this is just a little workaround for a bug in ar.h version 7.5
#ifndef AR_FILTER_ERRHANDLER_ENABLE
#define AR_FILTER_ERRHANDLER_ENABLE 1
#endif
			// Error Handler
			strmTmp.str("");
			if (this->pFilter->errorOptions == AR_FILTER_ERRHANDLER_ENABLE)
			{
				strmTmp << pInside->LinkToFilter(this->pFilter->errorFilterName, rootLevel);
			}
			else 
			{
				strmTmp << "None";
			}
			tblRow.AddCellList(CTableCell("Error Handler"), strmTmp.str());
			tblObjProp.AddRow(tblRow);

			//Workflow	
			if(this->pFilter->schemaList.u.schemaList->numItems > 0)
			{		
				for(unsigned int i=0; i< this->pFilter->schemaList.u.schemaList->numItems; i++)
				{
					//Workflowlink to each page	
					tblRow.AddCellList(CTableCell(this->pInside->LinkToSchema(this->pFilter->schemaList.u.schemaList->nameList[i], rootLevel)), 
						CTableCell(this->CreateSpecific(this->pFilter->schemaList.u.schemaList->nameList[i])));
					tblObjProp.AddRow(tblRow);

				}
			}
			else // Filter is not related to any form
			{
				tblRow.AddCellList(CTableCell("No schema specified"), CTableCell(this->CreateSpecific("")));
				tblObjProp.AddRow(tblRow);
			}	

			//Filter used in container?
			tblRow.AddCellList(CTableCell("Container References"), CTableCell(this->ContainerReferences()));
			tblObjProp.AddRow(tblRow);		

			//Table description
			stringstream tblDesc;
			tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Filter Properties";
			tblObjProp.description = tblDesc.str();

			//Add table to page
			webPage.AddContent(tblObjProp.ToXHtml());
			tblObjProp.Clear();

			//Properties
			webPage.AddContent(CARProplistHelper::GetList(*this->pInside, this->pFilter->objPropList));

			// Workflow References
			webPage.AddContent(this->WorkflowReferences());

			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(this->pFilter, this->rootLevel));
			webPage.SaveInFolder(this->path);
		}
	}
	catch(...)
	{
		cout << "EXCEPTION filter details common props: " << this->pFilter->name << endl;
	}
}

string CDocFilterDetails::ContainerReferences()
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
						if(strcmp(cont->references.referenceList[nCnt].reference.u.name, this->pFilter->name.c_str())==0
							&& cont->references.referenceList[nCnt].type == ARREF_FILTER)
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
		cout << "EXCEPTION enumerating filter container references: " << this->pFilter->name << endl;
	}

	return strm.str();
}

string CDocFilterDetails::CreateSpecific(string schemaName)
{
	stringstream pgStrm;
	pgStrm.str("");

	try
	{
		//Query
		stringstream strmQuery;
		if(this->pFilter->query.operation != NULL)
		{		
			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = AR_STRUCT_ITEM_XML_FILTER;
			refItem->description = "Run If";
			refItem->fromName = this->pFilter->name;

			CARQualification arQual(*this->pInside);

			int pFormId = this->pInside->SchemaGetInsideId(schemaName.c_str());
			int sFormId = pFormId;
			arQual.CheckQuery(&this->pFilter->query, *refItem, 0, pFormId, sFormId, strmQuery, rootLevel);

			delete refItem;
		}
		else
		{
			strmQuery << EmptyRunIf << endl;
		}

		pgStrm << "Run If Qualification: <br/>" << strmQuery.str();

		//If-Actions		
		CDocFilterActionStruct actionStruct(*this->pInside, *this->pFilter, schemaName, this->path, this->rootLevel, AR_STRUCT_ITEM_XML_FILTER);
		pgStrm << actionStruct.Get("If", this->pFilter->actionList);

		//Else-Actions
		pgStrm << actionStruct.Get("Else", this->pFilter->elseList);
	}
	catch(...)
	{
		cout << "EXCEPTION filter specific props: " << this->pFilter->name << endl;
	}

	return pgStrm.str();
}

string CDocFilterDetails::WorkflowReferences()
{
	unsigned int refCount = 0; 
	stringstream strm;
	strm.str("");

	try
	{
		//create table for workflow references
		CTable tblRef("referenceList", "TblObjectList");
		tblRef.AddColumn(15, "Type");
		tblRef.AddColumn(25, "Server object");
		tblRef.AddColumn(10, "Enabled");
		tblRef.AddColumn(50, "Description");

		// output error handler callers here
		list<string>::iterator iter;
		for ( iter = this->pFilter->errorCallers.begin(); iter != this->pFilter->errorCallers.end(); iter++)
		{		
			CTableRow row("cssStdRow");
			row.AddCell(CAREnum::XmlStructItem(AR_STRUCT_ITEM_XML_FILTER));
			row.AddCell(pInside->LinkToXmlObjType(AR_STRUCT_ITEM_XML_FILTER, *iter, rootLevel));

			string tmpEnabled = this->pInside->XmlObjEnabled(AR_STRUCT_ITEM_XML_FILTER, *iter);
			string tmpCssEnabled = "";

			if(strcmp(tmpEnabled.c_str(), "Disabled")==0)
				tmpCssEnabled = "objStatusDisabled";

			row.AddCell(CTableCell(tmpEnabled, tmpCssEnabled));
			row.AddCell("Selected as Error Handler");				
			tblRef.AddRow(row);
			refCount++;
		}
			
		// output table on if references exists
		if (refCount > 0)
		{
			stringstream tblDesc;
			tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Workflow Reference:";

			tblRef.description = tblDesc.str();

			strm << tblRef.ToXHtml();
		}
		tblRef.Clear();
	}
	catch (...)
	{
		cout << "EXCEPTION enumerating workflow references for filter: " << this->pFilter->name << endl;
	}

	return strm.str();
}


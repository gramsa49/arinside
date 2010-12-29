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
#include "DocFilterDetails.h"

CDocFilterDetails::CDocFilterDetails(unsigned int filterInsideId)
: filter(filterInsideId)
{	
}

CDocFilterDetails::~CDocFilterDetails(void)
{
}

void CDocFilterDetails::Documentation()
{	
	CPageParams file(PAGE_DETAILS, &filter);
	this->rootLevel = file->GetRootLevel();
	this->path = file->GetPath();

	try
	{
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage(file->GetFileName(), filter.GetName(), this->rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");

			strmHead << CWebUtil::LinkToFilterIndex(this->rootLevel) << MenuSeparator << CWebUtil::ObjName(filter.GetName());
			if(!filter.GetAppRefName().empty())
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(filter.GetAppRefName(), this->rootLevel);

			webPage.AddContentHead(strmHead.str());

			//Filter Properties
			stringstream strmTmp;
			CTable tblObjProp("objProperties", "TblObjectList");
			tblObjProp.AddColumn(30, "Property");	
			tblObjProp.AddColumn(70, "Value");

			//Status
			CTableRow tblRow("");
			tblRow.AddCellList(CTableCell("Status"), CTableCell(CAREnum::ObjectEnable(filter.GetEnabled())));
			tblObjProp.AddRow(tblRow);

			//Execution Order		
			strmTmp << filter.GetOrder();
			tblRow.AddCellList(CTableCell("Execution Order"), CTableCell(strmTmp.str()));
			tblObjProp.AddRow(tblRow);

			//Execute On
			tblRow.AddCellList(CTableCell("Execute On"), CTableCell(filter.GetExecuteOn()));
			tblObjProp.AddRow(tblRow);

			// Error Handler
			strmTmp.str("");
			if (filter.GetErrorOption() == AR_FILTER_ERRHANDLER_ENABLE)
			{
				strmTmp << pInside->LinkToFilter(filter.GetErrorHandler(), rootLevel);
			}
			else 
			{
				strmTmp << "None";
			}
			tblRow.AddCellList(CTableCell("Error Handler"), strmTmp.str());
			tblObjProp.AddRow(tblRow);

			//Workflow
			const ARWorkflowConnectStruct &schemas = filter.GetSchemaList();

			if(schemas.u.schemaList->numItems > 0)
			{		
				for(unsigned int i=0; i< schemas.u.schemaList->numItems; i++)
				{
					//Workflowlink to each page	
					tblRow.AddCellList(
						CTableCell(this->pInside->LinkToSchema(schemas.u.schemaList->nameList[i], rootLevel)), 
						CTableCell(this->CreateSpecific(schemas.u.schemaList->nameList[i]))
					);
					
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
			webPage.AddContent(CARProplistHelper::GetList(filter.GetPropList()));

			// Workflow References
			webPage.AddContent(this->WorkflowReferences());

			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(&this->filter, this->rootLevel));
			webPage.SaveInFolder(this->path);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION filter details common props of '" << filter.GetName() << "': " << e.what() << endl;
	}
}

string CDocFilterDetails::ContainerReferences()
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
					if(refs.referenceList[nCnt].type == ARREF_FILTER)
					{
						if(refs.referenceList[nCnt].reference.u.name != NULL && 
							 refs.referenceList[nCnt].reference.u.name == filter.GetName())
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
		cout << "EXCEPTION enumerating filter container references of '" << this->filter.GetName() << "': " << e.what() << endl;
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
		if(this->filter.GetRunIf().operation != 0)
		{		
			CRefItem refItem(this->filter, REFM_RUNIF);
			CARQualification arQual(*this->pInside);

			int pFormId = this->pInside->SchemaGetInsideId(schemaName.c_str());
			int sFormId = pFormId;

			arQual.CheckQuery(&this->filter.GetRunIf(), refItem, 0, pFormId, sFormId, strmQuery, rootLevel);
		}
		else
		{
			strmQuery << EmptyRunIf << endl;
		}

		pgStrm << "Run If Qualification: <br/>" << strmQuery.str();

		//If-Actions		
		CDocFilterActionStruct actionStruct(*this->pInside, this->filter, schemaName, this->rootLevel, AR_STRUCT_ITEM_XML_FILTER);
		pgStrm << actionStruct.Get(IES_IF, this->filter.GetIfActions());

		//Else-Actions
		pgStrm << actionStruct.Get(IES_ELSE, this->filter.GetElseActions());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION filter specific props of '" << this->filter.GetName() << "': " << e.what() << endl;
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
		tblRef.AddColumn(10, "Type");
		tblRef.AddColumn(45, "Server object");
		tblRef.AddColumn(5, "Enabled");
		tblRef.AddColumn(40, "Description");

		// output error handler callers here
		vector<string>::const_iterator iter = this->filter.ErrorCallers().begin();
		vector<string>::const_iterator endIt = this->filter.ErrorCallers().end();
		for ( ; iter != endIt; ++iter)
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

			strm << tblRef;
		}
	}
	catch (...)
	{
		cout << "EXCEPTION enumerating workflow references for filter: " << this->filter.GetName() << endl;
	}

	return strm.str();
}


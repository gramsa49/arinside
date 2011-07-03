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
#include "DocAlDetails.h"
#include "../output/IFileStructure.h"

CDocAlDetails::CDocAlDetails(int alInsideId)
: al(alInsideId)
{
	this->props = NULL;
}

CDocAlDetails::~CDocAlDetails(void)
{
}

void CDocAlDetails::Documentation()
{
	CPageParams file(PAGE_DETAILS, &al);
	this->rootLevel = file->GetRootLevel();

	try
	{
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(file->GetPath())>=0)
		{
			stringstream pgStream;
			CWebPage webPage(file->GetFileName(), this->al.GetName(), this->rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");

			// temp list to output only infos that arent already displayed
			props = new CARProplistHelper(&this->al.GetPropList());

			strmHead << CWebUtil::LinkToActiveLinkIndex(this->rootLevel) << MenuSeparator << CWebUtil::ObjName(this->al.GetName());

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			ARValueStruct* overlayProp = CARProplistHelper::Find(this->al.GetPropList(), AR_SMOPROP_OVERLAY_PROPERTY);
			if (overlayProp != NULL && overlayProp->dataType == AR_DATA_TYPE_INTEGER)
			{
				switch (overlayProp->u.intVal)
				{
				case AR_OVERLAID_OBJECT:
					{
						CARActiveLink ovAl(this->al.GetName() + (this->pInside->overlayMode == 0 ? AR_RESERV_OVERLAY_STRING : ""));
						strmHead << " (Overlaid by " << (ovAl.Exists() ? ovAl.GetURL(rootLevel, false) : "") << ")";
					}
					break;
				case AR_OVERLAY_OBJECT:
					{
						CARActiveLink oriAl(this->al.GetName() + (this->pInside->overlayMode == 1 ? AR_RESERV_OVERLAY_STRING : ""));
						strmHead << " (Overlay of " << (oriAl.Exists() ? oriAl.GetURL(rootLevel, false) : "") << ")";
					}
					break;
				}
			}
#endif

			if(!this->al.GetAppRefName().empty())
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->al.GetAppRefName(), this->rootLevel);

			webPage.AddContentHead(strmHead.str());

			//ActiveLink Properties
			stringstream strmTmp;
			CTable tblObjProp("objProperties", "TblObjectList");
			tblObjProp.AddColumn(30, "Property");	
			tblObjProp.AddColumn(70, "Value");

			//Status
			CTableRow row("cssStdRow");		
			CTableCell cellProp("Status", "");				
			CTableCell cellPropValue(CAREnum::ObjectEnable(this->al.GetEnabled()), "");
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//Execution Order
			row.ClearCells();
			strmTmp << this->al.GetOrder();

			cellProp.content = "Execution Order";
			cellPropValue.content = strmTmp.str();
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//Execute On
			row.ClearCells();
			cellProp.content = "Execute On";
			cellPropValue.content = this->al.GetExecuteOn(false, props);
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//Workflow	
			if(this->al.GetSchemaList().u.schemaList->numItems > 0)
			{		
				const ARWorkflowConnectStruct &formList = this->al.GetSchemaList();

				for(unsigned int i=0; i< formList.u.schemaList->numItems; i++)
				{
					//Workflowlink to each page	
					strmTmp.str("");
					row.ClearCells();
					cellProp.content = this->pInside->LinkToSchema(formList.u.schemaList->nameList[i], rootLevel);

					cellPropValue.content = this->CreateSpecific(formList.u.schemaList->nameList[i]);
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
			webPage.AddContent(this->pInside->ServerObjectHistory(&this->al, this->rootLevel));

			webPage.SaveInFolder(file->GetPath());

			delete props; props = NULL;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating common active link informations of '" << this->al.GetName() << "': " << e.what() << endl;
	}
}

string CDocAlDetails::Permissions()
{
	stringstream strm;
	strm.str("");
	try
	{
		CGroupTable *grpTbl = new CGroupTable(*this->pInside);
		for(unsigned int i=0; i< this->al.GetGroupList().numItems; i++)
		{			
			grpTbl->AddRow(this->al.GetAppRefName(), this->al.GetGroupList().internalIdList[i], this->rootLevel);
		}

		strm << *grpTbl;
		delete grpTbl;

	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating active link permissions of '" << this->al.GetName() << "': " << e.what() << endl;
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
		unsigned int cntCount = this->pInside->containerList.GetCount();
		for ( unsigned int cntIndex = 0; cntIndex < cntCount; ++cntIndex )
		{
			CARContainer cont(cntIndex);

			if(cont.GetType() != ARCON_APP)
			{
				const ARReferenceList& refs = cont.GetReferences();
				for(unsigned int nCnt = 0; nCnt < refs.numItems; nCnt++)
				{
					if(refs.referenceList[nCnt].type == ARREF_ACTLINK)
					{
						if(refs.referenceList[nCnt].reference.u.name != NULL &&
						   this->al.GetName() == refs.referenceList[nCnt].reference.u.name)
						{
							contTable->AddRow(cont, rootLevel);
						}
					}
				}
			}
		}

		strm << *contTable;
		delete contTable;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating active link container references of '" << this->al.GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}


string CDocAlDetails::CreateSpecific(const string &schemaName)
{
	stringstream pgStrm;
	pgStrm.str("");
	int schemaInsideId = -2;

	try
	{	
		//Control Field
		stringstream strmTmp;
		strmTmp.str("");

		if(this->al.GetControlField() > 0)
		{
			if (schemaInsideId == -2)
				schemaInsideId = this->pInside->SchemaGetInsideId(schemaName);

			pgStrm << "Control Field: " << this->pInside->LinkToField(schemaInsideId, this->al.GetControlField(), rootLevel) << "<br/><br/>" << endl;
			CRefItem refItem(this->al, REFM_CONTROLFIELD);
			pInside->AddFieldReference(schemaInsideId, this->al.GetControlField(), refItem);
		}


		if(this->al.GetFocusField() > 0)
		{
			if (schemaInsideId == -2)
				schemaInsideId = this->pInside->SchemaGetInsideId(schemaName);

			pgStrm << "Focus Field: " << this->pInside->LinkToField(schemaInsideId, this->al.GetFocusField(), rootLevel) << "<br/><br/>" << endl;
			CRefItem refItem(this->al, REFM_FOCUSFIELD);
			pInside->AddFieldReference(schemaInsideId, al.GetFocusField(), refItem);
		}

		//Query
		strmTmp.str("");
		if(this->al.GetRunIf().operation != AR_COND_OP_NONE)
		{		
			if (schemaInsideId == -2)
				schemaInsideId = this->pInside->SchemaGetInsideId(schemaName);

			CRefItem refItem(this->al, REFM_RUNIF);

			CARQualification arQual(*this->pInside);
			int pFormId = schemaInsideId;
			int sFormId = schemaInsideId;
			arQual.CheckQuery(&this->al.GetRunIf(), refItem, 0, pFormId, sFormId, strmTmp, rootLevel);
		}
		else
		{
			strmTmp << EmptyRunIf << endl;
		}

		pgStrm << "Run If Qualification: <br/>" << strmTmp.str();

		//If-Actions		
		CDocAlActionStruct actionStruct(*this->pInside, this->al, schemaName, this->rootLevel);
		pgStrm << actionStruct.Get(IES_IF, this->al.GetIfActions());

		//Else-Actions
		pgStrm << actionStruct.Get(IES_ELSE, this->al.GetElseActions());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating active link details of '" << this->al.GetName() << "': " << e.what() << endl;
	}

	return pgStrm.str();
}


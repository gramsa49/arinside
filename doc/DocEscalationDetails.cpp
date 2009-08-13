
/****************************************************************************** 
 * 
 *  file:  DocEscalationDetails.cpp
 * 
 *  Copyright (c) 2007, Stefan Nerlich | stefan.nerlich@hotmail.com 
 *  All rights reverved.
 * 
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *  
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.  
 *  
 *****************************************************************************/

#include "StdAfx.h"
#include ".\docescalationdetails.h"

CDocEscalationDetails::CDocEscalationDetails(CARInside &arInside, CAREscalation &arEscalation, string path, int rootLevel)
{
	this->pInside = &arInside;
	this->pEscal = &arEscalation;
	this->path = path;
	this->rootLevel = rootLevel;
}

CDocEscalationDetails::~CDocEscalationDetails(void)
{
}

void CDocEscalationDetails::Documentation()
{
	try
	{
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage("index", this->pEscal->name, this->rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");
			
			strmHead << CWebUtil::LinkToEscalationIndex(this->rootLevel) + MenuSeparator + CWebUtil::ObjName(this->pEscal->name);
			if(this->pEscal->appRefName.c_str() != NULL && this->pEscal->appRefName.size() > 0)
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->pEscal->appRefName, this->rootLevel);

			webPage.AddContentHead(strmHead.str());
		

			//Escalation Properties
			stringstream strmTmp;
			CTable tblObjProp("objProperties", "TblObjectList");
			tblObjProp.AddColumn(30, "Property");	
			tblObjProp.AddColumn(70, "Value");
			
			//Status
			CTableRow row("cssStdRow");		
			CTableCell cellProp("Status", "");				
			CTableCell cellPropValue(CAREnum::ObjectEnable(this->pEscal->enable), "");    
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	

			//Time criteria
			row.ClearCells();
			cellProp.content = "Time Criteria";
			cellPropValue.content = this->pEscal->GetTimeCriteria();    
			row.AddCell(cellProp);
			row.AddCell(cellPropValue);
			tblObjProp.AddRow(row);	
					
			//Workflow	
			if(this->pEscal->schemaList.u.schemaList->numItems > 0)
			{		
				for(unsigned int i=0; i< this->pEscal->schemaList.u.schemaList->numItems; i++)
				{
					//Workflowlink to each page	
					strmTmp.str("");
					row.ClearCells();
					cellProp.content = this->pInside->LinkToSchema(this->pEscal->schemaList.u.schemaList->nameList[i], rootLevel);

					cellPropValue.content = this->CreateSpecific(this->pEscal->schemaList.u.schemaList->nameList[i]);
					row.AddCell(cellProp);
					row.AddCell(cellPropValue);
					tblObjProp.AddRow(row);	
				}
			}
			else // Escalation is not related to any form
			{
				strmTmp.str("");
				row.ClearCells();
				cellProp.content = "No schema specified";

				cellPropValue.content = this->CreateSpecific("");
				row.AddCell(cellProp);
				row.AddCell(cellPropValue);
				tblObjProp.AddRow(row);	
			}	
		    
			//Table description
			stringstream tblDesc;
			tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Escalation Properties";
			tblObjProp.description = tblDesc.str();
			
			//Escalation used in container?
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
			webPage.AddContent(CARProplistHelper::GetList(*this->pInside, this->pEscal->objPropList));

			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(this->pEscal, this->rootLevel));

			webPage.SaveInFolder(this->path);
		}
	}
	catch(...)
	{
		cout << "EXCEPTION escalation details common props: " << this->pEscal->name << endl;
	}
}


string CDocEscalationDetails::ContainerReferences()
{
	stringstream strm;
	strm.str("");
	try
	{
		CContainerTable *contTable = new CContainerTable(*this->pInside);

		list<CARContainer>::iterator listContIter;		
		CARContainer *cont;
		for ( listContIter = this->pInside->containerList.begin();  listContIter != this->pInside->containerList.end(); listContIter++ )
		{
			cont = &(*listContIter);
			for(unsigned int nCnt = 0; nCnt < cont->references.numItems; nCnt++)
			{
				if(cont->type != ARCON_APP)
				{
					if(cont->references.referenceList[nCnt].reference.u.name != NULL)
					{
						if(strcmp(cont->references.referenceList[nCnt].reference.u.name, this->pEscal->name.c_str())==0
							&& cont->references.referenceList[nCnt].type == ARREF_ESCALATION)
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
		cout << "EXCEPTION enumerating escalation container references: " << this->pEscal->name << endl;
	}

	return strm.str();
}

string CDocEscalationDetails::CreateSpecific(string schemaName)
{
	stringstream pgStrm;
	pgStrm.str("");

	try
	{
		//Query
		stringstream strmQuery;
		if(this->pEscal->query.operation != NULL)
		{		
			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = AR_STRUCT_ITEM_XML_ESCALATION;
			refItem->description = "Run If";
			refItem->fromName = this->pEscal->name;

			CARQualification arQual(*this->pInside);
			int pFormId = this->pInside->SchemaGetInsideId(schemaName.c_str());
			int sFormId = this->pInside->SchemaGetInsideId(schemaName.c_str());
			arQual.CheckQuery(&this->pEscal->query, *refItem, 0, pFormId, sFormId, strmQuery, rootLevel);
			
			delete refItem;
		}
		else
		{
			strmQuery << EmptyRunIf << endl;
		}

		pgStrm << "Run If Qualification: <br/>" << strmQuery.str();
	   
		//If-Actions		
		CDocFilterActionStruct actionStruct(*this->pInside, *this->pEscal, schemaName, this->path, this->rootLevel, AR_STRUCT_ITEM_XML_ESCALATION);
		pgStrm << actionStruct.Get("If", this->pEscal->actionList);

		//Else-Actions
		pgStrm << actionStruct.Get("Else", this->pEscal->elseList);
	}
	catch(...)
	{
		cout << "EXCEPTION escalation details specific props: " << this->pEscal->name << endl;
	}
	
	return pgStrm.str();
}
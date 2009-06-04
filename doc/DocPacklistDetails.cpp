
/****************************************************************************** 
 * 
 *  file:  DocPacklistDetails.cpp
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
#include ".\docpacklistdetails.h"

CDocPacklistDetails::CDocPacklistDetails(CARInside &arIn, CARContainer &packList)
{
	this->pInside = &arIn;
	this->pPackList = &packList;
	this->rootLevel = 2;
}

CDocPacklistDetails::~CDocPacklistDetails(void)
{
}

void CDocPacklistDetails::Documentation()
{
	try
	{
		string dir = CAREnum::ContainerDir(ARCON_PACK)+"\\"+this->pPackList->FileID();

		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(dir)>=0)
		{
			CWebPage webPage("index", this->pPackList->name, this->rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");
			
			strmHead << CWebUtil::LinkToPackingListIndex(this->rootLevel) + MenuSeparator + CWebUtil::ObjName(this->pPackList->name);
			if(this->pPackList->appRefName.c_str() != NULL && this->pPackList->appRefName.size() > 0)
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->pPackList->appRefName, this->rootLevel);

			webPage.AddContentHead(strmHead.str());

			//Container Base Informations
			CDocContainerHelper *contHelper = new CDocContainerHelper(*this->pInside, *this->pPackList, this->rootLevel);
			webPage.AddContent(contHelper->BaseInfo());
			delete contHelper;

			//Object specific documentation
			webPage.AddContent(PackListInformation());

			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(this->pPackList, this->rootLevel));

			//Save File
			webPage.SaveInFolder(dir);
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in PacklistDetails_Documentation" << endl; 
	}
}

string CDocPacklistDetails::PackListInformation()
{
	//Objects in packing list
	CTable tblProp("specificPropList", "TblObjectList");
	tblProp.AddColumn(20, "Type");
	tblProp.AddColumn(80, "Server Object");
	
	try
	{
		for(unsigned int i=0; i< this->pPackList->references.numItems; i++)
		{			
			stringstream srvType, srvObj;
			srvType.str("");
			srvObj.str("");
			
			switch(this->pPackList->references.referenceList[i].type)
			{							
				if(this->pPackList->references.referenceList[i].reference.u.name != NULL
					&& strcmp(this->pPackList->references.referenceList[i].reference.u.name, "")!=0)
				{
					case ARREF_SCHEMA:
					{
						srvType << CAREnum::ContainerRefType(this->pPackList->references.referenceList[i].type);
						srvObj << this->pInside->LinkToSchema(this->pPackList->references.referenceList[i].reference.u.name, rootLevel);
					}
					break;
					case ARREF_FILTER:
					{
						srvType << CAREnum::ContainerRefType(this->pPackList->references.referenceList[i].type);
						srvObj << this->pInside->LinkToFilter(this->pPackList->references.referenceList[i].reference.u.name, rootLevel);
					}
					break;
					case ARREF_ESCALATION:
					{
						srvType << CAREnum::ContainerRefType(this->pPackList->references.referenceList[i].type);
						srvObj << this->pInside->LinkToEscalation(this->pPackList->references.referenceList[i].reference.u.name, rootLevel);
					}
					break;
					case ARREF_ACTLINK:
					{
						srvType << CAREnum::ContainerRefType(this->pPackList->references.referenceList[i].type);
						srvObj << this->pInside->LinkToAl(this->pPackList->references.referenceList[i].reference.u.name, rootLevel);
					}
					break;
					case ARREF_CONTAINER:
					{
						srvType << CAREnum::ContainerRefType(this->pPackList->references.referenceList[i].type);
						srvObj << this->pInside->LinkToContainer(this->pPackList->references.referenceList[i].reference.u.name, rootLevel);
					}
					break;
					case ARREF_CHAR_MENU:
					{
						srvType << CAREnum::ContainerRefType(this->pPackList->references.referenceList[i].type);
						srvObj << this->pInside->LinkToMenu(this->pPackList->references.referenceList[i].reference.u.name, rootLevel);
					}
					break;
				}
			}
		
			//Avoid empty rows in table
			if(srvType.str().size() > 0)
			{
				CTableRow row("");
				row.AddCell(CTableCell(srvType.str()));
				row.AddCell(CTableCell(srvObj.str()));
				tblProp.AddRow(row);
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in PacklistDetails_BaseDocumentation" << endl; 
	}

	tblProp.description = "Objects in Packing List";
	return tblProp.ToXHtml();
}

/****************************************************************************** 
 * 
 *  file:  DocAlGuideDetails.cpp
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
#include ".\docalguidedetails.h"

CDocAlGuideDetails::CDocAlGuideDetails(CARInside &arIn, CARContainer &alGuide)
{
	this->pInside = &arIn;
	this->pAlGuide = &alGuide;
	this->rootLevel = 2;
}

CDocAlGuideDetails::~CDocAlGuideDetails(void)
{
}

void CDocAlGuideDetails::Documentation()
{
	string dir = CAREnum::ContainerDir(ARCON_GUIDE)+"\\"+pAlGuide->FileID();

	CWindowsUtil winUtil(pInside->appConfig);
	if(winUtil.CreateSubDirectory(dir)>=0)
	{
		CWebPage webPage("index", pAlGuide->name, rootLevel, pInside->appConfig);

		//ContentHead informations
		stringstream strmHead;
		strmHead.str("");
		
		strmHead << CWebUtil::LinkToActiveLinkGuideIndex(this->rootLevel) + MenuSeparator + CWebUtil::ObjName(this->pAlGuide->name);
		if(this->pAlGuide->appRefName.c_str() != NULL && this->pAlGuide->appRefName.size() > 0)
			strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->pAlGuide->appRefName, this->rootLevel);

		webPage.AddContentHead(strmHead.str());

		//Container Base Informations
		CDocContainerHelper *contHelper = new CDocContainerHelper(*this->pInside, *this->pAlGuide, this->rootLevel);
		webPage.AddContent(contHelper->BaseInfo());
		delete contHelper;

		//Object specific documentation
		webPage.AddContent(AlGuideInformation());
		webPage.AddContent(ActiveLinkActions());
		
		//History
		webPage.AddContent(this->pInside->ServerObjectHistory(this->pAlGuide, this->rootLevel));

		//Save File
		webPage.SaveInFolder(dir);
	}
}

string CDocAlGuideDetails::AlGuideInformation()
{
	//Container specific properties
	CTable tblProp("specificPropList", "TblObjectList");
	tblProp.AddColumn(50, "Label");
	tblProp.AddColumn(50, "Active Link in Guide");

	try
	{
		for(unsigned int i=0; i< pAlGuide->references.numItems; i++)
		{
			stringstream label, actLink;
			label.str("");
			actLink.str("");

			switch(pAlGuide->references.referenceList[i].type)
			{
				case ARREF_ACTLINK:
				{
					actLink << pInside->LinkToAl(pAlGuide->references.referenceList[i].reference.u.name, rootLevel);
				}
				break;
				case ARREF_NULL_STRING:
				{
					label << pAlGuide->references.referenceList[i].label;
				}
				break;			
			}

			CTableCell cellLabel(label.str(), "");
			CTableCell cellAl(actLink.str(), "");

			CTableRow row("");
			row.AddCell(cellLabel);
			row.AddCell(cellAl);
			tblProp.AddRow(row);
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in AlGuideInformation" << endl; 
	}

	tblProp.description = "Active Links in Guide";
	return tblProp.ToXHtml();
}

string CDocAlGuideDetails::ActiveLinkActions()
{	
	//Get a list of active links that trigger this al guide
	CTable tblPropEx("exuteInfoList", "TblObjectList");
	tblPropEx.AddColumn(20, "Action");
	tblPropEx.AddColumn(80, "Active Link");

	try
	{
		list<CARActiveLink>::iterator listIter;		
		for ( listIter = pInside->alList.begin(); listIter != pInside->alList.end(); listIter++ )
		{
			CARActiveLink *al = &(*listIter);
			
			//Search if-actions
			for(unsigned int nAction = 0; nAction < al->actionList.numItems; nAction++)
			{
				ARActiveLinkActionStruct action = al->actionList.actionList[nAction];
				if(action.action == AR_ACTIVE_LINK_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, pAlGuide->name.c_str())==0)
					{
						stringstream tmp;
						tmp << "If-Action " << nAction;
						CTableCell cellActionInfo(tmp.str(), "");
						CTableCell cellActiveLink( al->GetURL(rootLevel), "");

						CTableRow row("");
						row.AddCell(cellActionInfo);
						row.AddCell(cellActiveLink);
						tblPropEx.AddRow(row);
					}
				}				
			}

			//Search else-actions
			for(unsigned int nAction = 0; nAction < al->elseList.numItems; nAction++)
			{
				ARActiveLinkActionStruct action = al->elseList.actionList[nAction];
				if(action.action == AR_ACTIVE_LINK_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, pAlGuide->name.c_str())==0)
					{
						stringstream tmp;
						tmp << "Else-Action " << nAction;
						CTableCell cellActionInfo(tmp.str(), "");
						CTableCell cellActiveLink(al->GetURL(rootLevel), "");

						CTableRow row("");
						row.AddCell(cellActionInfo);
						row.AddCell(cellActiveLink);
						tblPropEx.AddRow(row);
					}
				}				
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in AlGuideDetailsEx" << endl; 
	}

	tblPropEx.description = "Active Links 'Call Guide' Action";
	return tblPropEx.ToXHtml();
}

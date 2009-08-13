
/****************************************************************************** 
 * 
 *  file:  DocFilterGuideDetails.cpp
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
#include ".\docfilterguidedetails.h"

CDocFilterGuideDetails::CDocFilterGuideDetails(CARInside &arIn, CARContainer &filterGuide)
{
	this->pInside = &arIn;
	this->pFilterGuide = &filterGuide;
	this->rootLevel = 2;
}

CDocFilterGuideDetails::~CDocFilterGuideDetails(void)
{
}

void CDocFilterGuideDetails::Documentation()
{
	try
	{
		string dir = CAREnum::ContainerDir(ARCON_FILTER_GUIDE)+"\\"+pFilterGuide->FileID();

		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(dir)>=0)
		{
			CWebPage webPage("index", pFilterGuide->name, rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");
			
			strmHead << CWebUtil::LinkToFilterGuideIndex(this->rootLevel) + MenuSeparator + CWebUtil::ObjName(this->pFilterGuide->name);
			if(this->pFilterGuide->appRefName.c_str() != NULL && this->pFilterGuide->appRefName.size() > 0)
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->pFilterGuide->appRefName, this->rootLevel);

			webPage.AddContentHead(strmHead.str());

			//Container Base Informations
			CDocContainerHelper *contHelper = new CDocContainerHelper(*this->pInside, *this->pFilterGuide, this->rootLevel);
			webPage.AddContent(contHelper->BaseInfo());
			delete contHelper;

			//Object specific documentation
			webPage.AddContent(FilterGuideInformation());
			webPage.AddContent(FilterActions());
			
			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(this->pFilterGuide, this->rootLevel));

			//Save File
			webPage.SaveInFolder(dir);
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterGuideDetails_GetPrimaryForm" << endl; 
	}
}

string CDocFilterGuideDetails::FilterGuideInformation()
{
	//Container specific properties
	CTable tblProp("specificPropList", "TblObjectList");
	tblProp.AddColumn(50, "Label");
	tblProp.AddColumn(50, "Filter in Guide");

	try
	{
		for(unsigned int i=0; i< pFilterGuide->references.numItems; i++)
		{
			stringstream label, filter;
			label.str("");
			filter.str("");

			switch(pFilterGuide->references.referenceList[i].type)
			{
				case ARREF_FILTER:
				{
					filter << pInside->LinkToFilter(pFilterGuide->references.referenceList[i].reference.u.name, rootLevel);
				}
				break;
				case ARREF_NULL_STRING:
				{
					label << pFilterGuide->references.referenceList[i].label;
				}
				break;			
			}

			CTableCell cellLabel(label.str(), "");
			CTableCell cellFilter(filter.str(), "");

			CTableRow row("");
			row.AddCell(cellLabel);
			row.AddCell(cellFilter);
			tblProp.AddRow(row);
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterGuideDetails" << endl; 
	}

	tblProp.description = "Filter in Guide";
	return tblProp.ToXHtml();
}

string CDocFilterGuideDetails::FilterActions()
{	
	//Get a list of filter that trigger this filter guide
	CTable tblPropEx("exuteInfoList", "TblObjectList");
	tblPropEx.AddColumn(20, "Action");
	tblPropEx.AddColumn(80, "Filter");

	try
	{
		list<CARFilter>::iterator listIter;

		for ( listIter = pInside->filterList.begin(); listIter != pInside->filterList.end(); listIter++ )
		{
			CARFilter *filter = &(*listIter);
			
			//Search if-actions
			for(unsigned int nAction = 0; nAction < filter->actionList.numItems; nAction++)
			{
				ARFilterActionStruct action = filter->actionList.actionList[nAction];
				if(action.action == AR_FILTER_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, pFilterGuide->name.c_str())==0)
					{
						stringstream tmp;
						tmp << "If-Action " << nAction;
						CTableCell cellActionInfo(tmp.str(), "");
						CTableCell cellFilter(filter->GetURL(this->rootLevel), "");

						CTableRow row("");
						row.AddCell(cellActionInfo);
						row.AddCell(cellFilter);
						tblPropEx.AddRow(row);
					}
				}				
			}

			//Search else-actions
			for(unsigned int nAction = 0; nAction < filter->elseList.numItems; nAction++)
			{
				ARFilterActionStruct action = filter->elseList.actionList[nAction];
				if(action.action == AR_FILTER_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, pFilterGuide->name.c_str())==0)
					{
						stringstream tmp;
						tmp << "Else-Action " << nAction;
						CTableCell cellActionInfo(tmp.str(), "");
						CTableCell cellFilter(filter->GetURL(this->rootLevel), "");

						CTableRow row("");
						row.AddCell(cellActionInfo);
						row.AddCell(cellFilter);
						tblPropEx.AddRow(row);
					}
				}				
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterGuideDetails_FilterActions" << endl; 
	}

	return tblPropEx.ToXHtml();
}

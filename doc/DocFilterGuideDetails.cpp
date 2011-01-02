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
#include "DocFilterGuideDetails.h"

CDocFilterGuideDetails::CDocFilterGuideDetails(CARContainer &filterGuide)
{
	this->pFilterGuide = &filterGuide;
}

CDocFilterGuideDetails::~CDocFilterGuideDetails(void)
{
}

void CDocFilterGuideDetails::Documentation()
{	
	CPageParams file(PAGE_DETAILS, pFilterGuide);
	this->rootLevel = file->GetRootLevel();

	try
	{
		string dir = file->GetPath(); // CAREnum::ContainerDir(ARCON_FILTER_GUIDE)+"/"+pFilterGuide->FileID();

		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(dir)>=0)
		{
			CWebPage webPage(file->GetFileName(), pFilterGuide->GetName(), rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");

			strmHead << CWebUtil::LinkToFilterGuideIndex(this->rootLevel) << MenuSeparator << CWebUtil::ObjName(this->pFilterGuide->GetName());
			if(!this->pFilterGuide->GetAppRefName().empty())
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->pFilterGuide->GetAppRefName(), this->rootLevel);

			webPage.AddContentHead(strmHead.str());

			//Container Base Informations
			CDocContainerHelper *contHelper = new CDocContainerHelper(*this->pFilterGuide, this->rootLevel);
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
	catch(exception& e)
	{
		cout << "EXCEPTION in FilterGuideDetails_GetPrimaryForm: " << e.what() << endl; 
	}
}

string CDocFilterGuideDetails::FilterGuideInformation()
{
	//Container specific properties
	CTable tblProp("specificPropList", "TblObjectList");
	tblProp.AddColumn(20, "Label");
	tblProp.AddColumn(80, "Filter in Guide");

	try
	{
		const ARReferenceList& refs = pFilterGuide->GetReferences();
		for(unsigned int i=0; i< refs.numItems; i++)
		{
			stringstream label, filter;
			label.str("");
			filter.str("");

			switch(refs.referenceList[i].type)
			{
			case ARREF_FILTER:
				{
					filter << pInside->LinkToFilter(refs.referenceList[i].reference.u.name, rootLevel);
				}
				break;
			case ARREF_NULL_STRING:
				{
					label << refs.referenceList[i].label;
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
	catch(exception& e)
	{
		cout << "EXCEPTION in FilterGuideDetails: " << e.what() << endl; 
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
		unsigned int filterCount = pInside->filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex )
		{
			CARFilter filter(filterIndex);

			//Search if-actions
			const ARFilterActionList& IfActions =	filter.GetIfActions();
			for(unsigned int nAction = 0; nAction < IfActions.numItems; ++nAction)
			{
				const ARFilterActionStruct& action = IfActions.actionList[nAction];
				if(action.action == AR_FILTER_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, pFilterGuide->GetName().c_str())==0)
					{
						stringstream tmp;
						tmp << "If-Action " << nAction;
						CTableCell cellActionInfo(tmp.str(), "");
						CTableCell cellFilter(filter.GetURL(this->rootLevel), "");

						CTableRow row("");
						row.AddCell(cellActionInfo);
						row.AddCell(cellFilter);
						tblPropEx.AddRow(row);
					}
				}				
			}

			//Search else-actions
			const ARFilterActionList& ElseActions =	filter.GetElseActions();
			for(unsigned int nAction = 0; nAction < ElseActions.numItems; nAction++)
			{
				ARFilterActionStruct action = ElseActions.actionList[nAction];
				if(action.action == AR_FILTER_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, pFilterGuide->GetName().c_str())==0)
					{
						stringstream tmp;
						tmp << "Else-Action " << nAction;
						CTableCell cellActionInfo(tmp.str(), "");
						CTableCell cellFilter(filter.GetURL(this->rootLevel), "");

						CTableRow row("");
						row.AddCell(cellActionInfo);
						row.AddCell(cellFilter);
						tblPropEx.AddRow(row);
					}
				}				
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in FilterGuideDetails_FilterActions: " << e.what() << endl; 
	}

	return tblPropEx.ToXHtml();
}

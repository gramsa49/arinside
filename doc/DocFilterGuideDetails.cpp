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
#include "DocOverlayHelper.h"

CDocFilterGuideDetails::CDocFilterGuideDetails(CARContainer &fltGuide)
: filterGuide(fltGuide)
{
}

CDocFilterGuideDetails::~CDocFilterGuideDetails(void)
{
}

void CDocFilterGuideDetails::Documentation()
{	
	CPageParams file(PAGE_DETAILS, &filterGuide);
	this->rootLevel = file->GetRootLevel();

	try
	{
		string dir = file->GetPath(); // CAREnum::ContainerDir(ARCON_FILTER_GUIDE)+"/"+pFilterGuide->FileID();

		FileSystemUtil fsUtil(this->pInside->appConfig);
		if(fsUtil.CreateSubDirectory(dir)>=0)
		{
			CWebPage webPage(file->GetFileName(), filterGuide.GetName(), rootLevel, this->pInside->appConfig);
			CDocOverlayHelper overlayHelper(filterGuide, rootLevel);

			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");
			int overlayType = this->filterGuide.GetOverlayType();

			strmHead << CWebUtil::LinkToFilterGuideIndex(this->rootLevel) << MenuSeparator << CWebUtil::ObjName(this->filterGuide.GetName()) << CAREnum::GetOverlayTypeString(overlayType);

			if(!this->filterGuide.GetAppRefName().empty())
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->filterGuide.GetAppRefName(), this->rootLevel);

			webPage.AddContentHead(strmHead.str(), overlayHelper.PlaceOverlayLink());
			webPage.AddContent(overlayHelper.PlaceOverlaidNotice());

			//Container Base Informations
			CDocContainerHelper *contHelper = new CDocContainerHelper(this->filterGuide, this->rootLevel);
			webPage.AddContent(contHelper->BaseInfo());
			delete contHelper;

			//Object specific documentation
			webPage.AddContent(FilterActions());

			//Workflow References
			webPage.AddContent(WorkflowReferences());

			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(&this->filterGuide, this->rootLevel));

			//Save File
			webPage.SaveInFolder(dir);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in FilterGuideDetails_GetPrimaryForm: " << e.what() << endl; 
	}
}

string CDocFilterGuideDetails::FilterActions()
{	
	//Get a list of filter that trigger this filter guide
	CTable tblPropEx("exuteInfoList", "TblObjectList");
	tblPropEx.AddColumn(20, "Action");
	tblPropEx.AddColumn(80, "Filter");

	try
	{
		// TODO: Move this loop to the scan phase
		unsigned int filterCount = pInside->filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex )
		{
			CARFilter filter(filterIndex);

			// skip this object in case it's overlaid (hidden)
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(filter))
				continue;

			//Search if-actions
			const ARFilterActionList& IfActions =	filter.GetIfActions();
			for(unsigned int nAction = 0; nAction < IfActions.numItems; ++nAction)
			{
				const ARFilterActionStruct& action = IfActions.actionList[nAction];
				if(action.action == AR_FILTER_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, filterGuide.GetName().c_str())==0)
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
					if(strcmp(action.u.callGuide.guideName, filterGuide.GetName().c_str())==0)
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

	tblPropEx.description = "Filters calling this guide";
	return tblPropEx.ToXHtml();
}

string CDocFilterGuideDetails::WorkflowReferences()
{
	stringstream strm;
	strm.str("");

	try
	{
		CTable tblRef("referenceList", "TblObjectList");
		tblRef.AddColumn(10, "Type");
		tblRef.AddColumn(45, "Server object");
		tblRef.AddColumn(5, "Enabled");
		tblRef.AddColumn(40, "Description");

		const CARContainer::ReferenceList& refs = filterGuide.GetReferences();
		CARContainer::ReferenceList::const_iterator curIt = refs.begin();
		CARContainer::ReferenceList::const_iterator endIt = refs.end();
		for ( ; curIt != endIt; ++curIt)
		{			
			CTableRow row("cssStdRow");		
			row.AddCell(CAREnum::XmlStructItem(curIt->GetObjectType()));				
			row.AddCell(pInside->LinkToObjByRefItem(*curIt, rootLevel));

			string tmpEnabled = "";
			string tmpCssEnabled = "";

			bool enabledSupported = false;
			int enabled = curIt->GetObjectEnabled(enabledSupported);

			if (enabledSupported)
			{
				tmpEnabled = CAREnum::ObjectEnable(enabled);
				if (!enabled) { tmpCssEnabled = "objStatusDisabled"; }
			}

			row.AddCell(CTableCell(tmpEnabled, tmpCssEnabled));
			row.AddCell(curIt->GetDescription(rootLevel));
			tblRef.AddRow(row);
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Workflow Reference";

		tblRef.description = tblDesc.str();

		strm << tblRef;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating workflow references for filterguide: " << filterGuide.GetName() << " -- " << e.what() << endl;
	}	

	return strm.str();
}

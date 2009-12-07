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
#include "DocAlGuideDetails.h"

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
	string dir = CAREnum::ContainerDir(ARCON_GUIDE)+"/"+pAlGuide->FileID();

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
	catch(exception& e)
	{
		cout << "EXCEPTION in AlGuideInformation: " << e.what() << endl; 
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
		unsigned int alCount = pInside->alList.GetCount();
		for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex)
		{
			CARActiveLink al(alIndex);

			//Search if-actions
			for(unsigned int nAction = 0; nAction < al.GetIfActions().numItems; nAction++)
			{
				const ARActiveLinkActionStruct &action = al.GetIfActions().actionList[nAction];
				if(action.action == AR_ACTIVE_LINK_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, pAlGuide->name.c_str())==0)
					{
						stringstream tmp;
						tmp << "If-Action " << nAction;
						CTableCell cellActionInfo(tmp.str(), "");
						CTableCell cellActiveLink(al.GetURL(rootLevel), "");

						CTableRow row("");
						row.AddCell(cellActionInfo);
						row.AddCell(cellActiveLink);
						tblPropEx.AddRow(row);
					}
				}
			}

			//Search else-actions
			for(unsigned int nAction = 0; nAction < al.GetElseActions().numItems; nAction++)
			{
				const ARActiveLinkActionStruct &action = al.GetElseActions().actionList[nAction];
				if(action.action == AR_ACTIVE_LINK_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, pAlGuide->name.c_str())==0)
					{
						stringstream tmp;
						tmp << "Else-Action " << nAction;
						CTableCell cellActionInfo(tmp.str(), "");
						CTableCell cellActiveLink(al.GetURL(rootLevel), "");

						CTableRow row("");
						row.AddCell(cellActionInfo);
						row.AddCell(cellActiveLink);
						tblPropEx.AddRow(row);
					}
				}
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlGuideDetailsEx: " << e.what() << endl; 
	}

	tblPropEx.description = "Active Links 'Call Guide' Action";
	return tblPropEx.ToXHtml();
}

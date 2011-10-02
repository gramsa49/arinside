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

CDocAlGuideDetails::CDocAlGuideDetails(CARContainer &container)
: alGuide(container)
{
}

CDocAlGuideDetails::~CDocAlGuideDetails(void)
{
}

void CDocAlGuideDetails::Documentation()
{
	CPageParams file(PAGE_DETAILS, &alGuide);
	this->rootLevel = file->GetRootLevel();

	string dir = file->GetPath(); //CAREnum::ContainerDir(ARCON_GUIDE)+"/"+alGuide.FileID();

	try
	{
		CWindowsUtil winUtil(pInside->appConfig);
		if(winUtil.CreateSubDirectory(dir)>=0)
		{
			CWebPage webPage(file->GetFileName(), alGuide.GetName(), this->rootLevel, pInside->appConfig);

			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");

			strmHead << CWebUtil::LinkToActiveLinkGuideIndex(file->GetRootLevel()) << MenuSeparator << CWebUtil::ObjName(this->alGuide.GetName());

			bool placeOverlayNote = false;
			CARContainer overlayObj;
			stringstream overlayLink;

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport)
			{
				ARValueStruct* overlayProp = CARProplistHelper::Find(alGuide.GetPropList(), AR_SMOPROP_OVERLAY_PROPERTY);
				if (overlayProp != NULL && overlayProp->dataType == AR_DATA_TYPE_INTEGER)
				{
					switch (overlayProp->u.intVal)
					{
					case AR_OVERLAID_OBJECT:
						{
							CARContainer ovlALG(alGuide.GetName() + (this->pInside->overlayMode == 0 ? AR_RESERV_OVERLAY_STRING : ""));
							strmHead << " (Base)";
							overlayLink << "Overlay: " << (ovlALG.Exists() ? ovlALG.GetURL(rootLevel, false) : alGuide.GetName());

							if (pInside->overlayMode == 1) // only if the server does execute custom- and overlay-objects
							{
								placeOverlayNote = true;
								overlayObj = ovlALG;
							}
						}
						break;
					case AR_OVERLAY_OBJECT:
						{
							CARContainer oriALG(alGuide.GetName() + (this->pInside->overlayMode == 1 ? AR_RESERV_OVERLAY_STRING : ""));
							strmHead << " (Overlay)";
							overlayLink << "Based on: " << (oriALG.Exists() ? oriALG.GetURL(rootLevel, false) : alGuide.GetName());
						}
						break;
					}
				}
			}
#endif

			if(!this->alGuide.GetAppRefName().empty())
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->alGuide.GetAppRefName(), this->rootLevel);

			webPage.AddContentHead(strmHead.str(), overlayLink.str());

			if (placeOverlayNote)
				webPage.AddContent(pInside->PlaceOverlaidNotice(overlayObj, rootLevel));

			//Container Base Informations
			CDocContainerHelper *contHelper = new CDocContainerHelper(this->alGuide, this->rootLevel);
			webPage.AddContent(contHelper->BaseInfo());
			delete contHelper;

			//Object specific documentation
			webPage.AddContent(ActiveLinkActions());

			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(&this->alGuide, this->rootLevel));

			//Save File
			webPage.SaveInFolder(file->GetPath());
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in DocAlGuideDetails::Documentation of '" << this->alGuide.GetName() << "': " << e.what() << endl;
	}
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
					if(strcmp(action.u.callGuide.guideName, alGuide.GetName().c_str())==0)
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
					if(strcmp(action.u.callGuide.guideName, alGuide.GetName().c_str())==0)
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

	tblPropEx.description = "Active Links calling this guide";
	return tblPropEx.ToXHtml();
}

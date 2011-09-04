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
#include "DocPacklistDetails.h"
#include "../core/ARImage.h"

CDocPacklistDetails::CDocPacklistDetails(CARContainer &packList)
: pPackList(packList)
{
}

CDocPacklistDetails::~CDocPacklistDetails(void)
{
}

void CDocPacklistDetails::Documentation()
{
	CPageParams file(PAGE_DETAILS, &pPackList);
	this->rootLevel = file->GetRootLevel();

	try
	{
		string dir = file->GetPath();

		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(dir)>=0)
		{
			CWebPage webPage(file->GetFileName(), this->pPackList.GetName(), this->rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");

			strmHead << CWebUtil::LinkToPackingListIndex(this->rootLevel) << MenuSeparator << CWebUtil::ObjName(this->pPackList.GetName());

			bool placeOverlayNote = false;
			CARContainer overlayObj;

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			ARValueStruct* overlayProp = CARProplistHelper::Find(pPackList.GetPropList(), AR_SMOPROP_OVERLAY_PROPERTY);
			if (overlayProp != NULL && overlayProp->dataType == AR_DATA_TYPE_INTEGER)
			{
				switch (overlayProp->u.intVal)
				{
				case AR_OVERLAID_OBJECT:
					{
						CARContainer ovlPkl(pPackList.GetName() + (this->pInside->overlayMode == 0 ? AR_RESERV_OVERLAY_STRING : ""));
						strmHead << " (Overlaid by " << (ovlPkl.Exists() ? ovlPkl.GetURL(rootLevel, false) : "") << ")";

						if (pInside->overlayMode == 1) // only if the server does execute custom- and overlay-objects
						{
							placeOverlayNote = true;
							overlayObj = ovlPkl;
						}
					}
					break;
				case AR_OVERLAY_OBJECT:
					{
						CARContainer oriPkl(pPackList.GetName() + (this->pInside->overlayMode == 1 ? AR_RESERV_OVERLAY_STRING : ""));
						strmHead << " (Overlay of " << (oriPkl.Exists() ? oriPkl.GetURL(rootLevel, false) : "") << ")";
					}
					break;
				}
			}
#endif

			if(!this->pPackList.GetAppRefName().empty())
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->pPackList.GetAppRefName(), this->rootLevel);

			webPage.AddContentHead(strmHead.str());

			if (placeOverlayNote)
				webPage.AddContent(pInside->PlaceOverlaidNotice(overlayObj, rootLevel));

			//Container Base Informations
			CDocContainerHelper *contHelper = new CDocContainerHelper(this->pPackList, this->rootLevel);
			webPage.AddContent(contHelper->BaseInfo());
			delete contHelper;

			//Object specific documentation
			webPage.AddContent(PackListInformation());

			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(&this->pPackList, this->rootLevel));

			//Save File
			webPage.SaveInFolder(dir);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in PacklistDetails_Documentation: " << e.what() << endl; 
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
		const ARReferenceList& refs = this->pPackList.GetReferences();
		for(unsigned int i=0; i< refs.numItems; i++)
		{			
			stringstream srvType, srvObj;
			srvType.str("");
			srvObj.str("");

			switch(refs.referenceList[i].type)
			{							
				if(refs.referenceList[i].reference.u.name != NULL && refs.referenceList[i].reference.u.name[0] != 0)
				{
			case ARREF_SCHEMA:
				{
					srvType << CAREnum::ContainerRefType(refs.referenceList[i].type);
					srvObj << this->pInside->LinkToSchema(refs.referenceList[i].reference.u.name, rootLevel);
				}
				break;
			case ARREF_FILTER:
				{
					srvType << CAREnum::ContainerRefType(refs.referenceList[i].type);
					srvObj << this->pInside->LinkToFilter(refs.referenceList[i].reference.u.name, rootLevel);
				}
				break;
			case ARREF_ESCALATION:
				{
					srvType << CAREnum::ContainerRefType(refs.referenceList[i].type);
					srvObj << this->pInside->LinkToEscalation(refs.referenceList[i].reference.u.name, rootLevel);
				}
				break;
			case ARREF_ACTLINK:
				{
					srvType << CAREnum::ContainerRefType(refs.referenceList[i].type);
					srvObj << this->pInside->LinkToAl(refs.referenceList[i].reference.u.name, rootLevel);
				}
				break;
			case ARREF_CONTAINER:
				{
					bool found = false;
					CARContainer container(refs.referenceList[i].reference.u.name);
					if (container.Exists())
						srvType << CAREnum::ContainerType(container.GetType());
					else
						srvType << CAREnum::ContainerRefType(refs.referenceList[i].type);
				
					srvObj << this->pInside->LinkToContainer(refs.referenceList[i].reference.u.name, rootLevel);
				}
				break;
			case ARREF_CHAR_MENU:
				{
					srvType << CAREnum::ContainerRefType(refs.referenceList[i].type);
					srvObj << this->pInside->LinkToMenu(refs.referenceList[i].reference.u.name, rootLevel);

					CRefItem refItem(this->pPackList, REFM_PACKINGLIST);
					pInside->AddMenuReference(refs.referenceList[i].reference.u.name, refItem);
				}
				break;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
			case ARREF_IMAGE:
				{
					srvType << CAREnum::ContainerRefType(refs.referenceList[i].type);

					CARImage image(refs.referenceList[i].reference.u.name);
					if (!image.Exists())
					{
						srvObj << this->pInside->LinkToImage(refs.referenceList[i].reference.u.name, rootLevel);
					}
					else
					{
						srvObj << this->pInside->LinkToImage(image.GetInsideId(), rootLevel);
						CRefItem refItem(pPackList, REFM_PACKINGLIST);
						image.AddReference(refItem);
					}
				}
				break;
#endif
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
	catch(exception& e)
	{
		cout << "EXCEPTION in PacklistDetails_BaseDocumentation: " << e.what() << endl; 
	}

	tblProp.description = "Objects in Packing List";
	return tblProp.ToXHtml();
}

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
#include "DocOverlayHelper.h"

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

		FileSystemUtil fsUtil(this->pInside->appConfig);
		if(fsUtil.CreateSubDirectory(dir)>=0)
		{
			CWebPage webPage(file->GetFileName(), this->pPackList.GetName(), this->rootLevel, this->pInside->appConfig);
			CDocOverlayHelper overlayHelper(this->pPackList, rootLevel);
			
			//ContentHead informations
			stringstream strmHead;
			strmHead.str("");
			int overlayType = pPackList.GetOverlayType();

			strmHead << CWebUtil::LinkToPackingListIndex(this->rootLevel) << MenuSeparator << CWebUtil::ObjName(this->pPackList.GetName()) << CAREnum::GetOverlayTypeString(overlayType);

			if(!this->pPackList.GetAppRefName().empty())
				strmHead << MenuSeparator << " Application " << this->pInside->LinkToContainer(this->pPackList.GetAppRefName(), this->rootLevel);

			webPage.AddContentHead(strmHead.str(), overlayHelper.PlaceOverlayLink());
			webPage.AddContent(overlayHelper.PlaceOverlaidNotice());

			//Container Base Informations
			CDocContainerHelper *contHelper = new CDocContainerHelper(this->pPackList, this->rootLevel);
			webPage.AddContent(contHelper->BaseInfo());
			delete contHelper;

			//Object specific documentation
			webPage.AddContent(PackListInformation());

			//Workflow References
			webPage.AddContent(WorkflowReferences());

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
		const ARReferenceList& refs = this->pPackList.GetContent();
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
					srvObj << CWebUtil::LinkToMenu(refs.referenceList[i].reference.u.name, rootLevel);

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

string CDocPacklistDetails::WorkflowReferences()
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

		const CARContainer::ReferenceList& refs = this->pPackList.GetReferences();
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
		cout << "EXCEPTION enumerating workflow references for packing list: " << this->pPackList.GetName() << " -- " << e.what() << endl;
	}	

	return strm.str();
}

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
		string dir = CAREnum::ContainerDir(ARCON_PACK)+"/"+this->pPackList->FileID();

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
					bool found = false;
					list<CARContainer>::iterator contIter;
					for ( contIter = pInside->containerList.begin(); contIter != pInside->containerList.end() && found == false; contIter++ )
					{	
						CARContainer *container = &(*contIter);
						if(strcmp(this->pPackList->references.referenceList[i].reference.u.name, container->name.c_str())==0)
						{
							srvType << CAREnum::ContainerType(container->type);
							found = true;
						}
					}
					if (!found)
						srvType << CAREnum::ContainerRefType(this->pPackList->references.referenceList[i].type);
					
					srvObj << this->pInside->LinkToContainer(this->pPackList->references.referenceList[i].reference.u.name, rootLevel);
				}
				break;
			case ARREF_CHAR_MENU:
				{
					bool menuFound = false;
					srvType << CAREnum::ContainerRefType(this->pPackList->references.referenceList[i].type);
					srvObj << this->pInside->LinkToMenu(this->pPackList->references.referenceList[i].reference.u.name, rootLevel, &menuFound);

					if (!menuFound)
					{
						CMissingMenuRefItem refItemNotFound(this->pPackList->references.referenceList[i].reference.u.name, AR_STRUCT_ITEM_XML_CONTAINER, this->pPackList->name);
						pInside->AddMissingMenu(refItemNotFound);
					}
				}
				break;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
			case ARREF_IMAGE:
				{
					srvType << CAREnum::ContainerRefType(this->pPackList->references.referenceList[i].type);

					int imageIndex = pInside->imageList.FindImage(this->pPackList->references.referenceList[i].reference.u.name);
					if (imageIndex < 0)
						srvObj << this->pInside->LinkToImage(this->pPackList->references.referenceList[i].reference.u.name, rootLevel);
					else
						srvObj << this->pInside->LinkToImage(imageIndex, rootLevel);

					if (imageIndex >= 0)
					{
						CImageRefItem refItem(imageIndex, "Contained in packing list", pPackList);
						pInside->imageList.AddReference(refItem);
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

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
#include "DocWebserviceDetails.h"

CDocWebserviceDetails::CDocWebserviceDetails(CARInside &arIn, CARContainer &obj)
{
	this->pInside = &arIn;
	this->pWs = &obj;
	this->rootLevel = 2;
}

CDocWebserviceDetails::~CDocWebserviceDetails(void)
{
}

void CDocWebserviceDetails::Documentation()
{
	try
	{
		string dir = CAREnum::ContainerDir(ARCON_WEBSERVICE)+"\\"+pWs->FileID();

		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(dir)>=0)
		{
			CWebPage webPage("index", pWs->name, rootLevel, pInside->appConfig);

			//ContentHead informations
			webPage.AddContentHead(CWebUtil::LinkToWebServiceIndex(this->rootLevel) + MenuSeparator + CWebUtil::ObjName(this->pWs->name) + " (Common Properties)");

			//Container Base Informations
			CDocContainerHelper *contHelper = new CDocContainerHelper(*this->pInside, *this->pWs, this->rootLevel);
			webPage.AddContent(contHelper->BaseInfo());
			delete contHelper;

			//Object specific documentation
			webPage.AddContent(WSInformation());

			//History
			webPage.AddContent(this->pInside->ServerObjectHistory(this->pWs, this->rootLevel));

			//Save File
			webPage.SaveInFolder(dir);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in WebserviceDetails_Documentation: " << e.what() << endl; 
	}
}

string CDocWebserviceDetails::WSInformation()
{
	//Container specific properties
	CTable tblProp("specificPropList", "TblObjectList");
	tblProp.AddColumn(30, "Description");
	tblProp.AddColumn(70, "Value");

	//Label
	try
	{
		if(pWs->label != NULL)
		{
			CTableCell cellWsDesc("Label", "");
			CTableCell cellWsValue(pWs->label, "");

			CTableRow row("");
			row.AddCell(cellWsDesc);
			row.AddCell(cellWsValue);
			tblProp.AddRow(row);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in WebserviceDetails_BaseDocumentation (Label): " << e.what() << endl; 
	}

	//Description
	try
	{
		if(pWs->description != NULL)
		{
			CTableCell cellWsDesc("Description", "");
			CTableCell cellWsValue(pWs->description, "");

			CTableRow row("");
			row.AddCell(cellWsDesc);
			row.AddCell(cellWsValue);
			tblProp.AddRow(row);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in WebserviceDetails_BaseDocumentation (Description): " << e.what() << endl; 
	}


	//Properties
	try
	{
		if(pWs->references.referenceList != NULL)
		{
			for(unsigned int i=0; i< pWs->references.numItems; i++)
			{
				stringstream strmWsDesc, strmWsValue;
				strmWsDesc.str("");
				strmWsValue.str("");

				ARReferenceStruct ref = pWs->references.referenceList[i];
				switch(ref.type)
				{
				case ARREF_WS_PROPERTIES:
					{
						strmWsDesc << "Property";                    
						switch(ref.reference.u.extRef.value.dataType)
						{
						case AR_DATA_TYPE_CHAR:
							{

								try
								{				
									int schemaId = pInside->SchemaGetInsideId(pWs->ownerObjList.ownerObjList[0].ownerName);

									CFieldRefItem *refItemMap = new CFieldRefItem();
									refItemMap->arsStructItemType = AR_STRUCT_ITEM_XML_CONTAINER;
									refItemMap->fromName = pWs->name;						
									refItemMap->schemaInsideId = schemaId;
									refItemMap->description = "Webservice Property";

									string tmpValue = CWebUtil::Validate(ref.reference.u.extRef.value.u.charVal);							
									strmWsValue << "<pre class=\"preWsInfo\">" << pInside->XMLFindFields(tmpValue, schemaId, rootLevel, refItemMap) << "</pre>";								

									delete refItemMap;
								}
								catch(...)
								{
								}								

							}
							break;
						}
					}
					break;
				case ARREF_WS_OPERATION:
					{
						strmWsDesc << "Operation";
						switch(ref.reference.u.extRef.value.dataType)
						{
						case AR_DATA_TYPE_CHAR:
							{
								try
								{				
									int schemaId = pInside->SchemaGetInsideId(pWs->ownerObjList.ownerObjList[0].ownerName);

									CFieldRefItem *refItemMap = new CFieldRefItem();
									refItemMap->arsStructItemType = AR_STRUCT_ITEM_XML_CONTAINER;
									refItemMap->fromName = pWs->name;						
									refItemMap->schemaInsideId = schemaId;
									refItemMap->description = "Webservice Operation";

									string tmpValue = CWebUtil::Validate(ref.reference.u.extRef.value.u.charVal);							
									strmWsValue << "<pre class=\"preWsInfo\">" << pInside->XMLFindFields(tmpValue, schemaId, rootLevel, refItemMap) << "</pre>";								

									delete refItemMap;
								}
								catch(...)
								{
								}
							}
							break;
						}
					}
					break;
				case ARREF_WS_ARXML_MAPPING:
					{
						strmWsDesc << "Mapping";
						switch(ref.reference.u.extRef.value.dataType)
						{
						case AR_DATA_TYPE_CHAR:
							{
								try
								{				
									int schemaId = pInside->SchemaGetInsideId(pWs->ownerObjList.ownerObjList[0].ownerName);

									CFieldRefItem *refItemMap = new CFieldRefItem();
									refItemMap->arsStructItemType = AR_STRUCT_ITEM_XML_CONTAINER;
									refItemMap->fromName = pWs->name;						
									refItemMap->schemaInsideId = schemaId;
									refItemMap->description = "Webservice Field Mapping";

									string tmpValue = CWebUtil::Validate(ref.reference.u.extRef.value.u.charVal);							
									strmWsValue << "<pre class=\"preWsInfo\">" << pInside->XMLFindFields(tmpValue, schemaId, rootLevel, refItemMap) << "</pre>";								

									delete refItemMap;
								}
								catch(...)
								{
								}
							}
							break;
						}
					}
					break;
				case ARREF_WS_WSDL:
					{
						strmWsDesc << "WSDL";
						switch(ref.reference.u.extRef.value.dataType)
						{
						case AR_DATA_TYPE_CHAR:
							{																
								if(ref.reference.u.extRef.value.u.charVal != NULL)
								{
									strmWsValue << "<pre class=\"preWsInfo\">" << CWebUtil::Validate(ref.reference.u.extRef.value.u.charVal) << "</pre>";								
								}
							}
							break;
						}
					}
					break;
				case ARREF_WS_PUBLISHING_LOC:
					{
						strmWsDesc << "Publishing Location";
						switch(ref.reference.u.extRef.value.dataType)
						{
						case AR_DATA_TYPE_CHAR:
							{
								if(ref.reference.u.extRef.value.u.charVal != NULL)
								{								
									strmWsValue << "<pre class=\"preWsInfo\">" << CWebUtil::Validate(ref.reference.u.extRef.value.u.charVal) << "</pre>";								
								}
							}
							break;
						}
					}
					break;
				case ARREF_WS_XML_SCHEMA_LOC:
					{
						strmWsDesc << "XML Schema";
						switch(ref.reference.u.extRef.value.dataType)
						{
						case AR_DATA_TYPE_CHAR:
							{
								if(ref.reference.u.extRef.value.u.charVal != NULL)
								{								
									strmWsValue << "<pre class=\"preWsInfo\">" << CWebUtil::Validate(ref.reference.u.extRef.value.u.charVal) << "</pre>";								
								}
							}
							break;
						}
					}
					break;
				}

				CTableRow row("");
				row.AddCell(CTableCell(strmWsDesc.str()));
				row.AddCell(CTableCell(strmWsValue.str()));
				tblProp.AddRow(row);
			}
		}

	}
	catch(exception& e)
	{
		cout << "EXCEPTION in WebserviceDetails_BaseDocumentation: " << e.what() << endl; 
	}

	return tblProp.ToXHtml();
}

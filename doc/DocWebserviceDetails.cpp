
/****************************************************************************** 
 * 
 *  file:  DocWebserviceDetails.cpp
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
#include ".\docwebservicedetails.h"

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
	catch(...)
	{
		cout << "EXCEPTION in WebserviceDetails_Documentation" << endl; 
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
	catch(...)
	{
		cout << "EXCEPTION in WebserviceDetails_BaseDocumentation (Label)" << endl; 
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
	catch(...)
	{
		cout << "EXCEPTION in WebserviceDetails_BaseDocumentation (Description)" << endl; 
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
	catch(...)
	{
		cout << "EXCEPTION in WebserviceDetails_BaseDocumentation" << endl; 
	}

	return tblProp.ToXHtml();
}
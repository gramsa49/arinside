
/****************************************************************************** 
 * 
 *  file:  DocVuiDetails.cpp
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
#include ".\docvuidetails.h"
#include "../output/csvpage.h"

CDocVuiDetails::CDocVuiDetails(CARInside &arInside, CARSchema &schema, CARVui &vui, string path, int rootLevel)
{
	this->pInside = &arInside;
	this->pSchema = &schema;
	this->pVui = &vui;
	this->path = path;
	this->rootLevel = rootLevel;
}

CDocVuiDetails::~CDocVuiDetails(void)
{
}

void CDocVuiDetails::Documentation()
{
	try
	{
		string fName = "vui_"+this->pVui->FileID();
		CWebPage webPage(fName, this->pVui->name, this->rootLevel, this->pInside->appConfig);

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToSchemaIndex(this->rootLevel) << endl;
		contHeadStrm << MenuSeparator << this->pInside->LinkToSchemaTypeList(this->pSchema->schema.schemaType, this->rootLevel) << endl;
		contHeadStrm << MenuSeparator << CWebUtil::Link(this->pSchema->name, CWebUtil::DocName("index"), "", rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::Link("View", CWebUtil::DocName("form_vui_list"), "", rootLevel) << endl;
		contHeadStrm << MenuSeparator << CWebUtil::ObjName(this->pVui->name) << endl;
		contHeadStrm << " (Id: " << this->pVui->insideId << ")" << endl;
		webPage.AddContentHead(contHeadStrm.str());

		//Properties
		webPage.AddContent(CARProplistHelper::GetList(*this->pInside, this->pVui->objPropList));

		webPage.AddContent(this->FieldProperties(fName).ToXHtml());

		webPage.AddContent(this->pInside->ServerObjectHistory(this->pVui, this->rootLevel));
		webPage.SaveInFolder(path);	

		//Save field information to ccs
		CCsvPage csvPage("vui_"+this->pVui->FileID(), this->pInside->appConfig);
		csvPage.SaveInFolder(path, this->FieldPropertiesCsv(fName).ToCsv());
		
	}
	catch(...)
	{
		cout << "EXCEPTION vui details documentation"<< endl;
	}
}

CTable CDocVuiDetails::FieldProperties(string fName)
{
	CTable tbl("fieldListIndex", "TblObjectList");
	tbl.AddColumn(25, "Field Name");
	tbl.AddColumn(10, "Field Id");
	tbl.AddColumn(20, "Label in View");
	tbl.AddColumn(10, "Datatype");
	tbl.AddColumn(20, "Modified");
	tbl.AddColumn(15, "By");

	try
	{
		list<CARField>::iterator fieldIter;				
		for( fieldIter = this->pSchema->fieldList.begin(); fieldIter != this->pSchema->fieldList.end(); fieldIter++)
		{
			bool bInView = false;
			stringstream tmpLabel;
			tmpLabel.str("");

			CARField *field = &(*fieldIter);
			for(unsigned int i=0; i< field->dInstanceList.numItems; i++)
			{
				if(field->dInstanceList.dInstanceList[i].vui == this->pVui->insideId)
				{
					for(unsigned int j=0; j< field->dInstanceList.dInstanceList[i].props.numItems; j++)
					{
						switch (field->dInstanceList.dInstanceList[i].props.props[j].prop)
						{	
							case AR_DPROP_BUTTON_TEXT:
							case AR_DPROP_LABEL:
								{
									if(field->dInstanceList.dInstanceList[i].props.props[j].value.u.charVal != NULL)
									{
										tmpLabel << field->dInstanceList.dInstanceList[i].props.props[j].value.u.charVal;	
									}
								}
							break;												
						}
					}	

					CTableRow row("");
					row.AddCell(CTableCell( field->GetURL(2)));
					row.AddCell(CTableCell(field->fieldId));
					row.AddCell(CTableCell(tmpLabel.str()));				
					row.AddCell(CTableCell(CAREnum::DataType(field->dataType)));				
					row.AddCell(CTableCell(CUtil::TimeToString(field->timestamp)));
					row.AddCell(CTableCell(this->pInside->LinkToUser(field->lastChanged, 1)));
					tbl.AddRow(row);
				}
			}
		}

		stringstream tblDesc;
		tblDesc << tbl.NumRows() << " fields in view (" << CWebUtil::Link("data", CWebUtil::CsvDocName(fName), "", 0) << ")" <<  endl;
		tbl.description = tblDesc.str();
	}
	catch(...)
	{
		cout << "EXCEPTION enumerating field properties in view: " << this->pVui->name << endl;
	}

	return tbl;
}


CTable CDocVuiDetails::FieldPropertiesCsv(string fName)
{
	CTable tbl("fieldListIndex", "TblObjectList");
	tbl.AddColumn(25, "Field Name");
	tbl.AddColumn(10, "Field Id");
	tbl.AddColumn(20, "Label in View");
	tbl.AddColumn(10, "Datatype");
	tbl.AddColumn(20, "Modified");
	tbl.AddColumn(15, "By");

	try
	{
		list<CARField>::iterator fieldIter;				
		for( fieldIter = this->pSchema->fieldList.begin(); fieldIter != this->pSchema->fieldList.end(); fieldIter++)
		{
			bool bInView = false;
			stringstream tmpLabel;
			tmpLabel.str("");

			CARField *field = &(*fieldIter);
			for(unsigned int i=0; i< field->dInstanceList.numItems; i++)
			{
				if(field->dInstanceList.dInstanceList[i].vui == this->pVui->insideId)
				{
					for(unsigned int j=0; j< field->dInstanceList.dInstanceList[i].props.numItems; j++)
					{
						switch (field->dInstanceList.dInstanceList[i].props.props[j].prop)
						{	
							case AR_DPROP_BUTTON_TEXT:
							case AR_DPROP_LABEL:
								{
									if(field->dInstanceList.dInstanceList[i].props.props[j].value.u.charVal != NULL)
									{
										tmpLabel << field->dInstanceList.dInstanceList[i].props.props[j].value.u.charVal;	
									}
								}
							break;												
						}
					}	

					CTableRow row("");
					row.AddCell(CTableCell(field->name));
					row.AddCell(CTableCell(field->fieldId));
					row.AddCell(CTableCell(tmpLabel.str()));				
					row.AddCell(CTableCell(CAREnum::DataType(field->dataType)));				
					row.AddCell(CTableCell(CUtil::TimeToTextString(field->timestamp)));
					row.AddCell(CTableCell(field->lastChanged));
					tbl.AddRow(row);
				}
			}
		}

		stringstream tblDesc;
		tblDesc << tbl.NumRows() << " fields in view (" << CWebUtil::Link("csv", CWebUtil::CsvDocName(fName), "", 0) << ")" <<  endl;
		tbl.description = tblDesc.str();
	}
	catch(...)
	{
		cout << "EXCEPTION enumerating field properties in view: " << this->pVui->name << endl;
	}

	return tbl;
}
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
#include "DocVuiDetails.h"
#include "../output/CsvPage.h"

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
		webPage.AddContent(CARProplistHelper::GetList(this->pVui->objPropList));

		webPage.AddContent(this->FieldProperties(fName).ToXHtml());

		webPage.AddContent(this->pInside->ServerObjectHistory(this->pVui, this->rootLevel));
		webPage.SaveInFolder(path);	

		//Save field information to ccs
		CCsvPage csvPage("vui_"+this->pVui->FileID(), this->pInside->appConfig);
		csvPage.SaveInFolder(path, this->FieldPropertiesCsv(fName).ToCsv());

	}
	catch(exception& e)
	{
		cout << "EXCEPTION vui details documentation: " << e.what() << endl;
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
					row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(field->timestamp)));
					row.AddCell(CTableCell(this->pInside->LinkToUser(field->lastChanged, 1)));
					tbl.AddRow(row);
				}
			}
		}

		stringstream tblDesc;
		tblDesc << tbl.NumRows() << " fields in view (" << CWebUtil::Link("data", CWebUtil::CsvDocName(fName), "", 0) << ")" <<  endl;
		tbl.description = tblDesc.str();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating field properties in view '" << this->pVui->name << "': " << e.what() << endl;
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
					row.AddCell(CTableCell(CUtil::DateTimeToString(field->timestamp)));
					row.AddCell(CTableCell(field->lastChanged));
					tbl.AddRow(row);
				}
			}
		}

		stringstream tblDesc;
		tblDesc << tbl.NumRows() << " fields in view (" << CWebUtil::Link("csv", CWebUtil::CsvDocName(fName), "", 0) << ")" <<  endl;
		tbl.description = tblDesc.str();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating field properties in view '" << this->pVui->name << "': " << e.what() << endl;
	}

	return tbl;
}

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

CDocVuiDetails::CDocVuiDetails(unsigned int SchemaInsideId, const CARVui& vuiObj, int rootLevel)
: schema(SchemaInsideId), vui(vuiObj)
{
	this->pInside = CARInside::GetInstance();
	this->path = "schema/" + schema.FileID();;
	this->rootLevel = rootLevel;
}

CDocVuiDetails::~CDocVuiDetails(void)
{
}

void CDocVuiDetails::Documentation()
{
	try
	{
		string fName = "vui_"+this->vui.FileID();
		CWebPage webPage(fName, this->vui.GetName(), this->rootLevel, this->pInside->appConfig);

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToSchemaIndex(this->rootLevel) << endl;
		contHeadStrm << MenuSeparator << this->pInside->LinkToSchemaTypeList(this->schema.GetCompound().schemaType, this->rootLevel) << endl;
		contHeadStrm << MenuSeparator << CWebUtil::Link(this->schema.GetName(), CWebUtil::DocName("index"), "", rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::Link("View", CWebUtil::DocName("form_vui_list"), "", rootLevel) << endl;
		contHeadStrm << MenuSeparator << CWebUtil::ObjName(this->vui.GetName()) << endl;
		contHeadStrm << " (Id: " << this->vui.GetInsideId() << ")" << endl;
		webPage.AddContentHead(contHeadStrm.str());

		//Properties
		webPage.AddContent(CARProplistHelper::GetList(this->vui.GetProps(), this));

		webPage.AddContent(this->FieldProperties(fName).ToXHtml());

		webPage.AddContent(this->pInside->ServerObjectHistory(&this->vui, this->rootLevel));
		webPage.SaveInFolder(path);	

		//Save field information to ccs
		CCsvPage csvPage("vui_"+this->vui.FileID(), this->pInside->appConfig);
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
		unsigned int fieldCount = schema.GetFields()->GetCount();
		for(unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
		{
			bool bInView = false;
			stringstream tmpLabel;

			CARField field(schema.GetInsideId(), 0, fieldIndex);
			const ARDisplayInstanceList& dispList = field.GetDisplayInstances();
			for(unsigned int i=0; i< dispList.numItems; ++i)
			{
				if(dispList.dInstanceList[i].vui == this->vui.GetInsideId())
				{
					for(unsigned int j=0; j< dispList.dInstanceList[i].props.numItems; ++j)
					{
						switch (dispList.dInstanceList[i].props.props[j].prop)
						{	
						case AR_DPROP_BUTTON_TEXT:
						case AR_DPROP_LABEL:
							{
								if(dispList.dInstanceList[i].props.props[j].value.dataType == AR_DATA_TYPE_CHAR &&
								   dispList.dInstanceList[i].props.props[j].value.u.charVal != NULL)
								{
									tmpLabel << dispList.dInstanceList[i].props.props[j].value.u.charVal;	
								}
								j = dispList.dInstanceList[i].props.numItems;
							}
							break;												
						}
					}	

					CTableRow row("");
					row.AddCell(CTableCell(field.GetURL(rootLevel)));
					row.AddCell(CTableCell(field.GetFieldId()));
					row.AddCell(CTableCell(tmpLabel.str()));				
					row.AddCell(CTableCell(CAREnum::DataType(field.GetDataType())));				
					row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(field.GetTimestamp())));
					row.AddCell(CTableCell(this->pInside->LinkToUser(field.GetLastChanged(), rootLevel)));
					tbl.AddRow(row);
					break; // exit for(i) loop
				}
			}
		}

		stringstream tblDesc;
		tblDesc << tbl.NumRows() << " fields in view (" << CWebUtil::Link("data", CWebUtil::CsvDocName(fName), "", 0) << ")" <<  endl;
		tbl.description = tblDesc.str();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating field properties in view '" << this->vui.GetName() << "': " << e.what() << endl;
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
		unsigned int fieldCount = schema.GetFields()->GetCount();
		for(unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
		{
			bool bInView = false;
			stringstream tmpLabel;
			tmpLabel.str("");

			CARField field(schema.GetInsideId(), 0, fieldIndex);
			const ARDisplayInstanceList& dispList = field.GetDisplayInstances();
			for(unsigned int i=0; i < dispList.numItems; ++i)
			{
				if(dispList.dInstanceList[i].vui == this->vui.GetInsideId())
				{
					for(unsigned int j=0; j< dispList.dInstanceList[i].props.numItems; ++j)
					{
						switch (dispList.dInstanceList[i].props.props[j].prop)
						{	
						case AR_DPROP_BUTTON_TEXT:
						case AR_DPROP_LABEL:
							{
								if(dispList.dInstanceList[i].props.props[j].value.dataType == AR_DATA_TYPE_CHAR &&
								   dispList.dInstanceList[i].props.props[j].value.u.charVal != NULL)
								{
									tmpLabel << dispList.dInstanceList[i].props.props[j].value.u.charVal;	
								}
								j = dispList.dInstanceList[i].props.numItems;
							}
							break;												
						}
					}	

					CTableRow row("");
					row.AddCell(CTableCell(field.GetName()));
					row.AddCell(CTableCell(field.GetFieldId()));
					row.AddCell(CTableCell(tmpLabel.str()));				
					row.AddCell(CTableCell(CAREnum::DataType(field.GetDataType())));				
					row.AddCell(CTableCell(CUtil::DateTimeToString(field.GetTimestamp())));
					row.AddCell(CTableCell(field.GetLastChanged()));
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
		cout << "EXCEPTION enumerating field properties in view '" << this->vui.GetName() << "': " << e.what() << endl;
	}

	return tbl;
}

bool CDocVuiDetails::SpecialPropertyCallback(ARULong32 propId, const ARValueStruct &value, std::string &displayValue)
{
	switch (propId)
	{
	case AR_DPROP_DETAIL_PANE_IMAGE:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		if (value.dataType == AR_DATA_TYPE_CHAR)
		{
			int imageIndex = pInside->imageList.FindImage(value.u.charVal);
			if (imageIndex < 0) return false;

			displayValue = pInside->imageList.ImageGetURL(imageIndex, rootLevel);
			
			CImageRefItem refItem(imageIndex,"Background in VUI of Form " + this->schema.GetURL(rootLevel), &vui);
			pInside->imageList.AddReference(refItem);
			return true;
		}
#endif
		break;
	}
	return false;
}
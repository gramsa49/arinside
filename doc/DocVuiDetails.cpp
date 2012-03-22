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
#include "../core/ARImage.h"

CDocVuiDetails::CDocVuiDetails(unsigned int SchemaInsideId, const CARVui& vuiObj, int rootLevel)
: schema(SchemaInsideId), vui(vuiObj)
{
}

CDocVuiDetails::~CDocVuiDetails(void)
{
}

void CDocVuiDetails::Documentation()
{
	try
	{
		CPageParams file(PAGE_DETAILS, &this->vui);
		this->rootLevel = file->GetRootLevel();

		CWebPage webPage(file->GetFileName(), this->vui.GetName(), this->rootLevel, this->pInside->appConfig);

		int overlayType = this->vui.GetOverlayType();
		int schemaOverlayType = this->schema.GetOverlayType();

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToSchemaIndex(this->rootLevel) << endl;
		contHeadStrm << MenuSeparator << this->pInside->LinkToSchemaTypeList(this->schema.GetCompound().schemaType, this->rootLevel) << endl;
		contHeadStrm << MenuSeparator << CWebUtil::Link(this->schema.GetName(), CPageParams(PAGE_DETAILS, &this->schema), "", rootLevel);
		if (overlayType == AR_ORIGINAL_OBJECT || overlayType == AR_CUSTOM_OBJECT)
			contHeadStrm << CAREnum::GetOverlayTypeString(schemaOverlayType);
		contHeadStrm << MenuSeparator << CWebUtil::Link("View", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_VUI, &this->schema), "", rootLevel) << endl;
		contHeadStrm << MenuSeparator << CWebUtil::ObjName(this->vui.GetName()) << endl;
		contHeadStrm << " (Id: " << this->vui.GetInsideId() << ")" << CAREnum::GetOverlayTypeString(overlayType) << endl;
		
		CARVui overlayObj;

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
		if (pInside->appConfig.bOverlaySupport && overlayType > 0)
		{
			string correspondingName;
			switch (overlayType)
			{
			case AR_OVERLAID_OBJECT:
				correspondingName = this->schema.GetOverlayName();
				break;
			case AR_OVERLAY_OBJECT:
				correspondingName = this->schema.GetOverlayBaseName();
				break;
			}

			CARSchema correspondingSchema(correspondingName);
			CARVui correspondingObject(correspondingSchema.GetInsideId(), vui.GetInsideId());
			overlayObj = correspondingObject;	
		}
#endif
		
		webPage.AddContentHead(contHeadStrm.str(), PlaceOverlayLink(overlayType, overlayObj));

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
		if (pInside->overlayMode == 1 && overlayType == AR_OVERLAID_OBJECT)
			webPage.AddContent(pInside->PlaceOverlaidNotice(overlayObj, rootLevel));
#endif

		//Properties
		webPage.AddContent(CARProplistHelper::GetList(this->vui.GetDisplayProps(), this));

		webPage.AddContent(this->FieldProperties().ToXHtml());

		webPage.AddContent(this->pInside->ServerObjectHistory(&this->vui, this->rootLevel));
		webPage.SaveInFolder(file->GetPath());

		//Save field information to cvs
		this->FieldPropertiesCsv();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION vui details documentation: " << e.what() << endl;
	}
}

CTable CDocVuiDetails::FieldProperties()
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
		tblDesc << tbl.NumRows() << " fields in view (" << CWebUtil::Link("data", CPageParams(PAGE_SCHEMA_VUIFIELDS_CSV, &this->vui), "", rootLevel) << ")" <<  endl;
		tbl.description = tblDesc.str();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating field properties in view '" << this->vui.GetName() << "': " << e.what() << endl;
	}

	return tbl;
}


void CDocVuiDetails::FieldPropertiesCsv()
{
	CPageParams csvFile(PAGE_SCHEMA_VUIFIELDS_CSV, &this->vui);

	try
	{
		CCsvPage csvPage(csvFile->GetFileName(), this->pInside->appConfig);

		CTable tbl("fieldListIndex", "TblObjectList");
		tbl.AddColumn(25, "Field Name");
		tbl.AddColumn(10, "Field Id");
		tbl.AddColumn(20, "Label in View");
		tbl.AddColumn(10, "Datatype");
		tbl.AddColumn(20, "Modified");
		tbl.AddColumn(15, "By");

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

		csvPage.SaveInFolder(csvFile->GetPath(), tbl.ToCsv());

		//stringstream tblDesc;
		//tblDesc << tbl.NumRows() << " fields in view (" << CWebUtil::Link("csv", CWebUtil::CsvDocName(fName), "", 0) << ")" <<  endl;
		//tbl.description = tblDesc.str();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating field properties in view '" << this->vui.GetName() << "': " << e.what() << endl;
	}
}

bool CDocVuiDetails::SpecialPropertyCallback(ARULong32 propId, const ARValueStruct &value, std::string &displayValue)
{
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	switch (propId)
	{
	case AR_DPROP_DETAIL_PANE_IMAGE:
	case AR_DPROP_TITLE_BAR_ICON_IMAGE:
		if (value.dataType == AR_DATA_TYPE_CHAR)
		{
			CARImage img(value.u.charVal);
			if (img.Exists())
			{
				displayValue = img.GetURL(rootLevel);

				int msgId = -1;
				switch (propId)
				{
				case AR_DPROP_DETAIL_PANE_IMAGE: 
					msgId = REFM_VUI_BACKGROUND;
					break;
				case AR_DPROP_TITLE_BAR_ICON_IMAGE:
					msgId = REFM_TITLE_BAR_ICON;
					break;
				}

				if (msgId > -1)
				{
					CRefItem refItem(this->vui, msgId);
					img.AddReference(refItem);
				}

				return true;
			}
			// TODO: add missing image reference
			//else
			//{
			//}
		}
		break;
	}
#endif
	return false;
}

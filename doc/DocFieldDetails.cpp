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
#include "DocFieldDetails.h"
#include "../util/ImageRefItem.h"

CDocFieldDetails::CDocFieldDetails(unsigned int SchemaInsideId, const CARField& fieldObj, int rootLevel)
: schema(SchemaInsideId), field(fieldObj)
{
}

CDocFieldDetails::~CDocFieldDetails(void)
{
}

void CDocFieldDetails::Documentation()
{
	try
	{
		CPageParams file(PAGE_DETAILS, &this->field);
		rootLevel = file->GetRootLevel();

		CWebPage webPage(file->GetFileName(), this->field.GetName(), rootLevel, this->pInside->appConfig);

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToSchemaIndex(this->rootLevel) << endl;
		contHeadStrm << MenuSeparator << this->pInside->LinkToSchemaTypeList(this->schema.GetCompound().schemaType, rootLevel) << endl;
		contHeadStrm << MenuSeparator << CWebUtil::Link(this->schema.GetName(), CPageParams(PAGE_DETAILS, &schema), "", rootLevel) << endl;
		contHeadStrm << MenuSeparator << CAREnum::DataType(this->field.GetDataType()) << " " << CWebUtil::Link("Field",  CPageParams(PAGE_DETAILS, &schema), "", rootLevel) << endl;
		contHeadStrm << MenuSeparator << CWebUtil::ObjName(this->field.GetName()) << endl;
		contHeadStrm << " (Id: " << this->field.GetFieldId() << ")" << endl;
		webPage.AddContentHead(contHeadStrm.str());

		//Field property table
		CTable tblFieldprops("commonPropList", "TblObjectList");
		tblFieldprops.AddColumn(30, "Description");
		tblFieldprops.AddColumn(70, "Value");

		//Create Mode
		CTableRow row("cssStdRow");		
		row.AddCellList("CreateMode", CAREnum::FieldCreateMode(this->field.GetCreateMode()));
		tblFieldprops.AddRow(row);

		//Option
		row.AddCellList("Option", CAREnum::FieldOption(this->field.GetOption()));
		tblFieldprops.AddRow(row);


		//Default value		
		string tmp = this->DefaultValue();
		if(tmp.empty())
			tmp = EmptyValue;

		row.AddCellList("Default Value", tmp);
		tblFieldprops.AddRow(row);


		//Permissions
		tmp = this->Permisssions();
		if(tmp.empty())
			tmp = EmptyValue;

		row.AddCellList("Permissions", tmp);
		tblFieldprops.AddRow(row);


		//Display Properties
		tmp = this->DisplayProperties();
		if(tmp.empty())
			tmp = EmptyValue;

		row.AddCellList("Display Properties", tmp);
		tblFieldprops.AddRow(row);


		//Fieldlimits		
		tmp = this->FieldLimits();
		if(tmp.empty())
			tmp = EmptyValue;

		row.AddCellList("Field Limits", tmp);
		tblFieldprops.AddRow(row);

		// Field Mapping (for Join, View and Vendor)
		switch (this->field.GetMapping().fieldType)
		{
		case AR_FIELD_JOIN:
		case AR_FIELD_VIEW:
		case AR_FIELD_VENDOR:
			tmp = this->FieldMapping();
			if (!tmp.empty())
			{
				stringstream rowDescription;
				rowDescription << CAREnum::FieldMappingType(this->field.GetMapping().fieldType) << " Information";
				row.AddCellList(rowDescription.str(), tmp);
				tblFieldprops.AddRow(row);
			}
			break;
		}


		//Join Form References
		if(this->schema.GetCompound().schemaType != AR_SCHEMA_DIALOG)
		{
			tmp = this->JoinFormReferences();
			if(tmp.empty())
				tmp = EmptyValue;

			row.AddCellList("References to Join-Forms", tmp);
			tblFieldprops.AddRow(row);
		}

		webPage.AddContent(tblFieldprops.ToXHtml());

		//Add Workflow references to page
		webPage.AddContent(this->WorkflowReferences());

		//History
		webPage.AddContent(this->pInside->ServerObjectHistory(&this->field, this->rootLevel));

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION field details: " << this->field.GetName() << " in form: " << this->schema.GetName() << " error: " << e.what() << endl;
	}
}

string CDocFieldDetails::WorkflowReferences()
{
	stringstream strm;
	strm.str("");

	try
	{
		//Field references
		CTable tblRef("referenceList", "TblObjectList");
		tblRef.AddColumn(10, "Type");
		tblRef.AddColumn(45, "Server object");
		tblRef.AddColumn(5, "Enabled");
		tblRef.AddColumn(40, "Description");

		list<CFieldRefItem>::iterator iter;
		list<CFieldRefItem>::iterator endIt = this->pInside->listFieldRefItem.end();
		for ( iter = this->pInside->listFieldRefItem.begin(); iter != endIt; ++iter )
		{	
			CFieldRefItem *item = &(*iter);
			if(item->schemaInsideId == this->schema.GetInsideId() && item->fieldInsideId == this->field.GetInsideId())
			{			
				if(item->fromName.compare(EmptyValue)!=0)
				{
					CTableRow row("cssStdRow");		
					row.AddCell(CAREnum::XmlStructItem(item->arsStructItemType));				
					row.AddCell(this->pInside->LinkToXmlObjType(item->arsStructItemType, item->fromName, item->fromFieldId, rootLevel));

					string tmpEnabled = this->pInside->XmlObjEnabled(item->arsStructItemType, item->fromName);
					string tmpCssEnabled = "";

					if(tmpEnabled.compare("Disabled")==0)
						tmpCssEnabled = "objStatusDisabled";

					row.AddCell(CTableCell(tmpEnabled, tmpCssEnabled));
					row.AddCell(item->description);				
					tblRef.AddRow(row);		
				}
			}
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Workflow Reference:";

		tblRef.description = tblDesc.str();

		strm << tblRef.ToXHtml();
		tblRef.Clear();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating workflow references for field: " << this->field.GetFieldId() <<" in schema " << this->schema.GetName() << " error: " << e.what() << endl;
	}	

	return strm.str();
}



int CDocFieldDetails::AttachmentFieldGetPool(const CARField& fldObj)
{
	try
	{
		const ARDisplayInstanceList& dispList = fldObj.GetDisplayInstances();
		for(unsigned int i=0; i< dispList.numItems; i++)
		{
			ARValueStruct* dParent = CARProplistHelper::Find(dispList.dInstanceList[i].props, AR_DPROP_DISPLAY_PARENT);
			if (dParent != NULL && (dParent->dataType == AR_DATA_TYPE_ULONG || dParent->dataType == AR_DATA_TYPE_INTEGER))
			{
				return dParent->u.intVal;
			}
		}		
	}
	catch(exception& e)
	{
		cout << "EXCEPTION find attachment field pool of '" << this->field.GetName() << "': " << e.what() << endl;
	}
	return 0;
}

// just a temporary struct for column sorting
class ColumnWithOrder
{
public:
	ColumnWithOrder(unsigned int theOrder, const CARField& theField) { order = theOrder; field = theField; }
	unsigned int order;
	CARField field;
};

// this is a special class used in the next method for sorting table columns by column ordering
class SortByColumnOrder
{
public:
	SortByColumnOrder(int schemaIndex) 
	{ 
		this->schemaIndex = schemaIndex;
	}
	bool operator()(const ColumnWithOrder& l, const ColumnWithOrder& r) 
	{
		return l.order < r.order;
	}

private:
	int schemaIndex;
};

string CDocFieldDetails::FieldLimits()
{
	stringstream strm;
	strm.str("");

	try
	{
		switch(this->field.GetDataType())
		{			
		case AR_DATA_TYPE_CHAR:
			{
				const ARCharLimitsStruct& fLimit = this->field.GetLimits().u.charLimits;

				if(fLimit.charMenu[0] != 0)
				{
					bool menuFound = false;
					strm << "Menu: " << this->pInside->LinkToMenu(fLimit.charMenu, rootLevel, &menuFound) << "<br/>" << endl;

					if (!menuFound)
					{
						CMissingMenuRefItem refItemNotFound(fLimit.charMenu, AR_STRUCT_ITEM_XML_FIELD, this->schema.GetInsideId(), this->field.GetInsideId());
						pInside->AddMissingMenu(refItemNotFound);
					}
				}

				strm << "Fulltext Option: " << CAREnum::FieldFTOption(fLimit.fullTextOptions) << "<br/>" << endl;
				strm << "QBE Match: " << CAREnum::FieldQbeMatch(fLimit.qbeMatchOperation) << "<br/>" << endl;
				strm << "Max. Length: " << fLimit.maxLength << "<br/>" << endl;

				if(fLimit.pattern != NULL && !strcmp(fLimit.pattern, "")==0 )
					strm << "Pattern: " << fLimit.pattern << "<br/>" << endl;
			}
			break;
		case AR_DATA_TYPE_ATTACH:
			{
				const ARAttachLimitsStruct& fLimit = this->field.GetLimits().u.attachLimits;
				strm << "Max. Size: " << fLimit.maxSize;

				int nPoolId = AttachmentFieldGetPool(field);

				if(nPoolId > 0)
				{
					strm << "<br/>Field in Attachment Pool: " << this->pInside->LinkToField(this->schema.GetInsideId(), nPoolId, rootLevel);
				}
			}
			break;
		case AR_DATA_TYPE_ATTACH_POOL:
			{				
				unsigned int fieldCount = schema.GetFields()->GetCount();
				for(unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
				{
					CARField attachField(schema.GetInsideId(), 0, fieldIndex);

					if (attachField.GetDataType() == AR_DATA_TYPE_ATTACH)
					{
						if (AttachmentFieldGetPool(attachField) == field.GetInsideId())
						{
							strm << "Attachment Field: " << attachField.GetURL(rootLevel) << "<br/>" << endl;
						}
					}
				}
			}
			break;
		case AR_DATA_TYPE_COLUMN:
			{
				const ARColumnLimitsStruct& fLimit = this->field.GetLimits().u.columnLimits;
				strm << "Column in Table: " << this->pInside->LinkToField(this->schema.GetInsideId(), fLimit.parent, rootLevel) << "<br/>" << endl;

				//To create a link to the datafield we first must find the source form and the source field of the column
				CARField colSourceField; // initialize to unknown field
				string colSourceForm;

				GetColumnSourceField(this->field, colSourceField, &colSourceForm);
				int schemaIndex = colSourceField.GetSchema().GetInsideId();
				int fieldIndex = fLimit.dataField;

				strm << "Source Type: " << CAREnum::ColumnDataSourceType(fLimit.dataSource) << "<br/>Source Field: " << this->pInside->LinkToField(schemaIndex, fLimit.dataField, rootLevel);
				strm << " In Schema: " << (schemaIndex > -1 ? this->pInside->LinkToSchema(colSourceField.GetSchema().GetInsideId(), rootLevel) : colSourceForm) << "<br/>";
				
				if (fLimit.dataSource == COLUMN_LIMIT_DATASOURCE_DATA_FIELD)
					strm << "Length: " << fLimit.colLength << "<br/>";

				strm << endl;
			}
			break;
		case AR_DATA_TYPE_CURRENCY:
			{
				const ARCurrencyLimitsStruct& fLimit = this->field.GetLimits().u.currencyLimits;

				strm << "Min: " << fLimit.rangeLow << " Max: " << fLimit.rangeHigh;
				strm << " Precision: " << fLimit.precision << "<br/>" << endl;			

				strm << "Allowable Types:<br/>" << endl;

				if(fLimit.allowableCurrencies.numItems == 0)
				{
					strm << "All Allowable Currency Types allowed<br/>" << endl;
				}
				else
				{
					for(unsigned int i=0; i< fLimit.allowableCurrencies.numItems; i++)
					{
						strm << "Code: " << fLimit.allowableCurrencies.currencyDetailList[i].currencyCode << "<br/>" << endl;
					}					
				}

				strm << "<br/>Functional Types:<br/>" << endl;
				for(unsigned int i=0; i< fLimit.functionalCurrencies.numItems; i++)
				{
					strm << "Code: " << fLimit.functionalCurrencies.currencyDetailList[i].currencyCode << " (" << fLimit.functionalCurrencies.currencyDetailList[i].precision  << ")<br/>" << endl;
				}
			}
			break;
		case AR_DATA_TYPE_DATE:
			{
				const ARDateLimitsStruct& fLimit = this->field.GetLimits().u.dateLimits;
				strm << "Max. " << fLimit.maxDate << " Min. " << fLimit.minDate << "<br/>" << endl;
			}
			break;
		case AR_DATA_TYPE_DECIMAL:
			{
				const ARDecimalLimitsStruct& fLimit = this->field.GetLimits().u.decimalLimits;

				strm << "Min: " << fLimit.rangeLow << " Max: " << fLimit.rangeHigh;
				strm << " Precision: " << fLimit.precision << "<br/>" << endl;			
			}
			break;
		case AR_DATA_TYPE_DIARY:
			{
				const ARDiaryLimitsStruct& fLimit = this->field.GetLimits().u.diaryLimits;
				strm << "Index For FTS: " << CAREnum::FieldFTOption(fLimit.fullTextOptions) << "<br/>" << endl;			
			}
			break;
		case AR_DATA_TYPE_DISPLAY:
			{
				const ARDisplayLimits& fLimit = this->field.GetLimits().u.displayLimits;
				strm << "Max. Length: " << fLimit.maxLength << "<br/>" << endl;	
			}
			break;		
		case AR_DATA_TYPE_ENUM:
			{
				const AREnumLimitsStruct& fLimit = this->field.GetLimits().u.enumLimits;

				strm << "List Style: " << CAREnum::EnumStyle(fLimit.listStyle) << "<br/>" << endl;						
				switch(fLimit.listStyle)
				{
				case AR_ENUM_STYLE_REGULAR:
					{
						strm << "Items: " << "<br/>" << endl;
						for(unsigned int i=0; i < fLimit.u.regularList.numItems; i++)
						{
							strm << i << " - " << fLimit.u.regularList.nameList[i] << "<br/>" << endl;
						}
					}
					break;
				case AR_ENUM_STYLE_CUSTOM:
					{
						strm << "Items: " << "<br/>" << endl;
						for(unsigned int i=0; i < fLimit.u.customList.numItems; i++)
						{
							strm << fLimit.u.customList.enumItemList[i].itemNumber << " - " << fLimit.u.customList.enumItemList[i].itemName << "<br/>" << endl;
						}
					}
					break;
				case AR_ENUM_STYLE_QUERY:
					{
						strm << "Query enum<br/>" << endl;
					}
					break;
				}

			}
			break;
		case AR_DATA_TYPE_INTEGER:
			{
				const ARIntegerLimitsStruct& fLimit = this->field.GetLimits().u.intLimits;
				strm << "Min: " << fLimit.rangeLow << " Max: " << fLimit.rangeHigh << "<br/>" << endl;
			}
			break;
		case AR_DATA_TYPE_BITMASK:
			{
				const AREnumLimitsStruct& fLimit = this->field.GetLimits().u.maskLimits;
				strm << "BitmaskLimit" << "<br/>" << endl;

			}
			break;
		case AR_DATA_TYPE_REAL:
			{
				const ARRealLimitsStruct& fLimit = this->field.GetLimits().u.realLimits;

				strm << "Min: " << fLimit.rangeLow << " Max: " << fLimit.rangeHigh;
				strm << " Precision: " << fLimit.precision << "<br/>" << endl;

			}
			break;			
		case AR_DATA_TYPE_TABLE:
			{
				const ARTableLimitsStruct& fLimit = this->field.GetLimits().u.tableLimits;
				string tableServer;
				string tableSchema;

				strm << "<p>Server: ";
				if (fLimit.server[0] == '$' && fLimit.sampleServer[0] != 0)
				{
					int fieldId = atoi(&fLimit.server[1]);
					tableServer = fLimit.sampleServer;

					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : pInside->LinkToField(schema.GetInsideId(), fieldId, rootLevel) ) << "$ (Sample Server: " << pInside->LinkToServerInfo(tableServer, rootLevel) << ")";

					if (fieldId > 0)
					{
						CFieldRefItem refItem(AR_STRUCT_ITEM_XML_FIELD, schema.GetName(), "Used as Server of Table Field", fieldId, schema.GetInsideId());
						pInside->AddReferenceItem(&refItem);
					}
				}
				else
				{
					strm << this->pInside->LinkToServerInfo(fLimit.server, rootLevel);
				}
				strm << "<br/>" << endl;

				strm << "Form: ";;
				if (fLimit.schema[0] == '$' && fLimit.sampleSchema[0] != 0)
				{
					int fieldId = atoi(&fLimit.schema[1]);
					tableSchema = fLimit.sampleSchema;

					if (tableSchema.compare(AR_CURRENT_SCHEMA_TAG) == 0)
						tableSchema = schema.GetARName();

					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : pInside->LinkToField(schema.GetInsideId(), fieldId, rootLevel) ) << "$ (Sample Form: " << pInside->LinkToSchema(tableSchema, rootLevel) << ")";

					if (fieldId > 0)
					{
						CFieldRefItem refItem(AR_STRUCT_ITEM_XML_FIELD, schema.GetName(), "Used as Schema of Table Field", fieldId, schema.GetInsideId());
						pInside->AddReferenceItem(&refItem);
					}
				}
				else
				{
					tableSchema = fLimit.schema;

					if (tableSchema.compare(AR_CURRENT_SCHEMA_TAG) == 0)
						tableSchema = schema.GetARName();

					strm << "Schema: " << this->pInside->LinkToSchema(tableSchema, rootLevel);
				}
				strm << "<p/>" << endl;

				stringstream strmQuery;
				if(fLimit.qualifier.operation != AR_COND_OP_NONE)
				{		
					CFieldRefItem *refItem = new CFieldRefItem();
					refItem->arsStructItemType = AR_STRUCT_ITEM_XML_NONE;  //Dont change
					refItem->description = "";
					refItem->fromName = this->field.GetName();

					CARQualification arQual(*this->pInside);

					int pFormId = schema.GetInsideId();
					int sFormId = this->pInside->SchemaGetInsideId(tableSchema);
					arQual.CheckQuery(&fLimit.qualifier, *refItem, 0, pFormId, sFormId, strmQuery, rootLevel);

					delete refItem;
				}
				else
				{
					strmQuery << EmptyRunIf << endl;
				}

				strm << "<p>Qualification: <br/>" << strmQuery.str() << "<p/>" << endl;


				strm << "<p>Max. Rows: " << fLimit.maxRetrieve << "<br/>" << endl;
				strm << "Num. Columns: " << fLimit.numColumns << "<p/>" << endl;						

				CTable colTab("tableColumnList", "TblObjectList");
				colTab.AddColumn(10,"Column Title");
				colTab.AddColumn(70,"Field");
				colTab.AddColumn(10,"Type");
				colTab.AddColumn(10,"Source");

				// get id of default vui
				CARVui defaultVUI(schema.GetInsideId(), schema.GetDefaultVUI());
				ARInternalId defaultVUIId = 0;
				if (defaultVUI.Exists())
					defaultVUIId = defaultVUI.GetId();

				vector<ColumnWithOrder> columns; columns.reserve(fLimit.numColumns);
				unsigned int fieldCount = schema.GetFields()->GetCount();
				for (unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
				{
					CARField columnField(schema.GetInsideId(), 0, fieldIndex);

					if(columnField.GetDataType() == AR_DATA_TYPE_COLUMN)
					{
						const ARColumnLimitsStruct& colLimits = columnField.GetLimits().u.columnLimits;
						if (colLimits.parent != this->field.GetInsideId()) { continue; } // this column doesn't belong to the current table

						const ARDisplayInstanceList& dinstList = columnField.GetDisplayInstances();
						
						for (unsigned int dinstIndex = 0; dinstIndex < dinstList.numItems; ++dinstIndex)
						{
							if (dinstList.dInstanceList[dinstIndex].vui == defaultVUIId)
							{
								const ARValueStruct* val = CARProplistHelper::Find(dinstList.dInstanceList[dinstIndex].props, AR_DPROP_COLUMN_ORDER);
								if (val != NULL && (val->dataType == AR_DATA_TYPE_INTEGER || val->dataType == AR_DATA_TYPE_ULONG))
									columns.push_back(ColumnWithOrder(val->u.ulongVal, columnField));
								break;
							}
						}

					}
				}
				sort(columns.begin(), columns.end(), SortByColumnOrder(schema.GetInsideId()));

				/*unsigned int*/ fieldCount = schema.GetFields()->GetCount();
				for(unsigned int fieldIndex = 0; fieldIndex < columns.size(); ++fieldIndex)
				{
					CARField columnField(columns[fieldIndex].field);

					const ARColumnLimitsStruct& colLimits = columnField.GetLimits().u.columnLimits;
					if (colLimits.parent != this->field.GetInsideId()) { continue; } // this column doesn't belong to the current table

					CARField colSourceField;	// init to none-existing field

					GetColumnSourceField(columnField, colSourceField, NULL);

					// for the output we need the column label, that is stored as a property
					// within the column. Each vui could have a different label for the column.
					// Just use the defaultVUI.
					const ARDisplayInstanceList& vuiList = columnField.GetDisplayInstances();
					string label;

					for (unsigned int vPos = 0; vPos < vuiList.numItems; ++vPos)
					{
						if (vuiList.dInstanceList[vPos].vui == defaultVUI.GetId())
						{
							const ARPropList& props = vuiList.dInstanceList[vPos].props;
							ARValueStruct* val = CARProplistHelper::Find(props, AR_DPROP_LABEL);
							if (val != NULL && val->dataType == AR_DATA_TYPE_CHAR)
							{
								label = val->u.charVal;
							}
							// else
							//   keep it empty (but that shouldn't happen at all)
							break;
						}
					}

					// now create the row
					CTableRow tblRow;
					tblRow.AddCell(label);
					tblRow.AddCell(columnField.GetURL(rootLevel));
					tblRow.AddCell((colSourceField.Exists() ? CAREnum::DataType(colSourceField.GetDataType()) : EnumDefault));
					tblRow.AddCell(CAREnum::ColumnDataSourceType(colLimits.dataSource));
					colTab.AddRow(tblRow);
				}
				strm << colTab.ToXHtml() << endl;
			}
			break;
		case AR_DATA_TYPE_VIEW:
			{
				const ARViewLimits& fLimit = this->field.GetLimits().u.viewLimits;

				strm << "Max. Length: " << fLimit.maxLength << "<br/>" << endl;
			}
			break;			
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION reading limits for field: " << this->field.GetFieldId() <<" in schema " << this->schema.GetName() << " error: " << e.what() << endl;
	}

	return strm.str();
}

string CDocFieldDetails::DefaultValue()
{
	stringstream strm;
	strm.str("");

	try
	{
		switch(this->field.GetDefaultValue().dataType)
		{
		case AR_DATA_TYPE_INTEGER:
			{
				strm << CARValue::ValueToString(this->field.GetDefaultValue()) << endl;
			}
			break;
		case AR_DATA_TYPE_REAL:
			{
				strm << CARValue::ValueToString(this->field.GetDefaultValue()) << endl;
			}
			break;
		case AR_DATA_TYPE_CHAR:
			{
				if(this->field.GetDefaultValue().u.charVal != NULL)
				{
					strm << "\"" << CARValue::ValueToString(this->field.GetDefaultValue()) << "\"" << endl;
				}
			}
			break;
		case AR_DATA_TYPE_DIARY:
			{
				if(this->field.GetDefaultValue().u.diaryVal != NULL)
				{
					strm << CARValue::ValueToString(this->field.GetDefaultValue()) << endl;
				}
			}
			break;	
		case AR_DATA_TYPE_ENUM:
			{
				strm << CARValue::ValueToString(this->field.GetDefaultValue()) << endl;
			}
			break;
		case AR_DATA_TYPE_TIME:
			{
				strm << CARValue::ValueToString(this->field.GetDefaultValue()) << endl;
			}
			break;
		case AR_DATA_TYPE_DECIMAL:
			{
				if(this->field.GetDefaultValue().u.decimalVal != NULL)
				{
					strm << CARValue::ValueToString(this->field.GetDefaultValue()) << endl;
				}
			}
			break;
		case AR_DATA_TYPE_CURRENCY:
			{
				if(this->field.GetDefaultValue().u.currencyVal->currencyCode != NULL)
				{
					strm << CARValue::ValueToString(this->field.GetDefaultValue()) << endl;
				}
			}
			break;
		case AR_DATA_TYPE_DATE:
			{
				strm << CARValue::ValueToString(this->field.GetDefaultValue()) << endl;
			}
			break;
		case AR_DATA_TYPE_TIME_OF_DAY:
			{
				strm << CARValue::ValueToString(this->field.GetDefaultValue()) << endl;
			}
			break;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating default value for field: " << this->field.GetFieldId() <<" in schema " << this->schema.GetName() << " error: " << e.what() << endl;
	}
	return strm.str();
}


string CDocFieldDetails::Permisssions()
{
	stringstream strm;
	strm.str("");

	try
	{
		CTable tbl("fieldListAll", "TblObjectList");
		tbl.AddColumn(5, "Permission");
		tbl.AddColumn(10, "Description");
		tbl.AddColumn(75, "Group Name");
		tbl.AddColumn(10, "Group Id");

		const ARPermissionList& perms = field.GetPermissions();
		for(unsigned int i=0; i < perms.numItems; i++)
		{
			string img;
			if(perms.permissionList[i].permissions == AR_PERMISSIONS_CHANGE)
				img = CWebUtil::ImageTag("edit.gif", rootLevel);
			else
				img = CWebUtil::ImageTag("visible.gif", rootLevel);

			CTableRow row("");
			row.AddCell( CTableCell(img));
			row.AddCell( CTableCell(CAREnum::FieldPermission(perms.permissionList[i].permissions)));
			row.AddCell( CTableCell(this->pInside->LinkToGroup(this->schema.GetAppRefName(), perms.permissionList[i].groupId, rootLevel)));
			row.AddCell( CTableCell(perms.permissionList[i].groupId));
			tbl.AddRow(row);
			row.ClearCells();
		}

		strm << tbl.ToXHtml();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating permissions for field: " << this->field.GetFieldId() << " in schema " << this->schema.GetName() << " error: " << e.what() << endl;
	}	

	return strm.str();
}


string CDocFieldDetails::DisplayProperties()
{
	stringstream strm;
	strm.str("");

	try
	{
		const ARDisplayInstanceList& dispList = field.GetDisplayInstances();
		for(unsigned int i=0; i < dispList.numItems; i++)
		{
			CTable tbl("displayPropList", "TblObjectList");
			tbl.AddColumn(20, "Description");
			tbl.AddColumn(80, "Values");

			for(unsigned int k=0; k < dispList.dInstanceList[i].props.numItems; k++)
			{
				const ARPropStruct &dProperty = dispList.dInstanceList[i].props.props[k];
				
				string value;

				//************** check for special properties here
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
				if ((dProperty.prop == AR_DPROP_PUSH_BUTTON_IMAGE || dProperty.prop == AR_DPROP_IMAGE) && dProperty.value.dataType == AR_DATA_TYPE_CHAR)
				{
					int imageIndex = pInside->imageList.FindImage(dProperty.value.u.charVal);
					if (imageIndex >= 0)
					{
						stringstream tmpDesc;
						switch (dProperty.prop)
						{
						case AR_DPROP_PUSH_BUTTON_IMAGE:
						case AR_DPROP_IMAGE:
							{
								tmpDesc << "Image on field of form " << this->schema.GetURL(rootLevel, true);
								CImageRefItem refItem(imageIndex, tmpDesc.str(), &field);
								pInside->imageList.AddReference(refItem);
								break;
							}
						}

						value = pInside->imageList.ImageGetURL(imageIndex, rootLevel);
					}
				}
#endif
				if (dProperty.prop == AR_DPROP_DISPLAY_PARENT && dProperty.value.u.ulongVal > 0)
				{
					value = pInside->LinkToField(schema.GetInsideId(), dProperty.value.u.ulongVal, rootLevel);
				}
				//************** end of special properties

				if (value.empty())
				{
					value = CARProplistHelper::GetValue(dProperty.prop, dProperty.value);
				}

				CTableRow row("");			
				row.AddCell( CTableCell(CARProplistHelper::GetLabel(dProperty.prop)));
				row.AddCell( CTableCell(value));
				tbl.AddRow(row);				
			}	

			stringstream viewTmpDesc;
			viewTmpDesc.str("");
			viewTmpDesc << "Display Properties in " << CWebUtil::Link("Schema",CPageParams(PAGE_DETAILS, &schema), "", rootLevel) << ", View: " << endl;
			viewTmpDesc << this->schema.LinkToVui(dispList.dInstanceList[i].vui, rootLevel);
			viewTmpDesc << " (Id: " << dispList.dInstanceList[i].vui << ", Label: " << this->schema.VuiGetLabel(dispList.dInstanceList[i].vui) << ")" << endl;


			tbl.description = viewTmpDesc.str();
			strm << tbl.ToXHtml();

			viewTmpDesc.str("");
		}	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating display properties for field: " << this->field.GetFieldId() <<" in schema " << this->schema.GetName() << " error: " << e.what() << endl;
	}	

	return strm.str();
}

string CDocFieldDetails::JoinFormReferences()
{
	stringstream strm;
	strm.str("");

	try
	{
		unsigned int schemaCount = this->pInside->schemaList.GetCount();
		for (unsigned int schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex)
		{			
			CARSchema tmpSchema(schemaIndex);

			const ARCompoundSchema& comp = tmpSchema.GetCompound();
			if(comp.schemaType == AR_SCHEMA_JOIN)
			{
				//Primary Join Form referes to this schema
				if(strcmp(comp.u.join.memberA, this->schema.GetARName())==0)					
				{
					unsigned int fieldCount = tmpSchema.GetFields()->GetCount();
					for (unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
					{
						CARField tmpField(schemaIndex, 0, fieldIndex);
						if(tmpField.GetMapping().u.join.realId == this->field.GetFieldId() && tmpField.GetMapping().u.join.schemaIndex == 0)
						{
							strm << tmpField.GetURL(rootLevel) << " in Join-Form: ";
							strm << tmpSchema.GetURL(rootLevel) << "<br/>" << endl;
						}
					}
				}

				//Secondary Join Form refers to this schema
				if(strcmp(comp.u.join.memberB, this->schema.GetARName())==0)					
				{
					unsigned int fieldCount = tmpSchema.GetFields()->GetCount();
					for (unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
					{
						CARField tmpField(schemaIndex, 0, fieldIndex);
						if(tmpField.GetMapping().u.join.realId == this->field.GetFieldId() && tmpField.GetMapping().u.join.schemaIndex == 1)
						{
							strm << tmpField.GetURL(rootLevel) << " in Join-Form: ";
							strm << tmpSchema.GetURL(rootLevel) << "<br/>" << endl;
						}
					}
				}
			}
		}

		if(strm.str().size() == 0)
			strm << EmptyValue;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating permissions for field: " << this->field.GetFieldId() << " in schema " << this->schema.GetName() << " error: " << e.what() << endl;
	}	

	return strm.str();
}

string CDocFieldDetails::FieldMapping()
{
	stringstream strm;
	const ARFieldMappingStruct& map = this->field.GetMapping();
	const ARCompoundSchema& compSchema = this->schema.GetCompound();

	switch (map.fieldType)
	{
	case AR_FIELD_JOIN:
		{
			if (this->field.GetFieldId() == 1)
			{
				strm << this->pInside->LinkToField(compSchema.u.join.memberA, 1, rootLevel) << "&nbsp;" << MenuSeparator << "&nbsp;" << this->pInside->LinkToSchema(compSchema.u.join.memberA, rootLevel) << "<br/>";
				strm << this->pInside->LinkToField(compSchema.u.join.memberB, 1, rootLevel) << "&nbsp;" << MenuSeparator << "&nbsp;" << this->pInside->LinkToSchema(compSchema.u.join.memberB, rootLevel);
			}
			else
			{
				string schemaName;

				strm << "Form Name: ";
				if (compSchema.schemaType == AR_SCHEMA_JOIN)
				{
					if (map.u.join.schemaIndex == 0) // primary form
						schemaName = compSchema.u.join.memberA;
					else if (map.u.join.schemaIndex == 1) // secondary form
						schemaName = compSchema.u.join.memberB;
				}

				CARSchema schema(schemaName);
				if (schema.Exists())
					strm << schema.GetURL(this->rootLevel);
				else
					strm << "<span class=\"fieldNotFound\">" << (schemaName.empty() ? EnumDefault : schemaName) << "</span>";
				strm << "<br/>";

				strm << "Field Name: ";
				
				CARField field(schema.GetInsideId(), map.u.join.realId);
				if (field.Exists())
					strm << field.GetURL(this->rootLevel);
				else
					strm << "<span class=\"fieldNotFound\">" << map.u.join.realId << "</span>";
			}
		}
		break;

	case AR_FIELD_VIEW:
		{
			strm << "Column: " << map.u.view.fieldName << "<br/>"; 
			strm << "Table: ";
			
			if (compSchema.schemaType == AR_SCHEMA_VIEW) 
				strm << compSchema.u.view.tableName;
			else
				strm << "<span class=\"fieldNotFound\">" << EnumDefault << "</span>";
		}
		break;

	case AR_FIELD_VENDOR:
		{
			strm << "Column: " << map.u.vendor.fieldName << "<br/>";

			strm << "Vendor: ";
			if (compSchema.schemaType == AR_SCHEMA_VENDOR)
				strm << compSchema.u.vendor.vendorName;
			else
				strm << "<span class=\"fieldNotFound\">" << EnumDefault << "</span>";
			strm << "<br/>";

			strm << "Table: ";
			if (compSchema.schemaType == AR_SCHEMA_VENDOR)
				strm << compSchema.u.vendor.tableName;
			else
				strm << "<span class=\"fieldNotFound\">" << EnumDefault << "</span>";
		}
		break;
	}

	return strm.str();
}

bool CDocFieldDetails::GetColumnSourceField(const CARField& col, CARField& source, std::string* colSourceSchemaName)
{
	if (col.GetLimits().dataType != AR_DATA_TYPE_COLUMN)
		throw AppException("invalid field type");

	const ARColumnLimitsStruct& fLimit = col.GetLimits().u.columnLimits;
	CARSchema columnSourceForm; // initialize it to invalid form
	string columnSourceFormName;				

	if (fLimit.dataSource == COLUMN_LIMIT_DATASOURCE_DISPLAY_FIELD || fLimit.dataSource == COLUMN_LIMIT_DATASOURCE_CONTROL_FIELD)
	{
		columnSourceForm = CARSchema(col.GetSchema().GetInsideId());
		columnSourceFormName = columnSourceForm.GetARName();
	}
	else if (fLimit.dataSource == COLUMN_LIMIT_DATASOURCE_DATA_FIELD)
	{
		CARField tableField(col.GetSchema().GetInsideId(), fLimit.parent);
		if (tableField.Exists() && tableField.GetDataType() == AR_DATA_TYPE_TABLE && tableField.GetLimits().dataType == AR_DATA_TYPE_TABLE)
		{
			const ARFieldLimitStruct& limits = tableField.GetLimits();
			columnSourceFormName = limits.u.tableLimits.schema;

			if (!columnSourceFormName.empty() && columnSourceFormName[0] == '$')
				columnSourceFormName = limits.u.tableLimits.sampleSchema;

			if (columnSourceFormName.compare(AR_CURRENT_SCHEMA_TAG) == 0)
				columnSourceFormName = col.GetSchema().GetARName();
		}
		columnSourceForm = CARSchema(columnSourceFormName);
	}

	source = CARField(columnSourceForm.GetInsideId(), fLimit.dataField);

	if (colSourceSchemaName)
	{
		(*colSourceSchemaName) = columnSourceFormName;
	}

	return columnSourceForm.Exists();
}

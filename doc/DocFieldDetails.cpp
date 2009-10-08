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

CDocFieldDetails::CDocFieldDetails(CARInside &arInside, CARSchema &schema, CARField &field, string path, int rootLevel)
{
	this->pInside = &arInside;
	this->pSchema = &schema;
	this->pField = &field;
	this->path = path;
	this->rootLevel = rootLevel;
}

CDocFieldDetails::~CDocFieldDetails(void)
{
}

void CDocFieldDetails::Documentation()
{
	try
	{
		CWebPage webPage(this->pField->FileID(), this->pField->name, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToSchemaIndex(this->rootLevel) << endl;
		contHeadStrm << MenuSeparator << this->pInside->LinkToSchemaTypeList(this->pSchema->schema.schemaType, rootLevel) << endl;
		contHeadStrm << MenuSeparator << CWebUtil::Link(this->pSchema->name, CWebUtil::DocName("index"), "", rootLevel) << endl;
		contHeadStrm << MenuSeparator << CAREnum::DataType(this->pField->dataType) << " " << CWebUtil::Link("Field", CWebUtil::DocName("index"), "", rootLevel) << endl;
		contHeadStrm << MenuSeparator << CWebUtil::ObjName(this->pField->name) << endl;
		contHeadStrm << " (Id: " << this->pField->fieldId << ")" << endl;
		webPage.AddContentHead(contHeadStrm.str());

		//Field property table
		CTable tblFieldprops("commonPropList", "TblObjectList");
		tblFieldprops.AddColumn(30, "Description");
		tblFieldprops.AddColumn(70, "Value");

		//Create Mode
		CTableRow row("cssStdRow");		
		row.AddCellList("CreateMode", CAREnum::FieldCreateMode(this->pField->createMode));
		tblFieldprops.AddRow(row);

		//Option
		row.AddCellList("Option", CAREnum::FieldOption(this->pField->option));
		tblFieldprops.AddRow(row);


		//Default value		
		string tmp = this->DefaultValue();
		if(tmp.size() == 0)
			tmp = EmptyValue;

		row.AddCellList("Default Value", tmp);
		tblFieldprops.AddRow(row);


		//Permissions
		tmp = this->Permisssions();
		if(tmp.size() == 0)
			tmp = EmptyValue;

		row.AddCellList("Permissions", tmp);
		tblFieldprops.AddRow(row);


		//Display Properties
		tmp = this->DisplayProperties();
		if(tmp.size() == 0)
			tmp = EmptyValue;

		row.AddCellList("Display Properties", tmp);
		tblFieldprops.AddRow(row);


		//Fieldlimits		
		tmp = this->FieldLimits();
		if(tmp.size() == 0)
			tmp = EmptyValue;

		row.AddCellList("Field Limits", tmp);
		tblFieldprops.AddRow(row);


		//Join Form References
		if(this->pSchema->schema.schemaType != AR_SCHEMA_DIALOG)
		{
			tmp = this->JoinFormReferences();
			if(tmp.size() == 0)
				tmp = EmptyValue;

			row.AddCellList("References to Join-Forms", tmp);
			tblFieldprops.AddRow(row);
		}

		webPage.AddContent(tblFieldprops.ToXHtml());

		//Add Workflow references to page
		webPage.AddContent(this->WorkflowReferences());

		//History
		webPage.AddContent(this->pInside->ServerObjectHistory(this->pField, this->rootLevel));

		webPage.SaveInFolder(this->path);	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION field details: " << this->pField->name << " in form: " << this->pSchema->name << " error: " << e.what() << endl;
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
		tblRef.AddColumn(15, "Type");
		tblRef.AddColumn(25, "Server object");
		tblRef.AddColumn(10, "Enabled");
		tblRef.AddColumn(50, "Description");

		list<CFieldRefItem>::iterator iter;
		list<CFieldRefItem>::iterator endIt = this->pInside->listFieldRefItem.end();
		for ( iter = this->pInside->listFieldRefItem.begin(); iter != endIt; ++iter )
		{	
			CFieldRefItem *item = &(*iter);
			if(item->schemaInsideId == this->pSchema->insideId && item->fieldInsideId == this->pField->insideId)
			{			
				if(item->fromName.compare(EmptyValue)!=0)
				{
					CTableRow row("cssStdRow");		
					row.AddCell(CAREnum::XmlStructItem(item->arsStructItemType));				
					row.AddCell(this->pInside->LinkToXmlObjType(item->arsStructItemType, item->fromName, rootLevel));

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
		cout << "EXCEPTION enumerating workflow references for field: " << this->pField->fieldId <<" in schema " << this->pSchema->name << " error: " << e.what() << endl;
	}	

	return strm.str();
}



int CDocFieldDetails::AttachmentFieldGetPool()
{
	int nFieldId = 0;
	try
	{
		for(unsigned int i=0; i< this->pField->dInstanceList.numItems; i++)
		{
			for(unsigned int k=0; k < this->pField->dInstanceList.dInstanceList[i].props.numItems; k++)
			{
				switch(this->pField->dInstanceList.dInstanceList[i].props.props[k].prop)
				{
				case AR_DPROP_DISPLAY_PARENT:
					{								
						nFieldId = this->pField->dInstanceList.dInstanceList[i].props.props[k].value.u.intVal;
					}
					break;
				}
			}
		}		
	}
	catch(exception& e)
	{
		cout << "EXCEPTION find attachment field pool of '" << this->pField->name << "': " << e.what() << endl;
	}

	return nFieldId;
}

string CDocFieldDetails::FieldLimits()
{
	stringstream strm;
	strm.str("");

	try
	{
		switch(this->pField->limit.dataType)
		{			
		case AR_DATA_TYPE_CHAR:
			{
				ARCharLimitsStruct fLimit = this->pField->limit.u.charLimits;

				if(fLimit.charMenu[0] != 0)
				{
					bool menuFound = false;
					strm << "Menu: " << this->pInside->LinkToMenu(fLimit.charMenu, rootLevel, &menuFound) << "<br/>" << endl;

					if (!menuFound)
					{
						CMissingMenuRefItem refItemNotFound(fLimit.charMenu, AR_STRUCT_ITEM_XML_FIELD, this->pSchema->insideId, this->pField->insideId);
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
				ARAttachLimitsStruct fLimit = this->pField->limit.u.attachLimits;
				strm << "Max. Size: " << fLimit.maxSize;

				int nPoolId = AttachmentFieldGetPool();

				if(nPoolId > 0)
				{
					strm << "<br/>Field in Attachment Pool: " << this->pInside->LinkToField(this->pSchema->insideId, nPoolId, rootLevel);
				}
			}
			break;
		case AR_DATA_TYPE_ATTACH_POOL:
			{				
				list<CARSchema>::iterator schemaIter;		
				CARSchema *searchSchema;
				for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
				{			
					searchSchema = &(*schemaIter);

					if(this->pSchema->insideId == searchSchema->insideId)
					{
						list<CARField>::iterator fieldIter;		
						CARField *attachField;
						for( fieldIter = searchSchema->fieldList.begin(); fieldIter != searchSchema->fieldList.end(); fieldIter++)
						{
							attachField = &(*fieldIter);

							int nTmpPoolId = this->AttachmentFieldGetPool();
							if(nTmpPoolId == this->pField->fieldId)
							{
								strm << "Attachment Field: " << attachField->GetURL(rootLevel) << "<br/>" << endl;
							}
						}
					}
				}            

			}
			break;
		case AR_DATA_TYPE_COLUMN:
			{
				ARColumnLimitsStruct fLimit = this->pField->limit.u.columnLimits;
				strm << "Length: " << fLimit.colLength << "<br/>" << endl;
				strm << "Column in Table: " << this->pInside->LinkToField(this->pSchema->name, fLimit.parent, rootLevel) << "<br/>" << endl;

				//To create a link to the datafield we first must find the target schema of the table
				int tblTargetSchemaInsideId = this->pSchema->insideId;

				list<CARSchema>::iterator schemaIter;		
				CARSchema *searchSchema;
				for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
				{			
					searchSchema = &(*schemaIter);

					if(this->pSchema->insideId == searchSchema->insideId)
					{
						list<CARField>::iterator fieldIter;		
						CARField *field;
						for( fieldIter = searchSchema->fieldList.begin(); fieldIter != searchSchema->fieldList.end(); fieldIter++)
						{
							field = &(*fieldIter);
							if(field->insideId == fLimit.parent) // we found the table
							{				
								if(field->limit.dataType == AR_DATA_TYPE_TABLE)
								{
									ARTableLimitsStruct tblLimits = field->limit.u.tableLimits;

									if(!strcmp(tblLimits.schema, AR_CURRENT_SCHEMA_TAG)==0)
									{
										tblTargetSchemaInsideId = this->pInside->SchemaGetInsideId(tblLimits.schema);
									}
								}
							}
						}
					}
				}            

				strm << "Source Data Field: " << this->pInside->LinkToField(tblTargetSchemaInsideId, fLimit.dataField, rootLevel);
				strm << " In Schema: " << pSchema->GetURL(rootLevel) << "<br/>" << endl;
			}
			break;
		case AR_DATA_TYPE_CURRENCY:
			{
				ARCurrencyLimitsStruct fLimit = this->pField->limit.u.currencyLimits;

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
				ARDateLimitsStruct fLimit = this->pField->limit.u.dateLimits;
				strm << "Max. " << fLimit.maxDate << " Min. " << fLimit.minDate << "<br/>" << endl;
			}
			break;
		case AR_DATA_TYPE_DECIMAL:
			{
				ARDecimalLimitsStruct fLimit = this->pField->limit.u.decimalLimits;

				strm << "Min: " << fLimit.rangeLow << " Max: " << fLimit.rangeHigh;
				strm << " Precision: " << fLimit.precision << "<br/>" << endl;			
			}
			break;
		case AR_DATA_TYPE_DIARY:
			{
				ARDiaryLimitsStruct fLimit = this->pField->limit.u.diaryLimits;
				strm << "Index For FTS: " << CAREnum::FieldFTOption(fLimit.fullTextOptions) << "<br/>" << endl;			
			}
			break;
		case AR_DATA_TYPE_DISPLAY:
			{
				ARDisplayLimits fLimit = this->pField->limit.u.displayLimits;
				strm << "Max. Length: " << fLimit.maxLength << "<br/>" << endl;	
			}
			break;		
		case AR_DATA_TYPE_ENUM:
			{
				AREnumLimitsStruct fLimit = this->pField->limit.u.enumLimits;

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
				ARIntegerLimitsStruct fLimit = this->pField->limit.u.intLimits;
				strm << "Min: " << fLimit.rangeLow << " Max: " << fLimit.rangeHigh << "<br/>" << endl;
			}
			break;
		case AR_DATA_TYPE_BITMASK:
			{
				AREnumLimitsStruct fLimit = this->pField->limit.u.maskLimits;
				strm << "BitmaskLimit" << "<br/>" << endl;

			}
			break;
		case AR_DATA_TYPE_REAL:
			{
				ARRealLimitsStruct fLimit = this->pField->limit.u.realLimits;

				strm << "Min: " << fLimit.rangeLow << " Max: " << fLimit.rangeHigh;
				strm << " Precision: " << fLimit.precision << "<br/>" << endl;

			}
			break;			
		case AR_DATA_TYPE_TABLE:
			{
				ARTableLimitsStruct fLimit = this->pField->limit.u.tableLimits;				
				strm << "Server: " << this->pInside->LinkToServerInfo(fLimit.server, rootLevel) << "<br/>" << endl;
				string tmpSchema = fLimit.schema;
				if(strcmp(fLimit.schema, AR_CURRENT_SCHEMA_TAG) == 0)
				{
					tmpSchema = this->pSchema->name;
				}
				strm << "Schema: " << this->pInside->LinkToSchema(tmpSchema, rootLevel) << "<br/>" << endl;

				stringstream strmQuery;
				if(fLimit.qualifier.operation != NULL)
				{		
					CFieldRefItem *refItem = new CFieldRefItem();
					refItem->arsStructItemType = AR_STRUCT_ITEM_XML_NONE;  //Dont change
					refItem->description = "";
					refItem->fromName = this->pField->name;

					CARQualification arQual(*this->pInside);

					int pFormId = this->pInside->SchemaGetInsideId(this->pSchema->name.c_str());
					int sFormId = this->pInside->SchemaGetInsideId(tmpSchema.c_str());
					arQual.CheckQuery(&fLimit.qualifier, *refItem, 0, pFormId, sFormId, strmQuery, rootLevel);

					delete refItem;
				}
				else
				{
					strmQuery << EmptyRunIf << endl;
				}

				strm << "<br/>Qualification: <br/>" << strmQuery.str() << "<br/><br/>" << endl;


				strm << "Max. Rows: " << fLimit.maxRetrieve << "<br/>" << endl;
				strm << "Num. Columns: " << fLimit.numColumns << "<br/>" << endl;						

				strm << "Table Columns:<br/>" << endl;
				list<CARSchema>::iterator schemaIter;		
				CARSchema *searchSchema;
				for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
				{			
					searchSchema = &(*schemaIter);

					if(this->pSchema->insideId == searchSchema->insideId)
					{
						list<CARField>::iterator fieldIter;		
						CARField *field;
						for( fieldIter = searchSchema->fieldList.begin(); fieldIter != searchSchema->fieldList.end(); fieldIter++)
						{
							field = &(*fieldIter);										
							if(field->limit.dataType == AR_DATA_TYPE_COLUMN)
							{
								ARColumnLimitsStruct colLimits = field->limit.u.columnLimits;
								if(colLimits.parent == this->pField->insideId)
								{
									strm << field->GetURL(rootLevel);

									strm << " [ Datafield: " << this->pInside->LinkToField(fLimit.schema, colLimits.dataField, rootLevel) << " ]<br/>" << endl; 
								}
							}						
						}
					}
				}                
			}
			break;
		case AR_DATA_TYPE_VIEW:
			{
				ARViewLimits fLimit = this->pField->limit.u.viewLimits;

				strm << "Max. Length: " << fLimit.maxLength << "<br/>" << endl;
			}
			break;			
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION reading limits for field: " << this->pField->fieldId <<" in schema " << this->pSchema->name << " error: " << e.what() << endl;
	}

	return strm.str();
}

string CDocFieldDetails::DefaultValue()
{
	stringstream strm;
	strm.str("");

	try
	{
		switch(this->pField->dataType)
		{
		case AR_DATA_TYPE_INTEGER:
			{
				strm << CARValue::ValueToString(this->pField->defaultVal) << endl;
			}
			break;
		case AR_DATA_TYPE_REAL:
			{
				strm << CARValue::ValueToString(this->pField->defaultVal) << endl;
			}
			break;
		case AR_DATA_TYPE_CHAR:
			{
				if(this->pField->defaultVal.u.charVal != NULL)
				{
					strm << "\"" << CARValue::ValueToString(this->pField->defaultVal) << "\"" << endl;
				}
			}
			break;
		case AR_DATA_TYPE_DIARY:
			{
				if(this->pField->defaultVal.u.diaryVal != NULL)
				{
					strm << CARValue::ValueToString(this->pField->defaultVal) << endl;
				}
			}
			break;	
		case AR_DATA_TYPE_ENUM:
			{
				strm << CARValue::ValueToString(this->pField->defaultVal) << endl;
			}
			break;
		case AR_DATA_TYPE_TIME:
			{
				strm << CARValue::ValueToString(this->pField->defaultVal) << endl;
			}
			break;
		case AR_DATA_TYPE_DECIMAL:
			{
				if(this->pField->defaultVal.u.decimalVal != NULL)
				{
					strm << CARValue::ValueToString(this->pField->defaultVal) << endl;
				}
			}
			break;
		case AR_DATA_TYPE_CURRENCY:
			{
				if(this->pField->defaultVal.u.currencyVal->currencyCode != NULL)
				{
					strm << CARValue::ValueToString(this->pField->defaultVal) << endl;
				}
			}
			break;
		case AR_DATA_TYPE_DATE:
			{
				strm << CARValue::ValueToString(this->pField->defaultVal) << endl;
			}
			break;
		case AR_DATA_TYPE_TIME_OF_DAY:
			{
				strm << CARValue::ValueToString(this->pField->defaultVal) << endl;
			}
			break;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating default value for field: " << this->pField->fieldId <<" in schema " << this->pSchema->name << " error: " << e.what() << endl;
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

		for(unsigned int i=0; i<this->pField->permissions.numItems; i++)
		{
			string img = CWebUtil::ImageTag("visible.gif", rootLevel);
			if(this->pField->permissions.permissionList[i].permissions == AR_PERMISSIONS_CHANGE)
				img = CWebUtil::ImageTag("edit.gif", rootLevel);

			CTableRow row("");
			row.AddCell( CTableCell(img));
			row.AddCell( CTableCell(CAREnum::FieldPermission(this->pField->permissions.permissionList[i].permissions)));
			row.AddCell( CTableCell(this->pInside->LinkToGroup(this->pField->appRefName, this->pField->permissions.permissionList[i].groupId, rootLevel)));
			row.AddCell( CTableCell(this->pField->permissions.permissionList[i].groupId));
			tbl.AddRow(row);
			row.ClearCells();
		}

		strm << tbl.ToXHtml();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating permissions for field: " << this->pField->fieldId << " in schema " << this->pSchema->name << " error: " << e.what() << endl;
	}	

	return strm.str();
}


string CDocFieldDetails::DisplayProperties()
{
	stringstream strm;
	strm.str("");

	try
	{	
		for(unsigned int i=0; i<this->pField->dInstanceList.numItems; i++)
		{
			CTable tbl("displayPropList", "TblObjectList");
			tbl.AddColumn(20, "Description");
			tbl.AddColumn(80, "Values");

			for(unsigned int k=0; k < this->pField->dInstanceList.dInstanceList[i].props.numItems; k++)
			{		
				CTableRow row("");			
				row.AddCell( CTableCell(CARProplistHelper::GetLabel(this->pField->dInstanceList.dInstanceList[i].props.props[k].prop)));
				row.AddCell( CTableCell(CARProplistHelper::GetValue(this->pField->dInstanceList.dInstanceList[i].props.props[k].prop, this->pField->dInstanceList.dInstanceList[i].props.props[k].value)));
				tbl.AddRow(row);				
			}	

			stringstream viewTmpDesc;
			viewTmpDesc.str("");
			viewTmpDesc << "Display Properties in " << CWebUtil::Link("Schema","index."+CWebUtil::WebPageSuffix(), "", rootLevel) << ", View: " << endl;					
			viewTmpDesc << this->pSchema->LinkToVui(this->pField->dInstanceList.dInstanceList[i].vui, rootLevel);
			viewTmpDesc << " (Id: " << this->pField->dInstanceList.dInstanceList[i].vui << ", Label: " << this->pSchema->VuiGetLabel(this->pField->dInstanceList.dInstanceList[i].vui) << ")" << endl;


			tbl.description = viewTmpDesc.str();
			strm << tbl.ToXHtml();

			viewTmpDesc.str("");
		}	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating display properties for field: " << this->pField->fieldId <<" in schema " << this->pSchema->name << " error: " << e.what() << endl;
	}	

	return strm.str();
}

string CDocFieldDetails::JoinFormReferences()
{
	stringstream strm;
	strm.str("");

	try
	{
		list<CARSchema>::iterator schemaIter;		
		CARSchema *tmpSchema;
		for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
		{			
			tmpSchema = &(*schemaIter);
			if(tmpSchema->schema.schemaType == AR_SCHEMA_JOIN)
			{
				//Primary Join Form referes to this schema
				if(strcmp(tmpSchema->schema.u.join.memberA, this->pSchema->name.c_str())==0)					
				{
					list<CARField>::iterator fieldListIter;			
					CARField *tmpField;
					for ( fieldListIter = tmpSchema->fieldList.begin(); fieldListIter != tmpSchema->fieldList.end(); fieldListIter++ )
					{
						tmpField = &(*fieldListIter);
						if(tmpField->fieldMap.u.join.realId == this->pField->fieldId && tmpField->fieldMap.u.join.schemaIndex == 0)
						{
							strm << tmpField->GetURL(rootLevel) << " in Join-Form: ";
							strm << tmpSchema->GetURL(rootLevel) << "<br/>" << endl;
						}
					}
				}

				//Secondary Join Form refers to this schema
				if(strcmp(tmpSchema->schema.u.join.memberB, this->pSchema->name.c_str())==0)					
				{
					list<CARField>::iterator fieldListIter;			
					CARField *tmpField;
					for ( fieldListIter = tmpSchema->fieldList.begin(); fieldListIter != tmpSchema->fieldList.end(); fieldListIter++ )
					{
						tmpField = &(*fieldListIter);
						if(tmpField->fieldMap.u.join.realId == this->pField->fieldId && tmpField->fieldMap.u.join.schemaIndex == 1)
						{
							strm << tmpField->GetURL(rootLevel) << " in Join-Form: ";
							strm << tmpSchema->GetURL(rootLevel) << "<br/>" << endl;
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
		cout << "EXCEPTION enumerating permissions for field: " << this->pField->fieldId << " in schema " << this->pSchema->name << " error: " << e.what() << endl;
	}	

	return strm.str();
}

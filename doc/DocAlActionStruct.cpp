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
#include "DocAlActionStruct.h"
#include "DocActionOpenWindowHelper.h"

CDocAlActionStruct::CDocAlActionStruct(CARInside &arIn, CARActiveLink &obj, string schemaName, int rootLevel)
{
	this->arIn = &arIn;
	this->obj = &obj;	
	this->rootLevel = rootLevel;
	this->schemaName = schemaName;
	this->schemaInsideId = this->arIn->SchemaGetInsideId(this->schemaName);
	this->structItemType = AR_STRUCT_ITEM_XML_ACTIVE_LINK;
}

CDocAlActionStruct::~CDocAlActionStruct(void)
{
}


string CDocAlActionStruct::Get(const string &ifElse, const ARActiveLinkActionList &actList)
{
	stringstream strm;
	strm.str("");

	try
	{
		this->ifElse = ifElse;

		CTable tblListAction("tblListAlAction", "TblObjectList");
		tblListAction.AddColumn(5, "Position");	
		tblListAction.AddColumn(15, "Action Type");
		tblListAction.AddColumn(80, "Description");

		for(unsigned int nAction = 0; nAction < actList.numItems; nAction++)
		{
			ARActiveLinkActionStruct action = actList.actionList[nAction];

			CTableRow row("cssStdRow");		
			row.AddCell(CTableCell(nAction));				

			CPageParams file(PAGE_ACTION_OBJLIST, AR_STRUCT_ITEM_XML_ACTIVE_LINK, action.action);
			row.AddCell(CTableCell(CWebUtil::Link(CAREnum::ActiveLinkAction(action.action), file, "", rootLevel)));			

			stringstream actionDesc;
			actionDesc.str("");

			switch(action.action)
			{
			case AR_ACTIVE_LINK_ACTION_NONE:
				{
					actionDesc << ActionNone(nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_MACRO:
				{
					actionDesc << ActionMacro(action.u.macro, nAction);					
				}
				break;
			case AR_ACTIVE_LINK_ACTION_FIELDP:
				{					
					actionDesc << ActionPushFields(action.u.pushFields, nAction);					
				}
				break;
			case AR_ACTIVE_LINK_ACTION_FIELDS:
				{
					actionDesc << ActionSetFields(action.u.setFields, nAction);	
				}
				break;
			case AR_ACTIVE_LINK_ACTION_PROCESS:
				{
					actionDesc << ActionProcess(action.u.process, nAction);	
				}
				break;
			case AR_ACTIVE_LINK_ACTION_MESSAGE:
				{
					actionDesc << ActionMessage(action.u.message, nAction);					
				}
				break;
			case AR_ACTIVE_LINK_ACTION_SET_CHAR:
				{
					actionDesc << ActionSetChar(action.u.characteristics, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_DDE:
				{
					actionDesc << ActionDde(action.u.dde, nAction);
				}
				break;				
			case AR_ACTIVE_LINK_ACTION_SQL:
				{
					actionDesc << ActionSql(action.u.sqlCommand, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_AUTO:
				{
					actionDesc << ActionAutomation(action.u.automation, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_OPENDLG:
				{
					actionDesc << ActionOpenDlg(action.u.openDlg, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_COMMITC:
				{
					actionDesc << ActionCommitChanges(action.u.commitChanges, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_CLOSEWND:
				{
					actionDesc << ActionCloseWindow(action.u.closeWnd, nAction);		
				}
				break;
			case AR_ACTIVE_LINK_ACTION_CALLGUIDE:
				{
					actionDesc << ActionCallGuide(action.u.callGuide, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_EXITGUIDE:
				{
					actionDesc << ActionExitGuide(action.u.exitGuide, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_GOTOGUIDELABEL:
				{
					actionDesc << ActionGotoGuideLabel(action.u.gotoGuide, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_WAIT:
				{
					actionDesc << ActionWait(action.u.waitAction, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_GOTOACTION:
				{					
					actionDesc << ActionGotoAction(action.u.gotoAction, nAction);				
				}
				break;
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
			case AR_ACTIVE_LINK_ACTION_SERVICE:
				{
					actionDesc << ActionService(action.u.service, nAction);
				}
				break;
#endif
			default:
				{
					actionDesc.str("");				
				}
				break;
			}

			row.AddCell(CTableCell(actionDesc.str()));
			tblListAction.AddRow(row);	
		}

		stringstream tblDesc;
		tblDesc.str("");
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << ifElse << "-Actions";
		tblListAction.description = tblDesc.str();

		strm << tblListAction;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in CDocAlActionStruct of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

//All matching Ids
string CDocAlActionStruct::AllMatchingIds(string table1, string table2, string description, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{		
		CARSchema schema1(table1);
		CARSchema schema2(table2);

		CTable tblListField("tblListMatchingIds", "TblObjectList");
		tblListField.AddColumn(0, "Id");
		tblListField.AddColumn(0, "Field Name");
		tblListField.AddColumn(0, "Value");

		if(schema1.Exists() && schema2.Exists())
		{
			unsigned int fieldCount1 = schema1.GetFields()->GetCount();
			for(unsigned int fieldIndex1 = 0; fieldIndex1 < fieldCount1; ++fieldIndex1)
			{
				CARField tmpField1(schema1.GetInsideId(), 0, fieldIndex1);

				if (tmpField1.GetDataType() <= AR_MAX_STD_DATA_TYPE)
				{
					CARField tmpField2(schema2.GetInsideId(), tmpField1.GetFieldId());

					if(tmpField2.Exists() && tmpField2.GetDataType() <= AR_MAX_STD_DATA_TYPE)
					{
						//Reference field1
						CFieldRefItem *refItemField1 = new CFieldRefItem();
						refItemField1->arsStructItemType = this->structItemType;
						refItemField1->fromName = this->obj->GetName();
						refItemField1->schemaInsideId = schema1.GetInsideId();
						refItemField1->fieldInsideId = tmpField1.GetInsideId();

						stringstream strmTmpDesc;
						strmTmpDesc.str("");
						strmTmpDesc << description << " (All Matching Ids) [Target] " << ifElse << "-Action " << nAction;
						refItemField1->description = strmTmpDesc.str();
						arIn->AddReferenceItem(refItemField1);
						delete refItemField1;

						//Reference field2
						CFieldRefItem *refItemField2 = new CFieldRefItem();
						refItemField2->arsStructItemType = this->structItemType;
						refItemField2->fromName = this->obj->GetName();
						refItemField2->schemaInsideId = schema2.GetInsideId();
						refItemField2->fieldInsideId = tmpField2.GetInsideId();

						strmTmpDesc.str("");
						strmTmpDesc << description << " (All Matching Ids) [Value] " << ifElse << "-Action " << nAction;
						refItemField2->description = strmTmpDesc.str();
						arIn->AddReferenceItem(refItemField2);
						delete refItemField2;


						//Matching ID
						CTableRow row("cssStdRow");		
						row.AddCell(CTableCell(tmpField1.GetFieldId()));
						row.AddCell(CTableCell(arIn->LinkToField(schema1.GetARName(), tmpField1.GetInsideId(), this->rootLevel)));
						row.AddCell(CTableCell(arIn->LinkToField(schema2.GetARName(), tmpField2.GetInsideId(), this->rootLevel)));
						tblListField.AddRow(row);
					}
				}
			}
		}	

		strm << tblListField;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlAllMatchingIds of " << table1 << ", " << table2 << ": " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_NONE
string CDocAlActionStruct::ActionNone(int nAction)
{
	return "None";
}

// AR_ACTIVE_LINK_ACTION_MACRO
string CDocAlActionStruct::ActionMacro(const ARActiveLinkMacroStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action.macroName != NULL)
		{
			if(action.macroName != NULL)
			{
				strm << "Macro Name: " << action.macroName << "<br/>" << endl;
			}
			else
			{
				strm << "Macro Name: <Unknown><br/>" << endl;
			}
		}

		if(action.macroParms.parms != NULL)
		{
			CTable tblMacro("tblMacroParameter", "TblObjectList");
			tblMacro.AddColumn(50, "Parameter Name");	
			tblMacro.AddColumn(50, "Value");

			for(unsigned int nCnt = 0; nCnt < action.macroParms.numItems; nCnt++)
			{
				if(action.macroParms.parms[nCnt].name != NULL
					&& action.macroParms.parms[nCnt].value != NULL)
				{
					stringstream pName, pValue;
					pName.str("");
					pValue.str("");

					pName << action.macroParms.parms[nCnt].name;
					pValue << action.macroParms.parms[nCnt].value;

					CFieldRefItem *refItemTmp = new CFieldRefItem();
					refItemTmp->arsStructItemType = this->structItemType;

					stringstream strmTmpDesc;
					strmTmpDesc << "Field in Macro " << ifElse << "-Action " << nAction;
					refItemTmp->description = strmTmpDesc.str();

					refItemTmp->fromName = this->obj->GetName();
					refItemTmp->schemaInsideId = schemaInsideId;	

					string parameterValue = arIn->TextFindFields(pValue.str(), "$", schemaInsideId, rootLevel, true, refItemTmp);
					delete refItemTmp;

					CTableRow mRow("cssStdRow");		
					mRow.AddCell(CTableCell(pName.str()));				
					mRow.AddCell(CTableCell(parameterValue));
					tblMacro.AddRow(mRow);	
				}
			}
			strm << tblMacro;
			strm << "<br/>Macro Text: <br/>" << action.macroText << endl;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionMacro of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_FIELDS
string CDocAlActionStruct::ActionSetFields(const ARSetFieldsActionStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{		
		//Find the seconds form in the set fields assignemt
		//Possible values: "", "*", "schemaName" 
		stringstream secondaryFormRaw, secondaryFormDisplay, serverRaw, qualification;
		secondaryFormRaw.str("");
		secondaryFormDisplay.str("");
		serverRaw.str("");
		qualification.str("");

		CDocActionSetFieldsHelper *alHelper = new CDocActionSetFieldsHelper(*arIn, *obj, action, structItemType, ifElse, nAction);
		alHelper->SetFieldsGetSecondaryForm(schemaName, rootLevel, secondaryFormRaw, secondaryFormDisplay, serverRaw, qualification);
		delete alHelper;

		//For the following internal calculations we need a secondary form
		string schemaName2 = secondaryFormRaw.str();

		string tmpDisplayName = secondaryFormDisplay.str();
		if(tmpDisplayName.size()==0)
			tmpDisplayName = schemaName2;

		strm << "Server Name: " << serverRaw.str() << "<br/>" << endl;
		strm << "Read Value for Field from: " << arIn->LinkToSchema(tmpDisplayName, rootLevel) << "<br/>" << endl;

		//Qualification
		strm << qualification.str() << endl;		

		//All matching Ids
		string setFieldInfo = "Field Mapping";
		for(unsigned int i= 0; i< action.fieldList.numItems; i++)
		{
			if(action.fieldList.fieldAssignList[i].fieldId == AR_LIKE_ID)
			{
				setFieldInfo = "Field Mapping: All Matching Ids";

				strm << this->AllMatchingIds(schemaName, tmpDisplayName, "Set Fields", nAction);
			}
		}

		if(strcmp(setFieldInfo.c_str(), "Field Mapping") ==  0)
		{
			strm << setFieldInfo << ":<br/>" << endl;

			CARAssignHelper assignHelper(*arIn, rootLevel, this->obj->GetName(), this->structItemType, schemaName, schemaName2);
			strm << assignHelper.SetFieldsAssignment(action, nAction, ifElse);
		}
		else
		{
			strm << setFieldInfo << endl;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionSetFields of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_PROCESS
string CDocAlActionStruct::ActionProcess(const char *action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action != NULL)
		{
			CFieldRefItem *refItemTmp = new CFieldRefItem();
			refItemTmp->arsStructItemType = this->structItemType;

			stringstream strmTmpDesc;
			strmTmpDesc << "Field in Run Process " << ifElse << "-Action " << nAction;
			refItemTmp->description = strmTmpDesc.str();

			refItemTmp->fromName = this->obj->GetName();
			refItemTmp->schemaInsideId = schemaInsideId;

			strm << arIn->TextFindFields(action, "$", refItemTmp->schemaInsideId, rootLevel, true, refItemTmp) << endl;
			delete refItemTmp;
		}
		else
		{
			strm << "No Run Process command specified." << endl;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionProcess of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_MESSAGE
string CDocAlActionStruct::ActionMessage(const ARMessageStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		strm << "Message Number: " << action.messageNum << "<br/>" << endl;
		strm << "Message Type: " << CAREnum::MessageType(action.messageType) << "<br/>" << endl;

		if(action.messageText != NULL)
		{
			CFieldRefItem *refItemTmp = new CFieldRefItem();
			refItemTmp->arsStructItemType = this->structItemType;

			stringstream strmTmpDesc;
			strmTmpDesc << "Message " << ifElse << "-Action " << nAction;
			refItemTmp->description = strmTmpDesc.str();

			refItemTmp->fromName = this->obj->GetName();
			refItemTmp->schemaInsideId = schemaInsideId;
			strm << "Message Text:<br/>" << arIn->TextFindFields(action.messageText, "$", this->schemaInsideId, rootLevel, true, refItemTmp) << "<br/>" << endl;
			delete refItemTmp;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionMessage of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_SET_CHAR
string CDocAlActionStruct::ActionSetChar(const ARFieldCharacteristics &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		stringstream strmTmpDesc;
		strmTmpDesc << "Change Field " << (action.option == AR_FIELD_CHAR_OPTION_REFERENCE ? "From Value " : "") << ifElse << "-Action " << nAction;

		CFieldRefItem *refItem = new CFieldRefItem(this->structItemType, this->obj->GetName(), strmTmpDesc.str(), action.fieldId, schemaInsideId);
		arIn->AddReferenceItem(refItem);
		delete refItem;	

		//FieldName
		strm << "Field ";
		if (action.option == AR_FIELD_CHAR_OPTION_REFERENCE)
		{
			strm << "(Use Value Of): $" << arIn->LinkToField(schemaName, action.fieldId, rootLevel) << "$<br/>" << endl;
		}
		else
		{
			strm << "Name: " << arIn->LinkToField(schemaName, action.fieldId, rootLevel) << "<br/>" << endl;
		}

		//Access Option
		strm << "Field Access: " << CAREnum::SetCharFieldAccess(action.accessOption) << "<br/>" << endl;					


		//Font
		string tmp = "";
		for(unsigned int i=0; i< action.props.numItems; i++)
		{	
			switch(action.props.props[i].prop)
			{
			case AR_DPROP_LABEL_FONT_STYLE:
				{
					if(action.props.props[i].value.u.charVal != NULL)
					{
						tmp = action.props.props[i].value.u.charVal;
					}
				}
				break;
			}
		}
		if(tmp.size() > 0)
			strm << "Field Font: " << tmp << "<br/>" << endl;
		else
			strm << "Field Font: Unchanged<br/>" << endl;



		//Visibility
		tmp = "";
		for(unsigned int i=0; i< action.props.numItems; i++)
		{	
			switch(action.props.props[i].prop)
			{				
			case AR_DPROP_VISIBLE:
				{					
					if(action.props.props[i].value.dataType == AR_DATA_TYPE_ENUM || action.props.props[i].value.dataType == AR_DATA_TYPE_INTEGER)
					{
						tmp =  CAREnum::SetCharFieldVisibility(action.props.props[i].value.u.intVal);
					}
				}
				break;
			}
		}

		if(tmp.size() > 0)
			strm << "Visibility: " << tmp << "<br/>" << endl;
		else
			strm << "Visibility: Unchanged<br/>" << endl;



		//CharMenu
		if(action.charMenu != NULL)
		{
			if(strcmp(action.charMenu, "") != 0)
			{
				bool menuFound = false;
				strm << "Menu: " << arIn->LinkToMenu(action.charMenu, rootLevel, &menuFound) << "<br/>" << endl;

				if (!menuFound)
				{
					CMissingMenuRefItem refItemNotFound(action.charMenu, AR_STRUCT_ITEM_XML_ACTIVE_LINK, this->obj->GetName());
					arIn->AddMissingMenu(refItemNotFound);
				}
			}
		}

		//Set focus to field
		if(action.focus == AR_FOCUS_SET_TO_FIELD)
		{
			strm << "<input type=\"checkbox\" name=\"fieldSetFocus\" value=\"fieldSetFocus\" checked>Set Focus to Field" << "<br/>" << endl;
		}	


		for(unsigned int i=0; i< action.props.numItems; i++)
		{	
			switch(action.props.props[i].prop)
			{
			case AR_DPROP_LABEL_COLOR_TEXT:
				{
					stringstream strmTmp;
					//strmTmp << CARValue::ValueToString(action.props.props[i].value);

					bool colorUnknown = true;
					char color[7];
					size_t colorStrLen = 0;

					if (action.props.props[i].value.dataType == AR_DATA_TYPE_CHAR)
					{
						colorStrLen = strlen(action.props.props[i].value.u.charVal);
					}

					if (action.props.props[i].value.dataType == AR_DATA_TYPE_CHAR && strncmp(action.props.props[i].value.u.charVal,"0x",2) == 0 && colorStrLen == 8)
					{
						color[0] = action.props.props[i].value.u.charVal[6];
						color[1] = action.props.props[i].value.u.charVal[7];
						color[2] = action.props.props[i].value.u.charVal[4];
						color[3] = action.props.props[i].value.u.charVal[5];
						color[4] = action.props.props[i].value.u.charVal[2];
						color[5] = action.props.props[i].value.u.charVal[3];
						color[6] = 0;
						colorUnknown = false;
					}

					strm << "Label Color: ";

					if(colorStrLen > 0)
					{
						if (colorUnknown)
						{
							strm << CARValue::ValueToString(action.props.props[i].value);
						}
						else
						{
							strm << "<span style='background-color:#" << color << "; width:16px; height:16px;'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>";
						}
					}
					else
						strm << "Default";

					strm << "<br/>" << endl;
				}
				break;
			case AR_DPROP_REFRESH:
				{
					strm << "<input type=\"checkbox\" name=\"refTableFieldInAl\" value=\"refrTblField\" checked>Refresh Table Field" << endl;
				}
				break;
			case AR_DPROP_LABEL:
				{	
					//if(action.props.props[i].value != NULL)
					{
						stringstream strmTmp;
						strmTmp << CARValue::ValueToString(action.props.props[i].value);
						if(strmTmp.str().size() > 0)
						{
							CFieldRefItem *refItemTmp = new CFieldRefItem();
							refItemTmp->arsStructItemType = this->structItemType;

							stringstream strmTmpDesc;
							strmTmpDesc << "Change Field Label " << ifElse << "-Action " << nAction;
							refItemTmp->description = strmTmpDesc.str();

							refItemTmp->fromName = this->obj->GetName();
							refItemTmp->schemaInsideId = schemaInsideId;

							strm << "Field Label: " << arIn->TextFindFields(strmTmp.str(), "$", schemaInsideId, rootLevel, true, refItemTmp)  << "<br/>" << endl;					
							delete refItemTmp;
						}
					}
				}
				break;

			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionSetChar of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_DDE
string CDocAlActionStruct::ActionDde(const ARDDEStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		strm << "Action: " << CAREnum::DDEAction(action.action) << "<br/>" << endl;					

		if(action.serviceName != NULL)
			strm << "Service Name: " << action.serviceName << "<br/>" << endl;

		if(action.topic != NULL)
			strm << "Topic: " << action.topic << "<br/>" << endl;

		if(action.item != NULL)
			strm << "Item: " << action.item << "<br/>" << endl;

		if(action.pathToProgram != NULL)
			strm << "Path: " << action.pathToProgram << "<br/>" << endl;

		if(action.command != NULL)
			strm << "Command: " << action.command << "<br/>" << endl;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionDde of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_FIELDP
string CDocAlActionStruct::ActionPushFields(const ARPushFieldsActionStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		string pushServer;
		string pushSchema;

		// target server **************************************
		strm << "Server Name: ";
		if (action.pushFieldsList.pushFieldsList[0].field.server[0] == '$' && action.sampleServer[0] != 0)
		{
			int fieldId = atoi(&action.pushFieldsList.pushFieldsList[0].field.server[1]);

			pushServer = action.sampleServer;
			strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$ (Sample Server: " << arIn->LinkToServerInfo(action.sampleServer, rootLevel) << ")";

			// create field reference
			stringstream tmpDesc;
			tmpDesc << "Server Name in 'Push Fields' " << ifElse << "-Action " << nAction;
			CFieldRefItem refItem(this->structItemType, this->obj->GetName(), tmpDesc.str(), fieldId, schemaInsideId);
			arIn->AddReferenceItem(&refItem);
		}
		else
		{
			pushServer = action.pushFieldsList.pushFieldsList[0].field.server;
			strm << arIn->LinkToServerInfo(pushServer, rootLevel);
		}
		strm << "<br/>" << endl;

		// target form ******************************************
		strm << "Push Value To: ";		
		if (action.pushFieldsList.pushFieldsList[0].field.schema[0] == '$' && action.sampleSchema[0] != 0)
		{
			int fieldId = atoi(&action.pushFieldsList.pushFieldsList[0].field.schema[1]);

			if (action.sampleSchema[0] == '@' && action.sampleSchema[1] == 0)
			{
				pushSchema = schemaName;
			}
			else
			{
				pushSchema = action.sampleSchema;
			}
			strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$ (Sample Schema: " << arIn->LinkToSchema(action.sampleSchema, rootLevel) << ")";

			// create field reference
			stringstream tmpDesc;
			tmpDesc << "Form Name in 'Push Fields' " << ifElse << "-Action " << nAction;
			CFieldRefItem refItem(this->structItemType, this->obj->GetName(), tmpDesc.str(), fieldId, schemaInsideId);
			arIn->AddReferenceItem(&refItem);
		}
		else
		{
			pushSchema = action.pushFieldsList.pushFieldsList[0].field.schema;
			strm << arIn->LinkToSchema(pushSchema, rootLevel);
		}
		strm << "<br/>" << endl;

		// push field if **************************************
		strm << "<br/>Push Field If<br/>" << endl;
		stringstream strmTmpQual;

		CFieldRefItem *refItem = new CFieldRefItem(this->structItemType, this->obj->GetName(), "Push Field If", -1, -1);
		CARQualification arQual(*arIn);

		int pFormId = this->arIn->SchemaGetInsideId(schemaName);
		int sFormId = this->arIn->SchemaGetInsideId(pushSchema);

		arQual.CheckQuery(&action.pushFieldsList.pushFieldsList[0].field.qualifier, *refItem, 0, pFormId, sFormId, strmTmpQual, rootLevel);

		delete refItem;

		if(strmTmpQual.str().length() > 0)
		{
			strm << strmTmpQual.str() << "<br/><br/>" << endl;
		}
		else
		{
			strm << EmptyValue << "<br/><br/>" << endl;
		}

		strm << "If No Requests Match: " << CAREnum::NoMatchRequest(action.pushFieldsList.pushFieldsList[0].field.noMatchOption) << "<br/>" << endl;
		strm << "If Multiple Requests Match: " << CAREnum::MultiMatchRequest(action.pushFieldsList.pushFieldsList[0].field.multiMatchOption) << "<br/><br/>" << endl;

		// push field mapping *********************************
		strm << "Field Mapping:";
		if(action.pushFieldsList.pushFieldsList[0].field.u.fieldId == AR_LIKE_ID)
		{
			strm << " All Matching Ids<br/>";
			strm << this->AllMatchingIds(schemaName, pushSchema, "Push Fields", nAction);
		}
		else
		{
			strm << "<br/>" << endl;
			CARAssignHelper assignHelper(*arIn, rootLevel, obj->GetName(), this->structItemType, schemaName, pushSchema);
			strm << assignHelper.PushFieldsAssignment(action, nAction, ifElse);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionPushFields of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_SQL
string CDocAlActionStruct::ActionSql(const ARSQLStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{		
		if(action.server != NULL)
			strm << "Server: " << arIn->LinkToServerInfo(action.server, rootLevel) << "<br/>" << endl;

		if(action.command != NULL)
		{			
			CFieldRefItem *refItemTmp = new CFieldRefItem();
			refItemTmp->arsStructItemType = this->structItemType;

			stringstream strmTmpDesc;
			strmTmpDesc << "Value in Direct SQL " << ifElse << "-Action " << nAction;
			refItemTmp->description = strmTmpDesc.str();

			refItemTmp->fromName = this->obj->GetName();
			refItemTmp->schemaInsideId = schemaInsideId;

			strm << "SQL command: <br/>" << arIn->TextFindFields(action.command, "$", refItemTmp->schemaInsideId, rootLevel, true, refItemTmp) << endl;
			delete refItemTmp;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionSql of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_AUTO
string CDocAlActionStruct::ActionAutomation(const ARAutomationStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action.action != NULL)
			strm << "Action: " << action.action << "<br/>" << endl;

		if(action.autoServerName != NULL)
			strm << "Server Name: " << action.autoServerName << "<br/>" << endl;

		if(action.clsId != NULL)
			strm << "CLS Id: " << action.clsId << "<br/>" << endl;

		if(action.methodList.methodList != NULL)
		{
			CTable tblOleMethods("tblOleMethods", "TblObjectList");
			tblOleMethods.AddColumn(30, "Id");	
			tblOleMethods.AddColumn(30, "Name");

			for(unsigned int nCnt = 0; nCnt < action.methodList.numItems; nCnt++)
			{
				stringstream mId, mName, mValue;
				mId.str("");
				mName.str("");
				mValue.str("");

				if(action.methodList.methodList[nCnt].methodIId != NULL)
					mId << action.methodList.methodList[nCnt].methodIId;

				if(action.methodList.methodList[nCnt].methodName != NULL)
					mName << action.methodList.methodList[nCnt].methodName;


				CTableRow mRow("cssStdRow");
				mRow.AddCell( CTableCell(mId.str()));
				mRow.AddCell( CTableCell(mName.str()));
				tblOleMethods.AddRow(mRow);	
			}	
			strm << tblOleMethods;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionAutomation of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}


bool CDocAlActionStruct::ActionOpenDlgQualifier(int nWindowType)
{
	switch(nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DETAIL:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_LST:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_SPLIT:
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY_LST:
	case AR_ACTIVE_LINK_ACTION_OPEN_REPORT:
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY_DETAIL:
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY_SPLIT:
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY:
		return true;
	default: return false;
	}
}

// AR_ACTIVE_LINK_ACTION_OPENDLG
string CDocAlActionStruct::ActionOpenDlg(const AROpenDlgStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		string openWindowServer;
		string openWindowSchema;

		// check if we need to get the sample data
		if(action.serverName[0] == '$')
		{
			CDocActionOpenWindowHelper::GetSampleData(*obj, ifElse, nAction, openWindowServer, openWindowSchema);
		}

		// window type
		int windowMode = CAREnum::OpenWindowModeMapped(action.windowMode);
		strm << "<p>Window Type: " << CAREnum::OpenWindowMode(windowMode) << endl;

		// display type
		if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_MODIFY || windowMode == AR_ACTIVE_LINK_ACTION_OPEN_DSPLY)
		{
			strm << "<br/>Display Type: ";
			switch (action.windowMode)
			{
			case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_LST:
			case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY_LST:
				strm << "List Only";
				break;
			case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DETAIL:
			case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY_DETAIL:
				strm << "Details Only";
				break;
			case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_SPLIT:
			case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY_SPLIT:
				strm << "Split Window";
				break;
			case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY:
			case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY:
				strm << "&lt;Clear&gt;";
			}
			strm << endl;
		}

		// target location
		if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_SEARCH || windowMode == AR_ACTIVE_LINK_ACTION_OPEN_SUBMIT ||
		    windowMode == AR_ACTIVE_LINK_ACTION_OPEN_MODIFY || windowMode == AR_ACTIVE_LINK_ACTION_OPEN_REPORT)
		{
			strm << "<br/>Target Location: ";

			if (action.targetLocation != NULL) 
			{
				if (action.targetLocation[0] == '$')
				{
					int fieldId = atoi(&action.targetLocation[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$";

					if (fieldId > 0)
					{
						stringstream strmTmpDesc;
						strmTmpDesc << "Window Open Location " << ifElse << "-Action " << nAction;

						CFieldRefItem *refItemLocation = new CFieldRefItem(structItemType, obj->GetName(), strmTmpDesc.str(), fieldId, schemaInsideId);
						arIn->AddReferenceItem(refItemLocation);
						delete refItemLocation;
					}
				}
				else
				{
					strm << action.targetLocation;
				}
			}
			strm << endl;
		}
		strm << "</p>" << endl;

		// ****************************************************
		// server
		strm << "<p>Server Name: ";
		if(action.serverName[0] == '$' && !openWindowServer.empty())
		{
			int fieldId = atoi(&action.serverName[1]);
			strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$ (Sample Server: " << arIn->LinkToServerInfo(openWindowServer, rootLevel) << ")";

			if (fieldId > 0)
			{
				stringstream strmTmpDesc;
				strmTmpDesc << "Window Open Server Name " << ifElse << "-Action " << nAction;
				CFieldRefItem *refItemSrvName = new CFieldRefItem(structItemType, obj->GetName(), strmTmpDesc.str(), fieldId, schemaInsideId);
				arIn->AddReferenceItem(refItemSrvName);
				delete refItemSrvName;
			}
		}
		else
		{
			openWindowServer = action.serverName;
			strm << arIn->LinkToServerInfo(openWindowServer, rootLevel);
		}

		strm << "<br/>Form Name: ";
		if (action.schemaName[0] == '$')
		{
			int fieldId = atoi(&action.schemaName[1]);
			strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$ (Sample Schema: " << arIn->LinkToSchema(openWindowSchema, rootLevel) << ")";

			if (fieldId > 0)
			{
				stringstream strmTmpDesc;
				strmTmpDesc << "Window Open Form Name " << ifElse << "-Action " << nAction;
				CFieldRefItem *refItemSrvName = new CFieldRefItem(structItemType, obj->GetName(), strmTmpDesc.str(), fieldId, schemaInsideId);
				arIn->AddReferenceItem(refItemSrvName);
				delete refItemSrvName;
			}
		}
		else
		{
			openWindowSchema = action.schemaName;
			strm << arIn->LinkToSchema(openWindowSchema, rootLevel) << endl;
		}

		strm << "<br/>View Name: ";
		if(action.vuiLabel[0] == '$')// == NULL || strcmp(action.vuiLabel, "")==0)
		{
			int fieldId = atoi(&action.vuiLabel[1]);
			strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$";

			if (fieldId > 0)
			{
				stringstream strmTmpDesc;
				strmTmpDesc << "Window Open View Name " << ifElse << "-Action " << nAction;
				CFieldRefItem *refItemSrvName = new CFieldRefItem(structItemType, obj->GetName(), strmTmpDesc.str(), fieldId, schemaInsideId);
				arIn->AddReferenceItem(refItemSrvName);
				delete refItemSrvName;
			}
		}
		else
		{
			if (action.vuiLabel[0] == 0)
			{ 
				strm << "(Clear)" << endl;
			}
			else
			{
				CARSchema rSchema(openWindowSchema);
				if (!rSchema.Exists())
				{
					strm << "<span class=\"fieldNotFound\">" << action.vuiLabel << "</span>";
				}
				else
				{
					strm << rSchema.LinkToVui(action.vuiLabel, rootLevel);	
				}
			}
		}
		strm << "</p>" << endl;

		// report details here
		string *entryIDs = NULL;
		string *queryOverride = NULL;
		string *reportOperation = NULL;
		string *charEncoding = NULL;
		if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_REPORT)
		{
			string reportType;
			string reportLocation;
			string reportName;
			string reportDestination;
			entryIDs = new string();
			queryOverride = new string();
			reportOperation = new string();
			charEncoding = new string();

			if (!CDocActionOpenWindowHelper::GetReportData(action.reportString, reportType, reportLocation, reportName, reportDestination, *entryIDs, *queryOverride, *reportOperation, *charEncoding))
			{
				strm << "<p><span class=\"fieldNotFound\">Could not load report informations!</span></p>";
			}
			else
			{
				stringstream tmpDesc;

				strm << "<p>Report Type: ";
				if (!reportType.empty() && reportType[0] == '$')
				{
					int fieldId = atoi(&reportType.c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$";
					if (fieldId > 0)
					{
						tmpDesc.str("");
						tmpDesc << "Window Open Report Type " << ifElse << "-Action " << nAction;
						CFieldRefItem refItem(structItemType, obj->GetName(), tmpDesc.str(), fieldId, schemaInsideId);
						arIn->AddReferenceItem(&refItem);
					}
				}
				else
				{
					strm << reportType;
				}
				strm <<  "<br/>";

				int iReportLocation = atoi(reportLocation.c_str());
				strm << "Report Location: " << CAREnum::ReportLocation(iReportLocation);
				
				strm << "<br/>Report Name: ";
				if (!reportName.empty() && reportName[0] == '$')
				{
					int fieldId = atoi(&reportName.c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$";
					if (fieldId > 0)
					{
						tmpDesc.str("");
						tmpDesc << "Window Open Report Name " << ifElse << "-Action " << nAction;
						CFieldRefItem refItem(structItemType, obj->GetName(), tmpDesc.str(), fieldId, schemaInsideId);
						arIn->AddReferenceItem(&refItem);
					}
				}
				else
				{
					strm << reportName;
				}
				strm << "<br/>";
				
				// to-screen:
				// to-print:
				// to-file:
				strm << "Report Destination: ";
				if (!reportDestination.empty() && reportDestination[0] == '$')
				{
					int fieldId = atoi(&reportDestination.c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$";
					if (fieldId > 0)
					{
						tmpDesc.str("");
						tmpDesc << "Window Open Report Destination " << ifElse << "-Action " << nAction;
						CFieldRefItem refItem(structItemType, obj->GetName(), tmpDesc.str(), fieldId, schemaInsideId);
						arIn->AddReferenceItem(&refItem);
					}
				}
				else
				{
					// map the string value to the displayed combobox item of dev studio
					strm << reportDestination;
				}
				strm << "</p>";
			}
		}

		// qualification
		if(ActionOpenDlgQualifier(action.windowMode))
		{
			if(action.query.operation != NULL)
			{
				stringstream strmTmpQual;

				strmTmpQual << "Open Window Qualification " << ifElse << "-Action " << nAction;

				CFieldRefItem *refItem = new CFieldRefItem();
				refItem->arsStructItemType = this->structItemType;
				refItem->description = strmTmpQual.str();
				refItem->fromName = this->obj->GetName();

				strmTmpQual.str("");
				CARQualification arQual(*this->arIn);
				int pFormId = schemaInsideId; // this->arIn->SchemaGetInsideId(schemaName.c_str());
				int sFormId = this->arIn->SchemaGetInsideId(openWindowSchema);
				arQual.CheckQuery(&action.query, *refItem, 0, pFormId, sFormId, strmTmpQual, rootLevel);

				delete refItem;

				strm << "<p>Qualification:<br/>" << endl;
				strm << strmTmpQual.str() << "</p>";
			}	
		}

		strm << "<p>";
		// show close button
		if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_DLG)
		{
			strm << "<input type=\"checkbox\" name=\"closeWnd\" value=\"closeWndAll\" " << (action.closeBox ? "checked" : "") << ">Show Close Button in Dialog<br/>" << endl;
		}

		// suppres empty list
		if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_MODIFY || windowMode == AR_ACTIVE_LINK_ACTION_OPEN_DSPLY)
		{
			strm << "<input type=\"checkbox\" name=\"suprEmptyLst\" value=\"suprEmptyLstVal\" " << (action.suppressEmptyLst ? "checked" : "") << ">Suppress Empty List<br/>" << endl;		
		}

		// set fields to defaults
		bool setToDefault = false;
		if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_SEARCH || windowMode == AR_ACTIVE_LINK_ACTION_OPEN_SUBMIT)
		{

			for(unsigned int i= 0; i < action.inputValueFieldPairs.numItems; ++i)
			{
				if (action.inputValueFieldPairs.fieldAssignList[i].fieldId == AR_SET_DEFAULTS_ID)
				{
					setToDefault = true;
					break;
				}
			}

			strm << "<input type=\"checkbox\" name=\"setToDefault\" value=\"setToDefaultVal\" " << (setToDefault ? "checked" : "") << ">Set Fields To Defaults<br/>" << endl;		
		}

		// input mapping
		if ((windowMode == AR_ACTIVE_LINK_ACTION_OPEN_DLG || windowMode == AR_ACTIVE_LINK_ACTION_OPEN_SEARCH || windowMode == AR_ACTIVE_LINK_ACTION_OPEN_SUBMIT) && !setToDefault)
		{
			CARAssignHelper assignHelper(*arIn, rootLevel, this->obj->GetName(), this->structItemType, openWindowSchema, schemaName);
			//assignHelper.pushFieldFlag = true;
			strm << assignHelper.OpenWindowAssignment(action.inputValueFieldPairs, nAction, ifElse, "Open Window");
		}

		// output mapping (dialog on close)
		if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_DLG)
		{
			strm << "On Dialog Close Action:<br/>";

			CARAssignHelper assignHelper(*arIn, rootLevel, this->obj->GetName(), this->structItemType, schemaName, openWindowSchema);
			strm << assignHelper.CloseWindowAssignment(action.outputValueFieldPairs, nAction, ifElse, "Close Window");
		}
		strm << "</p>";

		if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_DSPLY || windowMode == AR_ACTIVE_LINK_ACTION_OPEN_MODIFY || windowMode == AR_ACTIVE_LINK_ACTION_OPEN_REPORT)
		{
			// message
			strm << "<p>If No Request Match: ";
			if (action.noMatchContinue)
			{
				strm << "Do not show any message";
			}
			else if (action.msg.messageText == NULL || action.msg.messageText[0] == 0)	// dont know, if thats the correct criteria for the default message
			{
				strm << "Show default message";
			}
			else if (action.msg.messageType > 0)
			{
				strm << "Show message<br/>";
				strm << ActionMessage(action.msg, nAction);
			}
			strm << "</p>" << endl;

			// sorting
			if (action.sortOrderList.numItems > 0)	
			{
				CTable tblSortList("sortList", "TblObjectList");
				tblSortList.AddColumn(40, "Field Name");
				tblSortList.AddColumn(15, "Field Id");
				tblSortList.AddColumn(15, "Field Type");
				tblSortList.AddColumn(30, "Sort Order");
				
				char strFieldId[20];				
				CARSchema rSchema(openWindowSchema);

				for (unsigned int i = 0; i < action.sortOrderList.numItems; ++i)
				{
					CARField rField(rSchema.GetInsideId(), action.sortOrderList.sortList[i].fieldId);

					strFieldId[0] = 0;
					sprintf(strFieldId, "%d", action.sortOrderList.sortList[i].fieldId);

					CTableRow row("cssStdRow");
					row.AddCell(arIn->LinkToField(openWindowSchema, action.sortOrderList.sortList[i].fieldId, rootLevel));
					row.AddCell(strFieldId);
					row.AddCell((rField.Exists() ? CAREnum::DataType(rField.GetDataType()) : "Unknown"));
					row.AddCell(CAREnum::SchemaSortOrder(action.sortOrderList.sortList[i].sortOrder));
					
					tblSortList.AddRow(row);

					if (rSchema.Exists())
					{
						stringstream tmpDesc;
						tmpDesc << "Open Window SortBy " << ifElse << "-Action " << nAction;
						CFieldRefItem refItem(structItemType, this->obj->GetName(), tmpDesc.str(), action.sortOrderList.sortList[i].fieldId, rSchema.GetInsideId());
						arIn->AddReferenceItem(&refItem);
					}
				}

				strm << "<p>Sort Order" << tblSortList << "</p>";
			}

			// additional report informations
			if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_REPORT)
			{
				stringstream tmpDesc;

				strm << "<p>EntryIDs: ";
				if (!entryIDs->empty() && entryIDs->operator[](0) == '$')
				{
					int fieldId = atoi(&entryIDs->c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$";
					if (fieldId > 0)
					{
						tmpDesc.str("");
						tmpDesc << "Window Open EntryIDs " << ifElse << "-Action " << nAction;
						CFieldRefItem refItem(structItemType, obj->GetName(), tmpDesc.str(), fieldId, schemaInsideId);
						arIn->AddReferenceItem(&refItem);
					}
				}
				else
				{
					strm << *entryIDs;
				}
				
				strm << "<br/>Query Override: ";
				if (!queryOverride->empty() && queryOverride->operator[](0) == '$')
				{
					int fieldId = atoi(&entryIDs->c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$";
					if (fieldId > 0)
					{
						tmpDesc.str("");
						tmpDesc << "Window Open Query Override " << ifElse << "-Action " << nAction;
						CFieldRefItem refItem(structItemType, obj->GetName(), tmpDesc.str(), fieldId, schemaInsideId);
						arIn->AddReferenceItem(&refItem);
					}
				}
				else
				{
					strm << *queryOverride;
				}
				
				strm << "<br/>Report Operation: ";
				int iReportOperation = atoi(reportOperation->c_str());
				if (iReportOperation == 0) iReportOperation = 2;	// set default to Run
				strm << CAREnum::ReportOperation(iReportOperation);

				strm << "<br/>Character Encoding: ";
				if (!charEncoding->empty() && charEncoding->operator[](0) == '$')
				{
					int fieldId = atoi(&charEncoding->c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$";
					if (fieldId > 0)
					{
						tmpDesc.str("");
						tmpDesc << "Window Open Character Encoding " << ifElse << "-Action " << nAction;
						CFieldRefItem refItem(structItemType, obj->GetName(), tmpDesc.str(), fieldId, schemaInsideId);
						arIn->AddReferenceItem(&refItem);
					}
				}
				else
				{
					strm << *charEncoding;
				}
				strm << "</p>" << endl;

				// clean up
				delete entryIDs;
				delete queryOverride;
				delete reportOperation;
				delete charEncoding;
			}

			// polling interval
			if ((windowMode == AR_ACTIVE_LINK_ACTION_OPEN_DSPLY || windowMode == AR_ACTIVE_LINK_ACTION_OPEN_MODIFY) && action.pollinginterval > 0)
			{
				strm << "<p>Polling interval: " << action.pollinginterval << "</p>" << endl;
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionOpenDlg of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_COMMITC
string CDocAlActionStruct::ActionCommitChanges(const ARCommitChangesStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		strm << "Schema: " << arIn->LinkToSchema(action.schemaName, rootLevel) << "<br/>" << endl;
		strm << "The Commit Changes action is applicable to regular form, join form or dialog box. ";
		strm << "When the Open Window active link action is set to Dialog Window Type Commit Changes pushes predetermined values to fields in the parent form. ";
		strm << "The values for these fields are specified in the Open Window active link action when the Field Mapping Mode is set to On Close. ";
		strm << "When called within a regular form or join form, this action applies the changes." << endl;		
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionCommitChanges of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_CLOSEWND
string CDocAlActionStruct::ActionCloseWindow(const ARCloseWndStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{		
		if(action.closeAll == TRUE)
			strm << "<input type=\"checkbox\" name=\"closeWnd\" value=\"closeWndAll\" checked>Close All Windows" << endl;
		else
			strm << "<input type=\"checkbox\" name=\"closeWnd\" value=\"closeWndAll\">Close All Windows" << endl;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionCloseWindow of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_CALLGUIDE
string CDocAlActionStruct::ActionCallGuide(const ARCallGuideStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if (action.serverName[0] == '$' /*&& action.sampleServer[0] != 0*/)
		{
			int fieldId = atoi(&action.serverName[1]);
			strm << "Server: " << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(this->schemaName, fieldId, rootLevel)) << "$<br/>" << endl;

			stringstream strmTmpDesc;
			strmTmpDesc << "Used as CallGuide Server in " << ifElse << "-Action " << nAction;

			CFieldRefItem *refItem = new CFieldRefItem(this->structItemType, this->obj->GetName(), strmTmpDesc.str(), fieldId, schemaInsideId);
			arIn->AddReferenceItem(refItem);
			delete refItem;
		}
		else
		{
			strm << "Server : " << arIn->LinkToServerInfo(action.serverName, rootLevel) << "<br/>" << endl;
		}

		if (action.guideName[0] == '$' /*&& action.sampleGuide[0] != 0*/)
		{
			int fieldId = atoi(&action.guideName[1]);
			strm << "Guide: $" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(this->schemaName, fieldId, rootLevel)) << "$<br/>" << endl;

			stringstream strmTmpDesc;
			strmTmpDesc << "Used as CallGuide Name in " << ifElse << "-Action " << nAction;

			CFieldRefItem *refItem = new CFieldRefItem(this->structItemType, this->obj->GetName(), strmTmpDesc.str(), fieldId, schemaInsideId);
			arIn->AddReferenceItem(refItem);
			delete refItem;
		}
		else
		{
			strm << "Guide: " << arIn->LinkToContainer(action.guideName, rootLevel) << "<br/>" << endl;
		}

		if(action.guideTableId > 0)
		{
			strm << "Table Loop: " << arIn->LinkToField(schemaName, action.guideTableId, rootLevel) << "<br/>" << endl;

			if(action.guideMode == AR_CALL_GUIDE_LOOP_SELECTED_ONLY)
				strm << "<input type=\"checkbox\" name=\"tblLoopInGuide\" value=\"loopTbl\" checked>Table Loop Selected Rows Only" << endl;
			else
				strm << "<input type=\"checkbox\" name=\"tblLoopInGuide\" value=\"loopTbl\">Table Loop Selected Rows Only" << endl;

			stringstream strmTmpDesc;
			strmTmpDesc << "Guide Table Loop " << ifElse << "-Action " << nAction;

			CFieldRefItem *refItem = new CFieldRefItem(this->structItemType, this->obj->GetName(), strmTmpDesc.str(), action.guideTableId, schemaInsideId);
			arIn->AddReferenceItem(refItem);
			delete refItem;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionCallGuide of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_EXITGUIDE
string CDocAlActionStruct::ActionExitGuide(const ARExitGuideStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{		
		if(action.closeAll == TRUE)
		{
			strm << "<input type=\"checkbox\" name=\"exitGuide\" value=\"exitGuideCloseAll\" checked>Close all guides on exit" << endl;
		}
		else
		{
			strm << "<input type=\"checkbox\" name=\"exitGuide\" value=\"exitGuideCloseAll\">Close all guides on exit" << endl;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionExitGuide of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_GOTOGUIDELABEL
string CDocAlActionStruct::ActionGotoGuideLabel(const ARGotoGuideLabelStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action.label != NULL)
		{
			strm << "Guide Label: " << action.label << endl;	
		}
		else
		{
			strm << "Guide Label: " << EmptyValue << endl;	
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionGuideLabel of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_WAIT
string CDocAlActionStruct::ActionWait(const ARWaitStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action.continueButtonTitle != NULL)
		{
			strm << "Label for Continue Button: " << action.continueButtonTitle << endl;
		}
		else
		{
			strm << "Label for Continue Button: " << EmptyValue << endl;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionWait of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_GOTOACTION
string CDocAlActionStruct::ActionGotoAction(const ARGotoActionStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action.fieldIdOrValue != NULL)
		{
			strm << "FieldId or Value: " << arIn->LinkToField(schemaName, action.fieldIdOrValue, rootLevel) << endl;
		}
		else
		{
			strm << "FieldId or Value: " << EmptyValue << endl;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionGotoAction of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_710
// AR_ACTIVE_LINK_ACTION_SERVICE
string CDocAlActionStruct::ActionService(const ARActiveLinkSvcActionStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");
	string serviceServer;
	string serviceSchema;

	try
	{
		strm << "<p>";
		// check for sample data for the server part
		if (action.serverName[0] == '$' && action.sampleServer[0] != 0)
		{
			int fieldId = atoi(&action.serverName[1]);
			serviceServer = action.sampleServer;			
			strm << "Server Name: " << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(this->schemaName, fieldId, rootLevel)) << "$ (Sample Server: " << arIn->LinkToServerInfo(action.sampleServer, rootLevel) << ")<br/>" << endl;

			stringstream strmTmpDesc;
			strmTmpDesc << "Used as Service Server in " << ifElse << "-Action " << nAction;

			CFieldRefItem *refItem = new CFieldRefItem(this->structItemType, this->obj->GetName(), strmTmpDesc.str(), fieldId, schemaInsideId);
			arIn->AddReferenceItem(refItem);
			delete refItem;
		}
		else
		{
			serviceServer = action.serverName;
			strm << "Server Name: " << arIn->LinkToServerInfo(action.serverName, rootLevel) << "<br/>" << endl;
		}

		if (action.serviceSchema[0] == '$' && action.sampleSchema[0] != 0)
		{
			int fieldId = atoi(&action.serviceSchema[1]);

			if (action.sampleSchema[0] == '@' && action.sampleSchema[1] == 0)
			{
				serviceSchema = schemaName;
			}
			else
			{
				serviceSchema = action.sampleSchema;
			}

			strm << "Service Form: " << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(this->schemaName, fieldId, rootLevel)) << "$ (Sample Form: " << arIn->LinkToSchema(serviceSchema, rootLevel) << ")<br/>" << endl;

			stringstream strmTmpDesc;
			strmTmpDesc << "Used as Service Form in " << ifElse << "-Action " << nAction;

			CFieldRefItem *refItem = new CFieldRefItem(this->structItemType, this->obj->GetName(), strmTmpDesc.str(), fieldId, schemaInsideId);
			arIn->AddReferenceItem(refItem);
			delete refItem;
		}
		else
		{
			serviceSchema = action.serviceSchema;
			strm << "Service Form: " << arIn->LinkToSchema(action.serviceSchema, rootLevel) << "<br/>" << endl;
		}

		strm << "Request Id: ";
		if (action.requestIdMap != NULL)
		{
			strm << arIn->LinkToField(schemaName, action.requestIdMap, rootLevel);
			
			stringstream strmTmpDesc;
			strmTmpDesc << "Service Request-Id " << ifElse << "-Action " << nAction;

			CFieldRefItem *refItem = new CFieldRefItem(this->structItemType, this->obj->GetName(), strmTmpDesc.str(), action.requestIdMap, schemaInsideId);
			arIn->AddReferenceItem(refItem);
			delete refItem;
		}
		strm << "</p>" << endl;

		// input mapping
		strm << "Input Mapping: "; if (action.inputFieldMapping.numItems == 0) strm << "None"; strm << "<br/>" << endl;
		if (action.inputFieldMapping.numItems > 0)
		{
			CARAssignHelper assignHelper(*arIn, rootLevel, this->obj->GetName(), this->structItemType, serviceSchema, schemaName);
			strm << assignHelper.ServiceAssignment(action.inputFieldMapping, nAction, ifElse, "Service Input Mapping");
		}

		// output mapping
		strm << "Output Mapping: "; if (action.outputFieldMapping.numItems == 0) strm << "None"; strm << "<br/>" << endl;
		if (action.outputFieldMapping.numItems > 0)
		{
			CARAssignHelper assignHelper(*arIn, rootLevel, this->obj->GetName(), this->structItemType, schemaName, serviceSchema);
			strm << assignHelper.ServiceAssignment(action.outputFieldMapping, nAction, ifElse, "Service Output Mapping");
		}
	}
	catch (...)
	{
		cout << "EXCEPTION in AlActionService: " << this->obj->GetName() << endl;
	}

	return strm.str();
}
#endif

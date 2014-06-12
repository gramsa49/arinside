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
#include "DocAllMatchingIdsTable.h"
#include "DocActionOpenWindowHelper.h"
#include "../output/ImageTag.h"
#include "../output/ObjNotFound.h"
#include "../output/URLLink.h"

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


string CDocAlActionStruct::Get(IfElseState ifElse, const ARActiveLinkActionList &actList)
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
			row.AddCell(CTableCell(URLLink(CAREnum::ActiveLinkAction(action.action), file, rootLevel)));			

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
					ActionMacro(actionDesc, action.u.macro, nAction);					
				}
				break;
			case AR_ACTIVE_LINK_ACTION_FIELDP:
				{					
					ActionPushFields(actionDesc, action.u.pushFields, nAction);					
				}
				break;
			case AR_ACTIVE_LINK_ACTION_FIELDS:
				{
					ActionSetFields(actionDesc, action.u.setFields, nAction);	
				}
				break;
			case AR_ACTIVE_LINK_ACTION_PROCESS:
				{
					ActionProcess(actionDesc, action.u.process, nAction);	
				}
				break;
			case AR_ACTIVE_LINK_ACTION_MESSAGE:
				{
					ActionMessage(actionDesc, action.u.message, nAction);					
				}
				break;
			case AR_ACTIVE_LINK_ACTION_SET_CHAR:
				{
					ActionSetChar(actionDesc, action.u.characteristics, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_DDE:
				{
					ActionDde(actionDesc, action.u.dde, nAction);
				}
				break;				
			case AR_ACTIVE_LINK_ACTION_SQL:
				{
					ActionSql(actionDesc, action.u.sqlCommand, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_AUTO:
				{
					ActionAutomation(actionDesc, action.u.automation, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_OPENDLG:
				{
					ActionOpenDlg(actionDesc, action.u.openDlg, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_COMMITC:
				{
					ActionCommitChanges(actionDesc, action.u.commitChanges, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_CLOSEWND:
				{
					ActionCloseWindow(actionDesc, action.u.closeWnd, nAction);		
				}
				break;
			case AR_ACTIVE_LINK_ACTION_CALLGUIDE:
				{
					ActionCallGuide(actionDesc, action.u.callGuide, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_EXITGUIDE:
				{
					ActionExitGuide(actionDesc, action.u.exitGuide, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_GOTOGUIDELABEL:
				{
					ActionGotoGuideLabel(actionDesc, action.u.gotoGuide, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_WAIT:
				{
					ActionWait(actionDesc, action.u.waitAction, nAction);
				}
				break;
			case AR_ACTIVE_LINK_ACTION_GOTOACTION:
				{					
					ActionGotoAction(actionDesc, action.u.gotoAction, nAction);				
				}
				break;
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
			case AR_ACTIVE_LINK_ACTION_SERVICE:
				{
					ActionService(actionDesc, action.u.service, nAction);
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
		tblDesc << ImageTag(ImageTag::Document, rootLevel) << IfElse(ifElse) << "-Actions";
		tblListAction.description = tblDesc.str();

		strm << tblListAction;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in CDocAlActionStruct of '" << this->obj->GetName() << "': " << e.what() << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_NONE
string CDocAlActionStruct::ActionNone(int nAction)
{
	return "None";
}

// AR_ACTIVE_LINK_ACTION_MACRO
void CDocAlActionStruct::ActionMacro(std::ostream& strm, const ARActiveLinkMacroStruct &action, int nAction)
{
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
					const char* pName = action.macroParms.parms[nCnt].name;
					const char* pValue = action.macroParms.parms[nCnt].value;

					// create reference item description
					CRefItem refItemTmp(*this->obj, ifElse, nAction, REFM_MACRO);

					// find field references
					string parameterValue = arIn->TextFindFields(pValue, "$", schemaInsideId, rootLevel, true, &refItemTmp);

					// add the al action row
					CTableRow mRow("cssStdRow");		
					mRow.AddCell(CTableCell(pName));
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
}

// AR_ACTIVE_LINK_ACTION_FIELDS
void CDocAlActionStruct::ActionSetFields(std::ostream& strm, const ARSetFieldsActionStruct &action, int nAction)
{
	try
	{
		CDocActionSetFieldsHelper setFieldHelper(*arIn, *obj, schemaName, action, structItemType, ifElse, nAction, rootLevel);
		setFieldHelper.ShowActionWithServerName(true);
		setFieldHelper.ToStream(strm);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionSetFields of '" << this->obj->GetName() << "': " << e.what() << endl;
	}
}

// AR_ACTIVE_LINK_ACTION_PROCESS
void CDocAlActionStruct::ActionProcess(std::ostream& strm, const char *action, int nAction)
{
	try
	{
		if(action != NULL)
		{
			CRefItem refItemTmp(*this->obj, ifElse, nAction, REFM_RUN_PROCESS);
			strm << arIn->TextFindFields(action, "$", schemaInsideId, rootLevel, true, &refItemTmp) << endl;
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
}

// AR_ACTIVE_LINK_ACTION_MESSAGE
void CDocAlActionStruct::ActionMessage(std::ostream& strm, const ARMessageStruct &action, int nAction)
{
	try
	{
		strm << "Message Number: " << action.messageNum << "<br/>" << endl;
		strm << "Message Type: " << CAREnum::MessageType(action.messageType) << "<br/>" << endl;

		if(action.messageText != NULL)
		{
			CRefItem refItemTmp(*this->obj, ifElse, nAction, REFM_MESSAGE);
			strm << "Message Text:<br/>" << arIn->TextFindFields(action.messageText, "$", this->schemaInsideId, rootLevel, true, &refItemTmp) << "<br/>" << endl;
		}

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
		if (this->arIn->CompareServerVersion(7,6) >= 0 || (this->arIn->appConfig.fileMode == true && this->arIn->CompareServerVersion(7,5) >= 0))
		{
			strm << "<input type=\"checkbox\" name=\"showInPromptPane\" value=\"showInPromptPane\"" << (action.usePromptingPane ? " checked" : "") << ">Show Message in Prompt Bar" << "<br/>" << endl;
		}
#endif
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionMessage of '" << this->obj->GetName() << "': " << e.what() << endl;
	}
}

// AR_ACTIVE_LINK_ACTION_SET_CHAR
void CDocAlActionStruct::ActionSetChar(std::ostream& strm, const ARFieldCharacteristics &action, int nAction)
{
	try
	{
		//FieldName
		strm << "Field ";
		if (action.option == AR_FIELD_CHAR_OPTION_REFERENCE)
		{
			strm << "(Use Value Of): $" << arIn->LinkToField(schemaInsideId, action.fieldId, rootLevel) << "$<br/>" << endl;
		}
		else
		{
			strm << "Name: " << arIn->LinkToField(schemaInsideId, action.fieldId, rootLevel) << "<br/>" << endl;
		}

		//Access Option
		strm << "Field Access: " << CAREnum::SetCharFieldAccess(action.accessOption) << "<br/>" << endl;					


		//Font
		bool fontPropFound = false;
		for(unsigned int i=0; i< action.props.numItems; i++)
		{	
			if(action.props.props[i].prop == AR_DPROP_LABEL_FONT_STYLE && action.props.props[i].value.dataType == AR_DATA_TYPE_CHAR)
			{
				strm << "Field Font: " << action.props.props[i].value.u.charVal << "<br/>" << endl;
				fontPropFound = true;
				break;
			}
		}
		if (!fontPropFound)
			strm << "Field Font: Unchanged<br/>" << endl;


		//Visibility
		bool visibilityPropFound = false;
		for(unsigned int i=0; i< action.props.numItems; i++)
		{	
			if(action.props.props[i].prop == AR_DPROP_VISIBLE && 
				action.props.props[i].value.dataType == AR_DATA_TYPE_ENUM || action.props.props[i].value.dataType == AR_DATA_TYPE_INTEGER)
			{
				strm << "Visibility: " << CAREnum::SetCharFieldVisibility(action.props.props[i].value.u.intVal) << "<br/>" << endl;
				visibilityPropFound = true;
				break;
			}
		}
		if(!visibilityPropFound)
			strm << "Visibility: Unchanged<br/>" << endl;



		//CharMenu
		if(action.charMenu != NULL)
		{
			if(strcmp(action.charMenu, "") != 0)
			{
				strm << "Menu: " << CWebUtil::LinkToMenu(action.charMenu, rootLevel) << "<br/>" << endl;
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
					string value = CARValue::ValueToString(action.props.props[i].value);
					if(!value.empty())
					{
						CRefItem refItemTmp(*this->obj, ifElse, nAction, REFM_CHANGEFIELD_LABEL);
						strm << "Field Label: " << arIn->TextFindFields(value, "$", schemaInsideId, rootLevel, true, &refItemTmp)  << "<br/>" << endl;					
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
}

// AR_ACTIVE_LINK_ACTION_DDE
void CDocAlActionStruct::ActionDde(std::ostream& strm, const ARDDEStruct &action, int nAction)
{
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
}

// AR_ACTIVE_LINK_ACTION_FIELDP
void CDocAlActionStruct::ActionPushFields(std::ostream& strm, const ARPushFieldsActionStruct &action, int nAction)
{
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
			CRefItem refItem(*this->obj, ifElse, nAction, REFM_PUSHFIELD_SERVER);
			arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
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
			strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$ (Sample Schema: " << arIn->LinkToSchema(pushSchema, rootLevel) << ")";

			// create field reference
			CRefItem refItem(*this->obj, ifElse, nAction, REFM_PUSHFIELD_FORM);
			arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
		}
		else
		{
			pushSchema = action.pushFieldsList.pushFieldsList[0].field.schema;
			strm << arIn->LinkToSchema(pushSchema, rootLevel);
		}
		strm << "<br/>" << endl;

		// create schema reference
		if (!arIn->appConfig.bOverlaySupport || IsVisibleObject(*obj))
		{
			CARSchema targetSchema(pushSchema);
			if (targetSchema.Exists())
			{
				CRefItem refItem(*this->obj, ifElse, nAction, REFM_PUSHFIELD_TARGET);
				if (!targetSchema.ReferenceExists(refItem))
					targetSchema.AddReference(refItem);
			}
		}

		// push field if **************************************
		strm << "<br/>Push Field If<br/>" << endl;
		stringstream strmTmpQual;

		CRefItem refItem(*this->obj, ifElse, nAction, REFM_PUSHFIELD_IF);
		CARQualification arQual(*arIn, refItem);

		int pFormId = this->arIn->SchemaGetInsideId(schemaName);
		int sFormId = this->arIn->SchemaGetInsideId(pushSchema);

		arQual.CheckQuery(&action.pushFieldsList.pushFieldsList[0].field.qualifier, pFormId, sFormId, strmTmpQual, rootLevel);

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
			CDocAllMatchingIdsTable allMatchingFieldsTbl(pushSchema, schemaName, *obj, CDocAllMatchingIdsTable::AMM_PUSHFIELDS, nAction, ifElse, rootLevel);
			allMatchingFieldsTbl.ToStream(strm);
		}
		else
		{
			strm << "<br/>" << endl;
			CARAssignHelper assignHelper(*arIn, rootLevel, *obj, schemaName, pushSchema);
			strm << assignHelper.PushFieldsAssignment(action, nAction, ifElse);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionPushFields of '" << this->obj->GetName() << "': " << e.what() << endl;
	}
}

// AR_ACTIVE_LINK_ACTION_SQL
void CDocAlActionStruct::ActionSql(std::ostream& strm, const ARSQLStruct &action, int nAction)
{
	try
	{		
		if(action.server != NULL)
			strm << "Server: " << arIn->LinkToServerInfo(action.server, rootLevel) << "<br/>" << endl;

		if(action.command != NULL)
		{			
			CRefItem refItemTmp(*this->obj, ifElse, nAction, REFM_DIRECTSQL);
			strm << "SQL command: <br/>" << arIn->TextFindFields(action.command, "$", schemaInsideId, rootLevel, true, &refItemTmp) << endl;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionSql of '" << this->obj->GetName() << "': " << e.what() << endl;
	}
}

// AR_ACTIVE_LINK_ACTION_AUTO
void CDocAlActionStruct::ActionAutomation(std::ostream& strm, const ARAutomationStruct &action, int nAction)
{
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
				string mId, mName;

				if(action.methodList.methodList[nCnt].methodIId != NULL)
					mId = action.methodList.methodList[nCnt].methodIId;

				if(action.methodList.methodList[nCnt].methodName != NULL)
					mName = action.methodList.methodList[nCnt].methodName;

				CTableRow mRow("cssStdRow");
				mRow.AddCell( CTableCell(mId) );
				mRow.AddCell( CTableCell(mName) );
				tblOleMethods.AddRow(mRow);	
			}	
			strm << tblOleMethods;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionAutomation of '" << this->obj->GetName() << "': " << e.what() << endl;
	}
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
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT_LST:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT_DETAIL:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT_SPLIT:
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT:
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT_LST:
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT_DETAIL:
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT_SPLIT:
#endif
		return true;
	default: return false;
	}
}

bool CDocAlActionStruct::ActionOpenDlgTargetLocation(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_SEARCH:
	case AR_ACTIVE_LINK_ACTION_OPEN_SUBMIT:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY:
	case AR_ACTIVE_LINK_ACTION_OPEN_REPORT:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT: 
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT:
#endif
		return true;
	default: return false;
	}
}

bool CDocAlActionStruct::ActionOpenDlgCloseButton(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_DLG:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
	case AR_ACTIVE_LINK_ACTION_OPEN_POPUP:
#endif
		return true;
	default: return false;
	}
}

bool CDocAlActionStruct::ActionOpenDlgSuppressEmptyList(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY:
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT:
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT:
#endif
		return true;
	default: return false;
			
	}
}

bool CDocAlActionStruct::ActionOpenDlgInputMapping(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_DLG:
	case AR_ACTIVE_LINK_ACTION_OPEN_SEARCH:
	case AR_ACTIVE_LINK_ACTION_OPEN_SUBMIT:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
	case AR_ACTIVE_LINK_ACTION_OPEN_POPUP:
#endif
		return true;
	default: return false;
	}
}

bool CDocAlActionStruct::ActionOpenDlgOutputMapping(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_DLG:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
	case AR_ACTIVE_LINK_ACTION_OPEN_POPUP:
#endif
		return true;
	default: return false;
	}
}

bool CDocAlActionStruct::ActionOpenDlgPoolingInterval(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT:
#endif
		return true;
	default: return false;
	}
}

bool CDocAlActionStruct::ActionOpenDlgMessage(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT:
#endif
	case AR_ACTIVE_LINK_ACTION_OPEN_REPORT:
		return true;
	default: return false;
	}
}

// AR_ACTIVE_LINK_ACTION_OPENDLG
void CDocAlActionStruct::ActionOpenDlg(std::ostream& strm, const AROpenDlgStruct &action, int nAction)
{
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
			strm << "<br/>Display Type: " << CAREnum::OpenWindowDisplayType(action.windowMode) << endl;
		}

		// target location
		if (ActionOpenDlgTargetLocation(windowMode))
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
						CRefItem refItem(*this->obj, ifElse, nAction, REFM_OPENWINDOW_LOCATION);
						arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
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
				CRefItem refItem(*this->obj, ifElse, nAction, REFM_OPENWINDOW_SERVER);
				arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
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
				CRefItem refItem(*this->obj, ifElse, nAction, REFM_OPENWINDOW_FORM);
				arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
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
				CRefItem refItem(*this->obj, ifElse, nAction, REFM_OPENWINDOW_VIEW);
				arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
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
					ObjNotFound notFound(strm);
					notFound << action.vuiLabel;
					notFound.End();
				}
				else
				{
					CARVui vui(rSchema.GetInsideId(), action.vuiLabel);
					if (vui.Exists())
						strm << URLLink(vui, rootLevel);
					else
					{
						ObjNotFound notFound(strm);
						notFound << action.vuiLabel;
						notFound.End();
					}
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
				strm << "<p>";
				ObjNotFound notFound(strm);
				notFound << "Could not load report informations!";
				notFound.End();
				strm << "</p>";
			}
			else
			{
				strm << "<p>Report Type: ";
				if (!reportType.empty() && reportType[0] == '$')
				{
					int fieldId = atoi(&reportType.c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$";
					if (fieldId > 0)
					{
						CRefItem refItem(*this->obj, ifElse, nAction, REFM_OPENWINDOW_REPORTTYPE);
						arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
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
						CRefItem refItem(*this->obj, ifElse, nAction, REFM_OPENWINDOW_REPORTNAME);
						arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
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
						CRefItem refItem(*this->obj, ifElse, nAction, REFM_OPENWINDOW_REPORTDESTINATION);
						arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
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
			if(action.query.operation != AR_COND_OP_NONE)
			{
				stringstream strmTmpQual;

				CRefItem refItem(*this->obj, ifElse, nAction, REFM_OPENWINDOW_QUALIFICATION);

				strmTmpQual.str("");
				CARQualification arQual(*this->arIn, refItem);
				int pFormId = schemaInsideId; // this->arIn->SchemaGetInsideId(schemaName.c_str());
				int sFormId = this->arIn->SchemaGetInsideId(openWindowSchema);
				arQual.CheckQuery(&action.query, pFormId, sFormId, strmTmpQual, rootLevel);

				strm << "<p>Qualification:<br/>" << endl;
				strm << strmTmpQual.str() << "</p>";
			}	
		}

		strm << "<p>";
		// show close button
		if (ActionOpenDlgCloseButton(windowMode))
		{
			strm << "<input type=\"checkbox\" name=\"closeWnd\" value=\"closeWndAll\" " << (action.closeBox ? "checked" : "") << ">Show Close Button in Dialog<br/>" << endl;
		}

		// suppres empty list
		if (ActionOpenDlgSuppressEmptyList(windowMode))
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
		if (ActionOpenDlgInputMapping(windowMode) && !setToDefault)
		{
			CARAssignHelper assignHelper(*arIn, rootLevel, *this->obj, openWindowSchema, schemaName);
			//assignHelper.pushFieldFlag = true;
			strm << assignHelper.OpenWindowAssignment(action.inputValueFieldPairs, nAction, ifElse, OWM_OPEN);
		}

		// output mapping (dialog on close)
		if (ActionOpenDlgOutputMapping(windowMode))
		{
			strm << "On Dialog Close Action:<br/>";

			CARAssignHelper assignHelper(*arIn, rootLevel, *this->obj, schemaName, openWindowSchema);
			strm << assignHelper.OpenWindowAssignment(action.outputValueFieldPairs, nAction, ifElse, OWM_CLOSE);
		}
		strm << "</p>";

		if (ActionOpenDlgMessage(windowMode))
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
				ActionMessage(strm, action.msg, nAction);
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
					sprintf(strFieldId, "%d", static_cast<int>(action.sortOrderList.sortList[i].fieldId));

					CTableRow row("cssStdRow");
					row.AddCell(arIn->LinkToField(openWindowSchema, action.sortOrderList.sortList[i].fieldId, rootLevel));
					row.AddCell(strFieldId);
					row.AddCell((rField.Exists() ? CAREnum::DataType(rField.GetDataType()) : "Unknown"));
					row.AddCell(CAREnum::SchemaSortOrder(action.sortOrderList.sortList[i].sortOrder));
					
					tblSortList.AddRow(row);

					if (rSchema.Exists())
					{
						CRefItem refItem(*this->obj, ifElse, nAction, REFM_OPENWINDOW_SORTBY);
						arIn->AddFieldReference(rSchema.GetInsideId(), action.sortOrderList.sortList[i].fieldId, refItem);
					}
				}

				strm << "<p>Sort Order" << tblSortList << "</p>";
			}

			// additional report informations
			if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_REPORT)
			{
				strm << "<p>EntryIDs: ";
				if (!entryIDs->empty() && entryIDs->operator[](0) == '$')
				{
					int fieldId = atoi(&entryIDs->c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(schemaInsideId, fieldId, rootLevel)) << "$";
					if (fieldId > 0)
					{
						CRefItem refItem(*this->obj, ifElse, nAction, REFM_OPENWINDOW_ENTRYIDS);
						arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
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
						CRefItem refItem(*this->obj, ifElse, nAction, REFM_OPENWINDOW_QUERYOVERRIDE);
						arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
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
						CRefItem refItem(*this->obj, ifElse, nAction, REFM_OPENWINDOW_CHARENC);
						arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
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
			if (ActionOpenDlgPoolingInterval(windowMode) && action.pollinginterval > 0)
			{
				strm << "<p>Polling interval: " << action.pollinginterval << "</p>" << endl;
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionOpenDlg of '" << this->obj->GetName() << "': " << e.what() << endl;
	}
}

// AR_ACTIVE_LINK_ACTION_COMMITC
void CDocAlActionStruct::ActionCommitChanges(std::ostream& strm, const ARCommitChangesStruct &action, int nAction)
{
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
}

// AR_ACTIVE_LINK_ACTION_CLOSEWND
void CDocAlActionStruct::ActionCloseWindow(std::ostream& strm, const ARCloseWndStruct &action, int nAction)
{
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
}

// AR_ACTIVE_LINK_ACTION_CALLGUIDE
void CDocAlActionStruct::ActionCallGuide(std::ostream& strm, const ARCallGuideStruct &action, int nAction)
{
	try
	{
		strm << "<p>";
		if (action.serverName[0] == '$' /*&& action.sampleServer[0] != 0*/)
		{
			int fieldId = atoi(&action.serverName[1]);
			strm << "Server: " << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(this->schemaName, fieldId, rootLevel)) << "$<br/>" << endl;

			CRefItem refItem(*this->obj, ifElse, nAction, REFM_CALLCUIDE_SERVER);
			arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
		}
		else
		{
			strm << "Server : " << arIn->LinkToServerInfo(action.serverName, rootLevel) << "<br/>" << endl;
		}

		if (action.guideName[0] == '$' /*&& action.sampleGuide[0] != 0*/)
		{
			int fieldId = atoi(&action.guideName[1]);
			strm << "Guide: $" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(this->schemaName, fieldId, rootLevel)) << "$<br/>" << endl;

			CRefItem refItem(*this->obj, ifElse, nAction, REFM_CALLGUIDE_NAME);
			arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
		}
		else
		{
			strm << "Guide: " << arIn->LinkToContainer(action.guideName, rootLevel) << "<br/>" << endl;
		}
		strm << "</p>";

		if(action.guideTableId > 0)
		{
			strm << "<p class='last'>" << "Table Loop: " << CAREnum::CallGuideMode(action.guideMode) << "<br/>" << endl;
			strm << "Table Field: " <<  arIn->LinkToField(schemaName, action.guideTableId, rootLevel);
			CRefItem refItem(*this->obj, ifElse, nAction, REFM_CALLGUIDE_TABLELOOP);
			arIn->AddFieldReference(schemaInsideId, action.guideTableId, refItem);
			strm << "</p>";
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionCallGuide of '" << this->obj->GetName() << "': " << e.what() << endl;
	}
}

// AR_ACTIVE_LINK_ACTION_EXITGUIDE
void CDocAlActionStruct::ActionExitGuide(std::ostream& strm, const ARExitGuideStruct &action, int nAction)
{
	try
	{
		strm << "<input type=\"checkbox\" name=\"exitGuide\" value=\"exitGuideCloseAll\"";
		if(action.closeAll == TRUE)
		{
			strm << " checked";
		}
		strm << ">Close all guides on exit" << endl;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionExitGuide of '" << this->obj->GetName() << "': " << e.what() << endl;
	}
}

// AR_ACTIVE_LINK_ACTION_GOTOGUIDELABEL
void CDocAlActionStruct::ActionGotoGuideLabel(std::ostream& strm, const ARGotoGuideLabelStruct &action, int nAction)
{
	try
	{
		strm << "Guide Label: " << (action.label != NULL ? action.label : EmptyValue) << endl;	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionGuideLabel of '" << this->obj->GetName() << "': " << e.what() << endl;
	}
}

// AR_ACTIVE_LINK_ACTION_WAIT
void CDocAlActionStruct::ActionWait(std::ostream& strm, const ARWaitStruct &action, int nAction)
{
	try
	{
		strm << "Label for Continue Button: " << (action.continueButtonTitle != NULL ? action.continueButtonTitle : EmptyValue) << endl;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionWait of '" << this->obj->GetName() << "': " << e.what() << endl;
	}
}

// AR_ACTIVE_LINK_ACTION_GOTOACTION
void CDocAlActionStruct::ActionGotoAction(std::ostream& strm, const ARGotoActionStruct &action, int nAction)
{
	try
	{
		strm << "FieldId or Value: ";
		switch (action.tag)
		{
		case AR_GOTO_FIELD_XREF:
			{
			// add new reference item to the field
			CRefItem refItem(*this->obj, ifElse, nAction, REFM_GOTO);
			arIn->AddFieldReference(schemaInsideId, action.fieldIdOrValue, refItem);

			// link to field in current page
			strm << arIn->LinkToField(schemaName, action.fieldIdOrValue, rootLevel) << endl;
			break;
			}
		case AR_GOTO_ABSOLUTE_ORDER:
			strm << action.fieldIdOrValue;
			break;
		case AR_GOTO_OFFSET_FORWARD:
			strm << "+" << action.fieldIdOrValue;
			break;
		case AR_GOTO_OFFSET_BACKWARD:
			strm << "-" << action.fieldIdOrValue;
			break;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionGotoAction of '" << this->obj->GetName() << "': " << e.what() << endl;
	}
}

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_710
// AR_ACTIVE_LINK_ACTION_SERVICE
void CDocAlActionStruct::ActionService(std::ostream& strm, const ARActiveLinkSvcActionStruct &action, int nAction)
{
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

			CRefItem refItem(*this->obj, ifElse, nAction, REFM_SERVICE_SERVER);
			arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
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

			CRefItem refItem(*this->obj, ifElse, nAction, REFM_SERVICE_FORM);
			arIn->AddFieldReference(schemaInsideId, fieldId, refItem);
		}
		else
		{
			serviceSchema = action.serviceSchema;
			strm << "Service Form: " << arIn->LinkToSchema(action.serviceSchema, rootLevel) << "<br/>" << endl;
		}

		// create schema reference
		CARSchema schemaToCall(serviceSchema);
		if (schemaToCall.Exists() && (!arIn->appConfig.bOverlaySupport || IsVisibleObject(*obj)))
		{
			CRefItem refItem(*this->obj, ifElse, nAction, REFM_SERVICE_CALL);
			if (!schemaToCall.ReferenceExists(refItem))
				schemaToCall.AddReference(refItem);
		}

		strm << "Request Id: ";
		if (action.requestIdMap != 0)
		{
			strm << arIn->LinkToField(schemaName, action.requestIdMap, rootLevel);
			
			CRefItem refItem(*this->obj, ifElse, nAction, REFM_SERVICE_REQUESTID);
			arIn->AddFieldReference(schemaInsideId, action.requestIdMap, refItem);
		}
		strm << "</p>" << endl;

		// input mapping
		strm << "Input Mapping: "; if (action.inputFieldMapping.numItems == 0) strm << "None"; strm << "<br/>" << endl;
		if (action.inputFieldMapping.numItems > 0)
		{
			CARAssignHelper assignHelper(*arIn, rootLevel, *this->obj, serviceSchema, schemaName);
			strm << assignHelper.ServiceAssignment(action.inputFieldMapping, nAction, ifElse, SMM_INPUT);
		}

		// output mapping
		strm << "Output Mapping: "; if (action.outputFieldMapping.numItems == 0) strm << "None"; strm << "<br/>" << endl;
		if (action.outputFieldMapping.numItems > 0)
		{
			CARSchema currentSchema(schemaName);

			CARAssignHelper assignHelper(*arIn, rootLevel, *this->obj, currentSchema, schemaToCall);
			strm << assignHelper.ServiceAssignment(action.outputFieldMapping, nAction, ifElse, SMM_OUTPUT);
		}
	}
	catch (...)
	{
		cout << "EXCEPTION in AlActionService: " << this->obj->GetName() << endl;
	}
}
#endif


/****************************************************************************** 
 * 
 *  file:  DocAlActionStruct.cpp
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
#include ".\docalactionstruct.h"

CDocAlActionStruct::CDocAlActionStruct(CARInside &arIn, CARActiveLink &obj, string schemaName, string dir, int rootLevel)
{
	this->arIn = &arIn;
	this->obj = &obj;	
	this->dir = dir;
	this->rootLevel = rootLevel;
	this->schemaName = schemaName;
	this->schemaInsideId = this->arIn->SchemaGetInsideId(this->schemaName);
	this->structItemType = AR_STRUCT_ITEM_XML_ACTIVE_LINK;
}

CDocAlActionStruct::~CDocAlActionStruct(void)
{
}


string CDocAlActionStruct::Get(string ifElse, ARActiveLinkActionList &actList)
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
			
			stringstream strmTmpLink;
			strmTmpLink.str("");
			strmTmpLink << "../../active_link/index_action_" << action.action << "." << CWebUtil::WebPageSuffix();
			row.AddCell(CTableCell(CWebUtil::Link(CAREnum::ActiveLinkAction(action.action), strmTmpLink.str(), "", rootLevel)));			
	        	                    
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

		strm << tblListAction.ToXHtml();
	}
	catch(...)
	{
		cout << "EXCEPTION in CDocAlActionStruct: " << this->obj->name << endl;
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
		CARSchema *schema1 = NULL;
		CARSchema *schema2 = NULL;

		list<CARSchema>::iterator schemaIter;			
		for ( schemaIter = arIn->schemaList.begin(); schemaIter != arIn->schemaList.end(); schemaIter++ )
		{			
			CARSchema *tmpSchema = &(*schemaIter);
			if(strcmp(tmpSchema->name.c_str(), table1.c_str())==0)
			{
				schema1=tmpSchema;
			}

			if(strcmp(tmpSchema->name.c_str(), table2.c_str())==0)
			{
				schema2=tmpSchema;
			}
		}

		CTable tblListField("tblListMatchingIds", "TblObjectList");
		tblListField.AddColumn(10, "Id");		
		tblListField.AddColumn(40, "Field Name");
		tblListField.AddColumn(10, "Id");	
		tblListField.AddColumn(40, "Value");

		if(schema1 != NULL && schema2 !=NULL)
		{
			list<CARField>::iterator fieldIter1;
			for( fieldIter1 = schema1->fieldList.begin(); fieldIter1 != schema1->fieldList.end(); fieldIter1++)
			{
				CARField *tmpField1 = &(*fieldIter1);

				list<CARField>::iterator fieldIter2;
				for( fieldIter2 = schema2->fieldList.begin(); fieldIter2 != schema2->fieldList.end(); fieldIter2++)
				{
					CARField *tmpField2 = &(*fieldIter2);

					if(tmpField1->fieldId == tmpField2->fieldId)
					{
						//Reference field1
						CFieldRefItem *refItemField1 = new CFieldRefItem();
						refItemField1->arsStructItemType = this->structItemType;
						refItemField1->fromName = this->obj->name;						
						refItemField1->schemaInsideId = schema1->insideId;
						refItemField1->fieldInsideId = tmpField1->insideId;

						stringstream strmTmpDesc;
						strmTmpDesc.str("");
						strmTmpDesc << description << " (All Matching Ids) [Target] " << ifElse << "-Action " << nAction;
						refItemField1->description = strmTmpDesc.str();
						arIn->AddReferenceItem(refItemField1);
						delete refItemField1;

						//Reference field2
						CFieldRefItem *refItemField2 = new CFieldRefItem();
						refItemField2->arsStructItemType = this->structItemType;
						refItemField2->fromName = this->obj->name;						
						refItemField2->schemaInsideId = schema2->insideId;
						refItemField2->fieldInsideId = tmpField2->insideId;

						strmTmpDesc.str("");
						strmTmpDesc << description << " (All Matching Ids) [Value] " << ifElse << "-Action " << nAction;
						refItemField2->description = strmTmpDesc.str();
						arIn->AddReferenceItem(refItemField2);
						delete refItemField2;


						//Matching ID
						CTableRow row("cssStdRow");		
						row.AddCell(CTableCell(tmpField1->fieldId));
						row.AddCell(CTableCell(arIn->LinkToField(schema1->name, tmpField1->insideId, this->rootLevel)));
						row.AddCell(CTableCell(tmpField2->fieldId));
						row.AddCell(CTableCell(arIn->LinkToField(schema2->name, tmpField2->insideId, this->rootLevel)));
						tblListField.AddRow(row);
					}
				}
			}
		}	

		strm << tblListField.ToXHtml();
	}
	catch(...)
	{
		cout << "EXCEPTION in AlAllMatchingIds: " << table1 << ", " << table2 << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_NONE
string CDocAlActionStruct::ActionNone(int nAction)
{
	return "None";
}

// AR_ACTIVE_LINK_ACTION_MACRO
string CDocAlActionStruct::ActionMacro(ARActiveLinkMacroStruct &action, int nAction)
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
					
					refItemTmp->fromName = this->obj->name;						
					refItemTmp->schemaInsideId = schemaInsideId;	

					string parameterValue = arIn->TextFindFields(pValue.str(), "$", schemaInsideId, rootLevel, true, refItemTmp);
					delete refItemTmp;

					CTableRow mRow("cssStdRow");		
					mRow.AddCell(CTableCell(pName.str()));				
					mRow.AddCell(CTableCell(parameterValue));
					tblMacro.AddRow(mRow);	
				}
			}
			strm << tblMacro.ToXHtml();
			strm << "<br/>Macro Text: <br/>" << action.macroText << endl;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in AlActionMacro: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_FIELDS
string CDocAlActionStruct::ActionSetFields(ARSetFieldsActionStruct &action, int nAction)
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

		CDocAlHelper *alHelper = new CDocAlHelper(*arIn, *obj);
		alHelper->SetFieldsGetSecondaryForm(action, schemaName, rootLevel, secondaryFormRaw, secondaryFormDisplay, serverRaw, qualification);
		delete alHelper;

		//For the following internal calculations we need a secondary form
		string schemaName2 = secondaryFormRaw.str();
		
		string tmpDisplayName = secondaryFormDisplay.str();
		if(tmpDisplayName.size()==0)
			tmpDisplayName = schemaName2;

		strm << "Server Name: " << arIn->LinkToServerInfo(serverRaw.str(), rootLevel) << "<br/>" << endl;
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

			CARAssignHelper assignHelper(*arIn, dir, rootLevel, this->obj->name, this->structItemType, schemaName, schemaName2);
			strm << assignHelper.SetFieldsAssignment(action, nAction, ifElse);
		}
		else
		{
			strm << setFieldInfo << endl;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in AlActionSetFields: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_PROCESS
string CDocAlActionStruct::ActionProcess(char *action, int nAction)
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

			refItemTmp->fromName = this->obj->name;						
			refItemTmp->schemaInsideId = schemaInsideId;		

			strm << "Command Line: " << arIn->TextFindFields(action, "$", refItemTmp->schemaInsideId, rootLevel, true, refItemTmp) << endl;
			delete refItemTmp;
		}
		else
		{
			strm << "No SQL command specified." << endl;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in AlActionProcess: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_MESSAGE
string CDocAlActionStruct::ActionMessage(ARMessageStruct &action, int nAction)
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
			
			refItemTmp->fromName = this->obj->name;						
			refItemTmp->schemaInsideId = schemaInsideId;
			strm << "Message Text:<br/>" << arIn->TextFindFields(action.messageText, "$", this->schemaInsideId, rootLevel, true, refItemTmp) << "<br/>" << endl;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in AlActionMessage: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_SET_CHAR
string CDocAlActionStruct::ActionSetChar(ARFieldCharacteristics &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{		
		CFieldRefItem *refItem = new CFieldRefItem();
		refItem->arsStructItemType = this->structItemType;

		stringstream strmTmpDesc;
		strmTmpDesc << "Change Field " << ifElse << "-Action " << nAction;
		refItem->description = strmTmpDesc.str();

		refItem->fromName = this->obj->name;	
		refItem->fieldInsideId = action.fieldId;
		refItem->schemaInsideId = schemaInsideId;
		arIn->AddReferenceItem(refItem);
		delete refItem;	

		//FieldName
		strm << "Field Name: " << arIn->LinkToField(schemaName, action.fieldId, rootLevel) << "<br/>" << endl;
		
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
					if(action.props.props[i].value.u.intVal != NULL)
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
				strm << "Menu: " << arIn->LinkToMenu(action.charMenu, rootLevel) << "<br/>" << endl;
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
					strmTmp << CARValue::ValueToString(action.props.props[i].value);
					

					if(strmTmp.str().size() > 0)
						strm << "Default Label Color: " << strmTmp.str() << "<br/>" << endl;
					else
						strm << "Default Label Color" << "<br/>" << endl;
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

							refItemTmp->fromName = this->obj->name;						
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
	catch(...)
	{
		cout << "EXCEPTION in AlActionSetChar: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_DDE
string CDocAlActionStruct::ActionDde(ARDDEStruct &action, int nAction)
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
	catch(...)
	{
		cout << "EXCEPTION in AlActionDde: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_FIELDP
string CDocAlActionStruct::ActionPushFields(ARPushFieldsActionStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		stringstream secondaryFormRaw, serverRaw, assignQual;
		for(unsigned int i=0; i< action.pushFieldsList.numItems; i++)
		{
			secondaryFormRaw.str("");
			serverRaw.str("");
			assignQual.str("");

			if(action.pushFieldsList.pushFieldsList[i].field.schema != NULL)
			{
				secondaryFormRaw << action.pushFieldsList.pushFieldsList[i].field.schema;
			}

			if(action.pushFieldsList.pushFieldsList[i].field.server != NULL)
			{
				serverRaw << action.pushFieldsList.pushFieldsList[i].field.server;
			}

			assignQual << "<br/>Push Field If<br/>" << endl;
			if(action.pushFieldsList.pushFieldsList[i].field.qualifier.operation != NULL)
			{
				stringstream strmTmpQual;

				CFieldRefItem *refItem = new CFieldRefItem();
				refItem->arsStructItemType = this->structItemType;

				stringstream strmTmpDesc;
				strmTmpDesc << "Push Field Qualification " << ifElse << "-Action " << nAction;
				refItem->description = strmTmpDesc.str();
				
				refItem->fromName = this->obj->name;

				CARQualification arQual(*arIn);
				int pFormId = this->arIn->SchemaGetInsideId(schemaName.c_str());
				int sFormId = this->arIn->SchemaGetInsideId(secondaryFormRaw.str());
				arQual.CheckQuery(&action.pushFieldsList.pushFieldsList[i].field.qualifier, *refItem, 0, pFormId, sFormId, strmTmpQual, rootLevel);
				
				delete refItem;

				if(strmTmpQual.str().length() > 0)
				{
					assignQual << strmTmpQual.str() << "<br/><br/>" << endl;
				}
				else
				{
					assignQual << EmptyValue << "<br/><br/>" << endl;
				}

				assignQual << "If No Requests Match: " << CAREnum::NoMatchRequest(action.pushFieldsList.pushFieldsList[i].field.noMatchOption) << "<br/>" << endl;
				assignQual << "If Multiple Requests Match: " << CAREnum::MultiMatchRequest(action.pushFieldsList.pushFieldsList[i].field.multiMatchOption) << "<br/><br/>" << endl;

			}
		}

		strm << "Server Name: " << arIn->LinkToServerInfo(serverRaw.str(), rootLevel) << "<br/>" << endl;
		strm << "Push Value To: " << arIn->LinkToSchema(secondaryFormRaw.str(), rootLevel) << "<br/>" << endl;
		
		//Qualification
		strm << assignQual.str() << endl;

		//All matching Ids
		string pushFieldInfo = "Field Mapping";
		for(unsigned int i= 0; i< action.pushFieldsList.numItems; i++)
		{
			if(action.pushFieldsList.pushFieldsList[i].field.u.fieldId == AR_LIKE_ID)
			{
				pushFieldInfo = "Field Mapping: All Matching Ids";

				strm << this->AllMatchingIds(schemaName.c_str(), secondaryFormRaw.str(), "Push Fields", nAction);
			}
		}

		if(strcmp(pushFieldInfo.c_str(), "Field Mapping") ==  0)
		{
			strm << pushFieldInfo << ":<br/>" << endl;

			CARAssignHelper assignHelper(*arIn, dir, rootLevel, obj->name, this->structItemType, schemaName, secondaryFormRaw.str());
			strm << assignHelper.PushFieldsAssignment(action, nAction, ifElse);
		}
		else
		{
			strm << pushFieldInfo << endl;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in AlActionPushFields: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_SQL
string CDocAlActionStruct::ActionSql(ARSQLStruct &action, int nAction)
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

			refItemTmp->fromName = this->obj->name;						
			refItemTmp->schemaInsideId = schemaInsideId;		

			strm << "SQL command: <br/>" << arIn->TextFindFields(action.command, "$", refItemTmp->schemaInsideId, rootLevel, true, refItemTmp) << endl;
			delete refItemTmp;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in AlActionSql: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_AUTO
string CDocAlActionStruct::ActionAutomation(ARAutomationStruct &action, int nAction)
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
			strm << tblOleMethods.ToXHtml();
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in AlActionAutomation: " << this->obj->name << endl;
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
string CDocAlActionStruct::ActionOpenDlg(AROpenDlgStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if( action.serverName != NULL)
		{
			if(strcmp(AR_CURRENT_SERVER_TAG, action.serverName)==0)
			{
				strm << "Server Name: " << arIn->LinkToServerInfo(action.serverName, rootLevel) << "<br/>" << endl;
			}
			else
			{
				CFieldRefItem *refItemSrvName = new CFieldRefItem();
				refItemSrvName->arsStructItemType = this->structItemType;
				refItemSrvName->fromName = this->obj->name;						
				refItemSrvName->schemaInsideId = arIn->SchemaGetInsideId(action.schemaName);

				stringstream strmTmpDesc;
				strmTmpDesc << "Window Open Server Name " << ifElse << "-Action " << nAction;
				refItemSrvName->description = strmTmpDesc.str();

				strm << "Server Name: " << arIn->TextFindFields(action.serverName, "$", this->schemaInsideId , rootLevel, true, refItemSrvName) << "<br/>" << endl;
			}
		}

		if(action.targetLocation != NULL)
		{
			CFieldRefItem *refItemLocation = new CFieldRefItem();
			refItemLocation->arsStructItemType = this->structItemType;
			refItemLocation->fromName = this->obj->name;						
			refItemLocation->schemaInsideId = arIn->SchemaGetInsideId(action.schemaName);

			stringstream strmTmpDesc;
			strmTmpDesc << "Window Open Location " << ifElse << "-Action " << nAction;
			refItemLocation->description = strmTmpDesc.str();

			strm << "Target Location: " << arIn->TextFindFields(action.targetLocation, "$", this->schemaInsideId, this->rootLevel, true, refItemLocation) << "<br/>" << endl;
		}
		
		strm << "Window Mode: " << CAREnum::OpenWindowMode(action.windowMode) << "<br/>" << endl;
		strm << "Form Name: " << arIn->LinkToSchema(action.schemaName, rootLevel) << "<br/>" << endl;
		
		if(action.vuiLabel == NULL || strcmp(action.vuiLabel, "")==0)
		{
			strm << "Form View: (Clear)<br/>" << endl; 
		}
		else
		{

			list<CARSchema>::iterator schemaIter;
			for ( schemaIter = this->arIn->schemaList.begin(); schemaIter != this->arIn->schemaList.end(); schemaIter++ )
			{			
				CARSchema *tmpSchema = &(*schemaIter);
				if(strcmp(action.schemaName, tmpSchema->name.c_str())==0)
				{					
					strm << "Form View: " << tmpSchema->LinkToVui(action.vuiLabel, rootLevel);	
				}
			}
		}

		
		if(ActionOpenDlgQualifier(action.windowMode))
		{
			if(action.query.operation != NULL)
			{
				stringstream strmTmpQual;
				strmTmpQual.str("");

				CFieldRefItem *refItem = new CFieldRefItem();
				refItem->arsStructItemType = this->structItemType;
				refItem->description = "Open Window Qualification";
				refItem->fromName = this->obj->name;

				CARQualification arQual(*this->arIn);
				int pFormId = this->arIn->SchemaGetInsideId(schemaName.c_str());
				int sFormId = this->arIn->SchemaGetInsideId(action.schemaName);
				arQual.CheckQuery(&action.query, *refItem, 0, pFormId, sFormId, strmTmpQual, rootLevel);

				delete refItem;

				strm << "<br/>Qualification:<br/>" << endl;
				strm << strmTmpQual.str();
			}	
		}

		strm << "<br/>" << endl;

		if(action.closeBox == TRUE)
			strm << "<input type=\"checkbox\" name=\"closeWnd\" value=\"closeWndAll\" checked>Show Close Button in Dialog" << endl;
		else
			strm << "<input type=\"checkbox\" name=\"closeWnd\" value=\"closeWndAll\">Show Close Button in Dialog" << endl;
		
		strm << "<br/>" << endl;

		//Field Mapping WindowOpen
		string setFieldInfo = "Field Mapping (On Open)";
		for(unsigned int i= 0; i< action.inputValueFieldPairs.numItems; i++)
		{
			if(action.inputValueFieldPairs.fieldAssignList[i].fieldId == AR_LIKE_ID)			
			{
				setFieldInfo = "Field Mapping (On Open): All Matching Ids<br/>";
			}

			if(action.inputValueFieldPairs.fieldAssignList[i].fieldId == AR_SET_DEFAULTS_ID)		
			{
				setFieldInfo = "Field Mapping (On Open): Set Fields to Default Values<br/>";
			}
		}
		
		if(strcmp(setFieldInfo.c_str(), "Field Mapping (On Open)") ==  0)
		{
			strm << setFieldInfo << ":<br/>" << endl;

			CARAssignHelper assignHelper(*arIn, dir, rootLevel, this->obj->name, this->structItemType, schemaName, action.schemaName);
			strm << assignHelper.OpenWindowAssignment(action.inputValueFieldPairs, nAction, ifElse, "Open Window");
		}
		else
		{
			strm << setFieldInfo << endl;
		}


		//Field Mapping WindowClose
		if(action.outputValueFieldPairs.numItems > 0)
		{
			setFieldInfo = "Field Mapping (On Close)";
			for(unsigned int i= 0; i< action.outputValueFieldPairs.numItems; i++)
			{
				if(action.outputValueFieldPairs.fieldAssignList[i].fieldId == AR_LIKE_ID)
				{
					setFieldInfo = "Field Mapping (On Close): All Matching Ids<br/>";
				}

				if(action.outputValueFieldPairs.fieldAssignList[i].fieldId == AR_SET_DEFAULTS_ID)
				{
					setFieldInfo = "Field Mapping (On Close): Set Fields to Default Values<br/>";
				}
			}
			
			if(strcmp(setFieldInfo.c_str(), "Field Mapping (On Close)") ==  0)
			{
				strm << setFieldInfo << ":<br/>" << endl;

				CARAssignHelper assignHelper(*arIn, dir, rootLevel, this->obj->name, this->structItemType, action.schemaName, schemaName);
				strm << assignHelper.CloseWindowAssignment(action.outputValueFieldPairs, nAction, ifElse, "Close Window");
			}
			else
			{
				strm << setFieldInfo << endl;
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in AlActionOpenDlg: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_COMMITC
string CDocAlActionStruct::ActionCommitChanges(ARCommitChangesStruct &action, int nAction)
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
	catch(...)
	{
		cout << "EXCEPTION in AlActionCommitChanges: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_CLOSEWND
string CDocAlActionStruct::ActionCloseWindow(ARCloseWndStruct &action, int nAction)
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
	catch(...)
	{
		cout << "EXCEPTION in AlActionCloseWindow: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_CALLGUIDE
string CDocAlActionStruct::ActionCallGuide(ARCallGuideStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		strm << "Server: " << arIn->LinkToServerInfo(action.serverName, rootLevel) << "<br/>" << endl;
		strm << "Guide: " << arIn->LinkToContainer(action.guideName, rootLevel) << "<br/>" << endl;

		if(action.guideTableId > 0)
		{
			strm << "Table Loop: " << arIn->LinkToField(schemaName, action.guideTableId, rootLevel) << "<br/>" << endl;
			
			if(action.guideMode == AR_CALL_GUIDE_LOOP_SELECTED_ONLY)
				strm << "<input type=\"checkbox\" name=\"tblLoopInGuide\" value=\"loopTbl\" checked>Table Loop Selected Rows Only" << endl;
			else
				strm << "<input type=\"checkbox\" name=\"tblLoopInGuide\" value=\"loopTbl\">Table Loop Selected Rows Only" << endl;

			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = this->structItemType;

			stringstream strmTmpDesc;
			strmTmpDesc << "Guide Table Loop " << ifElse << "-Action " << nAction;
			refItem->description = strmTmpDesc.str();
			
			refItem->fromName = this->obj->name;	
			refItem->fieldInsideId = action.guideTableId;
			refItem->schemaInsideId = schemaInsideId;
			arIn->AddReferenceItem(refItem);
			delete refItem;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in AlActionCallGuide: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_EXITGUIDE
string CDocAlActionStruct::ActionExitGuide(ARExitGuideStruct &action, int nAction)
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
	catch(...)
	{
		cout << "EXCEPTION in AlActionExitGuide: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_GOTOGUIDELABEL
string CDocAlActionStruct::ActionGotoGuideLabel(ARGotoGuideLabelStruct &action, int nAction)
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
	catch(...)
	{
		cout << "EXCEPTION in AlActionGuideLabel: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_WAIT
string CDocAlActionStruct::ActionWait(ARWaitStruct &action, int nAction)
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
	catch(...)
	{
		cout << "EXCEPTION in AlActionWait: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_ACTIVE_LINK_ACTION_GOTOACTION
string CDocAlActionStruct::ActionGotoAction(ARGotoActionStruct &action, int nAction)
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
	catch(...)
	{
		cout << "EXCEPTION in AlActionGotoAction: " << this->obj->name << endl;
	}

	return strm.str();
}
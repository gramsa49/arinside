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

#include "StdAfx.h"
#include ".\docfilteractionstruct.h"

CDocFilterActionStruct::CDocFilterActionStruct(CARInside &arIn, CARServerObject &obj, string schemaName, string dir, int rootLevel, int structItemType)
{
	this->arIn = &arIn;
	this->obj = &obj;	
	this->dir = dir;
	this->rootLevel = rootLevel;
	this->schemaName = schemaName;
	this->schemaInsideId = this->arIn->SchemaGetInsideId(this->schemaName);
	this->structItemType = structItemType;
}

CDocFilterActionStruct::~CDocFilterActionStruct(void)
{
}


string CDocFilterActionStruct::Get(string ifElse, ARFilterActionList &actList)
{
	stringstream strm;
	strm.str("");

	try
	{
		CTable tblListAction("tblListAlAction", "TblObjectList");
		tblListAction.AddColumn(5, "Position");	
		tblListAction.AddColumn(15, "Action Type");
		tblListAction.AddColumn(80, "Description");

		for(unsigned int nAction = 0; nAction < actList.numItems; nAction++)
		{
			ARFilterActionStruct action = actList.actionList[nAction];

			CTableRow row("cssStdRow");		
			CTableCell cellPosition(nAction, "");				

			string tmpActionType = CAREnum::FilterAction(action.action);
			if(this->structItemType == AR_STRUCT_ITEM_XML_FILTER)
			{
				stringstream tmpLink;
				tmpLink.str("");

				tmpLink << "../../filter/index_action_" << action.action << "." << CWebUtil::WebPageSuffix();				
				tmpActionType = CWebUtil::Link(CAREnum::FilterAction(action.action), tmpLink.str(), "", rootLevel);
			}
			CTableCell cellActionType(tmpActionType.c_str(), "");			

			stringstream actionDesc;
			actionDesc.str("");

			switch(action.action)
			{
			case AR_FILTER_ACTION_NONE:
				{					
					actionDesc << FilterActionNone(nAction);
				}
				break;
			case AR_FILTER_ACTION_NOTIFY:
				{
					actionDesc << FilterActionNotify(action.u.notify, nAction);
				}
				break;
			case AR_FILTER_ACTION_MESSAGE:
				{
					actionDesc << FilterActionMessage(action.u.message, nAction);
				}
				break;
			case AR_FILTER_ACTION_LOG:
				{
					actionDesc << FilterActionLog(action.u.logFile, nAction);					
				}
				break;
			case AR_FILTER_ACTION_FIELDS:
				{
					actionDesc << FilterActionSetFields(action.u.setFields, nAction);	
				}
				break;
			case AR_FILTER_ACTION_PROCESS:
				{
					actionDesc << FilterActionProcess(action.u.process, nAction);
				}
				break;
			case AR_FILTER_ACTION_FIELDP:
				{
					actionDesc << FilterActionPushFields(action.u.pushFields, nAction);
				}
				break;
			case AR_FILTER_ACTION_SQL:
				{
					actionDesc << FilterActionSql(action.u.sqlCommand, nAction);
				}
				break;
			case AR_FILTER_ACTION_GOTOACTION:
				{
					actionDesc << FilterActionGotoAction(action.u.gotoAction, nAction);
				}
				break;
			case AR_FILTER_ACTION_CALLGUIDE:
				{
					actionDesc << FilterActionCallGuide(action.u.callGuide, nAction);
				}
				break;
			case AR_FILTER_ACTION_EXITGUIDE:
				{
					actionDesc << FilterActionExitGuide(action.u.exitGuide, nAction);
				}
				break;
			case AR_FILTER_ACTION_GOTOGUIDELABEL:
				{
					actionDesc << FilterActionGotoGuideLabel(action.u.gotoGuide, nAction);
				}
				break;
			default:
				{
					actionDesc.str("");				
				}
				break;
			}

			CTableCell cellActionDesc(actionDesc.str(), "");
			row.AddCell(cellPosition);
			row.AddCell(cellActionType);
			row.AddCell(cellActionDesc);
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
		cout << "EXCEPTION enumerating filter action struct: " << this->obj->name << endl;
	}

	return strm.str();
}

//All matching Ids
string CDocFilterActionStruct::AllMatchingIds(string table1, string table2, string description, int nAction)
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

// AR_FILTER_ACTION_NONE
string CDocFilterActionStruct::FilterActionNone(int nAction)
{
	return "None";
}

// AR_FILTER_ACTION_NOTIFY
string CDocFilterActionStruct::FilterActionNotify(ARFilterActionNotify &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action.notifyText != NULL)
		{
			CFieldRefItem *refItemText = new CFieldRefItem();
			refItemText->arsStructItemType = this->structItemType;
			refItemText->fromName = this->obj->name;						
			refItemText->schemaInsideId = schemaInsideId;
			refItemText->description = "Field in Notify Action (Text)";
			strm << "Notify Text: " << arIn->TextFindFields(action.notifyText, "$", schemaInsideId, rootLevel, true, refItemText) << "<br/>" << endl;
			delete refItemText;
		}
		else
			strm << "Notify Text: " << EmptyValue << "<br/>" << endl;


		if(action.user != NULL)
		{
			CFieldRefItem *refItemUser = new CFieldRefItem();
			refItemUser->arsStructItemType = this->structItemType;
			refItemUser->fromName = this->obj->name;
			refItemUser->description = "Field in Notify Action (User Name)";
			strm << "User Name: " << arIn->TextFindFields(action.user, "$", schemaInsideId, rootLevel, true, refItemUser) << "<br/>" << endl;
			delete refItemUser;
		}
		else
			strm << "User Name: " << EmptyValue << "<br/>" << endl;


		strm << "Priority: " << action.notifyPriority << "<br/>" << endl;
		strm << "Mechanism: " << CAREnum::NotifyMechanism(action.notifyMechanism) << "<br/>" << endl;		

		if(action.notifyMechanism != AR_NOTIFY_VIA_NOTIFIER)
		{
			if(action.subjectText != NULL)
			{
				CFieldRefItem *refItemSubj = new CFieldRefItem();
				refItemSubj->arsStructItemType = this->structItemType;
				refItemSubj->fromName = this->obj->name;						
				refItemSubj->schemaInsideId = schemaInsideId;
				refItemSubj->description = "Field in Notify Action (Subject)";
				strm << "Subject: " << arIn->TextFindFields(action.subjectText, "$", schemaInsideId, rootLevel, true, refItemSubj) << "<br/>" << endl;
				delete refItemSubj;
			}

			//Fields
			strm << "<br/>Include Fields: " << CAREnum::NotifyFieldList(action.fieldIdListType) << "<br/>" << endl;
			for(unsigned int i=0; i<action.fieldIdList.numItems; i++)
			{
				if(i > 0)
				{
					CFieldRefItem *refItemNotifyTxtField = new CFieldRefItem();
					refItemNotifyTxtField->arsStructItemType = this->structItemType;
					refItemNotifyTxtField->fromName = this->obj->name;						
					refItemNotifyTxtField->schemaInsideId = schemaInsideId;
					refItemNotifyTxtField->fieldInsideId = action.fieldIdList.internalIdList[i];
					refItemNotifyTxtField->description = "Field in Notify Action (Message Text)";
					arIn->AddReferenceItem(refItemNotifyTxtField);
					strm << arIn->LinkToField(schemaInsideId, action.fieldIdList.internalIdList[i], rootLevel) << "<br/>" << endl;
				}
			}

			//Messages
			if(action.notifyAdvanced != NULL)
			{
				ARFilterActionNotifyAdvanced *adv = action.notifyAdvanced;

				strm << "<br/><br/>" << endl;

				if(adv->mailboxName != NULL)
				{
					CFieldRefItem *refItemMbName = new CFieldRefItem();
					refItemMbName->arsStructItemType = this->structItemType;
					refItemMbName->fromName = this->obj->name;						
					refItemMbName->schemaInsideId = schemaInsideId;
					refItemMbName->description = "Field in Notify Action (Mailbox Name)";

					strm << "Mailbox Name: " << arIn->TextFindFields(adv->mailboxName, "$", schemaInsideId, rootLevel, true, refItemMbName) << "<br/>" << endl;
					delete refItemMbName;
				}

				if(adv->from != NULL)
				{
					CFieldRefItem *refItemFrom = new CFieldRefItem();
					refItemFrom->arsStructItemType = this->structItemType;
					refItemFrom->fromName = this->obj->name;						
					refItemFrom->schemaInsideId = schemaInsideId;
					refItemFrom->description = "Field in Notify Action (From)";

					strm << "From: " << arIn->TextFindFields(adv->from, "$", schemaInsideId, rootLevel, true, refItemFrom) << "<br/>" << endl;
					delete refItemFrom;
				}

				if(adv->replyTo != NULL)
				{
					CFieldRefItem *refItemReplTo = new CFieldRefItem();
					refItemReplTo->arsStructItemType = this->structItemType;
					refItemReplTo->fromName = this->obj->name;						
					refItemReplTo->schemaInsideId = schemaInsideId;
					refItemReplTo->description = "Field in Notify Action (Reply To)";

					strm << "Reply To: " << arIn->TextFindFields(adv->replyTo, "$", schemaInsideId, rootLevel, true, refItemReplTo) << "<br/>" << endl;
					delete refItemReplTo;
				}

				if(adv->cc != NULL)
				{
					CFieldRefItem *refItemCc = new CFieldRefItem();
					refItemCc->arsStructItemType = this->structItemType;
					refItemCc->fromName = this->obj->name;						
					refItemCc->schemaInsideId = schemaInsideId;
					refItemCc->description = "Field in Notify Action (CC)";

					strm << "CC: " << arIn->TextFindFields(adv->cc, "$", schemaInsideId, rootLevel, true, refItemCc) << "<br/>" << endl;
					delete refItemCc;
				}

				if(adv->bcc != NULL)
				{
					CFieldRefItem *refItemBcc = new CFieldRefItem();
					refItemBcc->arsStructItemType = this->structItemType;
					refItemBcc->fromName = this->obj->name;						
					refItemBcc->schemaInsideId = schemaInsideId;
					refItemBcc->description = "Field in Notify Action (BCC)";

					strm << "BCC: " << arIn->TextFindFields(adv->bcc, "$", schemaInsideId, rootLevel, true, refItemBcc) << "<br/>" << endl;
					delete refItemBcc;
				}

				if(adv->organization != NULL)
				{
					CFieldRefItem *refItemOrg = new CFieldRefItem();
					refItemOrg->arsStructItemType = this->structItemType;
					refItemOrg->fromName = this->obj->name;						
					refItemOrg->schemaInsideId = schemaInsideId;
					refItemOrg->description = "Field in Notify Action (Organisation)";

					strm << "Organisation: " << arIn->TextFindFields(adv->organization, "$", schemaInsideId, rootLevel, true, refItemOrg) << "<br/>" << endl;
					delete refItemOrg;
				}

				if(adv->headerTemplate != NULL)
				{
					CFieldRefItem *refItemTmplHead = new CFieldRefItem();
					refItemTmplHead->arsStructItemType = this->structItemType;
					refItemTmplHead->fromName = this->obj->name;						
					refItemTmplHead->schemaInsideId = schemaInsideId;
					refItemTmplHead->description = "Field in Notify Action (Header Template)";

					strm << "Header Template: " << arIn->TextFindFields(adv->headerTemplate, "$", schemaInsideId, rootLevel, true, refItemTmplHead) << "<br/>" << endl;
					delete refItemTmplHead;
				}

				if(adv->contentTemplate != NULL)
				{
					CFieldRefItem *refItemTmplCont = new CFieldRefItem();
					refItemTmplCont->arsStructItemType = this->structItemType;
					refItemTmplCont->fromName = this->obj->name;						
					refItemTmplCont->schemaInsideId = schemaInsideId;
					refItemTmplCont->description = "Field in Notify Action (Content Template)";

					strm << "Content Template: " << arIn->TextFindFields(adv->contentTemplate, "$", schemaInsideId, rootLevel, true, refItemTmplCont) << "<br/>" << endl;
					delete refItemTmplCont;
				}

				if(adv->footerTemplate != NULL)
				{
					CFieldRefItem *refItemTmplFooter = new CFieldRefItem();
					refItemTmplFooter->arsStructItemType = this->structItemType;
					refItemTmplFooter->fromName = this->obj->name;						
					refItemTmplFooter->schemaInsideId = schemaInsideId;
					refItemTmplFooter->description = "Field in Notify Action (Footer Template)";

					strm << "Footer Template: " << arIn->TextFindFields(adv->footerTemplate, "$", schemaInsideId, rootLevel, true, refItemTmplFooter) << "<br/>" << endl;
					delete refItemTmplFooter;
				}
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterActionNotify: " << this->obj->name << endl;
	}

	return strm.str();
}


// AR_FILTER_ACTION_MESSAGE
string CDocFilterActionStruct::FilterActionMessage(ARFilterStatusStruct &action, int nAction)
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
			refItemTmp->description = "Field in Message";
			refItemTmp->fromName = this->obj->name;						
			refItemTmp->schemaInsideId = schemaInsideId;
			strm << "Message Text:<br/>" << arIn->TextFindFields(action.messageText, "$", this->schemaInsideId, rootLevel, true, refItemTmp) << "<br/>" << endl;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterActionMessage: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_FILTER_ACTION_LOG
string CDocFilterActionStruct::FilterActionLog(char* action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action != NULL)
		{
			strm << "File Name: " << action << endl; 
		}
		else
		{
			strm << "File Name: " << EmptyValue << endl; 
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterActionLog: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_FILTER_ACTION_FIELDS
string CDocFilterActionStruct::FilterActionSetFields(ARSetFieldsActionStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		//Find the secondary form in the set fields assignemt
		//Possible values: "", "*", "schemaName" 
		stringstream secondaryFormRaw, secondaryFormDisplay, serverRaw, qualification;

		CDocFilterHelper *filterHelper = new CDocFilterHelper(*this->arIn, *this->obj, this->structItemType);
		filterHelper->SetFieldsGetSecondaryForm(action, schemaName, rootLevel, secondaryFormRaw, secondaryFormDisplay,  serverRaw, qualification);
		delete filterHelper;

		//For the following internal calculations we need a secondary form
		string schemaName2 = secondaryFormRaw.str();

		string tmpDisplayName = secondaryFormDisplay.str();
		if(tmpDisplayName.size()==0)
			tmpDisplayName = schemaName2;

		strm << "Server Name: " << arIn->LinkToServerInfo(serverRaw.str(), rootLevel) << "<br/>" << endl;	



		//check if it is a webservice set fields
		if(strcmp(tmpDisplayName.c_str(), "ARSYS.ARF.WEBSERVICE")==0)
		{
			//cout << "NumItems: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->numItems << endl;
			//cout << "Unknown: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[0].u.value.u.charVal << endl;
			//cout << "Unknown: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[1].u.value.u.charVal << endl;
			//cout << "Unknown: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[2].u.value.u.charVal << endl;
			//cout << "Unknown: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[3].u.value.u.charVal << endl;
			//cout << "Sub?: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[6].u.value.u.charVal<< endl;
			//cout << "Sub?: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[9].u.value.u.charVal << endl;
			//cout << "Sub?: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[10].u.value.u.charVal << endl;

			strm << "Read Value for Field from: WEB SERVICE<br/>" << endl;
			strm << "Service Name: " << action.fieldList.fieldAssignList->assignment.u.filterApi->serviceName << "<br/>" << endl;
			strm << "WSDL Location: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[4].u.value.u.charVal << "<br/>" << endl;
			strm << "Web Service: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[5].u.value.u.charVal << "<br/>" << endl;
			strm << "Port: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[11].u.value.u.charVal << "<br/>" << endl;
			strm << "URI: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[7].u.value.u.charVal << "<br/>" << endl;
			strm << "URN: " << action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[8].u.value.u.charVal << "<br/>" << endl;
		}
		else
		{
			strm << "Read Value for Field from: " << arIn->LinkToSchema(tmpDisplayName, rootLevel) << "<br/>" << endl;

			//All matching Ids?
			string setFieldInfo = "Field Mapping";
			for(unsigned int i= 0; i< action.fieldList.numItems; i++)
			{
				if(action.fieldList.fieldAssignList[i].fieldId == AR_LIKE_ID)
				{
					setFieldInfo = "Field Mapping: All Matching Ids";

					strm << this->AllMatchingIds(schemaName, tmpDisplayName, "Set Fields", nAction);
				}
			}

			//Qualification
			strm << qualification.str() << endl;

			if(strcmp(setFieldInfo.c_str(), "Field Mapping") ==  0)
			{
				strm << setFieldInfo << ":<br/>" << endl;			

				CARAssignHelper assignHelper(*arIn, dir, rootLevel, obj->name, this->structItemType, schemaName, schemaName2);
				strm << assignHelper.SetFieldsAssignment(action, nAction, ifElse);
			}
			else
			{
				strm << setFieldInfo << endl;
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterActionSetFields: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_FILTER_ACTION_PROCESS
string CDocFilterActionStruct::FilterActionProcess(char *action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action != NULL)
		{
			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = this->structItemType;
			refItem->description = "Field in Run Process Action";
			refItem->fromName = this->obj->name;						
			refItem->schemaInsideId = schemaInsideId;	

			strm << "Command Line: " << arIn->TextFindFields(action, "$", schemaInsideId, rootLevel, true, refItem) << endl;
			delete refItem;
		}
		else
		{
			strm << "No SQL command specified." << endl;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterActionProcess: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_FILTER_ACTION_FIELDP
string CDocFilterActionStruct::FilterActionPushFields(ARPushFieldsActionStruct &action, int nAction)
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
				refItem->description = "Push Field If";
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

			CARAssignHelper assignHelper(*arIn, dir, rootLevel, this->obj->name, this->structItemType, schemaName, secondaryFormRaw.str());
			strm << assignHelper.PushFieldsAssignment(action, nAction, ifElse);
		}
		else
		{
			strm << pushFieldInfo << endl;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterActionPushFields: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_FILTER_ACTION_SQL
string CDocFilterActionStruct::FilterActionSql(ARSQLStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action.server != NULL)
			strm << "Server: " << arIn->LinkToServerInfo(action.server, rootLevel) << "<br/>" << endl;

		if(action.command != NULL)
			strm << "SQL command: <br/>" << action.command << endl;
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterActionSql: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_FILTER_ACTION_GOTOACTION 
string CDocFilterActionStruct::FilterActionGotoAction(ARGotoActionStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action.fieldIdOrValue != NULL)
		{
			strm << "FieldId or Value: " << arIn->LinkToField(schemaInsideId, action.fieldIdOrValue, rootLevel) << endl;
		}
		else
		{
			strm << "FieldId or Value: " << EmptyValue << endl;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterActionGotoAction: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_FILTER_ACTION_CALLGUIDE
string CDocFilterActionStruct::FilterActionCallGuide(ARCallGuideStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action.serverName != NULL)
			strm << "Server: " << arIn->LinkToServerInfo(action.serverName, rootLevel) << "<br/>" << endl;

		if(action.guideName != NULL)
			strm << "Guide: " << arIn->LinkToContainer(action.guideName, rootLevel) << "<br/>" << endl;

		if(action.guideTableId > 0)
		{
			strm << "Table Loop: " << arIn->LinkToField(schemaInsideId, action.guideTableId, rootLevel) << "<br/>" << endl;

			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = this->structItemType;
			refItem->description = "Guide Table Loop";
			refItem->fromName = this->obj->name;	
			refItem->fieldInsideId = action.guideTableId;
			refItem->schemaInsideId = schemaInsideId;
			arIn->AddReferenceItem(refItem);
			delete refItem;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterActionCallGuide: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_FILTER_ACTION_EXITGUIDE
string CDocFilterActionStruct::FilterActionExitGuide(ARExitGuideStruct &action, int nAction)
{
	stringstream strm;
	strm.str("");

	try
	{
		if(action.closeAll == TRUE)
			strm << "<input type=\"checkbox\" name=\"exitGuide\" value=\"exitGuideCloseAll\" checked>Close all guides on exit" << endl;
		else
			strm << "<input type=\"checkbox\" name=\"exitGuide\" value=\"exitGuideCloseAll\">Close all guides on exit" << endl;
	}
	catch(...)
	{
		cout << "EXCEPTION in FilterActionExitGuide: " << this->obj->name << endl;
	}

	return strm.str();
}

// AR_FILTER_ACTION_GOTOGUIDELABEL
string CDocFilterActionStruct::FilterActionGotoGuideLabel(ARGotoGuideLabelStruct &action, int nAction)
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
		cout << "EXCEPTION in FilterActionGotoGuideLabel: " << this->obj->name << endl;
	}

	return strm.str();
}
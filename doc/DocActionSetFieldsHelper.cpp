//Copyright (C) 2009 John Luthgers | jls17
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
#include "DocActionSetFieldsHelper.h"
#include "DocAllMatchingIdsTable.h"
#include "../core/ARSetFieldHelper.h"
#include "../core/ARAssignHelper.h"

CDocActionSetFieldsHelper::CDocActionSetFieldsHelper(CARInside &arInside, CARServerObject &arServerObject, const ARSetFieldsActionStruct& sFieldStruct, int structItemType, IfElseState ifElseMode, int numAction, int rootLevel)
: arIn(arInside), obj(arServerObject), setFieldsStruct(sFieldStruct), ifElse(ifElseMode), rootLevel(rootLevel)
{
	arStructItemType = structItemType;
	nAction = numAction;
	showServerNameInOutput = false;
}

CDocActionSetFieldsHelper::~CDocActionSetFieldsHelper(void)
{
}

void CDocActionSetFieldsHelper::ShowActionWithServerName(bool showIt)
{
	showServerNameInOutput = showIt;
}

void CDocActionSetFieldsHelper::SetFieldsGetSecondaryForm(const string& fromSchema, stringstream &strmSchema, stringstream &strmSchemaDisplay, stringstream &strmServer, stringstream &strmQual)
{
	bool useDefaultFieldMappingTable = true;

	CARSchema wfSchema(fromSchema);
	if (wfSchema.Exists())
	{
		CARSetFieldHelper sfh(*CARInside::GetInstance(), wfSchema, setFieldsStruct, ifElse, nAction);

		switch (sfh.GetType())
		{
		case SFT_CURRENT:
			strmServer << arIn.LinkToServerInfo(arIn.appConfig.serverName, rootLevel);
			strmSchemaDisplay << "CURRENT SCREEN";
			strmSchema << sfh.GetSchemaName();
			break;
		case SFT_SERVER:
		case SFT_SAMPLEDATA:
			{
				int pFormId = arIn.SchemaGetInsideId(fromSchema);
				int sFormId = -1;
				string readServer = sfh.GetServerName();
				string readSchema = sfh.GetSchemaName();

				if (readSchema.compare("@") == 0)
				{
					readSchema = fromSchema;
				}

				if (sfh.GetType() == SFT_SAMPLEDATA)
				{
					int fieldId = sfh.GetServerFieldId();
					strmServer << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn.LinkToField(fromSchema, fieldId, rootLevel)) << "$ (Sample Server: " << arIn.LinkToServerInfo(readServer, rootLevel) << ")";

					CRefItem refItemServer(obj, ifElse, nAction, REFM_SETFIELDS_SERVER);
					arIn.AddFieldReference(pFormId, fieldId, refItemServer);

					fieldId = sfh.GetSchemaFieldId();
					strmSchemaDisplay << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn.LinkToField(fromSchema, fieldId, rootLevel)) << "$ (Sample Form: " << arIn.LinkToSchema(readSchema, rootLevel) << ")";
					strmSchema << readSchema;

					CRefItem refItemForm(obj, ifElse, nAction, REFM_SETFIELDS_FORM);
					arIn.AddFieldReference(pFormId, fieldId, refItemForm);
				}
				else
				{
					strmServer << arIn.LinkToServerInfo(readServer, rootLevel);

					strmSchemaDisplay << readSchema;
					strmSchema << readSchema;
				}

				// *************************************************************************

				sFormId = arIn.SchemaGetInsideId(readSchema);

				strmQual << "<br/>Set Field If<br/>" << endl;
				stringstream strmTmpQual;
				CRefItem refItem(obj, ifElse, nAction, REFM_SETFIELDS_QUALIFICATION);

				CARQualification arQual(arIn);
				arQual.arsStructItemType = arStructItemType;
				arQual.CheckQuery(sfh.GetQualifier(), refItem, 0, pFormId, sFormId, strmTmpQual, rootLevel);

				if(strmTmpQual.str().length() > 0)
				{
					strmQual << strmTmpQual.str() << "<br/><br/>" << endl;
				}
				else
				{
					strmQual << EmptyValue << "<br/><br/>" << endl;
				}

				strmQual << "If No Requests Match: " << CAREnum::NoMatchRequest(sfh.GetNoMatchOption()) << "<br/>" << endl;
				strmQual << "If Multiple Requests Match: " << CAREnum::MultiMatchRequest(sfh.GetMultiMatchOption()) << "<br/><br/>" << endl;
			}
			break;
		case SFT_SQL:
			{
				strmServer << arIn.LinkToServerInfo(sfh.GetServerName(), rootLevel);
				strmSchema << AR_ASSIGN_SQL_SCHEMA_NAME;
				strmSchemaDisplay << "SQL";

				strmQual << "<br/>SQL command<br/>" << endl;

				if(!sfh.GetSqlCommand().empty())
				{
					CRefItem refItem(obj, ifElse, nAction, REFM_SETFIELDS_SQL_QUALIFICATION);
					strmQual << arIn.TextFindFields(sfh.GetSqlCommand(), "$", arIn.SchemaGetInsideId(fromSchema), rootLevel, true, &refItem) << "<br/><br/>" << endl;
				}
				else
					strmQual << EmptyValue << "<br/><br/>" << endl;

				strmQual << "If No Requests Match: " << CAREnum::NoMatchRequest(sfh.GetNoMatchOption()) << "<br/>" << endl;
				strmQual << "If Multiple Requests Match: " << CAREnum::MultiMatchRequest(sfh.GetMultiMatchOption()) << "<br/><br/>" << endl;	
			}
			break;
		case SFT_FILTERAPI:
			{
				string schemaName = sfh.GetSchemaName();
				strmSchema << sfh.GetSchemaName();

				if (schemaName.substr(0, 1) == "$")
				{
					CRefItem refItem(obj, ifElse, nAction, REFM_SETFIELDS_FILTERAPI_PLUGINNAME);
					schemaName = arIn.TextFindFields(sfh.GetSchemaName(), "$", arIn.SchemaGetInsideId(fromSchema), rootLevel, true, &refItem);
				}

				// the html-documtation for this action is generated directly here
				strmSchemaDisplay << "FILTER API<br/>";
				strmSchemaDisplay << "Plugin-Name: " << schemaName << "<br/><br/>" << endl;

				strmSchemaDisplay << "Input-Mapping: " << "<br/>";
				CARAssignHelper docInput(arIn, rootLevel, obj, fromSchema, fromSchema);
				strmSchemaDisplay << docInput.FilterApiInputAssignment(sfh.GetFilterAPIInputs(), sfh.GetFilterAPINumItems(), nAction, ifElse);

				strmSchemaDisplay << "Output-Mapping: " << "<br/>";
				CARAssignHelper assignHelper(arIn, rootLevel, obj, fromSchema, fromSchema);
				strmSchemaDisplay << assignHelper.SetFieldsAssignment(setFieldsStruct, nAction, ifElse);
			
				// we've generated our own html-table with input/output mapping. So avoid default table.
				useDefaultFieldMappingTable = false;
			}
			break;
		case SFT_WEBSERVICE:
			{
				//NumItems = action.fieldList.fieldAssignList[0].assignment.u.filterApi->numItems;
				//action.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[X].u.value
				//0 = Unknown
				//1 = Unknown
				//2 = Unknown
				//3 = Unknown
				//4 = WSDL Location
				//5 = Web Service
				//6 = Operation
				//7 = URI
				//8 = URN
				//9 = Input Mappings
				//10 = Output Mappings
				//11 = Port

				//populate operation string from input #6
				string operation = "";
				string form = "";

				TiXmlDocument operationXML;
				operationXML.Parse(setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[6].u.value.u.charVal, 0, TIXML_DEFAULT_ENCODING);
				TiXmlHandle opHandle(&operationXML);
				TiXmlElement *opElement = opHandle.FirstChild("operation").FirstChild("inputMapping").ToElement();
				if (opElement)
					operation = opElement->Attribute("name");

				strmSchemaDisplay << "Read Value for Field from: WEB SERVICE<br/>";
				strmSchemaDisplay << "WSDL Location: " << (setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->numItems > 4 ? CARValue::ValueToString(setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[4].u.value) : "") << "<br/>"; 
				strmSchemaDisplay << "Web Service: " << (setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->numItems > 5 ? CARValue::ValueToString(setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[5].u.value) : "") << "<br/>"; 
				strmSchemaDisplay << "Port: " << (setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->numItems > 11 ? CARValue::ValueToString(setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[11].u.value) : "") << "<br/>"; 
				strmSchemaDisplay << "Operation: " << operation << "<br/>";
				strmSchemaDisplay << "URI: " << (setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->numItems > 7 ? CARValue::ValueToString(setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[7].u.value) : "") << "<br/>"; 
				strmSchemaDisplay << "URN: " << (setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->numItems > 8 ? CARValue::ValueToString(setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[8].u.value) : "") << "<br/>"; 

				//process input mapping
				if (setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->numItems > 9)
				{
					stringstream input;
					input.str("");

					TiXmlDocument inputXML;
					inputXML.Parse(setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[9].u.value.u.charVal, 0, TIXML_DEFAULT_ENCODING);
					TiXmlHandle inputHandle(&inputXML);
					TiXmlNode *element = inputHandle.FirstChild("arDocMapping").FirstChild("formMapping").ToNode();

					CTable tblInputMappingList("pushFieldsList", "TblObjectList");
					tblInputMappingList.AddColumn(30, "Element");
					tblInputMappingList.AddColumn(70, "Field");
					input << processMappingXML(element, "", tblInputMappingList, "", WMM_INPUT);
					input << tblInputMappingList;
					strmSchemaDisplay << "<BR/>";
					strmSchemaDisplay << "Input Mapping: " << input.str() << "<BR/>";
				}

				//process output mapping
				if (setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->numItems > 10)
				{
					stringstream output;
					output.str("");

					TiXmlDocument outputXML;
					outputXML.Parse(setFieldsStruct.fieldList.fieldAssignList[0].assignment.u.filterApi->inputValues[10].u.value.u.charVal, 0, TIXML_DEFAULT_ENCODING);
					TiXmlHandle outputHandle(&outputXML);
					TiXmlNode *element = outputHandle.FirstChild("arDocMapping").FirstChild("formMapping").ToNode();

					CTable tblOutputMappingList("pushFieldsList", "TblObjectList");
					tblOutputMappingList.AddColumn(30, "Element");
					tblOutputMappingList.AddColumn(70, "Field");

					output << processMappingXML(element, "", tblOutputMappingList, "", WMM_OUTPUT);
					output << tblOutputMappingList;
					strmSchemaDisplay << "Output Mapping: " << output.str();
				}

				// we've generated our own html-table with input/output mapping. So avoid default table.
				useDefaultFieldMappingTable = false;
			}
			break;
		case SFT_ATRIUM_ORCHESTRATOR:
			{
				// TODO: implement documenting of filter-api actions correctly
				// normally we should set assignSchemaDisplay to "FILTER API" but CDocFilterActionStruct checks 
				// for "ARSYS.ARF.WEBSERVICE". To avoid breaking this implementation we leave it as it is until
				// there is enough time to re-work both (filter api & webservices) at the same time.
				//
				// Note: serviceName can hold a fieldId which is used to read the plugin-name from. add a 
				// reference tho this field later.
				strmServer << arIn.LinkToServerInfo(AR_CURRENT_SERVER_TAG, rootLevel);
				strmSchema << sfh.GetSchemaName();
				strmSchemaDisplay << sfh.GetSchemaName();

				useDefaultFieldMappingTable = false;
			}
			break;
		}
	}

	if (useDefaultFieldMappingTable)
	{
		GenerateDefaultMappingTable(fromSchema, strmSchema, strmSchemaDisplay, strmServer, strmQual);
	}
}

void CDocActionSetFieldsHelper::GenerateDefaultMappingTable(const string& fromSchema, stringstream &strmSchema, stringstream &strmSchemaDisplay, stringstream &strmServer, stringstream &strmQual)
{
	//For the following internal calculations we need a secondary form
	string readFromSchemaName = strmSchema.str();

	string tmpDisplayName = strmSchemaDisplay.str();
	if(tmpDisplayName.size()==0)
		tmpDisplayName = readFromSchemaName;


	if (this->showServerNameInOutput)
		strmSchemaDisplay << "Server: " << strmServer.str() << "<br/>" << endl;

	strmSchemaDisplay << "From: " << arIn.LinkToSchema(tmpDisplayName, rootLevel) << "<br/>" << endl;

	//Qualification
	strmSchemaDisplay << strmQual.str() << endl;

	// set field mapping
	strmSchemaDisplay << "Field Mapping:";
	if (setFieldsStruct.fieldList.fieldAssignList[0].fieldId == AR_LIKE_ID)
	{
		strmSchemaDisplay << " All Matching Ids<br/>";
		CDocAllMatchingIdsTable allMatchingFieldsTable(fromSchema, readFromSchemaName, obj, CDocAllMatchingIdsTable::AMM_SETFIELDS, nAction, ifElse, rootLevel);
		allMatchingFieldsTable.ToStream(strmSchemaDisplay);
	}
	else
	{
		strmSchemaDisplay << "<br/>" << endl;
		CARAssignHelper assignHelper(arIn, rootLevel, obj, fromSchema, readFromSchemaName);
		strmSchemaDisplay << assignHelper.SetFieldsAssignment(setFieldsStruct, nAction, ifElse);
	}
}

string CDocActionSetFieldsHelper::processMappingXML( TiXmlNode* pParent, string sParent, CTable &tblFieldList, string form, WebserviceMappingMode type)
{
	if ( !pParent )
		return "";

	stringstream strm;

	TiXmlNode* pChild;
	int t = pParent->Type();

	switch ( t )
	{
	case TiXmlNode::ELEMENT:
		if (strcmp("element",pParent->Value()) == 0)
		{
			sParent = pParent->ToElement()->Attribute("name");
		}
		else if (strcmp("formMapping",pParent->Value()) == 0)
		{
			form = pParent->FirstChild("form")->ToElement()->Attribute("formName");
		}
		else if (strcmp("fieldMapping",pParent->Value()) == 0)
		{
			int fieldID = atoi(pParent->ToElement()->Attribute("arFieldId"));

			CTableRow row("cssStdRow");
			row.AddCell(CTableCell(sParent));
			row.AddCell(CTableCell(arIn.LinkToField(form, fieldID, rootLevel)));
			//row.AddCell(CTableCell(form + " - "+arIn->LinkToField(form, fieldID, rootLevel)));
			tblFieldList.AddRow(row);	

			int msgId = 0;
			switch (type)
			{
			case WMM_INPUT: msgId = REFM_SETFIELDS_WS_INPUT; break;
			case WMM_OUTPUT: msgId = REFM_SETFIELDS_WS_OUTPUT; break;
			}
			CRefItem refItem(this->obj, ifElse, 0, msgId);
			arIn.AddFieldReference(arIn.SchemaGetInsideId(form), fieldID, refItem);
		}
		break;
	}
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		strm << processMappingXML( pChild, sParent, tblFieldList, form, type);
	}

	return strm.str();
}

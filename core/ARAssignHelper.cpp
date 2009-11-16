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
#include "ARAssignHelper.h"

CARAssignHelper::CARAssignHelper(CARInside &arIn, string dir, int rootLevel, string objName, int objType, string schemaName1, string schemaName2)
{
	this->arIn = &arIn;
	this->dir = dir;
	this->rootLevel = rootLevel;

	this->objName = objName;
	this->objType = objType;
	this->schemaName1 = schemaName1;
	this->schemaName2 = schemaName2;
	this->schemaInsideId1 = this->arIn->SchemaGetInsideId(this->schemaName1);
	this->schemaInsideId2 = this->arIn->SchemaGetInsideId(this->schemaName2);
	this->pushFieldFlag = false;
}

CARAssignHelper::~CARAssignHelper(void)
{
}


string CARAssignHelper::PushFieldsAssignment(ARPushFieldsActionStruct &action, int nAction, string ifElse)
{
	stringstream strm;
	strm.str("");
	try
	{
		CTable tblFieldList("pushFieldsList", "TblObjectList");
		tblFieldList.AddColumn(30, "Field Name");
		tblFieldList.AddColumn(70, "Value");
		this->pushFieldFlag = true;

		for(unsigned int i=0; i< action.pushFieldsList.numItems; i++)
		{
			int nTargetFieldId = action.pushFieldsList.pushFieldsList[i].field.u.fieldId;

			//Add a reference to the target field 
			stringstream desc;
			desc << "Target in 'Push Fields' " << ifElse << "-Action " << nAction;
			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = this->objType;
			refItem->description = desc.str();
			refItem->fromName = this->objName;	
			refItem->fieldInsideId = nTargetFieldId;
			refItem->schemaInsideId = this->schemaInsideId2;
			arIn->AddReferenceItem(refItem);
			delete refItem;	

			stringstream assignText;
			CheckAssignment(nTargetFieldId, ifElse, nAction, action.pushFieldsList.pushFieldsList[i].assign, assignText, "Push Fields");

			CTableRow row("cssStdRow");
			row.AddCell(CTableCell(arIn->LinkToField(schemaInsideId2, nTargetFieldId, rootLevel)));
			row.AddCell(CTableCell(assignText.str()));
			tblFieldList.AddRow(row);	
		}

		strm << tblFieldList.ToXHtml();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION PushFieldsAssignment of '" << objName << "':" << e.what() << endl;
	}

	return strm.str();
}

string CARAssignHelper::SetFieldsAssignment(ARSetFieldsActionStruct &action, int nAction, string ifElse)
{
	stringstream strm;
	strm.str("");
	try
	{
		CTable tblFieldList("setFieldsList", "TblObjectList");
		tblFieldList.AddColumn(30, "Field Name");
		tblFieldList.AddColumn(70, "Value");

		for(unsigned int i=0; i< action.fieldList.numItems; i++)
		{
			int nTargetFieldId = action.fieldList.fieldAssignList[i].fieldId;

			//Add a reference to the target field 
			stringstream desc;
			desc << "Target in 'Set Fields' " << ifElse << "-Action " << nAction;
			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = this->objType;
			refItem->description = desc.str();
			refItem->fromName = this->objName;	
			refItem->fieldInsideId = nTargetFieldId;
			refItem->schemaInsideId = this->schemaInsideId1;
			arIn->AddReferenceItem(refItem);
			delete refItem;	

			stringstream assignText;		
			CheckAssignment(nTargetFieldId, ifElse, nAction, action.fieldList.fieldAssignList[i].assignment, assignText, "Set Fields");

			CTableRow row("cssStdRow");
			row.AddCell(CTableCell(arIn->LinkToField(schemaInsideId1, nTargetFieldId, rootLevel)));
			row.AddCell(CTableCell(assignText.str()));
			tblFieldList.AddRow(row);	
		}

		strm << tblFieldList.ToXHtml();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION SetFieldsAssignment of '" << this->objName << "': " << e.what() << endl;
	}

	return strm.str();
}

string CARAssignHelper::OpenWindowAssignment(ARFieldAssignList &action, int nAction, string ifElse, string openCloseInfo)
{
	stringstream strm;
	strm.str("");
	try
	{
		CTable tblFieldList("setFieldsList", "TblObjectList");
		tblFieldList.AddColumn(30, "Field Name");
		tblFieldList.AddColumn(70, "Value");

		for(unsigned int i=0; i< action.numItems; i++)
		{
			int nTargetFieldId = action.fieldAssignList[i].fieldId;

			if (nTargetFieldId == AR_SET_DEFAULTS_ID)
				continue;

			//Add a reference to the target field 
			stringstream desc;
			desc << "Target in '" << openCloseInfo << "' " << ifElse << "-Action " << nAction;
			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = this->objType;
			refItem->description = desc.str();
			refItem->fromName = this->objName;	
			refItem->fieldInsideId = nTargetFieldId;
			refItem->schemaInsideId = this->schemaInsideId1;
			arIn->AddReferenceItem(refItem);
			delete refItem;	

			stringstream assignText;		
			CheckAssignment(nTargetFieldId, ifElse, nAction, action.fieldAssignList[i].assignment, assignText, openCloseInfo);

			CTableRow row("cssStdRow");
			row.AddCell(CTableCell(arIn->LinkToField(schemaInsideId1, nTargetFieldId, rootLevel)));
			row.AddCell(CTableCell(assignText.str()));
			tblFieldList.AddRow(row);	
		}

		strm << tblFieldList.ToXHtml();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION SetFieldsAssignment of '" << this->objName << "':" << e.what() << endl;
	}

	return strm.str();
}

string CARAssignHelper::CloseWindowAssignment(ARFieldAssignList &action, int nAction, string ifElse, string openCloseInfo)
{
	stringstream strm;
	strm.str("");
	try
	{
		CTable tblFieldList("setFieldsList", "TblObjectList");
		tblFieldList.AddColumn(30, "Field Name");
		tblFieldList.AddColumn(70, "Value");

		for(unsigned int i=0; i< action.numItems; i++)
		{
			int nTargetFieldId = action.fieldAssignList[i].fieldId;

			//Add a reference to the target field 
			stringstream desc;
			desc << "Target in '" << openCloseInfo << "' " << ifElse << "-Action " << nAction;
			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = this->objType;
			refItem->description = desc.str();
			refItem->fromName = this->objName;	
			refItem->fieldInsideId = nTargetFieldId;
			refItem->schemaInsideId = this->schemaInsideId1;
			arIn->AddReferenceItem(refItem);
			delete refItem;	

			stringstream assignText;		
			CheckAssignment(nTargetFieldId, ifElse, nAction, action.fieldAssignList[i].assignment, assignText, openCloseInfo);

			CTableRow row("cssStdRow");
			row.AddCell(CTableCell(arIn->LinkToField(schemaInsideId1, nTargetFieldId, rootLevel)));
			row.AddCell(CTableCell(assignText.str()));
			tblFieldList.AddRow(row);	
		}

		strm << tblFieldList.ToXHtml();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CloseWindowAssignment of '" << this->objName << "': " << e.what() << endl;
	}

	return strm.str();
}

string CARAssignHelper::ServiceAssignment(ARFieldAssignList &action, int nAction, string ifElse, string serviceInfo)
{
	stringstream strm;
	strm.str("");
	try
	{
		CTable tblFieldList("setFieldsList", "TblObjectList");
		tblFieldList.AddColumn(30, "Field Name");
		tblFieldList.AddColumn(70, "Value");

		for(unsigned int i=0; i< action.numItems; i++)
		{
			int nTargetFieldId = action.fieldAssignList[i].fieldId;

			//Add a reference to the target field 
			stringstream desc;
			desc << "Target in '" << serviceInfo << "' " << ifElse << "-Action " << nAction;
			CFieldRefItem *refItem = new CFieldRefItem(this->objType, this->objName, desc.str(), nTargetFieldId, this->schemaInsideId1);
			arIn->AddReferenceItem(refItem);
			delete refItem;	

			stringstream assignText;		
			CheckAssignment(nTargetFieldId, ifElse, nAction, action.fieldAssignList[i].assignment, assignText, serviceInfo);

			CTableRow row("cssStdRow");
			row.AddCell(CTableCell(arIn->LinkToField(schemaInsideId1, nTargetFieldId, rootLevel)));
			row.AddCell(CTableCell(assignText.str()));
			tblFieldList.AddRow(row);	
		}

		strm << tblFieldList.ToXHtml();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ServiceAssignment of '" << this->objName << "': " << e.what() << endl;
	}

	return strm.str();
}

void CARAssignHelper::CheckAssignment(int targetFieldId, string ifElse, int nAction, ARAssignStruct &assignment, stringstream &assignText, string refItemDesc)
{	
	try
	{
		switch(assignment.assignType)
		{
		case AR_ASSIGN_TYPE_VALUE:
			{					
				AssignValue(targetFieldId, ifElse, assignment.u.value, assignText, refItemDesc);
			}
			break;
		case AR_ASSIGN_TYPE_FIELD:
			{	
				if(assignment.u.field->u.statHistory.userOrTime == 0)
				{
					AssignField(ifElse, nAction, *assignment.u.field, assignText, refItemDesc);
				}
				else
				{
					assignText << "$Status History.";

					string tmp = arIn->GetFieldEnumValue(schemaInsideId2, 7, assignment.u.field->u.statHistory.enumVal);								
					if(strcmp(tmp.c_str(), EmptyValue)!=0)
						assignText << tmp;
					else
						assignText << assignment.u.field->u.statHistory.enumVal;

					switch(assignment.u.field->u.statHistory.userOrTime)
					{						
					case AR_STAT_HISTORY_USER:
						{
							assignText << ".USER$";
						}
						break;
					case AR_STAT_HISTORY_TIME:
						{
							assignText << ".TIME$";
						}
						break;
					}
				}				
			}
			break;
		case AR_ASSIGN_TYPE_PROCESS:
			{
				AssignProcess(ifElse, assignment.u.process, assignText, refItemDesc);
			}
			break;
		case AR_ASSIGN_TYPE_ARITH:
			{			
				switch (assignment.u.arithOp->operation) 
				{
				case AR_ARITH_OP_ADD:
					assignText << "(";
					CheckAssignment(targetFieldId, ifElse, nAction, assignment.u.arithOp->operandLeft, assignText, refItemDesc);
					assignText << CAREnum::Operand(AR_ARITH_OP_ADD);
					CheckAssignment(targetFieldId, ifElse, nAction, assignment.u.arithOp->operandRight, assignText, refItemDesc);
					assignText << ")";
					break;
				case AR_ARITH_OP_SUBTRACT:
					assignText << "(";
					CheckAssignment(targetFieldId, ifElse, nAction, assignment.u.arithOp->operandLeft, assignText, refItemDesc);
					assignText << CAREnum::Operand(AR_ARITH_OP_SUBTRACT);
					CheckAssignment(targetFieldId, ifElse, nAction, assignment.u.arithOp->operandRight, assignText, refItemDesc);
					assignText << ")";
					break;
				case AR_ARITH_OP_MULTIPLY:
					assignText << "(";
					CheckAssignment(targetFieldId, ifElse, nAction, assignment.u.arithOp->operandLeft, assignText, refItemDesc);
					assignText << CAREnum::Operand(AR_ARITH_OP_MULTIPLY);
					CheckAssignment(targetFieldId, ifElse, nAction, assignment.u.arithOp->operandRight, assignText, refItemDesc);
					assignText << ")";
					break;
				case AR_ARITH_OP_DIVIDE:
					assignText << "(";
					CheckAssignment(targetFieldId, ifElse, nAction, assignment.u.arithOp->operandLeft, assignText, refItemDesc);
					assignText << CAREnum::Operand(AR_ARITH_OP_DIVIDE);
					CheckAssignment(targetFieldId, ifElse, nAction, assignment.u.arithOp->operandRight, assignText, refItemDesc);
					assignText << ")";
					break;
				case AR_ARITH_OP_MODULO:
					assignText << "(";
					CheckAssignment(targetFieldId, ifElse, nAction, assignment.u.arithOp->operandLeft, assignText, refItemDesc);
					assignText << CAREnum::Operand(AR_ARITH_OP_MODULO);
					CheckAssignment(targetFieldId, ifElse, nAction, assignment.u.arithOp->operandRight, assignText, refItemDesc);
					assignText << ")";
					break;
				case AR_ARITH_OP_NEGATE:
					assignText << CAREnum::Operand(AR_ARITH_OP_NEGATE);
					CheckAssignment(targetFieldId, ifElse, nAction, assignment.u.arithOp->operandRight, assignText, refItemDesc);
					break;
				}

			}
			break;
		case AR_ASSIGN_TYPE_FUNCTION:
			{
				AssignFunction(targetFieldId, ifElse, nAction, *assignment.u.function, assignText, refItemDesc);
			}
			break;
		case AR_ASSIGN_TYPE_DDE:
			{
				AssignDDE(ifElse, *assignment.u.dde, assignText, refItemDesc);
			}
			break;
		case AR_ASSIGN_TYPE_SQL:
			{
				AssignSQL(ifElse, *assignment.u.sql, assignText, refItemDesc);
			}
			break;
		case AR_ASSIGN_TYPE_FILTER_API:
			{
				AssignFilterApi(ifElse, *assignment.u.filterApi, assignText, refItemDesc);
			}
			break;
		}	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ActiveLink CheckAssignment of '" << this->objName << "': " << e.what() << endl;
	}
}

void CARAssignHelper::AssignValue(int targetFieldId, string ifElse, ARValueStruct &v, stringstream &assignText, string refItemDesc)
{	
	try
	{
		stringstream strmValue;

		switch(v.dataType)
		{
		case AR_DATA_TYPE_NULL:
			{
				strmValue << "$NULL$";
			}
			break;
		case AR_DATA_TYPE_CHAR:
			{
				strmValue << "\"" << CARValue::ValueToString(v) << "\"";
			}
			break;
		case AR_DATA_TYPE_REAL:
		case AR_DATA_TYPE_ULONG:
		case AR_DATA_TYPE_INTEGER:
			{
				strmValue << CARValue::ValueToString(v);
			}
			break;
		case AR_DATA_TYPE_KEYWORD:
			{
				strmValue << "$" << CARValue::ValueToString(v) << "$";
			}
			break;
		case AR_DATA_TYPE_ENUM:
			{
				int nTmpActionSchemaId = schemaInsideId1;

				if(this->pushFieldFlag==true)
					nTmpActionSchemaId = schemaInsideId2;

				strmValue << arIn->GetFieldEnumValue(nTmpActionSchemaId, targetFieldId, v.u.enumVal);
			}
			break;
		default:
			{
				strmValue << "\"" << CARValue::ValueToString(v) << "\"";
			}
			break;
		}

		assignText << strmValue.str();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AssignValue of '" << this->objName << "': " << e.what() << endl;
	}
}

void CARAssignHelper::AssignField( string ifElse, int nAction, ARAssignFieldStruct &v, stringstream &assignText, string refItemDesc)
{
	try
	{
		stringstream desc;
		desc << "Value in '"<< refItemDesc <<"' " << ifElse << "-Action " << nAction;

		int nTmpActionSchemaId = schemaInsideId1;

		if(schemaInsideId1 != schemaInsideId2 && this->pushFieldFlag == false)
		{
			nTmpActionSchemaId = schemaInsideId2;
		}

		CFieldRefItem *refItem = new CFieldRefItem();
		refItem->arsStructItemType = this->objType;
		refItem->description = desc.str();
		refItem->fromName = this->objName;	
		refItem->fieldInsideId = v.u.fieldId;
		refItem->schemaInsideId = nTmpActionSchemaId;
		arIn->AddReferenceItem(refItem);
		delete refItem;	

		assignText << "$" << arIn->LinkToField(nTmpActionSchemaId, v.u.fieldId, rootLevel) << "$";

	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AssignField of '" << this->objName << "': " << e.what() << endl;
	}
}

void CARAssignHelper::AssignProcess(string ifElse, char *v, stringstream &assignText, string refItemDesc)
{
	try
	{
		if(v != NULL)
		{
			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = this->objType;
			refItem->description = "Value in SetField Process";
			refItem->fromName = objName;						
			refItem->schemaInsideId = schemaInsideId2;		

			assignText << "$PROCESS$ "<< arIn->TextFindFields(v, "$", this->schemaInsideId2, rootLevel, true, refItem) << endl;
			delete refItem;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AssignProcess of '" << this->objName << "': " << e.what() << endl;
	}
}

void CARAssignHelper::AssignFunction(int targetFieldId, string ifElse, int nAction, ARFunctionAssignStruct &v, stringstream &assignText, string refItemDesc)
{
	try
	{				
		assignText << CAREnum::Function(v.functionCode) << "(";

		switch (v.functionCode)
		{
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		case AR_FUNCTION_HOVER:
			{
				for(unsigned int i=0; i< v.numItems; ++i)
				{
					if(i > 0) assignText << ", ";

					if (i == 0 && v.parameterList[i].assignType == AR_ASSIGN_TYPE_VALUE && v.parameterList[i].u.value.dataType == AR_DATA_TYPE_INTEGER)
					{
						stringstream desc; desc << "HOVER-Parameter in '"<< refItemDesc <<"' " << ifElse << "-Action " << nAction;
						CFieldRefItem refItem(this->objType, this->objName, desc.str(), v.parameterList[i].u.value.u.intVal, arIn->SchemaGetInsideId(this->schemaName1));
						arIn->AddReferenceItem(&refItem);

						assignText << arIn->LinkToField(this->schemaName1, v.parameterList[i].u.value.u.intVal, rootLevel);
						continue;
					}

					CheckAssignment(targetFieldId, ifElse, nAction, v.parameterList[i], assignText, refItemDesc);
				}
			}
			break;
#endif
		default:
			{
				for(unsigned int i=0; i< v.numItems; ++i)
				{
					if(i > 0)	assignText << ", ";

					CheckAssignment(targetFieldId, ifElse, nAction, v.parameterList[i], assignText, refItemDesc);
				}
			}
			break;
		}
		assignText << ")";
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AssignFunction of '" << this->objName << "': " << e.what() << endl;
	}
}

void CARAssignHelper::AssignDDE(string ifElse, ARDDEStruct &v, stringstream &assignText, string refItemDesc)
{
	try
	{
		if(v.command != NULL)
		{
			assignText <<"Command: " << v.command << "<br/>" << endl;
		}

		if(v.item != NULL)
		{
			assignText <<"Item: " << v.item << "<br/>" << endl;
		}

		if(v.pathToProgram != NULL)
		{
			assignText << "Path to Program: " << v.pathToProgram << "<br/>" << endl;
		}

		if(v.serviceName != NULL)
		{
			assignText << "Server Name: " << v.serviceName << "<br/" << endl;
		}

		if(v.topic != NULL)
		{
			assignText << "Topic: " << v.topic << "<br/>" << endl;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AssignDDE of '" << this->objName << "': " << e.what() << endl;
	}
}

void CARAssignHelper::AssignSQL(string ifElse, ARAssignSQLStruct &v, stringstream &assignText, string refItemDesc)
{
	try
	{
		assignText << "$" << v.valueIndex << "$";
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AssignSQL of '" << this->objName << "': " << e.what() << endl;
	}
}

void CARAssignHelper::AssignFilterApi(string ifElse, ARAssignFilterApiStruct &v, stringstream &assignText, string refItemDesc)
{
	try
	{
		assignText << "$" << v.valueIndex << "$";		
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AssignFilterApi of '" << this->objName << "': " << e.what() << endl;
	}
}

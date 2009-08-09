
/****************************************************************************** 
 * 
 *  file:  ARAssignHelper.cpp
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
#include ".\arassignhelper.h"

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
	this->openWindowFlag = false;
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
	catch(...)
	{
		cout << "EXCEPTION PushFieldsAssignment: " << objName << endl;
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
	catch(...)
	{
		cout << "EXCEPTION SetFieldsAssignment: " << this->objName << endl;
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
		this->openWindowFlag = true;

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
			row.AddCell(CTableCell(arIn->LinkToField(schemaInsideId2, nTargetFieldId, rootLevel)));
			row.AddCell(CTableCell(assignText.str()));
			tblFieldList.AddRow(row);	
		}

		strm << tblFieldList.ToXHtml();
	}
	catch(...)
	{
		cout << "EXCEPTION SetFieldsAssignment: " << this->objName << endl;
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
		this->openWindowFlag = true;

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
			refItem->schemaInsideId = this->schemaInsideId2;
			arIn->AddReferenceItem(refItem);
			delete refItem;	
			
			stringstream assignText;		
			CheckAssignment(nTargetFieldId, ifElse, nAction, action.fieldAssignList[i].assignment, assignText, openCloseInfo);
            
			CTableRow row("cssStdRow");
			row.AddCell(CTableCell(arIn->LinkToField(schemaInsideId2, nTargetFieldId, rootLevel)));
			row.AddCell(CTableCell(assignText.str()));
			tblFieldList.AddRow(row);	
		}

		strm << tblFieldList.ToXHtml();
	}
	catch(...)
	{
		cout << "EXCEPTION SetFieldsAssignment: " << this->objName << endl;
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
			CFieldRefItem *refItem = new CFieldRefItem();
			refItem->arsStructItemType = this->objType;
			refItem->description = desc.str();
			refItem->fromName = this->objName;	
			refItem->fieldInsideId = nTargetFieldId;
			refItem->schemaInsideId = this->schemaInsideId2;
			arIn->AddReferenceItem(refItem);
			delete refItem;	
			
			stringstream assignText;		
			CheckAssignment(nTargetFieldId, ifElse, nAction, action.fieldAssignList[i].assignment, assignText, serviceInfo);
            
			CTableRow row("cssStdRow");
			row.AddCell(CTableCell(arIn->LinkToField(schemaInsideId2, nTargetFieldId, rootLevel)));
			row.AddCell(CTableCell(assignText.str()));
			tblFieldList.AddRow(row);	
		}

		strm << tblFieldList.ToXHtml();
	}
	catch(...)
	{
		cout << "EXCEPTION ServiceAssignment: " << this->objName << endl;
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
					if(strcmp(tmp.c_str(), EmptyValue.c_str())!=0)
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
	catch(...)
	{
		cout << "EXCEPTION in ActiveLink CheckAssignment: " << this->objName << endl;
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
	catch(...)
	{
		cout << "EXCEPTION in AssignValue: " << this->objName << endl;
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
		
		if(schemaInsideId1 != schemaInsideId2 && this->openWindowFlag == true)
		{
			nTmpActionSchemaId = schemaInsideId1;
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
	catch(...)
	{
		cout << "EXCEPTION in AssignField: " << this->objName << endl;
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

			assignText << "$PROCESS$"<< arIn->TextFindFields(v, "$", this->schemaInsideId2, rootLevel, true, refItem) << endl;
			delete refItem;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in AssignProcess: " << this->objName << endl;
	}
}

void CARAssignHelper::AssignFunction(int targetFieldId, string ifElse, int nAction, ARFunctionAssignStruct &v, stringstream &assignText, string refItemDesc)
{
	try
	{				
		assignText << CAREnum::Function(v.functionCode) << "(";

		for(unsigned int i=0; i< v.numItems; i++)
		{
			if(i > 0)
			{
				assignText << " ,";
			}

			CheckAssignment(targetFieldId, ifElse, nAction, v.parameterList[i], assignText, refItemDesc);
		}
		assignText << ")";
	}
	catch(...)
	{
		cout << "EXCEPTION in AssignFunction: " << this->objName << endl;
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
	catch(...)
	{
		cout << "EXCEPTION in AssignDDE: " << this->objName << endl;
	}
}

void CARAssignHelper::AssignSQL(string ifElse, ARAssignSQLStruct &v, stringstream &assignText, string refItemDesc)
{
	try
	{
		assignText << "$" << v.valueIndex << "$";
	}
	catch(...)
	{
		cout << "EXCEPTION in AssignSQL: " << this->objName << endl;
	}
}

void CARAssignHelper::AssignFilterApi(string ifElse, ARAssignFilterApiStruct &v, stringstream &assignText, string refItemDesc)
{
	try
	{
		assignText << "$" << v.valueIndex << "$";		
	}
	catch(...)
	{
		cout << "EXCEPTION in AssignFilterApi: " << this->objName << endl;
	}
}


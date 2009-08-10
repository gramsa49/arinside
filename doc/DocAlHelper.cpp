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
#include ".\docalhelper.h"

CDocAlHelper::CDocAlHelper(CARInside &arInside, CARActiveLink &arActiveLink)
{
	this->pInside = &arInside;
	this->pAl = &arActiveLink;
}

CDocAlHelper::~CDocAlHelper(void)
{
}

void CDocAlHelper::SetFieldsGetSecondaryForm(ARSetFieldsActionStruct sFieldStruct, string fromSchema, int rootLevel, stringstream &strmSchema, stringstream &strmSchemaDisplay, stringstream &strmServer, stringstream &strmQual)
{	
	bool bFieldAssignement = false;
	for(unsigned int i=0; i < sFieldStruct.fieldList.numItems; i++)
	{		
		if(sFieldStruct.fieldList.fieldAssignList[i].assignment.assignType == AR_ASSIGN_TYPE_FIELD)
		{
			this->FieldAssignement(sFieldStruct.fieldList.fieldAssignList[i].assignment, fromSchema, rootLevel, strmSchema, strmSchemaDisplay, strmServer, strmQual, "Set Field");	
			bFieldAssignement = true;
		}	
	}

	if(!bFieldAssignement)
	{
		for(unsigned int i=0; i < sFieldStruct.fieldList.numItems; i++)
		{
			this->CheckAssignment(sFieldStruct.fieldList.fieldAssignList[i].assignment, fromSchema, rootLevel, strmSchema, strmSchemaDisplay, strmServer, strmQual, "Set Field");	
		}
	}
}

void CDocAlHelper::FieldAssignement(ARAssignStruct &assignment, string fromSchema, int rootLevel, stringstream &assignSchema, stringstream &assignSchemaDisplay, stringstream &assignServer, stringstream &assignQual, string refItemDesc)
{	
	try
	{
		assignSchema.str("");
		assignSchemaDisplay.str("");
		assignServer.str("");
		assignQual.str("");

		if(strcmp(assignment.u.field->schema, AR_CURRENT_SCREEN_TAG)==0	|| strcmp(assignment.u.field->schema, "") ==0 )
		{			
			assignSchema << fromSchema;
			assignSchemaDisplay << "CURRENT SCREEN";
		}
		else
		{
			assignSchemaDisplay << assignment.u.field->schema;
			assignSchema << assignment.u.field->schema;
		}					

		assignServer << assignment.u.field->server;

		assignQual << "<br/>Set Field If<br/>" << endl;
		stringstream strmTmpQual;

		CFieldRefItem *refItem = new CFieldRefItem();
		refItem->arsStructItemType = AR_STRUCT_ITEM_XML_ACTIVE_LINK;
		refItem->description = "Set Field If Qualification";
		refItem->fromName = this->pAl->name;

		CARQualification arQual(*this->pInside);
		arQual.arsStructItemType = AR_STRUCT_ITEM_XML_ACTIVE_LINK;

		int pFormId = this->pInside->SchemaGetInsideId(fromSchema.c_str());
		int sFormId = this->pInside->SchemaGetInsideId(assignSchema.str().c_str());
		arQual.CheckQuery(&assignment.u.field->qualifier, *refItem, 0, pFormId, sFormId, strmTmpQual, rootLevel);

		delete refItem;

		if(strmTmpQual.str().length() > 0)
		{
			assignQual << strmTmpQual.str() << "<br/><br/>" << endl;
		}
		else
		{
			assignQual << EmptyValue << "<br/><br/>" << endl;
		}

		assignQual << "If No Requests Match: " << CAREnum::NoMatchRequest(assignment.u.field->noMatchOption) << "<br/>" << endl;
		assignQual << "If Multiple Requests Match: " << CAREnum::MultiMatchRequest(assignment.u.field->multiMatchOption) << "<br/><br/>" << endl;									

	}
	catch(...)
	{
		cout << "EXCEPTION in SetFieldsCheckAssignment: " << this->pAl->name << endl;
	}
}

void CDocAlHelper::CheckAssignment(ARAssignStruct &assignment, string fromSchema, int rootLevel, stringstream &assignSchema, stringstream &assignSchemaDisplay, stringstream &assignServer, stringstream &assignQual, string refItemDesc)
{	
	try
	{
		switch(assignment.assignType)
		{
		case AR_ASSIGN_TYPE_VALUE:
			{			
				if(assignSchema.str().size()==0)
				{
					assignSchema.str("");
					assignSchemaDisplay.str("");
					assignServer.str("");
					assignQual.str("");

					assignSchema << fromSchema;
					assignSchemaDisplay << "CURRENT SCREEN";
					assignServer << AR_CURRENT_SERVER_TAG;				
				}
			}
			break;
		case AR_ASSIGN_TYPE_FIELD:
			{
				assignSchema.str("");
				assignSchemaDisplay.str("");
				assignServer.str("");
				assignQual.str("");

				if(strcmp(assignment.u.field->schema, AR_CURRENT_SCREEN_TAG)==0	|| strcmp(assignment.u.field->schema, "") ==0 )
				{			
					assignSchema << fromSchema;
					assignSchemaDisplay << "CURRENT SCREEN";
				}
				else
				{
					assignSchemaDisplay << assignment.u.field->schema;
					assignSchema << assignment.u.field->schema;
				}					

				assignServer << assignment.u.field->server;

				assignQual << "<br/>Set Field If<br/>" << endl;
				stringstream strmTmpQual;

				CFieldRefItem *refItem = new CFieldRefItem();
				refItem->arsStructItemType = AR_STRUCT_ITEM_XML_ACTIVE_LINK;
				refItem->description = "Set Field If Qualification";
				refItem->fromName = this->pAl->name;

				CARQualification arQual(*this->pInside);
				arQual.arsStructItemType = AR_STRUCT_ITEM_XML_ACTIVE_LINK;

				int pFormId = this->pInside->SchemaGetInsideId(fromSchema.c_str());
				int sFormId = this->pInside->SchemaGetInsideId(assignSchema.str().c_str());
				arQual.CheckQuery(&assignment.u.field->qualifier, *refItem, 0, pFormId, sFormId, strmTmpQual, rootLevel);

				delete refItem;

				if(strmTmpQual.str().length() > 0)
				{
					assignQual << strmTmpQual.str() << "<br/><br/>" << endl;
				}
				else
				{
					assignQual << EmptyValue << "<br/><br/>" << endl;
				}

				assignQual << "If No Requests Match: " << CAREnum::NoMatchRequest(assignment.u.field->noMatchOption) << "<br/>" << endl;
				assignQual << "If Multiple Requests Match: " << CAREnum::MultiMatchRequest(assignment.u.field->multiMatchOption) << "<br/><br/>" << endl;									

			}
			break;
		case AR_ASSIGN_TYPE_PROCESS:
			{
				assignSchema.str("");
				assignSchemaDisplay.str("");
				assignServer.str("");
				assignQual.str("");

				assignSchema << fromSchema;
				assignSchemaDisplay << "CURRENT SCREEN";
				assignServer << AR_CURRENT_SERVER_TAG;		
			}
			break;
		case AR_ASSIGN_TYPE_ARITH:
			{			
				switch (assignment.u.arithOp->operation) 
				{
				case AR_ARITH_OP_ADD:			
					CheckAssignment(assignment.u.arithOp->operandLeft, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual, refItemDesc);						
					CheckAssignment(assignment.u.arithOp->operandRight, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual, refItemDesc);						
					break;
				case AR_ARITH_OP_SUBTRACT:						
					CheckAssignment(assignment.u.arithOp->operandLeft, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual, refItemDesc);						
					CheckAssignment(assignment.u.arithOp->operandRight, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual, refItemDesc);						
					break;
				case AR_ARITH_OP_MULTIPLY:						
					CheckAssignment(assignment.u.arithOp->operandLeft, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual, refItemDesc);						
					CheckAssignment(assignment.u.arithOp->operandRight, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual, refItemDesc);						
					break;
				case AR_ARITH_OP_DIVIDE:						
					CheckAssignment(assignment.u.arithOp->operandLeft, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual, refItemDesc);						
					CheckAssignment(assignment.u.arithOp->operandRight, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual, refItemDesc);						
					break;
				case AR_ARITH_OP_MODULO:						
					CheckAssignment(assignment.u.arithOp->operandLeft, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual, refItemDesc);						
					CheckAssignment(assignment.u.arithOp->operandRight, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual, refItemDesc);						
					break;
				case AR_ARITH_OP_NEGATE:						
					CheckAssignment(assignment.u.arithOp->operandRight, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual, refItemDesc);
					break;
				}

			}
			break;
		case AR_ASSIGN_TYPE_FUNCTION:
			{
				for(unsigned int i=0; i< assignment.u.function->numItems; i++)
				{
					CheckAssignment(assignment.u.function->parameterList[i], fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual, refItemDesc);
				}
			}
			break;
		case AR_ASSIGN_TYPE_DDE:
			{
				assignSchema.str("");
				assignSchemaDisplay.str("");
				assignServer.str("");
				assignQual.str("");

				assignSchema << fromSchema;
				assignSchemaDisplay << "CURRENT SCREEN";
				assignServer << AR_CURRENT_SERVER_TAG;		
			}
			break;
		case AR_ASSIGN_TYPE_SQL:
			{							
				assignSchema.str("");
				assignSchemaDisplay.str("");
				assignServer.str("");
				assignQual.str("");

				assignSchema << AR_ASSIGN_SQL_SCHEMA_NAME;
				assignSchemaDisplay << "SQL";
				assignServer << AR_CURRENT_SERVER_TAG;

				assignQual << "<br/>SQL command<br/>" << endl;

				stringstream strmTmpQual;
				strmTmpQual << assignment.u.sql->sqlCommand;

				if(strmTmpQual.str().length() > 0)
				{
					assignQual << strmTmpQual.str() << "<br/><br/>" << endl;
				}
				else
				{
					assignQual << EmptyValue << "<br/><br/>" << endl;
				}

				assignQual << "If No Requests Match: " << CAREnum::NoMatchRequest(assignment.u.sql->noMatchOption) << "<br/>" << endl;
				assignQual << "If Multiple Requests Match: " << CAREnum::MultiMatchRequest(assignment.u.sql->multiMatchOption) << "<br/><br/>" << endl;	

			}
			break;					
		}	
	}
	catch(...)
	{
		cout << "EXCEPTION in SetFieldsCheckAssignmentEx: " << this->pAl->name << endl;
	}
}
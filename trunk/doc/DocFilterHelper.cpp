
/****************************************************************************** 
 * 
 *  file:  DocFilterHelper.cpp
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
#include ".\docfilterhelper.h"

CDocFilterHelper::CDocFilterHelper(CARInside &arInside, CARServerObject &arServerObject, int structItemType)
{
	this->pInside = &arInside;
	this->obj = &arServerObject;
	this->structItemType = structItemType;
}

CDocFilterHelper::~CDocFilterHelper(void)
{
}

void CDocFilterHelper::SetFieldsGetSecondaryForm(ARSetFieldsActionStruct sFieldStruct, string fromSchema, int rootLevel, stringstream &strmSchema, stringstream &strmSchemaDisplay, stringstream &strmServer, stringstream &strmQual)
{	
	bool bFieldAssignement = false;
	for(unsigned int i=0; i < sFieldStruct.fieldList.numItems; i++)
	{		
		if(sFieldStruct.fieldList.fieldAssignList[i].assignment.assignType == AR_ASSIGN_TYPE_FIELD)
		{
			this->FieldAssignment(sFieldStruct.fieldList.fieldAssignList[i].assignment, fromSchema, rootLevel, strmSchema, strmSchemaDisplay, strmServer, strmQual, "Set Field");	
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

void CDocFilterHelper::FieldAssignment(ARAssignStruct &assignment, string fromSchema, int rootLevel, stringstream &assignSchema, stringstream &assignSchemaDisplay, stringstream &assignServer, stringstream &assignQual, string refItemDesc)
{	
	try
	{
		assignSchema.str("");
		assignSchemaDisplay.str("");
		assignServer.str("");
		assignQual.str("");
										
		if(strcmp(assignment.u.field->schema, AR_CURRENT_SCHEMA_TAG)==0 
			|| strcmp(assignment.u.field->schema, AR_CURRENT_TRAN_TAG)==0
			|| strcmp(assignment.u.field->schema, "") ==0
			)
		{
			assignSchema << fromSchema;
			assignSchemaDisplay << "CURRENT TRANSACTION";
		}
		else
		{
			assignSchema << assignment.u.field->schema;
		}					

		assignServer << assignment.u.field->server;
							
		assignQual << "<br/>Set Field If<br/>" << endl;

		stringstream strmTmpQual;

		CFieldRefItem *refItem = new CFieldRefItem();
		refItem->arsStructItemType = this->structItemType;
		refItem->description = "Set Field If Qualification";
		refItem->fromName = this->obj->name;

		CARQualification arQual(*this->pInside);
		arQual.arsStructItemType = this->structItemType;

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
		cout << "EXCEPTION in SetFieldsCheckAssignment: " << this->obj->name << endl;
	}
}

void CDocFilterHelper::CheckAssignment(ARAssignStruct &assignment, string fromSchema, int rootLevel, stringstream &assignSchema, stringstream &assignSchemaDisplay, stringstream &assignServer, stringstream &assignQual, string refItemDesc)
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
					assignSchemaDisplay << "CURRENT TRANSACTION";
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
												
				if(strcmp(assignment.u.field->schema, AR_CURRENT_SCHEMA_TAG)==0 
					|| strcmp(assignment.u.field->schema, AR_CURRENT_TRAN_TAG)==0
					|| strcmp(assignment.u.field->schema, "") ==0
					)
				{
					assignSchema << fromSchema;
					assignSchemaDisplay << "CURRENT TRANSACTION";
				}
				else
				{
					assignSchema << assignment.u.field->schema;
				}					

				assignServer << assignment.u.field->server;
									
				assignQual << "<br/>Set Field If<br/>" << endl;

				stringstream strmTmpQual;

				CFieldRefItem *refItem = new CFieldRefItem();
				refItem->arsStructItemType = this->structItemType;
				refItem->description = "Set Field If Qualification";
				refItem->fromName = this->obj->name;

				CARQualification arQual(*this->pInside);
				arQual.arsStructItemType = this->structItemType;

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
				assignSchemaDisplay << "CURRENT TRANSACTION";
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
				assignSchemaDisplay << "CURRENT TRANSACTION";
				assignServer << AR_CURRENT_SERVER_TAG;		
			}
			break;
			case AR_ASSIGN_TYPE_SQL:
			{							
				assignSchema.str("");
				assignServer.str("");
				assignQual.str("");

				assignSchema << AR_ASSIGN_SQL_SCHEMA_NAME;
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
			case AR_ASSIGN_TYPE_FILTER_API:
			{
				assignSchema.str("");
				assignSchemaDisplay.str("");
				assignServer.str("");
				assignQual.str("");

				assignSchema << fromSchema;
				assignSchemaDisplay << "FILTER API";
				assignServer << AR_CURRENT_SERVER_TAG;		
			}
			break;			
		}	
	}
	catch(...)
	{
		cout << "EXCEPTION in SetFieldsCheckAssignmentEx: " << this->obj->name << endl;
	}
}
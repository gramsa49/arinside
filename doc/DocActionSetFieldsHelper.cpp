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

CDocActionSetFieldsHelper::CDocActionSetFieldsHelper(CARInside &arInside, CARServerObject &arServerObject, const ARSetFieldsActionStruct& sFieldStruct, int structItemType, string& strIfElse, int numAction)
: arIn(arInside), obj(arServerObject), setFieldsStruct(sFieldStruct), ifElse(strIfElse)
{
	arStructItemType = structItemType;
	nAction = numAction;
}

CDocActionSetFieldsHelper::~CDocActionSetFieldsHelper(void)
{
}

void CDocActionSetFieldsHelper::SetFieldsGetSecondaryForm(const string& fromSchema, int rootLevel, stringstream &strmSchema, stringstream &strmSchemaDisplay, stringstream &strmServer, stringstream &strmQual)
{	
	for(unsigned int i=0; i < setFieldsStruct.fieldList.numItems; ++i)
	{
		if (this->CheckAssignment(setFieldsStruct.fieldList.fieldAssignList[i].assignment, fromSchema, rootLevel, strmSchema, strmSchemaDisplay, strmServer, strmQual))
			return;
	}

	strmSchema << fromSchema;
	
	if (arStructItemType == AR_STRUCT_ITEM_XML_ACTIVE_LINK)
	{
		strmSchemaDisplay << "CURRENT SCREEN";
		strmServer << arIn.LinkToServerInfo(arIn.appConfig.serverName, rootLevel);
	}
	else
	{
		strmSchemaDisplay << "CURRENT TRANSACTION";
		// for filter/escalations no server needed?
	}
}

bool CDocActionSetFieldsHelper::CheckAssignment(const ARAssignStruct &assignment, const string& fromSchema, int rootLevel, stringstream &assignSchema, stringstream &assignSchemaDisplay, stringstream &assignServer, stringstream &assignQual)
{	
	try
	{
		switch(assignment.assignType)
		{
		case AR_ASSIGN_TYPE_FIELD:
			{
				if((assignment.u.field->schema[0] == '@' ||  // AR_CURRENT_SCHEMA_TAG
				    assignment.u.field->schema[0] == '*') && // AR_CURRENT_SCREEN_TAG / AR_CURRENT_TRAN_TAG
				    assignment.u.field->schema[1] == 0)
				{
					// if this assignment uses a field from current screen, we are not interested
					return false;
				}

				// *************************************************************************
				// now check schema and form for keyword/fieldid and use sample data instead
				string readSchema;
				string readServer;
				int pFormId = arIn.SchemaGetInsideId(fromSchema);
				int sFormId = -1;

				if (assignment.u.field->schema[0] == '$' && setFieldsStruct.sampleSchema[0] != 0)
				{
					int fieldId = atoi(&assignment.u.field->schema[1]);

					if (setFieldsStruct.sampleSchema[0] == '@' && setFieldsStruct.sampleSchema[1] == 0)
					{
						readSchema = fromSchema;
					}
					else
					{
						readSchema = setFieldsStruct.sampleSchema;
					}
					assignSchemaDisplay << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn.LinkToField(fromSchema, fieldId, rootLevel)) << "$ (Sample Form: " << arIn.LinkToSchema(readSchema, rootLevel) << ")";
					assignSchema << setFieldsStruct.sampleSchema;

					stringstream tmpDesc;
					tmpDesc << "Form Name in 'SetFields' " << ifElse << "-Action " << nAction;

					CFieldRefItem refItem(arStructItemType, obj.GetName(), tmpDesc.str(), fieldId, pFormId);
					arIn.AddReferenceItem(&refItem);
				}
				else
				{
					readSchema = assignment.u.field->schema;
					assignSchemaDisplay << assignment.u.field->schema;
					assignSchema << assignment.u.field->schema;
				}

				if (assignment.u.field->server[0]== '$' && setFieldsStruct.sampleServer[0] != 0)
				{
					int fieldId = atoi(&assignment.u.field->server[1]);

					readServer = setFieldsStruct.sampleServer;
					assignServer << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn.LinkToField(fromSchema, fieldId, rootLevel)) << "$ (Sample Server: " << arIn.LinkToServerInfo(readServer, rootLevel) << ")";

					stringstream tmpDesc;
					tmpDesc << "Server Name in 'SetFields' " << ifElse << "-Action " << nAction;

					CFieldRefItem refItem(arStructItemType, obj.GetName(), tmpDesc.str(), fieldId, pFormId);
					arIn.AddReferenceItem(&refItem);
				}
				else
				{
					readServer = assignment.u.field->server;
					assignServer << arIn.LinkToServerInfo(assignment.u.field->server, rootLevel);
				}
				// *************************************************************************

				sFormId = arIn.SchemaGetInsideId(readSchema);

				assignQual << "<br/>Set Field If<br/>" << endl;
				stringstream strmTmpQual;
				CFieldRefItem refItem(arStructItemType, obj.GetName(), "Set Field If Qualification", -1, -1);

				CARQualification arQual(arIn);
				arQual.arsStructItemType = arStructItemType;
				arQual.CheckQuery(&assignment.u.field->qualifier, refItem, 0, pFormId, sFormId, strmTmpQual, rootLevel);

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
				return true;
			}
		case AR_ASSIGN_TYPE_ARITH:
			{			
				switch (assignment.u.arithOp->operation) 
				{
				case AR_ARITH_OP_ADD:			
					if (CheckAssignment(assignment.u.arithOp->operandLeft, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual)) return true;
					if (CheckAssignment(assignment.u.arithOp->operandRight, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual)) return true;
					break;
				case AR_ARITH_OP_SUBTRACT:						
					if (CheckAssignment(assignment.u.arithOp->operandLeft, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual)) return true;
					if (CheckAssignment(assignment.u.arithOp->operandRight, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual)) return true;
					break;
				case AR_ARITH_OP_MULTIPLY:						
					if (CheckAssignment(assignment.u.arithOp->operandLeft, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual)) return true;
					if (CheckAssignment(assignment.u.arithOp->operandRight, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual)) return true;
					break;
				case AR_ARITH_OP_DIVIDE:						
					if (CheckAssignment(assignment.u.arithOp->operandLeft, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual)) return true;
					if (CheckAssignment(assignment.u.arithOp->operandRight, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual)) return true;
					break;
				case AR_ARITH_OP_MODULO:						
					if (CheckAssignment(assignment.u.arithOp->operandLeft, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual)) return true;
					if (CheckAssignment(assignment.u.arithOp->operandRight, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual)) return true;
					break;
				case AR_ARITH_OP_NEGATE:						
					if (CheckAssignment(assignment.u.arithOp->operandRight, fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual)) return true;
					break;
				}

			}
			break;
		case AR_ASSIGN_TYPE_FUNCTION:
			{
				for(unsigned int i=0; i< assignment.u.function->numItems; ++i)
				{
					if (CheckAssignment(assignment.u.function->parameterList[i], fromSchema, rootLevel, assignSchema, assignSchemaDisplay, assignServer, assignQual))
						return true;
				}
			}
			break;
		case AR_ASSIGN_TYPE_SQL:
			{							
				assignSchema << AR_ASSIGN_SQL_SCHEMA_NAME;
				assignSchemaDisplay << "SQL";
				assignServer << arIn.LinkToServerInfo(assignment.u.sql->server, rootLevel);

				assignQual << "<br/>SQL command<br/>" << endl;

				if(assignment.u.sql->sqlCommand[0] != 0)
				{
					CFieldRefItem *refItem = new CFieldRefItem(arStructItemType, obj.GetName(), "SQL Set Field If Qualification", -1, arIn.SchemaGetInsideId(fromSchema));
					assignQual << arIn.TextFindFields(assignment.u.sql->sqlCommand, "$", arIn.SchemaGetInsideId(fromSchema), rootLevel, true, refItem) << "<br/><br/>" << endl;
					delete refItem;
				}
				else
					assignQual << EmptyValue << "<br/><br/>" << endl;

				assignQual << "If No Requests Match: " << CAREnum::NoMatchRequest(assignment.u.sql->noMatchOption) << "<br/>" << endl;
				assignQual << "If Multiple Requests Match: " << CAREnum::MultiMatchRequest(assignment.u.sql->multiMatchOption) << "<br/><br/>" << endl;	
				return true;
			}
		case AR_ASSIGN_TYPE_FILTER_API:
			{
				// TODO: implement documenting of filter-api actions correctly
				// normally we should set assignSchemaDisplay to "FILTER API" but CDocFilterActionStruct checks 
				// for "ARSYS.ARF.WEBSERVICE". To avoid breaking this implementation we leave it as it is until
				// there is enough time to re-work both (filter api & webservices) at the same time.
				//
				// Note: serviceName can hold a fieldId which is used to read the plugin-name from. add a 
				// reference tho this field later.
				assignSchema << fromSchema;
				assignSchemaDisplay << assignment.u.filterApi->serviceName;
				assignServer << arIn.LinkToServerInfo(AR_CURRENT_SERVER_TAG, rootLevel);
				return true;
			}
		}	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in CheckAssignment of '" << obj.GetName() << "': " << e.what() << endl;
	}
	return false;
}


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
#include ".\arqualification.h"

CARQualification::CARQualification(CARInside &arIn)
{
	this->arIn = &arIn;
	this->tmpFieldId = 0;
	this->tmpFormId = 0;
	arsStructItemType = AR_STRUCT_ITEM_XML_NONE;
}

CARQualification::~CARQualification(void)
{
}


void CARQualification::CheckQuery(ARQualifierStruct *query, CFieldRefItem refItem, int depth, int pFormId, int sFormId, stringstream &qText, int rootLevel)
{

	if (query != NULL)
	{
		switch(query->operation)
		{
		case AR_COND_OP_NONE:
			{
			}
			break;
		case AR_COND_OP_AND:
			{
				qText << "(";
				if(query->u.andor.operandLeft != NULL)
					CheckQuery(query->u.andor.operandLeft, refItem, depth+1, pFormId, sFormId, qText, rootLevel);

				qText << ") AND (";

				if(query->u.andor.operandRight != NULL)
					CheckQuery(query->u.andor.operandRight, refItem, depth+1, pFormId, sFormId, qText, rootLevel);

				qText << ")";
			}
			break;
		case AR_COND_OP_OR:
			{
				qText << "(";
				if(query->u.andor.operandLeft != NULL)
					CheckQuery(query->u.andor.operandLeft, refItem, depth+1, pFormId, sFormId, qText, rootLevel);

				qText << ") OR (";

				if(query->u.andor.operandRight != NULL)
					CheckQuery(query->u.andor.operandRight, refItem, depth+1, pFormId, sFormId, qText, rootLevel);

				qText << ")";
			}
			break;
		case AR_COND_OP_NOT:
			{
				qText << "NOT (";

				if(query->u.andor.operandRight != NULL)
					CheckQuery(query->u.andor.operandRight, refItem, depth+1, pFormId, sFormId, qText, rootLevel);

				if(query->u.andor.operandLeft != NULL)
					CheckQuery(query->u.andor.operandLeft, refItem, depth+1, pFormId, sFormId, qText, rootLevel);					

				qText << ")";
			}
			break;
		case AR_COND_OP_REL_OP:
			{
				CheckOperand(&query->u.relOp->operandLeft, refItem, pFormId, sFormId, qText, rootLevel);
				switch (query->u.relOp->operation) 
				{		
				case AR_REL_OP_EQUAL:
					{
						qText << " = ";
					}
					break;
				case AR_REL_OP_GREATER:
					{
						qText << " > ";
					}
					break;
				case AR_REL_OP_GREATER_EQUAL:
					{
						qText << " >= ";
					}
					break;
				case AR_REL_OP_LESS:
					{
						qText << " < ";
					}
					break;
				case AR_REL_OP_LESS_EQUAL:
					{
						qText << " <= ";
					}
					break;
				case AR_REL_OP_NOT_EQUAL:
					{
						qText << " != ";
					}
					break;
				case AR_REL_OP_LIKE:
					{
						qText << " LIKE ";
					}
					break;
				}
				CheckOperand(&query->u.relOp->operandRight, refItem, pFormId, sFormId, qText, rootLevel);
			}
			break;
		case AR_COND_OP_FROM_FIELD: //A qualification located in a field on the form.
			{
				qText << "EXTERNAL ($" << arIn->LinkToField(pFormId, query->u.fieldId, rootLevel) << "$)";

				CFieldRefItem *item = new CFieldRefItem();
				item->arsStructItemType = refItem.arsStructItemType;
				item->description = refItem.description;
				item->fromName = refItem.fromName;					
				item->fieldInsideId = query->u.fieldId;					
				item->schemaInsideId = pFormId;
				arIn->AddReferenceItem(item);
			}
			break;
		}
	}
}

void CARQualification::CheckOperand(ARFieldValueOrArithStruct *operand, CFieldRefItem refItem, int pFormId, int sFormId, stringstream &qText, int rootLevel)
{		
	switch(operand->tag)
	{
	case AR_FIELD: //Foreign field id
		{	
			tmpFieldId = operand->u.fieldId;								
			tmpFormId = sFormId;

			qText << "'" << arIn->LinkToField(sFormId, operand->u.fieldId, rootLevel) << "'";				

			if(!arIn->FieldreferenceExists(sFormId, operand->u.fieldId, refItem))
			{
				CFieldRefItem *item = new CFieldRefItem();
				item->arsStructItemType = refItem.arsStructItemType;
				item->description = refItem.description;
				item->fromName = refItem.fromName;					
				item->fieldInsideId = operand->u.fieldId;					
				item->schemaInsideId = sFormId;
				arIn->AddReferenceItem(item);
				delete item;
			}
		}
		break;	
	case AR_FIELD_TRAN:
		{								
			tmpFieldId = operand->u.fieldId;								
			tmpFormId = pFormId;

			qText << "'TR." << arIn->LinkToField(pFormId, operand->u.fieldId, rootLevel) << "'";		

			if(!arIn->FieldreferenceExists(pFormId, operand->u.fieldId, refItem))
			{
				CFieldRefItem *item = new CFieldRefItem();
				item->arsStructItemType = refItem.arsStructItemType;
				item->description = refItem.description;
				item->fromName = refItem.fromName;					
				item->fieldInsideId = operand->u.fieldId;					
				item->schemaInsideId = pFormId;
				arIn->AddReferenceItem(item);
				delete item;
			}
		}
		break;	
	case AR_FIELD_DB:
		{
			tmpFieldId = operand->u.fieldId;								
			tmpFormId = pFormId;

			qText << "'DB." << arIn->LinkToField(pFormId, operand->u.fieldId, rootLevel) << "'";				

			if(!arIn->FieldreferenceExists(pFormId, operand->u.fieldId, refItem))
			{
				CFieldRefItem *item = new CFieldRefItem();
				item->arsStructItemType = refItem.arsStructItemType;
				item->description = refItem.description;
				item->fromName = refItem.fromName;					
				item->fieldInsideId = operand->u.fieldId;					
				item->schemaInsideId = pFormId;
				arIn->AddReferenceItem(item);
				delete item;
			}
		}
		break;	
	case AR_FIELD_CURRENT:
		{				
			tmpFieldId = operand->u.fieldId;								
			tmpFormId = pFormId;


			if(arsStructItemType == AR_STRUCT_ITEM_XML_ACTIVE_LINK && pFormId == sFormId)
			{
				qText << "$" << arIn->LinkToField(pFormId, operand->u.fieldId, rootLevel) << "$";
			}
			else if(arsStructItemType == AR_STRUCT_ITEM_XML_FILTER && pFormId == sFormId)
			{
				qText << "$" << arIn->LinkToField(pFormId, operand->u.fieldId, rootLevel) << "$";
			}
			else if(arsStructItemType == AR_STRUCT_ITEM_XML_CHAR_MENU && pFormId == sFormId)
			{
				qText << "$" << arIn->LinkToMenuField(pFormId, operand->u.fieldId, rootLevel) << "$";
			}
			else
			{
				if(pFormId != sFormId)
				{
					qText << "$" << arIn->LinkToField(pFormId, operand->u.fieldId, rootLevel) << "$";
				}
				else
				{
					qText << "'" << arIn->LinkToField(pFormId, operand->u.fieldId, rootLevel) << "'";
				}
			}


			if(!arIn->FieldreferenceExists(pFormId, operand->u.fieldId, refItem) && arsStructItemType != AR_STRUCT_ITEM_XML_CHAR_MENU)
			{
				CFieldRefItem *item = new CFieldRefItem();
				item->arsStructItemType = refItem.arsStructItemType;				
				item->description = refItem.description;
				item->fromName = refItem.fromName;					
				item->fieldInsideId = operand->u.fieldId;					
				item->schemaInsideId = pFormId;
				arIn->AddReferenceItem(item);
				delete item;
			}

		}
		break;
	case AR_QUERY:
		{
			qText << "*QUERY*";
		}
		break;
	case AR_VALUE:
		{
			ARValueStruct *data;
			data = &operand->u.value;
			switch(data->dataType)
			{
			case AR_DATA_TYPE_NULL:
				{
					qText << "$NULL$";
				}
				break;
			case AR_DATA_TYPE_KEYWORD:
				{
					qText << "$" << CAREnum::Keyword(data->u.keyNum) << "$";
				}
				break;
			case AR_DATA_TYPE_INTEGER:
				{
					try
					{
						string tmp = arIn->GetFieldEnumValue(tmpFormId, tmpFieldId, data->u.intVal);

						if(strcmp(tmp.c_str(), EmptyValue.c_str())!=0)
							qText << "\"" << tmp << "\"";
						else
							qText << data->u.intVal;
					}
					catch(...)
					{
						cout << "EXCEPTION enumerating enum value" << endl;
					}						
				}
				break;
			case AR_DATA_TYPE_REAL:
				{
					qText << data->u.realVal;						
				}
				break;
			case AR_DATA_TYPE_CHAR:
				{
					qText << "\""<< data->u.charVal << "\"";
				}
				break;
			case AR_DATA_TYPE_ENUM:
				{
					try
					{
						string tmp = arIn->GetFieldEnumValue(tmpFormId, tmpFieldId, data->u.enumVal);

						if(strcmp(tmp.c_str(), EmptyValue.c_str())!=0)
							qText << "\"" << tmp << "\"";
						else
							qText << data->u.intVal;
					}
					catch(...)
					{
						cout << "EXCEPTION enumerating enum value" << endl;
					}
				}					
				break;
			case AR_DATA_TYPE_TIME:
				{
					qText << CUtil::TimeToString(data->u.timeVal);
				}
				break;
			case AR_DATA_TYPE_DECIMAL:
				{
					qText << data->u.decimalVal;
				}
				break;
			default:
				qText << "n/a";
				break;
			}
		}
		break;
	case AR_ARITHMETIC:
		{
			switch (operand->u.arithOp->operation) 
			{
			case AR_ARITH_OP_ADD:
				qText << "(";
				CheckOperand(&operand->u.arithOp->operandLeft, refItem, pFormId, sFormId, qText, rootLevel);
				qText << CAREnum::Operand(AR_ARITH_OP_ADD);
				CheckOperand(&operand->u.arithOp->operandRight, refItem, pFormId, sFormId, qText, rootLevel);
				qText << ")";
				break;
			case AR_ARITH_OP_SUBTRACT:
				qText << "(";
				CheckOperand(&operand->u.arithOp->operandLeft, refItem, pFormId, sFormId, qText, rootLevel);
				qText << CAREnum::Operand(AR_ARITH_OP_SUBTRACT);
				CheckOperand(&operand->u.arithOp->operandRight, refItem, pFormId, sFormId, qText, rootLevel);
				qText << ")";
				break;
			case AR_ARITH_OP_MULTIPLY:
				qText << "(";
				CheckOperand(&operand->u.arithOp->operandLeft, refItem, pFormId, sFormId, qText, rootLevel);
				qText << CAREnum::Operand(AR_ARITH_OP_MULTIPLY);
				CheckOperand(&operand->u.arithOp->operandRight, refItem, pFormId, sFormId, qText, rootLevel);
				qText << ")";
				break;
			case AR_ARITH_OP_DIVIDE:
				qText << "(";
				CheckOperand(&operand->u.arithOp->operandLeft, refItem, pFormId, sFormId, qText, rootLevel);
				qText << CAREnum::Operand(AR_ARITH_OP_DIVIDE);
				CheckOperand(&operand->u.arithOp->operandRight, refItem, pFormId, sFormId, qText, rootLevel);
				qText << ")";
				break;
			case AR_ARITH_OP_MODULO:
				qText << "(";
				CheckOperand(&operand->u.arithOp->operandLeft, refItem, pFormId, sFormId, qText, rootLevel);
				qText << CAREnum::Operand(AR_ARITH_OP_MODULO);
				CheckOperand(&operand->u.arithOp->operandRight, refItem, pFormId, sFormId, qText, rootLevel);
				qText << ")";
				break;
			case AR_ARITH_OP_NEGATE:
				qText << CAREnum::Operand(AR_ARITH_OP_NEGATE);
				CheckOperand(&operand->u.arithOp->operandRight, refItem, pFormId, sFormId, qText, rootLevel);
				break;
			}
		}
		break;
	case AR_STAT_HISTORY:
		{
			qText << "Status History.";

			string tmp = arIn->GetFieldEnumValue(pFormId, 7, operand->u.statHistory.enumVal);								
			if(strcmp(tmp.c_str(), EmptyValue.c_str())!=0)
				qText << tmp;
			else
				qText << operand->u.statHistory.enumVal;

			switch(operand->u.statHistory.userOrTime)
			{
			case AR_STAT_HISTORY_USER:
				{
					qText << ".USER";
				}
				break;
			case AR_STAT_HISTORY_TIME:
				{
					qText << ".TIME";
				}
				break;
			}
		}
		break;
	}
}

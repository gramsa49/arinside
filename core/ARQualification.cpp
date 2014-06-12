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
#include "ARQualification.h"
#include "../doc/DocCurrencyField.h"
#include "../output/URLLink.h"

CARQualification::CARQualification(CARInside &arIn, const CRefItem &referenceItem, int currentFormId, int rootLevel)
: refItem(referenceItem)
{
	this->arIn = &arIn;
	this->tmpFormId = 0;
	arsStructItemType = AR_STRUCT_ITEM_XML_NONE;

	this->primaryFormId = currentFormId;
	this->secondaryFormId = currentFormId;

	this->primaryFormDelimiter = '\'';
	this->secondaryFormDelimiter = '\'';

	this->rootLevel = rootLevel;
}

CARQualification::CARQualification(CARInside &arIn, const CRefItem &referenceItem, int currentFormId, int otherFormId, int rootLevel)
: refItem(referenceItem)
{
	this->arIn = &arIn;
	this->tmpFormId = 0;
	arsStructItemType = AR_STRUCT_ITEM_XML_NONE;

	this->primaryFormId = currentFormId;
	this->secondaryFormId = otherFormId;

	this->primaryFormDelimiter = '$';
	this->secondaryFormDelimiter = '\'';

	this->rootLevel = rootLevel;
}

CARQualification::~CARQualification(void)
{
}


void CARQualification::CheckQuery(const ARQualifierStruct *query, stringstream &qText)
{
	qualLevels.push_back(query);
	
	if (query != NULL)
	{
		switch(query->operation)
		{
		case AR_COND_OP_NONE:
			break;
		case AR_COND_OP_AND:
		case AR_COND_OP_OR:
			{
				if (query->u.andor.operandLeft->operation != query->operation && query->u.andor.operandLeft->operation != AR_COND_OP_REL_OP) qText << "(";
				CheckQuery(query->u.andor.operandLeft, qText);
				if (query->u.andor.operandLeft->operation != query->operation && query->u.andor.operandLeft->operation != AR_COND_OP_REL_OP) qText << ")";

				switch (query->operation)
				{
				case AR_COND_OP_AND: qText << " AND "; break;
				case AR_COND_OP_OR: qText << " OR "; break;
				}	

				if (query->u.andor.operandRight->operation != query->operation && query->u.andor.operandRight->operation != AR_COND_OP_REL_OP) qText << "(";
				CheckQuery(query->u.andor.operandRight, qText);
				if (query->u.andor.operandRight->operation != query->operation && query->u.andor.operandRight->operation != AR_COND_OP_REL_OP) qText << ")";
			}
			break;
		case AR_COND_OP_NOT:
			qText << "NOT ";
			if(query->u.notQual != NULL)
			{
				if (query->u.notQual->operation != AR_COND_OP_REL_OP) qText << "(";
				CheckQuery(query->u.notQual, qText);
				if (query->u.notQual->operation != AR_COND_OP_REL_OP) qText << ")"; 
			}
			break;
		case AR_COND_OP_REL_OP:
			CheckOperand(&query->u.relOp->operandLeft, NULL, qText);
			switch (query->u.relOp->operation) 
			{		
			case AR_REL_OP_EQUAL:
				qText << " = ";
				break;
			case AR_REL_OP_GREATER:
				qText << " > ";
				break;
			case AR_REL_OP_GREATER_EQUAL:
				qText << " >= ";
				break;
			case AR_REL_OP_LESS:
				qText << " < ";
				break;
			case AR_REL_OP_LESS_EQUAL:
				qText << " <= ";
				break;
			case AR_REL_OP_NOT_EQUAL:
				qText << " != ";
				break;
			case AR_REL_OP_LIKE:
				qText << " LIKE ";
				break;
			}
			CheckOperand(&query->u.relOp->operandRight, NULL, qText);
			break;
		case AR_COND_OP_FROM_FIELD: //A qualification located in a field on the form.
			qText << "EXTERNAL(" << primaryFormDelimiter << arIn->LinkToField(primaryFormId, query->u.fieldId, rootLevel) << primaryFormDelimiter << ")";

			arIn->AddFieldReference(primaryFormId, query->u.fieldId, refItem);
			break;
		}
	}
	qualLevels.pop_back();
}

void CARQualification::CheckOperand(ARFieldValueOrArithStruct *operand, ARFieldValueOrArithStruct *parent, stringstream &qText)
{		
	switch(operand->tag)
	{
	case AR_FIELD: //Foreign field id
		tmpFormId = secondaryFormId;

		qText << primaryFormDelimiter << arIn->LinkToField(secondaryFormId, operand->u.fieldId, rootLevel) << primaryFormDelimiter;

		if(!arIn->FieldreferenceExists(secondaryFormId, operand->u.fieldId, refItem))
		{
			arIn->AddFieldReference(secondaryFormId, operand->u.fieldId, refItem);
		}
		break;	
	case AR_FIELD_TRAN:
		tmpFormId = primaryFormId;

		qText << primaryFormDelimiter << "TR." << arIn->LinkToField(primaryFormId, operand->u.fieldId, rootLevel) << primaryFormDelimiter;

		if(!arIn->FieldreferenceExists(primaryFormId, operand->u.fieldId, refItem))
		{
			arIn->AddFieldReference(primaryFormId, operand->u.fieldId, refItem);
		}
		break;	
	case AR_FIELD_DB:
		tmpFormId = primaryFormId;

		qText << primaryFormDelimiter << "DB." << arIn->LinkToField(primaryFormId, operand->u.fieldId, rootLevel) << primaryFormDelimiter;

		if(!arIn->FieldreferenceExists(primaryFormId, operand->u.fieldId, refItem))
		{
			arIn->AddFieldReference(primaryFormId, operand->u.fieldId, refItem);
		}
		break;	
	case AR_FIELD_CURRENT:
		tmpFormId = primaryFormId;

		if(arsStructItemType == AR_STRUCT_ITEM_XML_ACTIVE_LINK && primaryFormId == secondaryFormId)
			qText << primaryFormDelimiter << arIn->LinkToField(primaryFormId, operand->u.fieldId, rootLevel) << primaryFormDelimiter;
		else if(arsStructItemType == AR_STRUCT_ITEM_XML_FILTER && primaryFormId == secondaryFormId)
			qText << primaryFormDelimiter << arIn->LinkToField(primaryFormId, operand->u.fieldId, rootLevel) << primaryFormDelimiter;
		else if(arsStructItemType == AR_STRUCT_ITEM_XML_CHAR_MENU && primaryFormId == secondaryFormId)
			qText << primaryFormDelimiter << arIn->LinkToMenuField(primaryFormId, operand->u.fieldId, rootLevel) << primaryFormDelimiter;
		else
		{
			if (primaryFormId != secondaryFormId)
				qText << primaryFormDelimiter << arIn->LinkToField(primaryFormId, operand->u.fieldId, rootLevel) << primaryFormDelimiter;
			else
				qText << primaryFormDelimiter << arIn->LinkToField(primaryFormId, operand->u.fieldId, rootLevel) << primaryFormDelimiter;
		}

		if(arsStructItemType != AR_STRUCT_ITEM_XML_CHAR_MENU && !arIn->FieldreferenceExists(primaryFormId, operand->u.fieldId, refItem))
		{
			arIn->AddFieldReference(primaryFormId, operand->u.fieldId, refItem);
		}

		break;
	case AR_QUERY:
		qText << "*QUERY*";
		break;
	case AR_VALUE:
		ARValueStruct *data;
		data = &operand->u.value;
		switch(data->dataType)
		{
		case AR_DATA_TYPE_NULL:
			qText << "$NULL$";
			break;
		case AR_DATA_TYPE_KEYWORD:
			qText << "$" << CAREnum::Keyword(data->u.keyNum) << "$";
			break;
		case AR_DATA_TYPE_INTEGER:
		case AR_DATA_TYPE_ENUM:
			try
			{
				int tmpFieldId = FindCurrentEnumFieldId();
				string tmp = arIn->GetFieldEnumValue(tmpFormId, tmpFieldId, data->u.intVal);

				if(!tmp.empty())
					qText << "\"" << tmp << "\"";
				else
					qText << data->u.intVal;
			}
			catch(exception& e)
			{
				cout << "EXCEPTION enumerating enum value: " << e.what() << endl;
			}						
			break;
		case AR_DATA_TYPE_REAL:
			qText << data->u.realVal;						
			break;
		case AR_DATA_TYPE_CHAR:
			qText << "\""<< data->u.charVal << "\"";
			break;
		case AR_DATA_TYPE_DIARY:
			qText << "\""<< data->u.diaryVal << "\"";
			break;
		case AR_DATA_TYPE_TIME:
			qText << "\"" << CUtil::DateTimeToHTMLString(data->u.timeVal) << "\"";
			break;
		case AR_DATA_TYPE_DECIMAL:
			qText << data->u.decimalVal;
			break;
		case AR_DATA_TYPE_ATTACH:
			qText << data->u.attachVal;
			break;
		case AR_DATA_TYPE_CURRENCY:
			qText << data->u.currencyVal;
			break;
		case AR_DATA_TYPE_DATE:
			qText << "\"" << CUtil::DateToString(data->u.dateVal) << "\"";
			break;
		case AR_DATA_TYPE_TIME_OF_DAY:
			qText << "\"" << CUtil::TimeOfDayToString(data->u.timeOfDayVal) << "\"";
			break;
		default:
			qText << "n/a";
			break;
		}
		break;
	case AR_ARITHMETIC:
	{
		// Rule 1: 
		// if the parent has a higher precedence (in this case a lower value), we have to add parentheses
		// Example 1: 2 * (3 + 4)      Original: (2 * (3 + 4))
		// Example 2: 2 + 3 * 4        Original: (2 + (3 * 4))
		// Example 3: (2 + 3) * 4      Original: ((2 + 3) * 4)
		//
		// Rule 2: 
		// if the parent and the current operand have the same precedence (mul, div and modulo use the
		// same, at least in C) and the current modulo operation is at the right side of the parent
		// operand, only then parentheses are needed. (If the modulo is on the left side, the operation
		// is executed from left to right and doesn't need any parentheses.)
		// Example 1: 2 * (3 mod 4)    Original: (2 * (3 mod 4))
		// Example 2: 2 * 3 mod 4      Original: ((2 * 3) mod 4)
		// Example 3: 2 mod 3 * 4      Original: ((2 mod 3) * 4)
		// Example 4: 2 mod (3 * 4)    Original: (2 mod (3 * 4))

		bool addBracket = false;
		if (parent != NULL && parent->tag == operand->tag)
		{
			unsigned int parentPrecedence = CAREnum::OperandPrecedence(parent->u.arithOp->operation);
			unsigned int currentPrecedence = CAREnum::OperandPrecedence(operand->u.arithOp->operation);

			if (parentPrecedence < currentPrecedence || operand->u.arithOp->operation == AR_ARITH_OP_MODULO &&
			    parentPrecedence == currentPrecedence && &parent->u.arithOp->operandRight == operand)
				addBracket = true;
		}

		switch (operand->u.arithOp->operation) 
		{
		case AR_ARITH_OP_ADD:
			if (addBracket) qText << "(";
			CheckOperand(&operand->u.arithOp->operandLeft, operand, qText);
			qText << CAREnum::Operand(AR_ARITH_OP_ADD);
			CheckOperand(&operand->u.arithOp->operandRight, operand, qText);
			if (addBracket) qText << ")";
			break;
		case AR_ARITH_OP_SUBTRACT:
			if (addBracket) qText << "(";
			CheckOperand(&operand->u.arithOp->operandLeft, operand, qText);
			qText << CAREnum::Operand(AR_ARITH_OP_SUBTRACT);
			CheckOperand(&operand->u.arithOp->operandRight, operand, qText);
			if (addBracket) qText << ")";
			break;
		case AR_ARITH_OP_MULTIPLY:
			if (addBracket) qText << "(";
			CheckOperand(&operand->u.arithOp->operandLeft, operand, qText);
			qText << CAREnum::Operand(AR_ARITH_OP_MULTIPLY);
			CheckOperand(&operand->u.arithOp->operandRight, operand, qText);
			if (addBracket) qText << ")";
			break;
		case AR_ARITH_OP_DIVIDE:
			if (addBracket) qText << "(";
			CheckOperand(&operand->u.arithOp->operandLeft, operand, qText);
			qText << CAREnum::Operand(AR_ARITH_OP_DIVIDE);
			CheckOperand(&operand->u.arithOp->operandRight, operand, qText);
			if (addBracket) qText << ")";
			break;
		case AR_ARITH_OP_MODULO:
			if (addBracket) qText << "(";
			CheckOperand(&operand->u.arithOp->operandLeft, operand, qText);
			qText << CAREnum::Operand(AR_ARITH_OP_MODULO);
			CheckOperand(&operand->u.arithOp->operandRight, operand, qText);
			if (addBracket) qText << ")";
			break;
		case AR_ARITH_OP_NEGATE:
			qText << CAREnum::Operand(AR_ARITH_OP_NEGATE);
			CheckOperand(&operand->u.arithOp->operandRight, operand, qText);
			break;
		}
		break;
	}
	case AR_STAT_HISTORY:
		{
			qText << "'" << arIn->LinkToField(primaryFormId, 15, rootLevel) << ".";

			if(!arIn->FieldreferenceExists(primaryFormId, 15, refItem))
			{
				arIn->AddFieldReference(primaryFormId, 15, refItem);
			}

			string tmp = arIn->GetFieldEnumValue(primaryFormId, 7, operand->u.statHistory.enumVal);								
			if(!tmp.empty())
				qText << tmp;
			else
				qText << operand->u.statHistory.enumVal;

			switch(operand->u.statHistory.userOrTime)
			{
			case AR_STAT_HISTORY_USER:
				qText << ".USER";
				break;
			case AR_STAT_HISTORY_TIME:
				qText << ".TIME";
				break;
			}
			qText << "'";
		}
		break;
	case AR_CURRENCY_FLD:
	case AR_CURRENCY_FLD_DB:
	case AR_CURRENCY_FLD_TRAN:
		{
			CDocCurrencyField docCurrency(primaryFormId, *operand->u.currencyField);
			char *prefix = getFieldPrefix(operand);
			
			qText << primaryFormDelimiter;
			if (prefix != NULL) qText << prefix;
			docCurrency.GetResolvedAndLinkedField(qText, refItem, rootLevel);
			qText << primaryFormDelimiter;
		}
		break;
	}
}

int CARQualification::FindCurrentEnumFieldId()
{
	int pos = (int)qualLevels.size() - 1;
	
	for (; pos > -1; --pos)
	{
		const ARQualifierStruct* current = qualLevels[pos];
		if (current->operation == AR_COND_OP_REL_OP)
		{
			// normally all relOps have two operators. check if there is a field on one side
			switch (current->u.relOp->operandLeft.tag)
			{
			case AR_FIELD:
				tmpFormId = secondaryFormId;
				return current->u.relOp->operandLeft.u.fieldId;
			case AR_FIELD_TRAN:
			case AR_FIELD_DB:
			case AR_FIELD_CURRENT:
				tmpFormId = primaryFormId;
				return current->u.relOp->operandLeft.u.fieldId;
			}

			switch (current->u.relOp->operandRight.tag)
			{
			case AR_FIELD:
				tmpFormId = secondaryFormId;
				return current->u.relOp->operandRight.u.fieldId;
			case AR_FIELD_TRAN:
			case AR_FIELD_DB:
			case AR_FIELD_CURRENT:
				tmpFormId = primaryFormId;
				return current->u.relOp->operandRight.u.fieldId;
			}

			// if there is a relOp without a field, this can't be a enum!
			return -1;
		}
	}

	return -1;
}

char* CARQualification::getFieldPrefix(ARFieldValueOrArithStruct *operand)
{
	if (operand == NULL) return NULL;
	switch (operand->tag)
	{
	case AR_FIELD_TRAN:
	case AR_CURRENCY_FLD_TRAN:
		return "TR.";
	case AR_FIELD_DB:
	case AR_CURRENCY_FLD_DB:
		return "DB.";
	}
	return NULL;
}

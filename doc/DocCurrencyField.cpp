//Copyright (C) 2014 John Lutghers | jls17
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
//    along with ARInside.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "../ARInside.h"
#include "../output/URLLink.h"
#include "DocCurrencyField.h"

CDocCurrencyField::CDocCurrencyField(int schemaInsideId, const ARCurrencyPartStruct &field)
: currencyField(field)
{
	schemaId = schemaInsideId;
}

void CDocCurrencyField::GetResolvedAndLinkedField(std::ostream &strm, const CRefItem &refItem, int rootLevel)
{
	CARField currencyFieldObj(schemaId, currencyField.fieldId);
	CARInside* arIn = CARInside::GetInstance();

	if (currencyFieldObj.Exists())
	{
		strm << URLLink(currencyFieldObj, rootLevel);
		arIn->AddFieldReference(schemaId, currencyField.fieldId, refItem);
	}
	else
		strm << currencyField.fieldId;  // TODO: missing fields are normally maked red (see CARInside::LinkToField)

	unsigned int currencyFieldPart = currencyField.partTag;
	if (currencyFieldPart != AR_CURRENCY_PART_FIELD)
	{
		strm << ".";
		switch (currencyFieldPart)
		{
		case AR_CURRENCY_PART_VALUE:
		case AR_CURRENCY_PART_TYPE:
		case AR_CURRENCY_PART_DATE:
			strm << CAREnum::CurrencyPart(currencyFieldPart);
			break;
		case AR_CURRENCY_PART_FUNCTIONAL:
			strm << currencyField.currencyCode;
			break;
		}
	}
}

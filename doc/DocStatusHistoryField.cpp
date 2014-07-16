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
#include "DocStatusHistoryField.h"
#include "../ARInside.h"
#include "../output/URLLink.h"

CDocStatusHistoryField::CDocStatusHistoryField(int schemaInsideId, const ARStatHistoryValue& statHistValue)
: schemaId(schemaInsideId), statusHistory(statHistValue)
{
}

void CDocStatusHistoryField::GetResolvedAndLinkedField(std::ostream &strm, const CRefItem &refItem, int rootLevel)
{
	const int fieldId = AR_CORE_STATUS_HISTORY;

	CARField statHistoryField(schemaId, fieldId);
	CARInside* arIn = CARInside::GetInstance();

	if (statHistoryField.Exists())
	{
		strm << URLLink(statHistoryField, rootLevel);
		arIn->AddFieldReference(schemaId, fieldId, refItem);
	}
	else
		strm << fieldId;  // TODO: missing fields are normally maked red (see CARInside::LinkToField)

	int enumId = statusHistory.enumVal;
	string enumValue = arIn->GetFieldEnumValue(schemaId, 7, enumId);

	strm << ".";
	if (enumValue.empty())
		strm << enumId;
	else
		strm << enumValue;

	strm << ".";
	strm << CAREnum::StatHistoryTag(statusHistory.userOrTime);
}

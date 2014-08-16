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
#include "OpenWindowHelper.h"

CDocActionOpenWindowHelper::CDocActionOpenWindowHelper(char* reportString)
{
	isValid = false;
	isParsed = false;
	this->reportString = reportString;

	reportType_Start = NULL;
	reportType_End = NULL;
	reportLocation_Start = NULL;
	reportLocation_End = NULL;
	reportName_Start = NULL;
	reportName_End = NULL;
	reportDestination_Start = NULL;
	//reportDestination_End = NULL;
	entryIds_Start = NULL;
	entryIds_End = NULL;
	queryOverride_Start = NULL;
	queryOverride_End = NULL;
	reportOperation_Start = NULL;
	reportOperation_End = NULL;
	charEncoding_Start = NULL;
	charEncoding_End = NULL;
	inlineForm_Start = NULL;
	inlineForm_End = NULL;
}

CDocActionOpenWindowHelper::~CDocActionOpenWindowHelper()
{
}

bool CDocActionOpenWindowHelper::ParseReportData()
{
	isParsed = true;
	if (reportString == NULL) return false;

	bool found = false;
	char *curScanPos = strstr(reportString, "Report: \1");	
	if (curScanPos == NULL) return false;
	curScanPos += 9;

	while (curScanPos[0] != 0)
	{
		if (curScanPos[0] == 0x0A)
		{
			++curScanPos;
			reportDestination_Start = curScanPos;
			return true;
		}

		// search for "=" character (the start of the value follows)
		int reportAttribute = atoi(curScanPos);
		curScanPos = strchr(curScanPos,'='); 
		if (curScanPos == NULL) return found;
		++curScanPos;

		// now search for \1, this marks the end of the value
		const char* pos = strchr(curScanPos,'\1');
		if (pos == NULL) return found;
		size_t len = (size_t)(pos - curScanPos);

		const char **start = NULL;
		const char **end = NULL;

		switch (reportAttribute)
		{
		case AR_REPORT_ATTR_TYPE:
			start = &reportType_Start;
			end = &reportType_End;
			break;
		case AR_REPORT_ATTR_FILENAME:
			start = &reportName_Start;
			end = &reportName_End;
			break;
		case AR_REPORT_ATTR_LOCATION:
			start = &reportLocation_Start;
			end = &reportLocation_End;
			break;
		case AR_REPORT_ATTR_CHAR_ENCODING:
			start = &charEncoding_Start;
			end = &charEncoding_End;
			break;
		case AR_REPORT_ATTR_ENTRYIDS:
			start = &entryIds_Start;
			end = &entryIds_End;
			break;
		case AR_REPORT_ATTR_QUERY_OVERRIDE:
			start = &queryOverride_Start;
			end = &queryOverride_End;
			break;
		case AR_REPORT_ATTR_OPERATION:
			start = &reportOperation_Start;
			end = &reportOperation_End;
			break;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
		case AR_REPORT_ATTR_INLINE_FORM:
			start = &inlineForm_Start;
			end = &inlineForm_End;
			break;
#endif
		}

		if (start != NULL && end != NULL)
		{
			*start = curScanPos;
			*end = pos;
		}
		curScanPos += len + 1;
	}

	return false;
}

void CDocActionOpenWindowHelper::CheckAlreadyParsed()
{
	if (!isParsed)
		isValid = ParseReportData();
}

bool CDocActionOpenWindowHelper::IsValid()
{
	CheckAlreadyParsed();
	return isValid;
}

string CDocActionOpenWindowHelper::getReportType()
{
	CheckAlreadyParsed();
	if (!isValid) return "";

	return string(reportType_Start, reportType_End);
}

string CDocActionOpenWindowHelper::getReportLocation()
{
	CheckAlreadyParsed();
	if (!isValid) return "";

	return string(reportLocation_Start, reportLocation_End);
}

string CDocActionOpenWindowHelper::getReportName()
{
	CheckAlreadyParsed();
	if (!isValid) return "";

	return string(reportName_Start, reportName_End);
}

string CDocActionOpenWindowHelper::getReportDestination()
{
	CheckAlreadyParsed();
	if (!isValid) return "";

	return reportDestination_Start;
}

string CDocActionOpenWindowHelper::getEntryIds()
{
	CheckAlreadyParsed();
	if (!isValid) return "";

	return string(entryIds_Start, entryIds_End);
}

string CDocActionOpenWindowHelper::getQueryOverride()
{
	CheckAlreadyParsed();
	if (!isValid) return "";

	return string(queryOverride_Start, queryOverride_End);
}

string CDocActionOpenWindowHelper::getReportOperation()
{
	CheckAlreadyParsed();
	if (!isValid) return "";

	return string(reportOperation_Start, reportOperation_End);
}

string CDocActionOpenWindowHelper::getCharEncoding()
{
	CheckAlreadyParsed();
	if (!isValid) return "";

	return string(charEncoding_Start, charEncoding_End);
}

string CDocActionOpenWindowHelper::getInlineForm()
{
	CheckAlreadyParsed();
	if (!isValid) return "";

	return string(inlineForm_Start, inlineForm_End);
}
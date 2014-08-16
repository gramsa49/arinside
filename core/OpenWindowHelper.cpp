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

bool CDocActionOpenWindowHelper::GetSampleData(CARActiveLink& actLink, IfElseState ifElse, int nAction, string &server, string &schema)
{
	// ok, this is a very ugly design of the ARS api. The sample data of
	// the action is stored within the object properties of the active-
	// link. that wouldnt be that bad if we just need to read the data
	// from there, but the data is something like an API structure and
	// there is no ARDecode*** function. So we need to implement our
	// own decode routine.

	const ARPropList &propList = actLink.GetPropList();

	// first, decide which property we are looking for
	unsigned int PropID = (ifElse == IES_IF ? AR_OPROP_WINDOW_OPEN_IF_SAMPLE_SERVER_SCHEMA : AR_OPROP_WINDOW_OPEN_ELSE_SAMPLE_SERVER_SCHEMA);

	// now find the index of the object property
	int propIndex = -1;
	for (unsigned int k=0; k < propList.numItems; ++k)
	{
		if (propList.props[k].prop == PropID)
		{
			propIndex = k;
			break;
		}
	}

	if (propIndex > -1 && propList.props[propIndex].value.dataType == AR_DATA_TYPE_CHAR)
	{
		// the property is found and has correct data type
		char* propData = propList.props[propIndex].value.u.charVal;
		unsigned int chrIndex = 0;	// this holds the current position within propData
		
		int listCount = atoi(propData);	// retrieve lists item count
		while (propData[chrIndex] != '\\' && propData[chrIndex] != 0) ++chrIndex;
		if (listCount > 0 && propData[chrIndex] != 0)
		{
			++chrIndex; // skip last "\"
			// now we need to check each entry of the list against this actions index
			for (int listIndex = 0; listIndex < listCount; ++listIndex)
			{
				int propActionIndex = atoi(&propData[chrIndex]);
				while (propData[chrIndex] != '\\' && propData[chrIndex] != 0) ++chrIndex;
				if (propData[chrIndex] == '\\') ++chrIndex;

				if (propActionIndex == nAction && propData[chrIndex] != 0)
				{
					// match! found the action we were looking for!
					unsigned int strStart = chrIndex;
					while (propData[chrIndex] != '\\' && propData[chrIndex] != 0) ++chrIndex;
					if (propData[chrIndex] == 0) break;

					unsigned int strLen = chrIndex - strStart;
					server.resize(strLen);
					strncpy(&server[0], &propData[strStart], chrIndex - strStart);

					++chrIndex; // skip last "\"
					strStart = chrIndex;
					while (propData[chrIndex] != '\\' && propData[chrIndex] != 0) ++chrIndex;
					//if (propData[chrIndex] == 0) break;

					strLen = chrIndex - strStart;
					schema.resize(strLen);
					strncpy(&schema[0], &propData[strStart], chrIndex - strStart);
					return true;	// we are done
				}

				// skip next two data entries now
				for (int tmpDataCount = 0; tmpDataCount < 2; ++tmpDataCount)
				{
					while(propData[chrIndex] != '\\' && propData[chrIndex] != 0) ++chrIndex;
					if (propData[chrIndex] == 0) break;
					++chrIndex; // skip last "\"
				}
				if (propData[chrIndex] == 0) break;
			}
		}
	}
	return false;
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
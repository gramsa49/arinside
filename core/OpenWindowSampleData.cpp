//Copyright (C) 2014 John Luthgers | jls17
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
#include "OpenWindowSampleData.h"

bool OpenWindowSampleData::GetSampleData(CARActiveLink& actLink, IfElseState ifElse, int nAction, string &server, string &schema)
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

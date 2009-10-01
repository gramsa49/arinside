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
#include "../ARInside.h"
#include "ARProplistHelper.h"

CARProplistHelper::CARProplistHelper(ARPropList* propList)
{
	if (propList == NULL) return;

	try
	{
		for (unsigned int i=0; i < propList->numItems; ++i)
		{
			properties.push_back(PropHelpData(propList->props[i].prop, &propList->props[i].value));
		}
		sort(properties.begin(), properties.end());
	}
	catch (...)
	{
		cerr << "EXCEPTION in constructing CARProplistHelper" << endl;
		throw;
	}
}

CARProplistHelper::~CARProplistHelper(void)
{
}

string CARProplistHelper::GetLabel(ARULong32 nProp)
{
	return CAREnum::FieldPropertiesLabel(nProp);
}

string CARProplistHelper::GetValue(ARULong32 nProp, ARValueStruct &arV)
{
	stringstream strmValue;
	strmValue.str("");

	switch(arV.dataType)
	{
	case AR_DATA_TYPE_NULL:
		{
			strmValue << "NULL";
		}
		break;
	case AR_DATA_TYPE_CHAR:
		{
			if (nProp == AR_DPROP_ENUM_LABELS){
				string enumLabels = arV.u.charVal;
				//find first separator
				size_t next=enumLabels.find('\\');
				//get the total entries off the front of the string
				int numItems = atoi(enumLabels.substr(0,next).c_str());
				//clear the number of entries off the front of the list
				enumLabels = enumLabels.substr(next+1);
				for(int i=0; i < numItems; i++)
				{
					//determine size of text
					next=enumLabels.find('\\');
					//add enumerated number to list
					strmValue << enumLabels.substr(0,next) << " - ";
					//get rid of enumerated number
					enumLabels = enumLabels.substr(next+1);
					//find next separator
					next=enumLabels.find('\\');
					//add enumerated value to list
					strmValue << enumLabels.substr(0,next) << "<BR/>";
					//get rid of enumerated value
					enumLabels = enumLabels.substr(next+1);
				}
			}
			else
			{
				if(arV.u.charVal != NULL)
					strmValue << CWebUtil::Validate( arV.u.charVal );	
			}
		}
		break;
	case AR_DATA_TYPE_INTEGER:
		{	
			strmValue << CAREnum::FieldPropertiesValue(nProp, arV.u.intVal);
			if(strmValue.str() == "Unknown")
			{
				strmValue.str("");
				strmValue << arV.u.intVal;
			}	
		}
		break;
	case AR_DATA_TYPE_DATE:
		{
			strmValue << arV.u.dateVal;
		}
		break;
	case AR_DATA_TYPE_DECIMAL:
		{
			if(arV.u.decimalVal != NULL)
			{
				strmValue << arV.u.decimalVal;
			}
		}
		break;
	case AR_DATA_TYPE_REAL:
		{	
			strmValue << arV.u.realVal;
		}
		break;
	case AR_DATA_TYPE_ULONG:
		{
			if(nProp == AR_OPROP_SCC_TIMESTAMP)
			{
				strmValue.str("");
				strmValue << CUtil::DateTimeToHTMLString(arV.u.ulongVal);
			}
			else
			{
				strmValue << CAREnum::FieldPropertiesValue(nProp, arV.u.ulongVal);

				if(strmValue.str() == "Unknown")
				{
					strmValue.str("");
					strmValue << arV.u.ulongVal;
				}
			}
		}
		break;
	case AR_DATA_TYPE_TIME:
		{
			strmValue << arV.u.timeVal;
		}
		break;
	case AR_DATA_TYPE_ENUM:
		{
			strmValue << CAREnum::FieldPropertiesValue(nProp, arV.u.enumVal);

			if(strmValue.str() == "Unknown")
			{
				strmValue.str("");
				strmValue << arV.u.enumVal;
			}
		}
		break;
	case AR_DATA_TYPE_COORDS:
		{				
			if(arV.u.coordListVal != NULL)
			{
				if(arV.u.coordListVal[0].coords != NULL)
				{
					strmValue << "X: " << arV.u.coordListVal[0].coords->x << " Y: " << arV.u.coordListVal[0].coords->y << "<br/>" << endl;
				}
				else
				{
					strmValue << "Error enumverating coordinates" << endl;
				}
			}
		}
		break;
	case AR_DATA_TYPE_BITMASK:
		{
			strmValue << CAREnum::FieldPropertiesValue(nProp, arV.u.maskVal);				
		}
		break;
	}

	return strmValue.str();
}


string CARProplistHelper::GetList(ARPropList &objPropList)
{
	stringstream strm;
	strm.str("");

	try
	{
		CTable tbl("displayPropList", "TblObjectList");
		tbl.AddColumn(20, "Description");
		tbl.AddColumn(80, "Values");

		for(unsigned int i=0; i< objPropList.numItems; i++)
		{					
			CTableRow row("");			
			row.AddCell(CTableCell(CARProplistHelper::GetLabel(objPropList.props[i].prop)));
			row.AddCell(CTableCell(CARProplistHelper::GetValue(objPropList.props[i].prop, objPropList.props[i].value)));		
			tbl.AddRow(row);			
		}

		tbl.description = "Object Properties:";
		strm << tbl.ToXHtml();

	}
	catch(...)
	{
		cout << "EXCEPTION enumerating object properties" << endl;
	}

	return strm.str();
}

ARValueStruct* CARProplistHelper::GetAndUseValue(ARULong32 nProp)
{
	vector<PropHelpData>::iterator propIter = 
		  lower_bound(properties.begin(), properties.end(), nProp);

	if (propIter != properties.end() && !(*propIter < nProp))
	{
		(*propIter).isUsed = true;
		return (*propIter).Value;
	}
	else
	{
		return NULL;
	}
}

ARValueStruct* CARProplistHelper::GetValue(ARULong32 nProp)
{
	vector<PropHelpData>::iterator propIter = 
		  lower_bound(properties.begin(), properties.end(), nProp);

	if (propIter != properties.end() && !(*propIter < nProp))
	{
		return (*propIter).Value;
	}
	else
	{
		return NULL;
	}
}

string CARProplistHelper::UnusedPropertiesToHTML()
{
	stringstream strm;
	strm.str("");

	try
	{
		CTable tbl("displayPropList", "TblObjectList");
		tbl.AddColumn(20, "Description");
		tbl.AddColumn(80, "Values");

		vector<PropHelpData>::iterator propIter = properties.begin();
		vector<PropHelpData>::iterator propEnd = properties.end();

		
		for ( ; propIter != propEnd; ++propIter)
		{
			if ((*propIter).isUsed == false)
			{
				CTableRow row("");			
				row.AddCell(CTableCell(CARProplistHelper::GetLabel((*propIter).pId)));
				row.AddCell(CTableCell(CARProplistHelper::GetValue((*propIter).pId,*(*propIter).Value)));		
				tbl.AddRow(row);
			}
		}

		tbl.description = "Object Properties:";
		strm << tbl.ToXHtml();

	}
	catch(...)
	{
		cout << "EXCEPTION enumerating unused object properties" << endl;
	}

	return strm.str();
}

CARProplistHelper::PropHelpData::PropHelpData(void)
{
	pId = NULL;
	Value = NULL;
	isUsed = false;
}

CARProplistHelper::PropHelpData::PropHelpData(ARULong32 propId, ARValueStruct *value)
{
	pId = propId;
	Value = value;
	isUsed = false;
}

CARProplistHelper::PropHelpData::~PropHelpData()
{
}


/****************************************************************************** 
 * 
 *  file:  ARProplistHelper.cpp
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
#include ".\arproplisthelper.h"

CARProplistHelper::CARProplistHelper(void)
{
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
				strmValue << CUtil::TimeToString(arV.u.ulongVal);
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


string CARProplistHelper::GetList(CARInside &arIn, ARPropList &objPropList)
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
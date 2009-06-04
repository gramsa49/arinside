
/****************************************************************************** 
 * 
 *  file:  ARValue.cpp
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
#include ".\arvalue.h"

CARValue::CARValue()
{	

}

CARValue::~CARValue(void)
{
}

string CARValue::ValueToString(ARValueStruct &value)
{
	switch(value.dataType)
	{
		case AR_DATA_TYPE_NULL:
		{
			return "NULL";
		}
		break;
		case AR_DATA_TYPE_KEYWORD:
		{
			return CAREnum::Keyword(value.u.keyNum);
		}
		break;
		case AR_DATA_TYPE_ATTACH:
		{		
			if(value.u.attachVal != NULL
				&& value.u.attachVal->name != NULL)
			{
				return value.u.attachVal->name;	
			}
		}
		break;
		case AR_DATA_TYPE_CHAR:
		{
			if(value.u.charVal != NULL)
			{
				return value.u.charVal;	
			}
			else
				return "";
		}
		break;
		case AR_DATA_TYPE_CURRENCY:
		{
			if(value.u.currencyVal != NULL)
			{	
				stringstream strm;
				strm.str("");
				
				strm << "Code: ";
				if(value.u.currencyVal->currencyCode != NULL)
				{
					strm << value.u.currencyVal->currencyCode;
				}
				
				if(value.u.currencyVal->value != NULL)
				{
					strm << " Value: " << value.u.currencyVal->value;
				}

				return strm.str();
			}
		}
		break;
		case AR_DATA_TYPE_DATE:
		{
			if(value.u.dateVal != NULL)
			{
				stringstream strm;
				strm.str("");
				strm << value.u.dateVal;

				return strm.str();
			}
			else
				return "0";
		}
		break;
		case AR_DATA_TYPE_DECIMAL:
		{
			if(value.u.decimalVal != NULL)
			{
				stringstream strm;
				strm.str("");
				strm << value.u.decimalVal;

				return strm.str();
			}
			else
				return "0.0";
		}
		break;
		case AR_DATA_TYPE_DIARY:
		{
			if(value.u.diaryVal != NULL)
			{
				stringstream strm;
				strm.str("");
				strm << value.u.diaryVal;
				
				return strm.str();
			}
			else
				return "";
		}
		break;	
		case AR_DATA_TYPE_ENUM:
		{
			
			stringstream strm;
			strm.str("");
			strm << value.u.enumVal;
			return strm.str();
			
		}
		break;
		case AR_DATA_TYPE_INTEGER:
		{
			stringstream strm;
			strm.str("");

			strm << value.u.intVal;
			return strm.str();
		}
		break;
		case AR_DATA_TYPE_REAL:
		{
			stringstream strm;
			strm.str("");

			strm <<value.u.realVal;
			return strm.str();
		}
		break;
		case AR_DATA_TYPE_BYTES:
		{
			return "Bytelist";
		}
		break;
		case AR_DATA_TYPE_ULONG:
		{
			stringstream strm;
			strm.str("");
			strm << value.u.ulongVal;
			
			return strm.str();
		}
		break;
	}

	return "";
}
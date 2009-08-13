
/****************************************************************************** 
 * 
 *  file:  ARServerInfo.cpp
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
#include ".\arserverinfo.h"

CARServerInfo::CARServerInfo(ARControlStruct &arControl, ARStatusList &arStatus)
{
	this->arControl = arControl;
	this->arStatus = arStatus;
}

CARServerInfo::~CARServerInfo(void)
{
}

void CARServerInfo::GetList(list<CARServerInfoItem> &listResult)
{	
	try
	{
		for(int i=0; i< 214; i++)
		{
			string infoValue = GetValue(i+1);
			CARServerInfoItem *infoItem = new CARServerInfoItem(i+1, infoValue);
			if(infoValue.length() > 0)
			{			
				listResult.push_back(*infoItem);
				cout << "Loading ServerInfo: " << CAREnum::ServerInfoApiCall(i+1) << "[OK]" << endl;
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION enumerating server properties" << endl;
	}
}

string CARServerInfo::GetValue(int apiCall)
{	
	stringstream strm;
	
	ARServerInfoRequestList requestList;
	ARServerInfoList serverInfo;

	requestList.numItems = 1;
	requestList.requestList = (unsigned int *) malloc (sizeof(unsigned int)*requestList.numItems);
	requestList.requestList[0] = apiCall;

	if( ARGetServerInfo(&arControl, &requestList, &serverInfo, &arStatus) == AR_RETURN_OK)
	{
		int datatype=serverInfo.serverInfoList->value.dataType;
		switch (datatype)
		{
			case AR_DATA_TYPE_NULL:
			{
				strm << "NULL";
			}
			break;
			case AR_DATA_TYPE_CHAR:
			{
				if(serverInfo.serverInfoList->value.u.charVal != NULL)
				{
					strm << serverInfo.serverInfoList->value.u.charVal;
				}
			}
			break;
			case AR_DATA_TYPE_INTEGER:
			{
				if(serverInfo.serverInfoList->value.u.intVal != NULL)
				{
					strm << serverInfo.serverInfoList->value.u.intVal;
				}
			}
			break;				
		}
	}

	delete requestList.requestList;
	FreeARServerInfoList(&serverInfo, false);
	FreeARStatusList(&arStatus, false);

	return strm.str();
}
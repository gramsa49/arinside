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
		for(int i=0; i< 255; i++)
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
				strm << serverInfo.serverInfoList->value.u.intVal;
			}
			break;				
		}
	}

	delete requestList.requestList;
	FreeARServerInfoList(&serverInfo, false);
	FreeARStatusList(&arStatus, false);

	return strm.str();
}
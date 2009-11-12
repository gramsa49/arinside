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
#include "ARServerInfo.h"
#include "../ARInside.h"

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
		stringstream strm;

		ARServerInfoRequestList requestList;
		ARServerInfoList serverInfo;
		CARInside* pInside = CARInside::GetInstance();

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		if      (pInside->CompareServerVersion(7,5,0) >= 0) { requestList.numItems = 324; } else
#endif
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_710
		if      (pInside->CompareServerVersion(7,1,0) >= 0) { requestList.numItems = 255; } else
#endif
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_700
		if      (pInside->CompareServerVersion(7,0,1) >= 0) { requestList.numItems = 247; }
		else if (pInside->CompareServerVersion(7,0,0) >= 0) { requestList.numItems = 246; }
		else if (pInside->CompareServerVersion(6,3,0) >= 0) { requestList.numItems = 215; }
		else if (pInside->CompareServerVersion(6,0,0) >= 0) { requestList.numItems = 206; }
		else if (pInside->CompareServerVersion(5,1,0) >= 0) { requestList.numItems = 184; }
		else if (pInside->CompareServerVersion(4,5,1) >= 0) { requestList.numItems = 121; }
#endif
		else
		{
			cerr << "[ERROR] Unsupported server version: " << pInside->arServerVersion << endl;
			return;
		}


		requestList.requestList = (unsigned int *) malloc (sizeof(unsigned int)*requestList.numItems);
		int infoProp = 1;
		for(unsigned int i=0; i < requestList.numItems; i++)
		{
			switch (infoProp)
			{
			case AR_SERVER_INFO_DB_PASSWORD:
				--requestList.numItems;
				--i;
				break;
			default:
				requestList.requestList[i] = infoProp;
				break;
			}
			infoProp++;
		}

		if( ARGetServerInfo(&arControl, &requestList, &serverInfo, &arStatus) == AR_RETURN_OK)
		{
			for(unsigned int i=0; i< serverInfo.numItems; i++)
			{
				strm.str("");
				ARServerInfoStruct info = serverInfo.serverInfoList[i];
				int datatype = info.value.dataType;
				switch (datatype)
				{
				case AR_DATA_TYPE_NULL:
					{
						strm << "NULL";
					}
					break;
				case AR_DATA_TYPE_CHAR:
					{
						if(info.value.u.charVal != NULL)
						{
							strm << info.value.u.charVal;
						}
					}
					break;
				case AR_DATA_TYPE_INTEGER:
					{
						strm << info.value.u.intVal;
					}
					break;				
				}
				string infoValue = strm.str();
				CARServerInfoItem *infoItem = new CARServerInfoItem(i+1, infoValue);
				if(infoValue.length() > 0)
				{			
					listResult.push_back(*infoItem);
					LOG << "Loading ServerInfo: " << CAREnum::ServerInfoApiCall(i+1) << "[OK]" << endl;
				}
			}
		}
		else
		{
			cerr << "Error Loading System Information properties:" << pInside->GetARStatusError(&arStatus);
		}
		delete requestList.requestList;
		FreeARServerInfoList(&serverInfo, false);
		FreeARStatusList(&arStatus, false);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating server properties: " << e.what() << endl;
	}
}

string CARServerInfo::GetValue(int apiCall)
{	
	stringstream strm;

	ARServerInfoRequestList requestList;
	ARServerInfoList serverInfo;

	requestList.numItems = 1;
	requestList.requestList = new unsigned int[requestList.numItems]; // (unsigned int *) malloc (sizeof(unsigned int)*requestList.numItems);
	requestList.requestList[0] = apiCall;

	if( ARGetServerInfo(&arControl, &requestList, &serverInfo, &arStatus) == AR_RETURN_OK)
	{
		if (serverInfo.numItems > 0)
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
	}

	delete[] requestList.requestList;
	FreeARServerInfoList(&serverInfo, false);
	FreeARStatusList(&arStatus, false);

	return strm.str();
}

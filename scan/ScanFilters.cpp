//Copyright (C) 2011 John Luthgers | jls17
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
#include "ScanFilters.h"
#include "../ARInside.h"

CScanFilters::CScanFilters(CARFilter& filter, ErrorCallMap& errCalls)
: flt(filter), errorCalls(errCalls)
{
}

CScanFilters::~CScanFilters(void)
{
}

void CScanFilters::Start()
{
	try
	{
		ErrorCallMap errCalls;

		cout << "Checking filter references";

		CARInside* pInside = CARInside::GetInstance();
		unsigned int filterCount = pInside->filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; filterIndex++)
		{
			CARFilter filter(filterIndex);
			if (pInside->appConfig.bOverlaySupport && !IsVisibleOverlay(filter))
				continue;

			CScanFilters scanFlt(filter, errCalls);
			scanFlt.Scan();
		}

		if (errCalls.size() > 0)
		{
			ErrorCallMap::iterator curIt = errCalls.begin();
			ErrorCallMap::iterator endIt = errCalls.end();

			for (; curIt != endIt; ++curIt)
			{
				// get the filter we are working with
				CARFilter flt(curIt->first);

				// reserve enough elements
				flt.ErrorCallers().resize(curIt->second.size());

				// copy it over
				std::copy(curIt->second.begin(), curIt->second.end(), flt.ErrorCallers().begin());
			}
		}
			//for (unsigned int filterIndex = 0; filterIndex < filterCount; filterIndex++)
			//{
			//	CARFilter filter(filterIndex);
			//	ErrorCallMap::iterator item = errorCalls.find(filter.GetName());
			//	if (item != errorCalls.end())
			//	{
			//		if (!item->second.empty())
			//		{
			//			filter.ErrorCallers().reserve(item->second.size());
			//			list<string>::iterator callerEnd = item->second.end();
			//			for (list<string>::iterator caller = item->second.begin(); caller != callerEnd; ++caller)
			//			{
			//				filter.ErrorCallers().push_back(*caller);
			//			}
			//		}
			//	}
			//}

		cout << endl;
	}
	catch (exception &e)
	{
		cerr << "EXCEPTION SearchFilterReferences: " << e.what() << endl;
	}
}

void CScanFilters::Scan()
{
	// add error handler refs
	if (flt.GetErrorOption() == AR_FILTER_ERRHANDLER_ENABLE)
	{
		CARFilter errFlt(flt.GetErrorHandler());
		if (errFlt.Exists())
		{
			ErrorCallMap::iterator item = errorCalls.find(errFlt.GetInsideId());
			if (item == errorCalls.end())
			{
				ErrCallPair newItem(errFlt.GetInsideId(), MapValue());
				newItem.second.push_back(flt.GetInsideId());
				errorCalls.insert(newItem);
			}
			else
			{
				item->second.push_back(flt.GetInsideId());
			}
		}
	}

	// add schema references
	const ARWorkflowConnectStruct& formList = flt.GetSchemaList();
	if (formList.type == AR_WORKFLOW_CONN_SCHEMA_LIST)
	{
		for (unsigned int listIndex = 0; listIndex < formList.u.schemaList->numItems; ++listIndex)
		{
			CARSchema schema(formList.u.schemaList->nameList[listIndex]);
			if (schema.Exists())
			{
				schema.AddFilter(flt);
			}
		}
	}
}

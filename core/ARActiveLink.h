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

#pragma once
#include "ARServerObject.h"
#include "ARProplistHelper.h"
#include "../lists/ARActiveLinkList.h"

class CARInside;

class CARActiveLink :
	public CARServerObject
{
public:
	CARActiveLink(int insideId);
	~CARActiveLink(void);

	// implement functions inherited from CARObject
	string GetName();
	string GetName() const;
	string GetNameFirstChar();
	bool NameStandardFirstChar();

	// implement functions inherited from CARServerObject
	const char* GetHelpText() const;
	ARTimestamp GetTimestamp();
	const ARAccessNameType& GetOwner() const;
	const ARAccessNameType& GetLastChanged() const;
	const char* GetChangeDiary() const;

	// implement access functions for active link data
	unsigned int GetOrder() const;
	const ARWorkflowConnectStruct& GetSchemaList() const;
	const ARInternalIdList& GetGroupList() const;
	unsigned int GetExecuteMask() const;
	const ARInternalId& GetControlField() const;
	const ARInternalId& GetFocusField() const;
	unsigned int GetEnabled();
	const ARQualifierStruct& GetRunIf() const;
	const ARActiveLinkActionList& GetIfActions() const;
	const ARActiveLinkActionList& GetElseActions() const;
	const ARPropList& GetPropList() const;

	const string& GetAppRefName() const;
	void SetAppRefName(const string& appName);

	string GetExecuteOn(bool singleLine=false, CARProplistHelper* props=NULL);
	string GetURL(int rootLevel, bool showImage = true);

	int GetServerObjectTypeXML() { return AR_STRUCT_ITEM_XML_ACTIVE_LINK; }
	bool IsClonable();
	CARServerObject* Clone();
};

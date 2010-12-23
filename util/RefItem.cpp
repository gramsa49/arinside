//Copyright (C) 2010 John Luthgers | jls17
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

#include "RefItem.h"
#include "../core/ARServerObject.h"
#include "../core/ARActiveLink.h"
#include "../core/ARFilter.h"
#include "../core/AREscalation.h"

// Implementation of CRefItemServerObject /////////////////////////////////////
CRefItemServerObject::CRefItemServerObject(CARServerObject& refItem, unsigned int dMessage)
: CRefItem(dMessage)
{
	this->object = refItem.Clone();
}

int CRefItemServerObject::GetObjectXMLType()
{
	return this->object->GetServerObjectTypeXML();
}

string CRefItemServerObject::GetObjectName()
{
	return this->object->GetName();
}

unsigned int CRefItemServerObject::GetObjectEnabled(bool &supportsEnabled)
{
	switch (object->GetServerObjectTypeXML())
	{
	case AR_STRUCT_ITEM_XML_FILTER:
		supportsEnabled = true;
		return static_cast<CARFilter*>(this->object)->GetEnabled();
	case AR_STRUCT_ITEM_XML_ACTIVE_LINK:
		supportsEnabled = true;
		return static_cast<CARActiveLink*>(this->object)->GetEnabled();
	case AR_STRUCT_ITEM_XML_ESCALATION:
		supportsEnabled = true;
		return static_cast<CAREscalation*>(this->object)->GetEnabled();
	////case AR_STRUCT_ITEM_XML_DIST_MAP: // TODO: add DSO support
	//case AR_STRUCT_ITEM_XML_DIST_POOL:
		
	default:
		supportsEnabled = false;
		return false;
	}
}

string CRefItemServerObject::GetDescription()
{
	stringstream strm;
	switch (this->messageId)
	{
		// TODO: add all known messages
	default:
		strm << "Unknown reference message: " << this->messageId;
		break;
	}
	return strm.str();
}

// Implementation of CRefItemWorkflowAction ///////////////////////////////////
CRefItemWorkflowAction::CRefItemWorkflowAction(unsigned int dMessage, CARServerObject &ref, unsigned int nAction)
: CRefItem(dMessage)
{
	this->object = ref.Clone();
	actionIndex = nAction;
}

int CRefItemWorkflowAction::GetObjectXMLType()
{
	return this->object->GetServerObjectTypeXML();
}

string CRefItemWorkflowAction::GetObjectName()
{
	return this->object->GetName();
}

unsigned int CRefItemWorkflowAction::GetObjectEnabled(bool &supportsEnabled)
{
	switch (object->GetServerObjectTypeXML())
	{
	case AR_STRUCT_ITEM_XML_FILTER:
		supportsEnabled = true;
		return static_cast<CARFilter*>(this->object)->GetEnabled();
	case AR_STRUCT_ITEM_XML_ACTIVE_LINK:
		supportsEnabled = true;
		return static_cast<CARActiveLink*>(this->object)->GetEnabled();
	case AR_STRUCT_ITEM_XML_ESCALATION:
		supportsEnabled = true;
		return static_cast<CAREscalation*>(this->object)->GetEnabled();
	//case AR_STRUCT_ITEM_XML_DIST_MAP: // TODO: add DSO support
	//case AR_STRUCT_ITEM_XML_DIST_POOL:
		
	default:
		supportsEnabled = false;
		return false;
	}
}

string CRefItemWorkflowAction::GetDescription()
{
	stringstream strm;
	switch (this->messageId)
	{
		// TODO: add all known messages
	default:
		strm << "Unknown reference message: " << this->messageId;
		break;
	}
	return strm.str();
}

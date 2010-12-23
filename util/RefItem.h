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

#pragma once

// forward declaration
class CARServerObject;

// Reference Types ////////////////////////////////////////////////////////////
// this values are returned by CRefItem::GetReferenceType to identify the
// underlying object type in case its necessary to cast to the real class.
const unsigned int REF_ITEM_TYPE_SERVEROBJECT = 1;
const unsigned int REF_ITEM_TYPE_WORKFLOW_ACTION = 2;
const unsigned int REF_ITEM_TYPE_OLD = 999; // just for compatibility; remove it later

// Messages ///////////////////////////////////////////////////////////////////
const unsigned int REFM_NONE = 0;


// base class
class CRefItem
{
protected:
	CRefItem(unsigned int dMessage) : messageId(dMessage) { }
public:
	virtual ~CRefItem(void) { }

public:
	// returns a value to identify the reference class .. see REF_ITEM_TYPE_...
	virtual int GetReferenceType() = 0;
	// returns the message id of the description
	unsigned int GetMessageId() { return messageId; }

	// for reference table output we need ...

	// returns the referenced xml object type (AR_STRUCT_ITEM_XML_...)
	virtual int GetObjectXMLType() = 0;
	// returns the name of the referenced object
	virtual string GetObjectName() = 0;
	// returns the enabled state of the referenced object
	virtual unsigned int GetObjectEnabled(bool &supportsEnabled) = 0; // supportsEnabled is set to false, if the object doesn't support the enabled/disabled state
	// returns the description text of the reference
	virtual string GetDescription() = 0;

protected:
	unsigned int messageId;
};

class CRefItemServerObject : public CRefItem
{
public:
	CRefItemServerObject(CARServerObject& refItem, unsigned int dMessage);
	virtual ~CRefItemServerObject(void) { }

public:
	virtual int GetReferenceType() { return REF_ITEM_TYPE_SERVEROBJECT; }
	virtual int GetObjectXMLType();
	virtual string GetObjectName();
	virtual unsigned int GetObjectEnabled(bool &supportsEnabled);
	virtual string GetDescription();

private:
	CARServerObject* object;
};

class CRefItemWorkflowAction : public CRefItem
{
public:
	CRefItemWorkflowAction(unsigned int dMessage, CARServerObject& ref, unsigned int nAction);
	virtual ~CRefItemWorkflowAction() { }

	virtual int GetReferenceType() { return REF_ITEM_TYPE_WORKFLOW_ACTION; }
	virtual int GetObjectXMLType();
	virtual string GetObjectName();
	virtual unsigned int GetObjectEnabled(bool &supportsEnabled);
	virtual string GetDescription();

private:
	CARServerObject* object;
	unsigned int actionIndex;
};

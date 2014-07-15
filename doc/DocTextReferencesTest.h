#pragma once

#include "DocTextReferences.h"

class CDocTextReferencesTest : public CDocTextReferences
{
public:
	CDocTextReferencesTest(const std::string& inText, const std::string &fieldSeparator, int schemaInsideId, int rootLevel, bool findKeywords, const CRefItem *refItem)
		: CDocTextReferences(inText, fieldSeparator, schemaInsideId, rootLevel, findKeywords, refItem) { }

protected:
	virtual string refFieldID(int iFieldId) 
	{ 
		stringstream tmp;	
		tmp << "<a href='Test'>" 
		    << iFieldId 
		    << "</a>"; 
		return tmp.str(); 
	}
	virtual bool isInvalidSchemaId() { return true; }
};

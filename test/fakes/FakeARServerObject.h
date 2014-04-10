#pragma once

#include "../../core/ARServerObject.h"
#include "../../lists/ARListHelpers.h"

class CFakeServerObject : public CARServerObject
{
public:
	CFakeServerObject(unsigned int xmlType = AR_STRUCT_ITEM_XML_ACTIVE_LINK) 
		: CARServerObject(0) { objName= "TheTestObj"; objType = xmlType; }

	ARAccessNameType name;
	string objName;
	unsigned int objType;

	// from CARServerObject
	virtual bool Exists() const { return true; }

	virtual const char* GetHelpText() const { return ""; }
	virtual ARTimestamp GetTimestamp() const { return 0; }
	virtual const ARAccessNameType& GetOwner() const { return name; }
	virtual const ARAccessNameType& GetLastChanged() const { return name; }
	virtual const char* GetChangeDiary() const { return ""; }
	virtual const ARPropList& GetPropList() const { return emptyPropList; }
	virtual string GetURL(int rootLevel, bool showImage = true) const { return ""; }
	virtual int GetServerObjectTypeXML() const { return objType; }

	virtual CARServerObject* Clone() const { return new CFakeServerObject(); }
	virtual bool IsClonable() const { return true; }

	// declarations for overlay support
	virtual int GetOverlayType() const { return 0; }
	virtual int GetOverlayGroup() const { return 0; }
	virtual string GetOverlayBaseName() const { return ""; }
	virtual string GetOverlayName() const { return ""; }

	// from CARObject
	virtual string GetName() { return objName; }
	virtual string GetName() const { return objName; }
	virtual string GetNameFirstChar() { return objName.substr(0, 1); }
	virtual bool NameStandardFirstChar() { return true; }
};


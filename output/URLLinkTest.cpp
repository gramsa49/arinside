#include "ARInsideTest.h"
#include "URLLink.h"

#include "../core/ARServerObject.h"
#include "../lists/ARListHelpers.h"
#include "FileNaming.h"
#include "IFileStructure.h"
#include "ImageTag.h"
#include "WebUtil.h"

using namespace testing;
using namespace OUTPUT;

const int rootLevel = 1; // we use rootLevel 1 in most cases

class URLLinkTests : public Test
{
public:
	// TODO: replace the following with a fake implementation!!
	OUTPUT::FileNaming::ObjectNameFileNamingStrategy defaultFileNaming;
	
	void SetUp() 
	{
		SetFileNamingStrategy(&defaultFileNaming);
		CWebUtil::webpageFileExtension = "htm";
	}

	void TearDown()
	{
		CWebUtil::webpageFileExtension = "";
	}
};

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

TEST_F(URLLinkTests, BasicMainSelfLink)
{
	CPageParams page(PAGE_MAINHOME);
	ImageTag image(ImageTag::NoImage, rootLevel);

	stringstream strm;
	strm << URLLink("LinkCaption", page, image, rootLevel, false, URLLink::TARGET_MODE_SELF);
	string result = strm.str();

	ASSERT_STREQ("<a href=\"../index.htm\">LinkCaption</a>", result.c_str());
}

TEST_F(URLLinkTests, MainLinkWithImageAndTarget)
{
	CPageParams page(PAGE_MAINHOME);
	ImageTag image(ImageTag::Server, rootLevel);

	stringstream strm;
	strm << URLLink("<Main Page>", page, image, rootLevel, true, URLLink::TARGET_MODE_PARENT);
	string result = strm.str();

	ASSERT_STREQ("<img src=\"../img/server.gif\" width=\"16\" height=\"16\" alt=\"server.gif\" /><a href=\"../index.htm\" target=\"_parent\">&lt;Main Page&gt;</a>", result.c_str());
}

TEST_F(URLLinkTests, ObjectLinkWithImage)
{
	CFakeServerObject fakeWorkflow;

	stringstream strm;
	strm << URLLink(fakeWorkflow, rootLevel);
	string result = strm.str();

	ASSERT_STREQ("<img src=\"../img/active_link.gif\" width=\"16\" height=\"16\" alt=\"active_link.gif\" /><a href=\"../active_link/TheTestObj.htm\">TheTestObj</a>", result.c_str());
}

TEST_F(URLLinkTests, ObjectFilterLinkWithImage)
{
	CFakeServerObject fakeWorkflow(AR_STRUCT_ITEM_XML_FILTER);

	stringstream strm;
	strm << URLLink(fakeWorkflow, rootLevel);
	string result = strm.str();

	ASSERT_STREQ("<img src=\"../img/filter.gif\" width=\"16\" height=\"16\" alt=\"filter.gif\" /><a href=\"../filter/TheTestObj.htm\">TheTestObj</a>", result.c_str());
}

TEST_F(URLLinkTests, ObjectEscalationLinkWithoutImage)
{
	CFakeServerObject fakeWorkflow(AR_STRUCT_ITEM_XML_ESCALATION);

	stringstream strm;
	strm << URLLink(fakeWorkflow, rootLevel, false);
	string result = strm.str();

	ASSERT_STREQ("<a href=\"../escalation/TheTestObj.htm\">TheTestObj</a>", result.c_str());
}

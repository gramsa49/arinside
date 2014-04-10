#include "ARInsideTest.h"
#include "URLLink.h"

#include "FileNaming.h"
#include "IFileStructure.h"
#include "ImageTag.h"
#include "WebUtil.h"

#include "../test/fakes/FakeARServerObject.h"

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
	CFakeServerObject fakeWorkflow("TheTestObj", AR_STRUCT_ITEM_XML_FILTER);

	stringstream strm;
	strm << URLLink(fakeWorkflow, rootLevel);
	string result = strm.str();

	ASSERT_STREQ("<img src=\"../img/filter.gif\" width=\"16\" height=\"16\" alt=\"filter.gif\" /><a href=\"../filter/TheTestObj.htm\">TheTestObj</a>", result.c_str());
}

TEST_F(URLLinkTests, ObjectEscalationLinkWithoutImage)
{
	CFakeServerObject fakeWorkflow("TheTestObj", AR_STRUCT_ITEM_XML_ESCALATION);

	stringstream strm;
	strm << URLLink(fakeWorkflow, rootLevel, false);
	string result = strm.str();

	ASSERT_STREQ("<a href=\"../escalation/TheTestObj.htm\">TheTestObj</a>", result.c_str());
}

TEST_F(URLLinkTests, ObjectOverlayLink)
{
	CFakeServerObject fakeWorkflow("SampleMenu", AR_STRUCT_ITEM_XML_CHAR_MENU, AR_OVERLAY_OBJECT);
	
	stringstream strm;
	strm << URLLink(fakeWorkflow, rootLevel);
	string result = strm.str();

	ASSERT_STREQ("<img src=\"../img/overlay.gif\" style=\"background:url(../img/menu.gif)\" width=\"16\" height=\"16\" alt=\"menu.gif\" /><a href=\"../menu/SampleMenu.htm\">SampleMenu</a>", result.c_str());
}

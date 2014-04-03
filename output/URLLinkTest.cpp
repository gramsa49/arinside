#include "ARInsideTest.h"
#include "URLLink.h"

#include "FileNaming.h"
#include "IFileStructure.h"
#include "ImageTag.h"
#include "WebUtil.h"

using namespace testing;
using namespace OUTPUT;

class URLLinkTests : public Test
{
public:
	// TODO: replace the following with a fake implementation!!
	OUTPUT::FileNaming::DefaultFileNamingStrategy defaultFileNaming;

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
	int rootLevel = 1;
	CPageParams page(PAGE_MAINHOME);
	ImageTag image(ImageTag::NoImage, rootLevel);

	stringstream strm;
	strm << URLLink("LinkCaption", page, image, rootLevel, false, URLLink::TARGET_MODE_SELF);
	string result = strm.str();

	ASSERT_STREQ("<a href=\"../index.htm\">LinkCaption</a>", result.c_str());
}

TEST_F(URLLinkTests, MainLinkWithImageAndTarget)
{
	int rootLevel = 1;
	CPageParams page(PAGE_MAINHOME);
	ImageTag image(ImageTag::Server, rootLevel);

	stringstream strm;
	strm << URLLink("<Main Page>", page, image, rootLevel, true, URLLink::TARGET_MODE_PARENT);
	string result = strm.str();

	ASSERT_STREQ("<img src=\"../img/server.gif\" width=\"16\" height=\"16\" alt=\"server.gif\" /><a href=\"../index.htm\" target=\"_parent\">&lt;Main Page&gt;</a>", result.c_str());
}

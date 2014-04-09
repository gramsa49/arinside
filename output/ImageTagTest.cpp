#include "ARInsideTest.h"
#include "ImageTag.h"

TEST(ImageTag, ImageToStream)
{
	stringstream strm;
	strm << OUTPUT::ImageTag::Schema;
	string result = strm.str();

	ASSERT_STREQ("<img src=\"img/schema.gif\" width=\"16\" height=\"16\" alt=\"schema.gif\" />", result.c_str());
}

TEST(ImageTag, ObjInstToStream)
{
	stringstream strm;

	strm << OUTPUT::ImageTag(OUTPUT::ImageTag::SchemaRegular, 1);
	string result = strm.str();
	ASSERT_STREQ("<img src=\"../img/schema.gif\" width=\"16\" height=\"16\" alt=\"schema.gif\" />", result.c_str());
}

TEST(ImageTag, ObjInstToString)
{	
	string result = OUTPUT::ImageTag(OUTPUT::ImageTag::ActiveLink, 1);
	ASSERT_STREQ("<img src=\"../img/active_link.gif\" width=\"16\" height=\"16\" alt=\"active_link.gif\" />", result.c_str());
}

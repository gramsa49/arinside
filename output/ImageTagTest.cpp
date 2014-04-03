#include "ARInsideTest.h"
#include "ImageTag.h"

TEST(ImageTag, ImageToStream)
{
	stringstream strm;
	strm << OUTPUT::ImageTag::Schema;
	string result = strm.str();

	ASSERT_STREQ("<img src=\"img/schema.gif\" width=\"16\" height=\"16\" alt=\"schema.gif\" />", result.c_str());
}

#include "ARInsideTest.h"
#include "DocTextReferencesTest.h"

TEST(DocTextReferencesTests, SimpleFieldReference)
{
	CDocTextReferencesTest textRef("select * from $8$", "$", 1, 1, true, NULL);
	string result = textRef.TextFindFields();
	ASSERT_STREQ("select * from $<a href='Test'>8</a>$", result.c_str());
}

TEST(DocTextReferencesTests, InvalidFieldFormat)
{
	CDocTextReferencesTest textRef("select * from $8.35.19$", "$", 1, 1, true, NULL);
	string result = textRef.TextFindFields();
	ASSERT_STREQ("select * from $8.35.19$", result.c_str());
}

TEST(DocTextReferencesTests, HasKeywordSupport)
{
	CDocTextReferencesTest textRef("select * from $-5$", "$", 1, 1, true, NULL);
	textRef.CallReal_refFieldId(true);
	string result = textRef.TextFindFields();
	ASSERT_STREQ("select * from $SCHEMA$", result.c_str());
}

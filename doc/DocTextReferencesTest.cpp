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

TEST(DocTextReferencesTests, DISABLED_CurrencyDate)
{
	// since DocTextReferences uses the CDocCurrencyField class, this test fails
	// because currently it's not possible to replace this dependency easily
	// without using virtual methods.
	//
	// TODO: change the responsibility of the DocTextReferences class a little more
	// ~~~~~~~~~~~~~
	// it splits text into parts, replaces the parts with links, scans for special
	// commands an so on. At least replacing the parts should be done by other
	// classes (like CDocCurrencyField does) which could be used in serveral places
	CDocTextReferencesTest textRef("select $8.3$", "$", 1, 1, true, NULL);
	string result = textRef.TextFindFields();
	ASSERT_STREQ("select $<a href='Test'>8</a>.DATE$", result.c_str());
}

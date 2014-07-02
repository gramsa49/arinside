#include "ARInsideTest.h"
#include "DocTextReferencesTest.h"

TEST(DocTextReferencesTests, SimpleFieldReference)
{
	CDocTextReferencesTest textRef("select * from $8$", "$", 1, 1, true, NULL);
	string result = textRef.TextFindFields();
	ASSERT_EQ("select * from <a href='Test'>8</a>", result.c_str());
}

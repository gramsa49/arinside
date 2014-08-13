#include "ARInsideTest.h"
#include "DocActionOpenWindowHelper.h"

using namespace testing;

char* testReportString = "Report: \00121=AR System\00122=\00129=\00127=\00128=null\00124=\00125=\00126=\00130=0\00131=\00132=true\001\nto-screen: ";

// A fixture class with a instance of CDocActionOpenWindowHelper using the testReportString above
class DocActionOpenWindowHelperTests : public Test
{
public:
	DocActionOpenWindowHelperTests() : reportData(testReportString) {}

protected:
	CDocActionOpenWindowHelper reportData;
};

// verify the testReportString above is valid for the parser
TEST_F(DocActionOpenWindowHelperTests, ValidInput)
{
	ASSERT_TRUE(reportData.IsValid());
}

// now just verify a few values we've in the report string
TEST_F(DocActionOpenWindowHelperTests, VerifyReportType)
{
	string result = reportData.getReportType();
	ASSERT_STREQ("AR System", result.c_str());
}

TEST_F(DocActionOpenWindowHelperTests, VerifyReportName)
{
	string result = reportData.getReportName();
	ASSERT_STREQ("", result.c_str());
}

TEST_F(DocActionOpenWindowHelperTests, VerifyQueryOverride)
{
	string result = reportData.getQueryOverride();
	ASSERT_STREQ("null", result.c_str());
}

TEST_F(DocActionOpenWindowHelperTests, VerifyReportDestination)
{
	string result = reportData.getReportDestination();
	ASSERT_STREQ("to-screen: ", result.c_str());
}

TEST_F(DocActionOpenWindowHelperTests, VerifyInlineForm)
{
	string result = reportData.getInlineForm();
	ASSERT_STREQ("true", result.c_str());
}

#include "ARInsideTest.h"
#include "FileSystemUtil.h"
#include "FileSystemUtilTest.h"

TEST(FileSystemUtil, IsDots_WithDots_Test)
{
	ASSERT_EQ(true, FileSystemUtil::IsDots("."));
	ASSERT_EQ(true, FileSystemUtil::IsDots(".."));
}

TEST(FileSystemUtil, IsDots_WithoutDots_Test)
{
	ASSERT_EQ(false, FileSystemUtil::IsDots("testfile"));
}

TEST(FileSystemUtil, GetExecutableDirectory)
{
	string directory = FileSystemUtil::GetExecutableDirectory(argv_0);
	directory = FileSystemUtil::GetRealPathName(directory);

	string expectedDir = DIRECTORY_OF_TEST_BINARY;
#ifdef WIN32
	expectedDir = CUtil::StrReplace("/","\\", expectedDir);
	std::transform(directory.begin(), directory.end(), directory.begin(), ::tolower);
	std::transform(expectedDir.begin(), expectedDir.end(), expectedDir.begin(), ::tolower);
#endif

	ASSERT_STREQ(expectedDir.c_str(), directory.c_str());
}
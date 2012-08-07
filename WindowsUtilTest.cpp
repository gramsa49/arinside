#include "ARInsideTest.h"
#include "WindowsUtil.h"

TEST(FileSystemUtil, IsDots_WithDots_Test)
{
	ASSERT_EQ(true, FileSystemUtil::IsDots("."));
	ASSERT_EQ(true, FileSystemUtil::IsDots(".."));
}

TEST(FileSystemUtil, IsDots_WithoutDots_Test)
{
	ASSERT_EQ(false, FileSystemUtil::IsDots("testfile"));
}

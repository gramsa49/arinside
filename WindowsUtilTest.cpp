#include "ARInsideTest.h"
#include "WindowsUtil.h"

TEST(WindowsUtil, IsDots_WithDots_Test)
{
	ASSERT_EQ(true, CWindowsUtil::IsDots("."));
	ASSERT_EQ(true, CWindowsUtil::IsDots(".."));
}

TEST(WindowsUtil, IsDots_WithoutDots_Test)
{
	ASSERT_EQ(false, CWindowsUtil::IsDots("testfile"));
}

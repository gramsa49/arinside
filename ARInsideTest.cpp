#include "ARInsideTest.h"
#include <iostream>

// http://code.google.com/p/googletest/
#include "gtest/gtest.h"

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv); 
  return RUN_ALL_TESTS();
}


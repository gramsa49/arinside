#include "ARInsideTest.h"
#include "ARListHelpers.h"

TEST(IndexSorter, VectorOfStringsTest)
{
	vector<string> testList;
	testList.push_back("  Test");
	testList.push_back("Demo");
	testList.push_back(" temp");

	vector<int> indexList;
	indexList.push_back(0);	// this is the index of testList's "  Test" value
	indexList.push_back(1); // this is the index of testList's "Demo" value
	indexList.push_back(2); // this is the index of testList's " temp" value

	IndexSorter sortableContent(testList);
	std::sort(indexList.begin(), indexList.end(), SortingDelegate(sortableContent));

	ASSERT_EQ(1, indexList[0]); // in the sorted index, the value "demo" comes first
	ASSERT_EQ(2, indexList[1]); // ... followed by the value "temp"
	ASSERT_EQ(0, indexList[2]); // ... and the value "test" is next
}

#include "ARInsideTest.h"
#include "IndexSorter.h"

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

	IndexSorter indexSorter(false);
	indexSorter.SortBy(testList).Sort(indexList);

	ASSERT_EQ(1, indexList[0]); // in the sorted index, the value "demo" comes first
	ASSERT_EQ(2, indexList[1]); // ... followed by the value "temp"
	ASSERT_EQ(0, indexList[2]); // ... and the value "test" is next
}

TEST(IndexSorter, VectorOfUTF8StringsTest)
{
	vector<string> testList;
	testList.push_back("Umgang");
	testList.push_back("Wartung");
	testList.push_back("\xc3\x9c""berschrift");
	testList.push_back(" Hallo Welt");
	testList.push_back("\xc3\x81ndre");
	testList.push_back("B\xc3\xbcro");
	testList.push_back("ugly");

	vector<int> indexList;
	indexList.push_back(0);	// this is the index of testList's "Umgang" value
	indexList.push_back(1); // this is the index of testList's "Wartung" value
	indexList.push_back(2); // this is the index of testList's "Überschrift" value
	indexList.push_back(3); // this is the index of testList's " Hallo Welt" value
	indexList.push_back(4); // this is the index of testList's "Ándre" value
	indexList.push_back(5); // this is the index of testList's "Büro" value
	indexList.push_back(6); // this is the index of testList's "ugly" value

	IndexSorter indexSorter(true);
	indexSorter.SortBy(testList).Sort(indexList);

	ASSERT_EQ(4, indexList[0]); // the value of "ándre"
	ASSERT_EQ(5, indexList[1]); // the value of "büro"
	ASSERT_EQ(3, indexList[2]); // the value of "hallo welt"
	ASSERT_EQ(2, indexList[3]); // the value of "überschrift"
	ASSERT_EQ(6, indexList[4]); // the value of "ugly"
	ASSERT_EQ(0, indexList[5]); // the value of "umgang"
	ASSERT_EQ(1, indexList[6]); // the value of "wartung"
}

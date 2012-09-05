#include "ARInsideTest.h"
#include "rapidjson/document.h"
#include "rapidjson/filestream.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

TEST(RapidJSONTests, GenerateArray)
{
	Document document;
	Document::AllocatorType& allocator = document.GetAllocator();

	document.SetArray();
	ASSERT_TRUE(document.IsArray());

	document.PushBack("Test1", allocator);
	document.PushBack("Test2", allocator);

	Value item;
	item.SetObject();
	item.AddMember("Id", 1, allocator);
	item.AddMember("Name", "ARInside", allocator);
	document.PushBack(item, allocator);

	// check array size
	ASSERT_EQ(3, document.Size());

	stringstream strm;
	FileStream f(stdout);
	Writer<FileStream> writer(f);
	document.Accept(writer);
}

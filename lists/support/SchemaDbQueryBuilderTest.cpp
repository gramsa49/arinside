#include "ARInsideTest.h"
#include "SchemaDbQueryBuilder.h"

using namespace testing;

#define BASE_STATEMENT "select schemaId, name, viewName, shViewName from arschema"
#define BASE_QUERY(N) " where schemaId > " #N
#define BASE_ORDER " order by schemaId asc"

#define DEFAULT_SQL_STATEMENT_WITHOUT_QUERY      (BASE_STATEMENT BASE_ORDER)
#define DEFAULT_SQL_STATEMENT_WITH_QUERY         (BASE_STATEMENT BASE_QUERY(0) BASE_ORDER)

TEST(SchemaDbQueryBuilder, DefaultQueryIfNoMaxRetrieve)
{
	SchemaDbQueryBuilder queryBuilder(0);

	const char* sql = queryBuilder.GetNextQuery();

	ASSERT_STREQ(DEFAULT_SQL_STATEMENT_WITHOUT_QUERY, sql);
}

TEST(SchemaDbQueryBuilder, ValidateQueryColumCount)
{
	ASSERT_EQ(4, SchemaDbQueryBuilder::ExpectedColumnCount());
}

class SchemaDbQueryWithMax100Results : public Test
{
public:
	SchemaDbQueryBuilder *queryBuilder;

	const static unsigned int MAX_RESULT = 100;

	void SetUp() override {
			queryBuilder = new SchemaDbQueryBuilder(MAX_RESULT);
	}
};

TEST_F(SchemaDbQueryWithMax100Results, FirstQueryShouldStartAtZero)
{
	const char* sql = queryBuilder->GetNextQuery();
	ASSERT_STREQ(DEFAULT_SQL_STATEMENT_WITH_QUERY, sql);
}

#define SECOND100_SQL_STATEMENT_WITH_QUERY (BASE_STATEMENT BASE_QUERY(112) BASE_ORDER)
TEST_F(SchemaDbQueryWithMax100Results, SecondQueryShouldStartAtLastSchemaId)
{
	const char* sql;
	sql = queryBuilder->GetNextQuery(); // generate first query
	queryBuilder->SetLastReceivedSchemaId(112); // this is the last row (schemaId) we did get from previous query result
	
	sql = queryBuilder->GetNextQuery(); // generate second query
	ASSERT_STREQ(SECOND100_SQL_STATEMENT_WITH_QUERY, sql);
}

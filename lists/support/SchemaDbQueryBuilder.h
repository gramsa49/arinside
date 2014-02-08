#pragma once

class SchemaDbQueryBuilder
{
public:
	SchemaDbQueryBuilder(unsigned int maxRetrieve);
	~SchemaDbQueryBuilder(void);

	const char* GetNextQuery();
	void SetLastReceivedSchemaId(int schemaId);

private:
	void AppendStatement(std::ostream &strm);
	void AppendWhereCondition(std::ostream &strm);
	void AppendOrdering(std::ostream &strm);

	void BuildWhereCondition(std::ostream &strm);

private:
	unsigned int maxRetrieve;
	int lastReceivedSchemaId;
	std::string query;
};

//Copyright (C) 2014 John Luthgers | jls17
//
//This file is part of ARInside.
//
//    ARInside is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    ARInside is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with ARInside.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "SchemaDbQueryBuilder.h"

SchemaDbQueryBuilder::SchemaDbQueryBuilder(unsigned int maxRetrieve)
: maxRetrieve(maxRetrieve), lastReceivedSchemaId(0)
{
}

SchemaDbQueryBuilder::~SchemaDbQueryBuilder(void)
{
}

void SchemaDbQueryBuilder::SetLastReceivedSchemaId(int schemaId)
{
	this->lastReceivedSchemaId = schemaId;
}

const char* SchemaDbQueryBuilder::GetNextQuery()
{
	stringstream strm;

	AppendStatement(strm);
	AppendWhereCondition(strm);
	AppendOrdering(strm);	

	query = strm.str();
	return query.c_str();
}

void SchemaDbQueryBuilder::AppendStatement(std::ostream &strm)
{
	strm << "select schemaId, name, viewName, shViewName from arschema";
}

void SchemaDbQueryBuilder::AppendOrdering(std::ostream &strm)
{
	strm << " order by schemaId asc";
}

void SchemaDbQueryBuilder::AppendWhereCondition(std::ostream &strm)
{
	if (this->maxRetrieve > 0)
	{
		BuildWhereCondition(strm);
	}
}

void SchemaDbQueryBuilder::BuildWhereCondition(std::ostream &strm)
{
	strm << " where schemaId > " << lastReceivedSchemaId;
}

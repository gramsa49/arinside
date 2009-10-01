//Copyright (C) 2009 Stefan Nerlich | stefan.nerlich@hotmail.com
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
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "DocAnalyzer.h"

CDocAnalyzer::CDocAnalyzer(CARInside &arIn, string path)
{
	this->pInside = &arIn;
	this->path = path;
	this->rootLevel = 1;
}

CDocAnalyzer::~CDocAnalyzer(void)
{
}

void CDocAnalyzer::Documentation()
{
	try
	{
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage("analyzer_main", "Analyzer", rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << "Performance Analyzer" << endl;
			webPage.AddContentHead(contHeadStrm.str());

			webPage.AddContent(CWebUtil::Link("Form Index Analyzer", "analyzer_schema_index."+CWebUtil::WebPageSuffix(), "doc.gif", rootLevel, true));
			IndexAnalyzer();		

			webPage.SaveInFolder(this->path);
		}
	}
	catch(...)
	{
		cout << "EXCEPTION Analyzer_Documentation" << endl;
	}
}

void CDocAnalyzer::IndexAnalyzer()
{
	try
	{
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage("analyzer_schema_index", "Form Index Analyzer", rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << "Form Index Analyzer" << endl;
			webPage.AddContentHead(contHeadStrm.str());


			CTable tbl("fieldListAll", "TblObjectList");
			tbl.AddColumn(20, "Field");
			tbl.AddColumn(20, "Form");
			tbl.AddColumn(60, "Comment");


			list<CARSchema>::iterator schemaIter;		
			CARSchema *schema;
			for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
			{			
				schema = &(*schemaIter);

				if(schema->indexList.numItems > 0)
				{
					for(unsigned int nIndex = 0; nIndex < schema->indexList.numItems; nIndex++)
					{
						for(unsigned int nField=0; nField < schema->indexList.indexList[nIndex].numFields; nField++)
						{
							list<CARField>::iterator listIter;			
							CARField *field;
							for ( listIter = schema->fieldList.begin(); listIter != schema->fieldList.end(); listIter++ )
							{
								field = &(*listIter);

								if(schema->indexList.indexList[nIndex].fieldIds[nField] == field->fieldId)
								{

									if(field->dataType == AR_DATA_TYPE_CHAR)
									{
										ARCharLimitsStruct fLimit = field->limit.u.charLimits;
										if(fLimit.qbeMatchOperation == AR_QBE_MATCH_ANYWHERE)
										{
											CTableRow row("");
											row.AddCell(CTableCell(field->GetURL(rootLevel)));
											row.AddCell(CTableCell(schema->GetURL(rootLevel)));

											stringstream strmTmp;
											strmTmp.str("");

											strmTmp << "Inefficient index because of QBE match anywhere in Index " << endl;
											strmTmp << this->pInside->LinkToSchemaIndex(schema->indexList.indexList[nIndex].indexName, schema->insideId, rootLevel) << endl;
											row.AddCell(CTableCell(strmTmp.str()));
											tbl.AddRow(row);
										}

										if(fLimit.maxLength > 255)
										{
											CTableRow row("");
											row.AddCell(CTableCell(field->GetURL(rootLevel)));
											row.AddCell(CTableCell(schema->GetURL(rootLevel)));

											stringstream strmTmp;
											strmTmp.str("");

											strmTmp << "Length of field is greater than 255 " << endl;
											strmTmp << this->pInside->LinkToSchemaIndex(schema->indexList.indexList[nIndex].indexName, schema->insideId, rootLevel) << endl;
											row.AddCell(CTableCell(strmTmp.str()));
											tbl.AddRow(row);
										}
									}
								}
							}
						}
					}
				}
			}

			tbl.description = "If the QBE Match property on a character field is set";
			tbl.description += " to Anywhere, the database will not be able to";
			tbl.description += " optimize its search and will perform a table scan.";
			tbl.description += " Avoid using this setting if possible.";

			webPage.AddContent(tbl.ToXHtml());
			webPage.SaveInFolder(this->path);
		}
	}
	catch(...)
	{
		cout << "EXCEPTION Analyzer_Index_Documentation" << endl;
	}
}
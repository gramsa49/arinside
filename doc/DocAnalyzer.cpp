
/****************************************************************************** 
 * 
 *  file:  DocAnalyzer.cpp
 * 
 *  Copyright (c) 2007, Stefan Nerlich | stefan.nerlich@hotmail.com 
 *  All rights reverved.
 * 
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *  
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.  
 *  
 *****************************************************************************/

#include "StdAfx.h"
#include "docanalyzer.h"

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
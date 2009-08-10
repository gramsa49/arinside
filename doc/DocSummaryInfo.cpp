
/****************************************************************************** 
 * 
 *  file:  DocsummaryInfo.cpp
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
#include "docsummaryinfo.h"

extern int nFilesCreated;

CDocSummaryInfo::CDocSummaryInfo(CARInside &arIn, string path)
{
	this->pInside = &arIn;
	this->path = path;
	this->rootLevel = 0;
}

CDocSummaryInfo::~CDocSummaryInfo(void)
{
}

void CDocSummaryInfo::Documentation()
{
	try
	{		
		stringstream pgStream;
		CWebPage webPage("index", "Documentation Index", rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead("Documentation index:");

		stringstream strm;
		strm.str("");
		
		CTable tblListObjectInfo("tblDocSummary", "TblObjectList");
		tblListObjectInfo.AddColumn(10, "Objects");		
		tblListObjectInfo.AddColumn(90, "Description");

		CTableRow row("cssStdRow");		
		row.AddCell(CTableCell((int)this->pInside->alList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Active Links", "active_link/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);
		
		unsigned int nWebservice = 0;
		unsigned int nAlGuide = 0;
		unsigned int nFilterGuide = 0;
		unsigned int nPacklist = 0;
		unsigned int nApplication = 0;
		list<CARContainer>::iterator contIter;
		for ( contIter = this->pInside->containerList.begin(); contIter != this->pInside->containerList.end(); contIter++ )
		{
			CARContainer *cont = &(*contIter);
			switch(cont->type)
			{
				case ARCON_WEBSERVICE:
				{
					nWebservice++;
				}
				break;
				case ARCON_GUIDE:
				{
					nAlGuide++;
				}
				break;
				case ARCON_FILTER_GUIDE:
				{
					nFilterGuide++;
				}
				break;
				case ARCON_PACK:
				{
					nPacklist++;
				}
				break;
				case ARCON_APP:
				{
					nApplication++;
				}
				break;
			}
		}

		row.ClearCells();
		row.AddCell(CTableCell(nWebservice));
		row.AddCell(CTableCell(CWebUtil::Link("Web Services", "webservice/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell(nAlGuide));
		row.AddCell(CTableCell(CWebUtil::Link("Active Link Guides", "active_link_guide/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell(nFilterGuide));
		row.AddCell(CTableCell(CWebUtil::Link("Filter Guides", "filter_guide/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell(nPacklist));
		row.AddCell(CTableCell(CWebUtil::Link("Packing Lists", "packing_list/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);
		
		row.ClearCells();
		row.AddCell(CTableCell(nApplication));
		row.AddCell(CTableCell(CWebUtil::Link("Applications", "application/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->escalList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Escalations", "escalation/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->filterList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Filters", "filter/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->groupList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Groups", "group/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->menuList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Menus", "menu/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->roleList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Roles", "role/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->schemaList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Forms", "schema/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->userList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Users", "user/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		unsigned int nNumTotalObjects = (unsigned int)this->pInside->alList.size();
		nNumTotalObjects += (unsigned int)this->pInside->containerList.size();
		nNumTotalObjects += (unsigned int)this->pInside->escalList.size();
		nNumTotalObjects += (unsigned int)this->pInside->filterList.size();
		nNumTotalObjects += (unsigned int)this->pInside->groupList.size();
		nNumTotalObjects += (unsigned int)this->pInside->menuList.size();
		nNumTotalObjects += (unsigned int)this->pInside->roleList.size();
		nNumTotalObjects += (unsigned int)this->pInside->schemaList.size();
		nNumTotalObjects += (unsigned int)this->pInside->userList.size();


		int nNumTotalFields = 0;
		list<CARSchema>::iterator schemaIter;
		for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
		{			
			CARSchema *schema = &(*schemaIter);
			nNumTotalFields += (int)schema->fieldList.size();
		}

		webPage.AddContent(tblListObjectInfo.ToXHtml());

		strm << nNumTotalObjects << " ARSystem objects and " << nNumTotalFields << " fields loaded in " << this->pInside->nDurationLoad << " seconds.<br/>" << endl;		
		strm << nFilesCreated << " files created in " << this->pInside->nDurationDocumentation << " seconds. <br/>" << endl;

		webPage.AddContent(strm.str());
		webPage.SaveInFolder(this->path);	
	}
	catch(...)
	{
		cout << "EXCEPTION SummaryInfo" << endl;
	}
}
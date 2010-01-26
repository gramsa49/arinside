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
#include "DocSummaryInfo.h"

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
		row.AddCell(CTableCell((int)this->pInside->alList.GetCount()));
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
		row.AddCell(CTableCell((int)this->pInside->escalationList.GetCount()));
		row.AddCell(CTableCell(CWebUtil::Link("Escalations", "escalation/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->filterList.GetCount()));
		row.AddCell(CTableCell(CWebUtil::Link("Filters", "filter/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->groupList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Groups", "group/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->menuList.GetCount()));
		row.AddCell(CTableCell(CWebUtil::Link("Menus", "menu/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->roleList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Roles", "role/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->schemaList.GetCount()));
		row.AddCell(CTableCell(CWebUtil::Link("Forms", "schema/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->userList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Users", "user/index.htm", "", 0)));
		tblListObjectInfo.AddRow(row);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		if (pInside->CompareServerVersion(7,5) >= 0)
		{
			row.ClearCells();
			row.AddCell(CTableCell(this->pInside->imageList.GetCount()));
			row.AddCell(CTableCell(CWebUtil::Link("Images", "image/index.htm", "", 0)));
			tblListObjectInfo.AddRow(row);
		}
#endif

		unsigned int nNumTotalObjects = (unsigned int)this->pInside->alList.GetCount();
		nNumTotalObjects += (unsigned int)this->pInside->containerList.size();
		nNumTotalObjects += (unsigned int)this->pInside->escalationList.GetCount();
		nNumTotalObjects += (unsigned int)this->pInside->filterList.GetCount();
		nNumTotalObjects += (unsigned int)this->pInside->groupList.size();
		nNumTotalObjects += (unsigned int)this->pInside->menuList.GetCount();
		nNumTotalObjects += (unsigned int)this->pInside->roleList.size();
		nNumTotalObjects += (unsigned int)this->pInside->schemaList.GetCount();
		nNumTotalObjects += (unsigned int)this->pInside->userList.size();
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		nNumTotalObjects += this->pInside->imageList.GetCount();
#endif			

		unsigned int nNumTotalFields = 0;
		unsigned int schemaCount = this->pInside->schemaList.GetCount();
		for ( unsigned int schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex )
		{			
			CARSchema schema(schemaIndex);
			nNumTotalFields += schema.GetFields()->GetCount();
		}

		webPage.AddContent(tblListObjectInfo.ToXHtml());

		strm << nNumTotalObjects << " ARSystem objects and " << nNumTotalFields << " fields loaded in " << this->pInside->nDurationLoad << " seconds.<br/>" << endl;		
		strm << nFilesCreated << " files created in " << this->pInside->nDurationDocumentation << " seconds. <br/>" << endl;

		if (!this->pInside->appConfig.runNotes.empty()) {
			strm << "<br/>";
			strm << "Run Notes: " << this->pInside->appConfig.runNotes;
		}

		webPage.AddContent(strm.str());
		webPage.SaveInFolder(this->path);	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION SummaryInfo: " << e.what() << endl;
	}
}

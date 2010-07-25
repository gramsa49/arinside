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

		unsigned int nWebservice = 0;
		unsigned int nAlGuide = 0;
		unsigned int nFilterGuide = 0;
		unsigned int nPacklist = 0;
		unsigned int nApplication = 0;

		unsigned int cntCount = this->pInside->containerList.GetCount();
		for ( unsigned int cntIndex = 0; cntIndex < cntCount; ++cntIndex )
		{
			CARContainer cont(cntIndex);

			switch(cont.GetType())
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

		CTableRow row("cssStdRow");
		row.AddCell(CTableCell((int)this->pInside->alList.GetCount()));
		row.AddCell(CTableCell(CWebUtil::Link("Active Links", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_ACTIVE_LINK) , "", 0)));
		tblListObjectInfo.AddRow(row);


		row.ClearCells();
		row.AddCell(CTableCell(nWebservice));
		row.AddCell(CTableCell(CWebUtil::Link("Web Services", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_WEBSERVICE), "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell(nAlGuide));
		row.AddCell(CTableCell(CWebUtil::Link("Active Link Guides", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_GUIDE), "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell(nFilterGuide));
		row.AddCell(CTableCell(CWebUtil::Link("Filter Guides", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_FILTER_GUIDE), "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell(nPacklist));
		row.AddCell(CTableCell(CWebUtil::Link("Packing Lists", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_PACK), "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell(nApplication));
		row.AddCell(CTableCell(CWebUtil::Link("Applications", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_APP), "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->escalationList.GetCount()));
		row.AddCell(CTableCell(CWebUtil::Link("Escalations", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_ESCALATION), "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->filterList.GetCount()));
		row.AddCell(CTableCell(CWebUtil::Link("Filters", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_FILTER), "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->groupList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Groups", CPageParams(PAGE_OVERVIEW, DATA_TYPE_GROUP), "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->menuList.GetCount()));
		row.AddCell(CTableCell(CWebUtil::Link("Menus", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CHAR_MENU), "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->roleList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Roles", CPageParams(PAGE_OVERVIEW, DATA_TYPE_ROLE), "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->schemaList.GetCount()));
		row.AddCell(CTableCell(CWebUtil::Link("Forms", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_SCHEMA), "", 0)));
		tblListObjectInfo.AddRow(row);

		row.ClearCells();
		row.AddCell(CTableCell((int)this->pInside->userList.size()));
		row.AddCell(CTableCell(CWebUtil::Link("Users", CPageParams(PAGE_OVERVIEW, DATA_TYPE_USER), "", 0)));
		tblListObjectInfo.AddRow(row);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		if (pInside->CompareServerVersion(7,5) >= 0)
		{
			row.ClearCells();
			row.AddCell(CTableCell(this->pInside->imageList.GetCount()));
			row.AddCell(CTableCell(CWebUtil::Link("Images", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_IMAGE), "", 0)));
			tblListObjectInfo.AddRow(row);
		}
#endif

		unsigned int nNumTotalObjects = (unsigned int)this->pInside->alList.GetCount();
		nNumTotalObjects += (unsigned int)this->pInside->containerList.GetCount();
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

//Copyright (C) 2012 John Luthgers | jls17
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
#include "DocCustomWorkflow.h"

CDocCustomWorkflow::CDocCustomWorkflow(void)
{
	pInside = CARInside::GetInstance();
}

CDocCustomWorkflow::~CDocCustomWorkflow(void)
{
}

void CDocCustomWorkflow::Documentation()
{
	try
	{
		CPageParams file(PAGE_CUSTOMWORKFLOW);
		rootLevel = file->GetRootLevel();

		CWebPage webpage(file->GetFileName(), "Workflow Customizations", rootLevel, pInside->appConfig);

		CTable tblRef("referenceList", "TblObjectList");
		tblRef.AddColumn(05, "Type");
		tblRef.AddColumn(05, "ObjType");
		tblRef.AddColumn(20, "Server object");
		tblRef.AddColumn(05, "Enabled");
		tblRef.AddColumn(10, "Changed");
		tblRef.AddColumn(10, "By");

		const CARSchemaList::ObjectRefList& customSchemas = pInside->schemaList.GetOverlayAndCustomWorkflow();
		const CARActiveLinkList::ObjectRefList& customActlinks = pInside->alList.GetOverlayAndCustomWorkflow();
		const CARFilterList::ObjectRefList& customFilters = pInside->filterList.GetOverlayAndCustomWorkflow();
		const CAREscalationList::ObjectRefList& customEscalations = pInside->escalationList.GetOverlayAndCustomWorkflow();
		const CARMenuList::ObjectRefList& customMenus = pInside->menuList.GetOverlayAndCustomWorkflow();
		const CARContainerList::ObjectRefList& customContainers = pInside->containerList.GetOverlayAndCustomWorkflow();
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		const CARImageList::ObjectRefList& customImages = pInside->imageList.GetOverlayAndCustomWorkflow();
#endif

		CARSchemaList::ObjectRefList::const_iterator curIt = customSchemas.begin();
		CARSchemaList::ObjectRefList::const_iterator endIt = customSchemas.end();
		for (; curIt != endIt; ++curIt)
		{
			CARSchema schema(*curIt);
			AddTableRow(tblRef, schema);
		}

		curIt = customActlinks.begin();
		endIt = customActlinks.end();
		for (; curIt != endIt; ++curIt)
		{
			CARActiveLink al(*curIt);
			AddTableRow(tblRef, al);
		}

		curIt = customFilters.begin();
		endIt = customFilters.end();
		for (; curIt != endIt; ++curIt)
		{
			CARFilter flt(*curIt);
			AddTableRow(tblRef, flt);
		}

		curIt = customEscalations.begin();
		endIt = customEscalations.end();
		for (; curIt != endIt; ++curIt)
		{
			CAREscalation esc(*curIt);
			AddTableRow(tblRef, esc);
		}

		curIt = customMenus.begin();
		endIt = customMenus.end();
		for (; curIt != endIt; ++curIt)
		{
			CARCharMenu mnu(*curIt);
			AddTableRow(tblRef, mnu);
		}

		curIt = customContainers.begin();
		endIt = customContainers.end();
		for (; curIt != endIt; ++curIt)
		{
			CARContainer cnt(*curIt);
			AddTableRow(tblRef, cnt);
		}

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		curIt = customImages.begin();
		endIt = customImages.end();
		for (; curIt != endIt; ++curIt)
		{
			CARImage img(*curIt);
			AddTableRow(tblRef, img);
		}
#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750

		webpage.AddContent(tblRef.ToXHtml());

		webpage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in CustomWorkflow: " << e.what() << endl;
	}	
}

void CDocCustomWorkflow::AddTableRow(CTable& tbl, CARSchema& schema)
{
	CTableRow row("cssStdRow");
	row.AddCell(CAREnum::GetOverlayType(schema.GetOverlayType()));
	row.AddCell(CAREnum::XmlStructItem(schema.GetServerObjectTypeXML()));
	row.AddCell(schema.GetURL(rootLevel));
	row.AddCell(""); // Enabled
	row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(schema.GetTimestamp())));
	row.AddCell(CTableCell(this->pInside->LinkToUser(schema.GetLastChanged(), rootLevel)));
	tbl.AddRow(row);
}

void CDocCustomWorkflow::AddTableRow(CTable& tbl, CARActiveLink& al)
{
	unsigned int enabled = al.GetEnabled();
	CARProplistHelper alProps(&al.GetPropList());

	CTableRow row("cssStdRow");
	row.AddCell(CAREnum::GetOverlayType(al.GetOverlayType()));
	row.AddCell(CAREnum::XmlStructItem(al.GetServerObjectTypeXML()));
	row.AddCell(al.GetURL(rootLevel));
	row.AddCell(CTableCell(CAREnum::ObjectEnable(enabled), (enabled == 0 ? "objStatusDisabled" : "")));
	row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(al.GetTimestamp())));
	row.AddCell(CTableCell(this->pInside->LinkToUser(al.GetLastChanged(), rootLevel)));
	tbl.AddRow(row);
}

void CDocCustomWorkflow::AddTableRow(CTable& tbl, CARFilter& flt)
{
	unsigned int enabled = flt.GetEnabled();

	CTableRow row("cssStdRow");
	row.AddCell(CAREnum::GetOverlayType(flt.GetOverlayType()));
	row.AddCell(CAREnum::XmlStructItem(flt.GetServerObjectTypeXML()));
	row.AddCell(flt.GetURL(rootLevel));
	row.AddCell(CTableCell(CAREnum::ObjectEnable(enabled), (enabled == 0 ? "objStatusDisabled" : "")));
	row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(flt.GetTimestamp())));
	row.AddCell(CTableCell(this->pInside->LinkToUser(flt.GetLastChanged(), rootLevel)));
	tbl.AddRow(row);
}

void CDocCustomWorkflow::AddTableRow(CTable& tbl, CAREscalation& escal)
{
	unsigned int enabled = escal.GetEnabled();

	CTableRow row("cssStdRow");
	row.AddCell(CAREnum::GetOverlayType(escal.GetOverlayType()));
	row.AddCell(CAREnum::XmlStructItem(escal.GetServerObjectTypeXML()));
	row.AddCell(escal.GetURL(rootLevel));
	row.AddCell(CTableCell(CAREnum::ObjectEnable(enabled), (enabled == 0 ? "objStatusDisabled" : "")));
	row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(escal.GetTimestamp())));
	row.AddCell(CTableCell(this->pInside->LinkToUser(escal.GetLastChanged(), rootLevel)));
	tbl.AddRow(row);
}

void CDocCustomWorkflow::AddTableRow(CTable& tbl, CARCharMenu& menu)
{
	CTableRow row("cssStdRow");
	row.AddCell(CAREnum::GetOverlayType(menu.GetOverlayType()));
	row.AddCell(CAREnum::XmlStructItem(menu.GetServerObjectTypeXML()));
	row.AddCell(menu.GetURL(rootLevel));
	row.AddCell(""); // Enabled
	row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(menu.GetTimestamp())));
	row.AddCell(CTableCell(this->pInside->LinkToUser(menu.GetLastChanged(), rootLevel)));
	tbl.AddRow(row);
}

void CDocCustomWorkflow::AddTableRow(CTable& tbl, CARContainer& container)
{
	CTableRow row("cssStdRow");
	row.AddCell(CAREnum::GetOverlayType(container.GetOverlayType()));
	row.AddCell(CAREnum::XmlStructItem(container.GetServerObjectTypeXML())); //TODO: maybe distinguish between each container type here
	row.AddCell(container.GetURL(rootLevel));
	row.AddCell(""); // Enabled
	row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(container.GetTimestamp())));
	row.AddCell(CTableCell(this->pInside->LinkToUser(container.GetLastChanged(), rootLevel)));
	tbl.AddRow(row);
}

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
void CDocCustomWorkflow::AddTableRow(CTable& tbl, CARImage& image)
{
	CTableRow row("cssStdRow");
	row.AddCell(CAREnum::GetOverlayType(image.GetOverlayType()));
	row.AddCell(CAREnum::XmlStructItem(image.GetServerObjectTypeXML()));
	row.AddCell(image.GetURL(rootLevel));
	row.AddCell(""); // Enabled
	row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(image.GetTimestamp())));
	row.AddCell(CTableCell(this->pInside->LinkToUser(image.GetLastChanged(), rootLevel)));
	tbl.AddRow(row);
}
#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
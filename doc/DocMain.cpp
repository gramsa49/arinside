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
#include "DocMain.h"
#include "../output/IFileStructure.h"
#include "../core/ARServerInfo.h"

CDocMain::CDocMain()
{
}

CDocMain::~CDocMain(void)
{
}

int CDocMain::Index()
{
	try
	{
		CWebPage webPage("index", "Documentation Index", 0, this->pInside->appConfig);	
		webPage.AddContent("This space is intentionally left blank...");
		return webPage.SaveInFolder("");
	}
	catch(exception& e)
	{
		cout << "EXCEPTION Index: " << e.what() << endl;
	}
	return 0;
}

void CDocMain::ServerInfoList()
{
	CPageParams file(PAGE_SERVER_INFO);

	try
	{
		int rootLevel = file->GetRootLevel();
		CWebPage webPage(file->GetFileName(), "Server details", rootLevel, this->pInside->appConfig);
		CTable tbl("serverDetailList", "TblObjectList");
		tbl.AddColumn(40, "Operation");
		tbl.AddColumn(60, "Value");

		unsigned int count = this->pInside->serverInfoList.GetCount();
		for (unsigned int index = 0; index < count; ++index)
		{	
			CARServerInfo srvInfo(index, CARServerInfo::INDEX);
			ARValueStruct* val = srvInfo.GetValue();

			CTableRow row("cssStdRow");
			row.AddCell(CTableCell(CAREnum::ServerInfoApiCall(srvInfo.GetOperation())));
			row.AddCell(CTableCell(CARValue::ValueToString(*val)));
			tbl.AddRow(row);		
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Server informations";
		tbl.description = tblDesc.str();

		webPage.AddContent(tbl.ToXHtml());

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ServerInfoList: " << e.what() << endl;
	}
}

string CDocMain::ShortMenu(string curCharacter, const CPageParams &curPage, std::vector<int>& objCountPerLetter)
{
	stringstream strm;
	try
	{
		strm << "<table><tr>" << endl;

		string strValue = objectNameFirstCharLetters; // "abcdefghijklmnopqrstuvwxyz0123456789#"
		for (unsigned int i = 0; i < strValue.size(); ++i) 
		{
			if(std::string(1, strValue.at(i)) != curCharacter)
			{
				// copy all page params over and change the page we want to link to
				CPageParams linkTo(curPage, strValue.at(i));
				if (objCountPerLetter[i] > 0)
				{
					strm << "<td>";
					strm << CWebUtil::Link( std::string(1, strValue.at(i)), linkTo , "", curPage->GetRootLevel());
				}
				else
				{
					strm << "<td class=\"disabledLetter\">";
					strm << strValue.at(i);
				}

				strm << "</td>" << endl;

			}
			else
				strm << "<td>" << std::string(1, strValue.at(i)) << "</td>" << endl;			
		}

		strm << "</tr></table>" << endl;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ShortMenu: " << e.what() << endl;
	}
	return strm.str();
}

void CDocMain::SchemaList(int nType, const CPageParams &file, string title, string searchChar, std::vector<int>& objCountPerLetter)
{
	try
	{
		int rootLevel = file->GetRootLevel();
		CSchemaTable tbl(*this->pInside);

		unsigned int schemaCount = this->pInside->schemaList.GetCount();
		for (unsigned int schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex)
		{	
			CARSchema schema(schemaIndex);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(schema))
				continue;
#endif

			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				if (nType == AR_SCHEMA_NONE)
				{
					// the first call to this function holds always "*" as search char. That's the
					// best time to sum up the object count per letter.
					string firstChar = schema.GetNameFirstChar();
					if (firstChar.empty()) firstChar = "*";
					int index = CARObject::GetFirstCharIndex(firstChar[0]);
					++(objCountPerLetter[index]);
				}

				if(nType == AR_SCHEMA_NONE || nType == schema.GetCompound().schemaType)  //Only check type
				{
					bInsert = true;
				}
			}		
			else if(searchChar == "#")
			{
				if(!schema.NameStandardFirstChar())
				{
					bInsert = true;		
				}
			}
			else
			{
				if(schema.GetNameFirstChar() == searchChar)
				{
					bInsert = true;
				}
			}

			if(bInsert)
			{
				tbl.AddRow(schema, rootLevel);
			}
		}

		if (tbl.NumRows() > 0 || searchChar == "*")
		{
			CWebPage webPage(file->GetFileName(), title, rootLevel, this->pInside->appConfig);

			stringstream strmTmp;
			strmTmp << CWebUtil::LinkToSchemaIndex(tbl.NumRows(), rootLevel);

			if(nType != AR_SCHEMA_NONE)
				strmTmp << MenuSeparator << CAREnum::SchemaType(nType);

			strmTmp << ShortMenu(searchChar, file, objCountPerLetter);

			tbl.SetDescription(strmTmp.str());
			webPage.AddContent(tbl.Print());

			webPage.SaveInFolder(file->GetPath());
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION SchemaList: " << e.what() << endl;
	}
}

void CDocMain::ActiveLinkList(string searchChar, std::vector<int>& objCountPerLetter)
{
	if (searchChar.size() != 1) return;

	CPageParams file(searchChar[0], AR_STRUCT_ITEM_XML_ACTIVE_LINK);

	try
	{
		int rootLevel = file->GetRootLevel();

		CAlTable tbl(*this->pInside);

		unsigned int alCount = this->pInside->alList.GetCount();
		
		for (unsigned int alIdx = 0; alIdx < alCount; ++alIdx)
		{
			CARActiveLink actLink(alIdx);
			bool bInsert = false;

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(actLink))
				continue;
#endif

			if(searchChar == "*")  //All objecte
			{
				// the first call to this function holds always "*" as search char. That's the
				// best time to sum up the object count per letter.
				string firstChar = actLink.GetNameFirstChar();
				if (firstChar.empty()) firstChar = "*";
				int index = CARObject::GetFirstCharIndex(firstChar[0]);
				++(objCountPerLetter[index]);
				bInsert = true;
			}
			else if(searchChar == "#")
			{
				if (!actLink.NameStandardFirstChar())
					bInsert = true;
			}
			else
			{
				if (actLink.GetNameFirstChar() == searchChar)
					bInsert = true;
			}

			if(bInsert)
			{
				tbl.AddRow(alIdx, rootLevel);
			}
		}

		if (tbl.NumRows() > 0 || searchChar == "*") 
		{
			CWebPage webPage(file->GetFileName(), "Active Link List", rootLevel, this->pInside->appConfig);

			stringstream strmTmp;
			strmTmp << CWebUtil::LinkToActiveLinkIndex(tbl.NumRows(), rootLevel) << ShortMenu(searchChar, file, objCountPerLetter);

			tbl.SetDescription(strmTmp.str());
			webPage.AddContent(tbl.Print());

			webPage.SaveInFolder(file->GetPath());
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ActiveLinkList: " << e.what() << endl;
	}
}

void CDocMain::ActiveLinkActionList()
{
	CPageParams file(PAGE_ACTION_OVERVIEW, AR_STRUCT_ITEM_XML_ACTIVE_LINK);
	
	try
	{
		int rootLevel = file->GetRootLevel();
		CWebPage webPage(file->GetFileName(), "Active Link Actions", rootLevel, this->pInside->appConfig);

		CTable tbl("alList", "TblObjectList");

		stringstream strmTmp;
		strmTmp << CWebUtil::ImageTag("active_link.gif", rootLevel) << CWebUtil::Link("Active Links", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_ACTIVE_LINK), "", rootLevel) << " with a specified action in If/Else list:";

		tbl.description = strmTmp.str();
		tbl.AddColumn(100, "Active Link Action (Items count if/else)");

		//Search all possible action type
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
#define LAST_ACTIVE_LINK_ACTION 18
#else
#define LAST_ACTIVE_LINK_ACTION 17
#endif
		for(int nActionType=AR_ACTIVE_LINK_ACTION_MACRO; nActionType<=LAST_ACTIVE_LINK_ACTION; nActionType++)
		{		
			int nCountIf = 0;
			int nCountElse = 0;

			//Create a new webpage for every action
			ActiveLinkActionDetails(nActionType, nCountIf, nCountElse);

			strmTmp.str("");
			strmTmp << CWebUtil::Link(CAREnum::ActiveLinkAction(nActionType), CPageParams(PAGE_ACTION_OBJLIST, AR_STRUCT_ITEM_XML_ACTIVE_LINK, nActionType), "doc.gif", rootLevel) << " (" << nCountIf << "/" << nCountElse << ")";

			CTableRow row("");
			row.AddCell(CTableCell(strmTmp.str()));
			tbl.AddRow(row);
		}

		webPage.AddContent(tbl.ToXHtml());

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ActiveLinkActionList: " << e.what() << endl;
	}
}

void CDocMain::ActiveLinkActionDetails(int nActionType, int &ifCount, int &elseCount)
{
	CPageParams file(PAGE_ACTION_OBJLIST, AR_STRUCT_ITEM_XML_ACTIVE_LINK, nActionType);
	try
	{		
		int rootLevel = 1;

		//stringstream strmTmp;
		//strmTmp << "index_action_" << nActionType;
		CWebPage webPage(file->GetFileName(), "Active Link Actions", rootLevel, this->pInside->appConfig);

		CAlTable *tbl = new CAlTable(*this->pInside);	

		unsigned int alCount = pInside->alList.GetCount();
		for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex )
		{
			CARActiveLink al(alIndex);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			// Hide overlaid objects on the list
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(al))
				continue;
#endif

			int nActionExists = 0;
			const ARActiveLinkActionList &ifActions = al.GetIfActions();
			const ARActiveLinkActionList &elseActions = al.GetElseActions();

			//actionList
			for(unsigned int nAction=0; nAction < ifActions.numItems; nAction++)
			{
				if(ifActions.actionList[nAction].action == nActionType)
				{
					++ifCount;
					++nActionExists;
				}
			}		

			//elseList
			for(unsigned int nAction=0; nAction < elseActions.numItems; nAction++)
			{
				if(elseActions.actionList[nAction].action == nActionType)
				{		
					++elseCount;			
					++nActionExists;
				}
			}		

			if(nActionExists > 0)
			{
				tbl->AddRow(alIndex, rootLevel);
			}
		}

		stringstream strmTmp;
		strmTmp << CWebUtil::Link("Active Links", CPageParams(PAGE_ACTION_OVERVIEW, AR_STRUCT_ITEM_XML_ACTIVE_LINK), "active_link.gif", rootLevel) << " with " << CAREnum::ActiveLinkAction(nActionType) << " action";
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ActiveLinkActionDetails: " << e.what() << endl;
	}
}


void CDocMain::FilterList(string searchChar, std::vector<int> &objCountPerLetter)
{
	CPageParams file((unsigned int)searchChar.at(0), AR_STRUCT_ITEM_XML_FILTER);

	try
	{
		int rootLevel = file->GetRootLevel();
		CFilterTable tbl(*this->pInside);

		unsigned int filterCount = this->pInside->filterList.GetCount();

		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex )
		{
			CARFilter filter(filterIndex);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(filter))
				continue;
#endif

			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				// the first call to this function holds always "*" as search char. That's the
				// best time to sum up the object count per letter.
				string firstChar = filter.GetNameFirstChar();
				if (firstChar.empty()) firstChar = "*";
				int index = CARObject::GetFirstCharIndex(firstChar[0]);
				++(objCountPerLetter[index]);
				bInsert = true;
			}
			else if(searchChar == "#")
			{
				if(filter.NameStandardFirstChar())
					bInsert = true;		
			}
			else
			{
				if(filter.GetNameFirstChar() == searchChar)
					bInsert = true;
			}

			if(bInsert)
			{
				tbl.AddRow(filterIndex, rootLevel);
			}
		}

		if (tbl.NumRows() > 0 || searchChar == "*")
		{
			CWebPage webPage(file->GetFileName(), "Filter List", file->GetRootLevel(), this->pInside->appConfig);

			stringstream strmTmp;
			strmTmp << CWebUtil::LinkToFilterIndex(tbl.NumRows(), rootLevel) << ShortMenu(searchChar, file, objCountPerLetter);

			tbl.SetDescription(strmTmp.str());
			webPage.AddContent(tbl.Print());

			webPage.SaveInFolder(file->GetPath());
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION FilterList: " << e.what() << endl;
	}
}

void CDocMain::FilterActionList()
{
	CPageParams file(PAGE_ACTION_OVERVIEW, AR_STRUCT_ITEM_XML_FILTER);

	try
	{
		int rootLevel = file->GetRootLevel();
		CWebPage webPage(file->GetFileName(), "Filter Actions", rootLevel, this->pInside->appConfig);

		CTable tbl("filterList", "TblObjectList");

		stringstream strmTmp;
		strmTmp << CWebUtil::ImageTag("filter.gif", rootLevel) << CWebUtil::Link("Filter", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_FILTER), "", rootLevel) << " with a specified action in If/Else list:";

		tbl.description = strmTmp.str();
		tbl.AddColumn(100, "Filter Action (Items count if/else)");

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750 // Version 7.5 and higher
#define LAST_FILTER_ACTION 12
#else
#define LAST_FILTER_ACTION 11
#endif

		//Search all possible actions
		for(int nActionType=AR_FILTER_ACTION_NOTIFY; nActionType<=LAST_FILTER_ACTION; nActionType++)
		{		
			int nCountIf = 0;
			int nCountElse = 0;

			// Create a new webpage for every action
			FilterActionDetails(nActionType, nCountIf, nCountElse);

			strmTmp.str("");
			strmTmp << CWebUtil::Link(CAREnum::FilterAction(nActionType), CPageParams(PAGE_ACTION_OBJLIST, AR_STRUCT_ITEM_XML_FILTER, nActionType), "doc.gif", rootLevel) << " (" << nCountIf << "/" << nCountElse << ")";

			CTableRow row("");
			row.AddCell(CTableCell(strmTmp.str()));		
			tbl.AddRow(row);	
		}

		webPage.AddContent(tbl.ToXHtml());

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION FilterActionList: " << e.what() << endl;
	}
}

void CDocMain::FilterActionDetails(int nActionType, int &ifCount, int &elseCount)
{
	CPageParams file(PAGE_ACTION_OBJLIST, AR_STRUCT_ITEM_XML_FILTER, nActionType);

	try
	{
		int rootLevel = file->GetRootLevel();

		//stringstream strmTmp;
		//strmTmp << "index_action_" << nActionType;
		CWebPage webPage(file->GetFileName(), "Filter Actions", rootLevel, this->pInside->appConfig);

		CFilterTable *tbl = new CFilterTable(*this->pInside);

		unsigned int filterCount = pInside->filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex )
		{	
			CARFilter flt(filterIndex);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			// Hide overlaid objects on the list
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(flt))
				continue;
#endif

			int nActionExists = 0;
			const ARFilterActionList &ifActions = flt.GetIfActions();
			const ARFilterActionList &elseActions = flt.GetElseActions();

			//actionList
			for(unsigned int nAction=0; nAction < ifActions.numItems; ++nAction)
			{
				if (ifActions.actionList[nAction].action == nActionType)
				{		
					++ifCount;
					nActionExists++;
				}
			}

			//elseList
			for(unsigned int nAction=0; nAction < elseActions.numItems; ++nAction)
			{
				if (elseActions.actionList[nAction].action == nActionType)
				{
					++elseCount;
					nActionExists++;
				}
			}

			if(nActionExists > 0)
			{
				tbl->AddRow(filterIndex, rootLevel);
			}
		}

		stringstream strmTmp;
		strmTmp << CWebUtil::Link("Filter", CPageParams(PAGE_ACTION_OVERVIEW, AR_STRUCT_ITEM_XML_FILTER), "filter.gif", rootLevel) << " with " << CAREnum::FilterAction(nActionType) << " action";
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION FilterActionDetails: " << e.what() << endl;
	}
}

void CDocMain::FilterErrorHandlers()
{
	CPageParams file(PAGE_FILTER_ERRORHANDLERS);

	try
	{
		int rootLevel = file->GetRootLevel();
		CWebPage webPage(file->GetFileName(), "Filter Error Handlers", rootLevel, this->pInside->appConfig);

		CFilterTable tbl(*pInside);

		unsigned int filterCount = pInside->filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex)
		{
			CARFilter flt(filterIndex);
			
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			// Hide overlaid objects on the list
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(flt))
				continue;
#endif

			if (flt.ErrorCallers().size() > 0)
			{
				tbl.AddRow(filterIndex, rootLevel);
			}
		}

		stringstream strm;
		strm << CWebUtil::LinkToFilterIndex(tbl.NumRows(), rootLevel) << " used as Error Handler";

		tbl.SetDescription(strm.str());
		webPage.AddContent(tbl.Print());

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION FilterErrorHandlers: " << e.what() << endl;
	}
}

void CDocMain::EscalationList(string searchChar, std::vector<int> &objCountPerLetter)
{
	CPageParams file(searchChar.at(0), AR_STRUCT_ITEM_XML_ESCALATION);

	try
	{
		int rootLevel = file->GetRootLevel();
		CEscalTable tbl(*this->pInside);

		unsigned int escalCount = pInside->escalationList.GetCount();
		for (unsigned int escalIndex = 0; escalIndex < escalCount; ++escalIndex)
		{
			CAREscalation escalation(escalIndex);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(escalation))
				continue;
#endif

			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				// the first call to this function holds always "*" as search char. That's the
				// best time to sum up the object count per letter.
				string firstChar = escalation.GetNameFirstChar();
				if (firstChar.empty()) firstChar = "*";
				int index = CARObject::GetFirstCharIndex(firstChar[0]);
				++(objCountPerLetter[index]);
				bInsert = true;
			}
			else if(searchChar == "#")
			{
				if(!escalation.NameStandardFirstChar())
					bInsert = true;		
			}
			else
			{
				if(escalation.GetNameFirstChar() == searchChar)
					bInsert = true;
			}

			if(bInsert)
			{
				tbl.AddRow(escalIndex, rootLevel);
			}
		}

		if (tbl.NumRows() > 0 || searchChar == "*")
		{
			CWebPage webPage(file->GetFileName(), "Escalation List", rootLevel, this->pInside->appConfig);

			stringstream strmTmp;
			strmTmp << CWebUtil::LinkToEscalationIndex(tbl.NumRows(), rootLevel) << ShortMenu(searchChar, file, objCountPerLetter);

			tbl.SetDescription(strmTmp.str());
			webPage.AddContent(tbl.Print());

			webPage.SaveInFolder(file->GetPath());
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION EscalationList: " << e.what() << endl;
	}
}

void CDocMain::EscalationActionList()
{
	CPageParams file(PAGE_ACTION_OVERVIEW, AR_STRUCT_ITEM_XML_ESCALATION);

	try
	{
		int rootLevel = 1;
		CWebPage webPage(file->GetFileName(), "Escalation Actions", rootLevel, this->pInside->appConfig);

		CTable tbl("escalList", "TblObjectList");

		stringstream strmTmp;
		strmTmp << CWebUtil::ImageTag("escalation.gif", rootLevel) << CWebUtil::Link("Escalation", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_ESCALATION), "", rootLevel) << " with a specified action in If/Else list:";

		tbl.description = strmTmp.str();
		tbl.AddColumn(100, "Escalation Action (Items count if/else)");

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750 // Version 7.5 and higher
#define LAST_ESCLATION_ACTION 12
#else
#define LAST_ESCLATION_ACTION 11
#endif

		//Search all possible actions
		for(int nActionType=AR_FILTER_ACTION_NOTIFY; nActionType<=LAST_ESCLATION_ACTION; nActionType++)
		{
			switch (nActionType)
			{
			case AR_FILTER_ACTION_GOTOACTION:
			case AR_FILTER_ACTION_CALLGUIDE:
			case AR_FILTER_ACTION_EXITGUIDE:
			case AR_FILTER_ACTION_GOTOGUIDELABEL:
				continue;
			}

			int nCountIf = 0;
			int nCountElse = 0;

			//Create a new webpage for every action
			EscalationActionDetails(nActionType, nCountIf, nCountElse);

			strmTmp.str("");
			strmTmp << CWebUtil::Link(CAREnum::FilterAction(nActionType), CPageParams(PAGE_ACTION_OBJLIST, AR_STRUCT_ITEM_XML_ESCALATION, nActionType), "doc.gif", 1) << " (" << nCountIf << "/" << nCountElse << ")";

			CTableRow row("");
			row.AddCell(CTableCell(strmTmp.str()));
			tbl.AddRow(row);
		}

		webPage.AddContent(tbl.ToXHtml());

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION EscalationActionList: " << e.what() << endl;
	}
}


void CDocMain::EscalationActionDetails(int nActionType, int &ifCount, int &elseCount)
{
	CPageParams file(PAGE_ACTION_OBJLIST, AR_STRUCT_ITEM_XML_ESCALATION, nActionType);

	try
	{
		int rootLevel = file->GetRootLevel();

		//stringstream strmTmp;
		//strmTmp << "index_action_" << nActionType;
		CWebPage webPage(file->GetFileName(), "Escalation Actions", rootLevel, this->pInside->appConfig);

		CEscalTable *tbl = new CEscalTable(*this->pInside);

		unsigned int escalCount = pInside->escalationList.GetCount();
		for (unsigned int escalIndex = 0; escalIndex < escalCount; ++escalIndex)
		{	
			CAREscalation esc(escalIndex);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			// Hide overlaid objects on the list
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(esc))
				continue;
#endif

			int nActionExists = 0;
			const ARFilterActionList &ifActions = esc.GetIfActions();
			const ARFilterActionList &elseActions = esc.GetElseActions();

			//actionList
			for(unsigned int nAction=0; nAction < ifActions.numItems; ++nAction)
			{
				if(ifActions.actionList[nAction].action == nActionType)
				{
					++ifCount;
					++nActionExists;
				}
			}		

			//elseList
			for(unsigned int nAction=0; nAction < elseActions.numItems; ++nAction)
			{
				if(elseActions.actionList[nAction].action == nActionType)
				{
					++elseCount;
					nActionExists++;
				}
			}		

			if(nActionExists > 0)
			{
				tbl->AddRow(escalIndex, rootLevel);
			}
		}

		stringstream strmTmp;
		strmTmp << CWebUtil::Link("Escalation", CPageParams(PAGE_ACTION_OVERVIEW, AR_STRUCT_ITEM_XML_ESCALATION), "escalation.gif", rootLevel) << " with " << CAREnum::FilterAction(nActionType) << " action";
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION EscalationActionDetails: " << e.what() << endl;
	}
}


void CDocMain::CharMenuList(string searchChar, std::vector<int> &objCountPerLetter)
{
	CPageParams file(searchChar[0], AR_STRUCT_ITEM_XML_CHAR_MENU);

	try
	{
		int rootLevel = file->GetRootLevel();
		CMenuTable tbl(*this->pInside);

		unsigned int menuCount = this->pInside->menuList.GetCount();
		for ( unsigned int menuIndex = 0; menuIndex < menuCount; ++menuIndex )
		{	
			CARCharMenu menu(menuIndex);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(menu))
				continue;
#endif

			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				// the first call to this function holds always "*" as search char. That's the
				// best time to sum up the object count per letter.
				string firstChar = menu.GetNameFirstChar();
				if (firstChar.empty()) firstChar = "*";
				int index = CARObject::GetFirstCharIndex(firstChar[0]);
				++(objCountPerLetter[index]);
				bInsert = true;
			}
			else if(searchChar == "#")
			{
				if(!menu.NameStandardFirstChar())
				{
					bInsert = true;
				}
			}
			else
			{
				if(menu.GetNameFirstChar() == searchChar)
				{
					bInsert = true;
				}
			}

			if(bInsert)
			{
				tbl.AddRow(menu, rootLevel);
			}
		}

		if (tbl.NumRows() > 0 || searchChar == "*")
		{
			CWebPage webPage(file->GetFileName(), "CharMenu List", 1, this->pInside->appConfig);

			stringstream strmTmp;
			strmTmp << CWebUtil::LinkToMenuIndex(tbl.NumRows(), rootLevel) << ShortMenu(searchChar, file, objCountPerLetter);
			
			tbl.SetDescription(strmTmp.str());
			webPage.AddContent(tbl.Print());
			webPage.AddContent("(!) Menu is not attached to a character field and no Active Link \"Change Field\" Action sets the menu to a field.");

			webPage.SaveInFolder(file->GetPath());
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CharMenuList: " << e.what() << endl;
	}
}

void CDocMain::ContainerList(int nType, string title, string searchChar, std::vector<int>& objCountPerLetter)
{
	unsigned int page = (unsigned int)searchChar[0];
	CPageParams file(page, AR_STRUCT_ITEM_XML_CONTAINER, nType);
	try
	{
		int rootLevel = file->GetRootLevel();
		CContainerTable tbl(*this->pInside);

		unsigned int cntCount = this->pInside->containerList.GetCount();
		for ( unsigned int cntIndex = 0; cntIndex < cntCount; ++cntIndex )
		{	
			CARContainer cont(cntIndex);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(cont))
				continue;
#endif

			if (cont.GetType() == nType)	// the type must match
			{
				bool bInsert = false;
				if(searchChar == "*")  //All objecte
				{
					// the first call to this function holds alwasy "*" as search char. That's the
					// best time to sum up the object count per letter.
					string firstChar = cont.GetNameFirstChar();
					if (firstChar.empty()) firstChar = "*";
					int index = CARObject::GetFirstCharIndex(firstChar[0]);
					++(objCountPerLetter[index]);
					bInsert = true;
				}
				else if(searchChar == "#")
				{
					if(!cont.NameStandardFirstChar())
					{
						bInsert = true;
					}
				}
				else
				{
					if(cont.GetNameFirstChar() == searchChar)
					{
						bInsert = true;
					}
				}

				if(bInsert)
				{
					tbl.AddRow(cont, rootLevel);
				}
			}
		}

		if (tbl.NumRows() > 0 || searchChar == "*")
		{
			CWebPage webPage(file->GetFileName(), title, rootLevel, this->pInside->appConfig);

			stringstream strmTmp;
			strmTmp << CWebUtil::LinkToContainer(tbl.NumRows(), rootLevel, nType);
			strmTmp << ShortMenu(searchChar, file, objCountPerLetter);
			
			tbl.SetDescription(strmTmp.str());
			webPage.AddContent(tbl.Print());

			webPage.AddContent("(!) No Active Link / Filter \"CallGuide\" Action uses this Guide.");

			webPage.SaveInFolder(file->GetPath());
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ContainerList: " << e.what() << endl;
	}
}

void CDocMain::RoleList(string searchChar, std::vector<int>& objCountPerLetter)
{
	unsigned int page = (unsigned int)searchChar[0];
	CPageParams file(page, AR_STRUCT_ITEM_XML_ROLE);

	try
	{
		int rootLevel = file->GetRootLevel();
		CRoleTable tbl(*this->pInside);

		unsigned int roleCount = this->pInside->roleList.GetCount();
		for (unsigned int roleIndex = 0; roleIndex < roleCount; ++roleIndex)
		{	
			CARRole role(roleIndex);

			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				// the first call to this function holds always "*" as search char. That's the
				// best time to sum up the object count per letter.
				string firstChar = role.GetNameFirstChar();
				if (firstChar.empty()) firstChar = "*";
				int index = CARObject::GetFirstCharIndex(firstChar[0]);
				++(objCountPerLetter[index]);
				bInsert = true;
			}
			else if(searchChar == "#")
			{
				if(!role.NameStandardFirstChar())
				{
					bInsert = true;
				}
			}
			else
			{
				if(role.GetNameFirstChar() == searchChar)
				{
					bInsert = true;
				}
			}

			if(bInsert)
			{	
				tbl.AddRow(role, rootLevel);
			}
		}

		if (tbl.NumRows() > 0 || searchChar == "*")
		{
			CWebPage webPage(file->GetFileName(), "Role List", rootLevel, this->pInside->appConfig);

			stringstream strmTmp;
			strmTmp << CWebUtil::LinkToRoleIndex(tbl.NumRows(), rootLevel) << ShortMenu(searchChar, file, objCountPerLetter);

			tbl.SetDescription(strmTmp.str());
			webPage.AddContent(tbl.Print());

			webPage.SaveInFolder(file->GetPath());
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION GroupList: " << e.what() << endl;
	}
}

void CDocMain::ImageList(string searchChar, std::vector<int> &objCountPerLetter)
{
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	// server version older than 7.5 ?? then there are no files to generate
	if (pInside->CompareServerVersion(7,5) < 0) return;
	if (searchChar.size() != 1) return;

	unsigned int page = (unsigned int)searchChar[0];
	CPageParams file(page, AR_STRUCT_ITEM_XML_IMAGE);

	try
	{
		int rootLevel = file->GetRootLevel();
		CImageTable imgTable(*this->pInside);

		unsigned int len = this->pInside->imageList.GetCount();
		for (unsigned int idx = 0; idx < len; ++idx)
		{
			CARImage img(idx);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(img))
				continue;
#endif

			bool bInsert = false;
			
			if (searchChar == "*") // All objects
			{
				// the first call to this function holds always "*" as search char. That's the
				// best time to sum up the object count per letter.
				string firstChar = img.GetNameFirstChar();
				if (firstChar.empty()) firstChar = "*";
				int index = CARObject::GetFirstCharIndex(firstChar[0]);
				++(objCountPerLetter[index]);
				bInsert = true;
			}
			else if (searchChar == "#")
			{
				if (!img.NameStandardFirstChar())
					bInsert = true;
			}
			else 
			{
				if (img.GetNameFirstChar() == searchChar)
					bInsert = true;
			}

			if (bInsert)
				imgTable.AddRow(idx, rootLevel);
		}

		if (imgTable.NumRows() > 0 || searchChar == "*")
		{
			CWebPage webPage(file->GetFileName(), "Image List", rootLevel, this->pInside->appConfig);

			stringstream strmTmp;
			strmTmp << CWebUtil::LinkToImageIndex(imgTable.NumRows(), rootLevel);
			strmTmp << ShortMenu(searchChar, file, objCountPerLetter);
			
			imgTable.SetDescription(strmTmp.str());
			
			webPage.AddContent(imgTable.Print());
			webPage.SaveInFolder(file->GetPath());
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ImageList: " << e.what() << endl;
	}
#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
}

void CDocMain::GroupList(string searchChar, std::vector<int>& objCountPerLetter)
{
	unsigned int page = (unsigned int)searchChar[0];
	CPageParams file(page, AR_STRUCT_ITEM_XML_GROUP);

	try
	{
		int rootLevel = file->GetRootLevel();
		CGroupTable tbl(*this->pInside);

		unsigned int groupCount = pInside->groupList.GetCount();
		for (unsigned int groupIndex = 0; groupIndex < groupCount; ++groupIndex)
		{	
			CARGroup grp(groupIndex);

			bool bInsert = false;
			if(searchChar == "*")  //All objects
			{
				// the first call to this function holds always "*" as search char. That's the
				// best time to sum up the object count per letter.
				string firstChar = grp.GetNameFirstChar();
				if (firstChar.empty()) firstChar = "*";
				int index = CARObject::GetFirstCharIndex(firstChar[0]);
				++(objCountPerLetter[index]);
				bInsert = true;
			}
			else if(searchChar == "#")
			{
				if(!grp.NameStandardFirstChar())
				{
					bInsert = true;
				}
			}
			else
			{
				if(grp.GetNameFirstChar() == searchChar)
				{
					bInsert = true;
				}
			}

			if(bInsert)
			{	
				tbl.AddRow("", grp.GetGroupId(), rootLevel);
			}
		}

		if (tbl.NumRows() > 0 || searchChar == "*")
		{
			CWebPage webPage(file->GetFileName(), "Group List", rootLevel, this->pInside->appConfig);

			stringstream strmTmp;
			strmTmp << CWebUtil::LinkToGroupIndex(tbl.NumRows(), rootLevel) << ShortMenu(searchChar, file, objCountPerLetter);

			tbl.SetDescription(strmTmp.str());
			webPage.AddContent(tbl.Print());

			webPage.SaveInFolder(file->GetPath());
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION GroupList: " << e.what() << endl;
	}
}

void CDocMain::UserList(string searchChar, std::vector<int>& objCountPerLetter)
{
	unsigned int page = (unsigned int)searchChar[0];
	CPageParams file(page, AR_STRUCT_ITEM_XML_USER);

	try
	{
		int rootLevel = file->GetRootLevel();
		CUserTable tbl(*this->pInside);

		unsigned int userCount = this->pInside->userList.GetCount();
		for (unsigned int userIndex = 0; userIndex < userCount; ++userIndex)
		{	
			CARUser user(userIndex);

			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				// the first call to this function holds always "*" as search char. That's the
				// best time to sum up the object count per letter.
				string firstChar = user.GetNameFirstChar();
				if (firstChar.empty()) firstChar = "*";
				int index = CARObject::GetFirstCharIndex(firstChar[0]);
				++(objCountPerLetter[index]);
				bInsert = true;
			}
			else if(searchChar == "#")
			{
				if(!user.NameStandardFirstChar())
				{
					bInsert = true;
				}
			}
			else
			{
				if(user.GetNameFirstChar() == searchChar)
				{
					bInsert = true;
				}
			}

			if(bInsert)
			{	
				tbl.AddRow(user, rootLevel);
			}
		}

		if (tbl.NumRows() > 0 || searchChar == "*")
		{
			CWebPage webPage(file->GetFileName(), "User List", rootLevel, this->pInside->appConfig);

			stringstream strmTmp;
			strmTmp << CWebUtil::LinkToUserIndex(tbl.NumRows(), rootLevel) << ShortMenu(searchChar, file, objCountPerLetter);

			tbl.SetDescription(strmTmp.str());
			webPage.AddContent(tbl.Print());

			webPage.SaveInFolder(file->GetPath());
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION UserList: " << e.what() << endl;
	}
}

void CDocMain::GlobalFieldList()
{
	CPageParams file(PAGE_GLOBALFIELS);

	try
	{
		int rootLevel = file->GetRootLevel();
		CWebPage webPage(file->GetFileName(), "Global Fields", rootLevel, this->pInside->appConfig);	

		// outer table
		CTable tbl("fieldListAll", "TblObjectList");
		tbl.AddColumn(20, "GlobalFieldId");
		tbl.AddColumn(80, "References");

		// inner table; reused per global field id
		CTable innerTbl("refList", "TblObjectList");
		innerTbl.AddColumn(50, "Schema Name");
		innerTbl.AddColumn(50, "Field Name");

		int currentGlobalFieldId = 0;

		//Search all global fields
		list<CARGlobalField>::iterator listIter = this->pInside->globalFieldList.begin();
		list<CARGlobalField>::iterator listEnd = this->pInside->globalFieldList.end();
		for (; listIter != listEnd; ++listIter )
		{	
			CARGlobalField &glField = (*listIter);
			CARField fld(glField.schemaInsideId, glField.fieldId);

			if (currentGlobalFieldId != fld.GetFieldId())
			{
				// add the previous global field to the table before resetting
				if (currentGlobalFieldId != 0)
					AddGlobalFieldRow(tbl, currentGlobalFieldId, innerTbl);

				// reset for current field
				currentGlobalFieldId = fld.GetFieldId();
				innerTbl.ClearRows();
			}

			CTableRow innerTblRow("");
			innerTblRow.AddCellList(
				this->pInside->LinkToSchema(glField.schemaInsideId, rootLevel),
				this->pInside->LinkToField(glField.schemaInsideId, fld.GetFieldId(), rootLevel));

			innerTbl.AddRow(innerTblRow);
		}
		if (innerTbl.NumRows() > 0)
			AddGlobalFieldRow(tbl, currentGlobalFieldId, innerTbl);

		stringstream strmTmp;
		strmTmp << CWebUtil::ImageTag("doc.gif", rootLevel) << tbl.NumRows() << " Global Fields" << endl;
		tbl.description = strmTmp.str();

		webPage.AddContent(tbl.ToXHtml());
		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION GlobalFieldList: " << e.what() << endl;
	}
}

void CDocMain::AddGlobalFieldRow(OUTPUT::CTable &tbl, int fieldId, OUTPUT::CTable &fields)
{
	CTableRow row("");
	row.AddCell(CTableCell(fieldId));
	row.AddCell(fields.ToXHtml());
	tbl.AddRow(row);
}

void CDocMain::MessageList()
{
	CPageParams file(PAGE_MESSAGES);

	try
	{
		int rootLevel = file->GetRootLevel();
		int curMsgNumber = 0;
		list<CMessageItem> listMsgItem;

		//Search all activelinks
		unsigned int alCount = pInside->alList.GetCount();
		for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex)
		{
			CARActiveLink al(alIndex);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(al))
				continue;
#endif

			CARSchema schema(al.GetSchemaList().u.schemaList[0].nameList[0]);

			for (unsigned int ifElse = 0; ifElse < 2; ++ifElse)
			{
				const ARActiveLinkActionList &actions = (ifElse == 0 ? al.GetIfActions() : al.GetElseActions());

				//actionList
				for(unsigned int nAction=0; nAction < actions.numItems; nAction++)
				{
					if(actions.actionList[nAction].action == AR_ACTIVE_LINK_ACTION_MESSAGE)
					{
						stringstream strmTmp;
						strmTmp.str("");
						strmTmp << (ifElse == 0 ? "If" : "Else") << "-Action " << nAction;

						ARMessageStruct &msg = actions.actionList[nAction].u.message;

						CMessageItem msgItem;
						msgItem.msgDetails = strmTmp.str();
						msgItem.msgNumber = msg.messageNum;
						msgItem.msgText = (schema.Exists() ? pInside->TextFindFields(msg.messageText, "$", schema.GetInsideId(), rootLevel, true, NULL) : msg.messageText);
						msgItem.msgType = msg.messageType;
						msgItem.objectLink = al.GetURL(rootLevel);
						listMsgItem.push_back(msgItem);
					}
				}
			}
		}


		//Search all filter
		unsigned int filterCount = pInside->filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex )
		{
			CARFilter flt(filterIndex);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(flt))
				continue;
#endif

			CARSchema schema(flt.GetSchemaList().u.schemaList[0].nameList[0]);

			for (unsigned int ifElse = 0; ifElse < 2; ++ifElse)
			{
				const ARFilterActionList &actions = (ifElse == 0 ? flt.GetIfActions() : flt.GetElseActions());

				//actionList
				for(unsigned int nAction=0; nAction < actions.numItems; ++nAction)
				{
					if(actions.actionList[nAction].action == AR_FILTER_ACTION_MESSAGE)
					{
						stringstream strmTmp;
						strmTmp.str("");
						strmTmp << (ifElse == 0 ? "If" : "Else") << "-Action " << nAction;

						const ARFilterStatusStruct &msg = actions.actionList[nAction].u.message;

						CMessageItem msgItem;
						msgItem.msgDetails = strmTmp.str();
						msgItem.msgNumber = msg.messageNum;
						msgItem.msgText = (schema.Exists() ? pInside->TextFindFields(msg.messageText, "$", schema.GetInsideId(), rootLevel, true, NULL) : msg.messageText);
						msgItem.msgType = msg.messageType;
						msgItem.objectLink = flt.GetURL(rootLevel);
						listMsgItem.push_back(msgItem);
					}
				}
			}
		}


		CWebPage webPage(file->GetFileName(), "Messages", rootLevel, this->pInside->appConfig);
		if(listMsgItem.size() > 0)
		{
			Sort(listMsgItem);

			//Print all Messages		
			CTable tbl("fieldListAll", "TblObjectList");
			tbl.AddColumn(5, "Number");
			tbl.AddColumn(25, "Object Name");
			tbl.AddColumn(5, "Details");
			tbl.AddColumn(5, "Type");	
			tbl.AddColumn(60, "Text");

			list<CMessageItem>::iterator msgIter;	
			CMessageItem *msg;
			for ( msgIter = listMsgItem.begin(); msgIter != listMsgItem.end(); msgIter++ )
			{
				msg = &(*msgIter);

				CTableRow row("");
				row.AddCell( CTableCell(msg->msgNumber));
				row.AddCell( CTableCell(msg->objectLink));
				row.AddCell( CTableCell(msg->msgDetails));
				row.AddCell( CTableCell(CAREnum::MessageType(msg->msgType)));
				row.AddCell( CTableCell(msg->msgText));

				tbl.AddRow(row);
			}

			webPage.AddContent(tbl.ToXHtml());
		}
		else
		{
			webPage.AddContent("No messages loaded.");
		}

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION MessageList: " << e.what() << endl;
	}
}

void CDocMain::NotificationList()
{
	CPageParams file(PAGE_NOTIFICATIONS);

	try
	{
		int rootLevel = file->GetRootLevel();

		//Print all Messages		
		CTable tbl("fieldListAll", "TblObjectList");
		tbl.AddColumn(25, "Object Name");
		tbl.AddColumn(5, "Details");
		tbl.AddColumn(5, "Type");	
		tbl.AddColumn(60, "Text");

		//Search all filter
		unsigned int filterCount = pInside->filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex)
		{
			CARFilter flt(filterIndex);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			// Hide overlaid objects on the list
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(flt))
				continue;
#endif

			CARSchema schema(flt.GetSchemaList().u.schemaList[0].nameList[0]);

			for (unsigned int ifElse = 0; ifElse < 2; ++ifElse)
			{
				const ARFilterActionList &actions = (ifElse == 0 ? flt.GetIfActions() : flt.GetElseActions());

				//actionList
				for(unsigned int nAction=0; nAction < actions.numItems; ++nAction)
				{
					if(actions.actionList[nAction].action == AR_FILTER_ACTION_NOTIFY)
					{
						if (!schema.Exists()) 
							continue;

						const ARFilterActionNotify& notifyAction = actions.actionList[nAction].u.notify;

						stringstream strm;
						strm << (ifElse == 0 ? "If" : "Else") << "-Action " << nAction;

						stringstream text;
						if (notifyAction.subjectText != NULL && notifyAction.subjectText[0] != 0)
							text << pInside->TextFindFields(notifyAction.subjectText, "$", schema.GetInsideId(), rootLevel, true, NULL) << "<BR/>";
						if (notifyAction.notifyText != NULL)
							text << pInside->TextFindFields(notifyAction.notifyText, "$", schema.GetInsideId(), rootLevel, true, NULL);

						CTableRow row("");
						row.AddCell(flt.GetURL(rootLevel));
						row.AddCell(strm.str());
						row.AddCell(CAREnum::NotifyMechanism(notifyAction.notifyMechanism)); // Type of Notifcation
						row.AddCell(text.str());

						tbl.AddRow(row);
					}
				}
			}
		}

		//Search all escalations
		unsigned int escalCount = pInside->escalationList.GetCount();
		for (unsigned int escalIndex = 0; escalIndex < escalCount; ++escalIndex)
		{
			CAREscalation esc(escalIndex);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			// Hide overlaid objects on the list
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(esc))
				continue;
#endif

			CARSchema schema(esc.GetSchemaList().u.schemaList[0].nameList[0]);

			for (unsigned int ifElse = 0; ifElse < 2; ++ifElse)
			{
				const ARFilterActionList &actions = (ifElse == 0 ? esc.GetIfActions() : esc.GetElseActions());

				//actionList
				for(unsigned int nAction=0; nAction < actions.numItems; ++nAction)
				{
					if(actions.actionList[nAction].action == AR_FILTER_ACTION_NOTIFY)
					{
						if (!schema.Exists()) 
							continue;

						const ARFilterActionNotify& notifyAction = actions.actionList[nAction].u.notify;

						stringstream strm;
						strm << (ifElse == 0 ? "If" : "Else") << "-Action " << nAction;

						stringstream text;
						if (notifyAction.subjectText != NULL && notifyAction.subjectText[0] != 0)
							text << pInside->TextFindFields(notifyAction.subjectText, "$", schema.GetInsideId(), rootLevel, true, NULL) << "<BR/>";
						if (notifyAction.notifyText != NULL)
							text << pInside->TextFindFields(notifyAction.notifyText, "$", schema.GetInsideId(), rootLevel, true, NULL);

						CTableRow row("");
						row.AddCell(esc.GetURL(rootLevel));
						row.AddCell(strm.str());
						row.AddCell(CAREnum::NotifyMechanism(notifyAction.notifyMechanism)); // Type of Notifcation
						row.AddCell(text.str());

						tbl.AddRow(row);
					}
				}
			}
		}

		CWebPage webPage(file->GetFileName(), "Notifications", rootLevel, this->pInside->appConfig);
		if(tbl.NumRows() > 0)
		{
			webPage.AddContent(tbl.ToXHtml());
		}
		else
		{
			webPage.AddContent("No notifications loaded.");
		}

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION NotificationList: " << e.what() << endl;
	}
}

void CDocMain::Sort(list<CMessageItem> &listResult)
{
	listResult.sort(SortByMsgNum);
}

bool CDocMain::SortByMsgNum(const CMessageItem& t1, const CMessageItem& t2 )
{	
	return ( t1.msgNumber < t2.msgNumber);
}

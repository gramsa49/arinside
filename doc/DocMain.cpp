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

CDocMain::CDocMain(CARInside &arIn)
{
	this->pInside = &arIn;
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
	try
	{
		int rootLevel = 1;
		CWebPage webPage("server", "Server details", rootLevel, this->pInside->appConfig);
		CTable tbl("serverDetailList", "TblObjectList");
		tbl.AddColumn(40, "API CALL");
		tbl.AddColumn(60, "Result");

		list<CARServerInfoItem>::iterator listIter;
		CARServerInfoItem *infoItem;

		for ( listIter = this->pInside->serverInfoList.begin(); listIter != this->pInside->serverInfoList.end(); listIter++ )
		{	
			infoItem = &(*listIter);

			CTableRow row("cssStdRow");		
			row.AddCell(CTableCell(CAREnum::ServerInfoApiCall(infoItem->apiCall)));				
			row.AddCell(CTableCell(infoItem->value));
			tbl.AddRow(row);		
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Server informations";
		tbl.description = tblDesc.str();

		webPage.AddContent(tbl.ToXHtml());
		tbl.Clear();

		webPage.SaveInFolder("other");	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ServerInfoList: " << e.what() << endl;
	}
}

string CDocMain::ShortMenu(string curCharacter)
{
	stringstream strm;
	try
	{
		strm << "<table><tr>" << endl;

		string strValue = "abcdefghijklmnopqrstuvwxyz0123456789";
		for (unsigned int i = 0; i < strValue.size(); ++i) 
		{	
			stringstream tmp;
			tmp << "index_" << std::string(1, strValue.at(i));

			if(std::string(1, strValue.at(i)) != curCharacter)
				strm << "<td>" << CWebUtil::Link( std::string(1, strValue.at(i)), CWebUtil::DocName(tmp.str()), "", 1) << "</td>" << endl;
			else
				strm << "<td>" << std::string(1, strValue.at(i)) << "</td>" << endl;			
		}

		if(curCharacter == "?")
			strm << "<td>" << "#" << "</td>" << endl;
		else
			strm << "<td>" << CWebUtil::Link( "#", CWebUtil::DocName("index_other"), "", 1) << "</td>" << endl;	


		strm << "</tr></table>" << endl;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ShortMenu: " << e.what() << endl;
	}
	return strm.str();
}

void CDocMain::SchemaList(int nType, string fileName, string title, string searchChar)
{
	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, title, rootLevel, this->pInside->appConfig);

		CSchemaTable *tbl = new CSchemaTable(*this->pInside);

		unsigned int schemaCount = this->pInside->schemaList.GetCount();
		for (unsigned int schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex)
		{	
			CARSchema schema(schemaIndex);

			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				if(nType == AR_SCHEMA_NONE || nType == schema.GetCompound().schemaType)  //Only check type
				{
					bInsert = true;
				}
			}		
			else if(searchChar != "#" && searchChar != "*")
			{
				if(schema.GetNameFirstChar() == searchChar)
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


			if(bInsert)
			{
				tbl->AddRow(schema, rootLevel);
			}
		}

		stringstream strmTmp;
		strmTmp.str("");
		strmTmp << CWebUtil::LinkToSchemaIndex(tbl->NumRows(), rootLevel);

		if(nType != AR_SCHEMA_NONE)
			strmTmp << MenuSeparator << CAREnum::SchemaType(nType);

		strmTmp << ShortMenu(searchChar);
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder("schema");	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION SchemaList: " << e.what() << endl;
	}
}

void CDocMain::ActiveLinkList(string fileName, string searchChar)
{
	if (searchChar.size() != 1) return;

	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, "Active Link List", 1, this->pInside->appConfig);

		CAlTable *tbl = new CAlTable(*this->pInside);

		unsigned int alCount = this->pInside->alList.GetCount();
		
		for (unsigned int alIdx = 0; alIdx < alCount; ++alIdx)
		{	
			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				bInsert = true;
			}
			else if(searchChar == "#")
			{
				if (!CARObject::NameStandardFirstChar(pInside->alList.ActiveLinkGetName(alIdx)[0]))
					bInsert = true;
			}
			else
			{
				if (searchChar[0] == tolower(pInside->alList.ActiveLinkGetName(alIdx)[0]))
					bInsert = true;
			}

			if(bInsert)
			{
				tbl->AddRow(alIdx, rootLevel);
			}
		}

		stringstream strmTmp;
		strmTmp.str("");
		strmTmp << CWebUtil::LinkToActiveLinkIndex(tbl->NumRows(), rootLevel) << ShortMenu(searchChar);

		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder("active_link");
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ActiveLinkList: " << e.what() << endl;
	}
}

void CDocMain::ActiveLinkActionList(string fileName)
{
	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, "Active Link Actions", rootLevel, this->pInside->appConfig);

		CTable tbl("alList", "TblObjectList");

		stringstream strmTmp;
		strmTmp << CWebUtil::ImageTag("active_link.gif", rootLevel) << CWebUtil::Link("Active Links", CWebUtil::DocName("index"), "", 1) << " with a specified action in If/Else list:";

		tbl.description = strmTmp.str();
		tbl.AddColumn(100, "Active Link Action (Items count if/else)");

		//Search all possible action type
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
#define LAST_ACTIVE_LINK_ACTION 18
#else
#define LAST_ACTIVE_LINK_ACTION 17
#endif
		for(int nActionType=0; nActionType<=LAST_ACTIVE_LINK_ACTION; nActionType++)
		{		
			int nCountIf = 0;
			int nCountElse = 0;

			//Create a new webpage for every action
			ActiveLinkActionDetails(nActionType, nCountIf, nCountElse);

			stringstream linkto;
			linkto << "index_action_" << nActionType;

			strmTmp.str("");
			strmTmp << CWebUtil::Link(CAREnum::ActiveLinkAction(nActionType), CWebUtil::DocName(linkto.str()), "doc.gif", 1) << " (" << nCountIf << "/" << nCountElse << ")";

			CTableRow row("");
			row.AddCell(CTableCell(strmTmp.str()));
			tbl.AddRow(row);
		}

		webPage.AddContent(tbl.ToXHtml());
		tbl.Clear();

		webPage.SaveInFolder("active_link");
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ActiveLinkActionList: " << e.what() << endl;
	}
}

void CDocMain::ActiveLinkActionDetails(int nActionType, int &ifCount, int &elseCount)
{
	try
	{		
		int rootLevel = 1;

		stringstream strmTmp;
		strmTmp << "index_action_" << nActionType;
		CWebPage webPage(strmTmp.str(), "Active Link Actions", rootLevel, this->pInside->appConfig);

		CAlTable *tbl = new CAlTable(*this->pInside);	

		unsigned int alCount = pInside->alList.GetCount();
		for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex )
		{
			int nActionExists = 0;
			const ARActiveLinkActionList &ifActions = pInside->alList.ActiveLinkGetIfActions(alIndex);
			const ARActiveLinkActionList &elseActions = pInside->alList.ActiveLinkGetElseActions(alIndex);

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

		strmTmp.str("");
		strmTmp << CWebUtil::Link("Active Links", CWebUtil::DocName("index_action"), "active_link.gif", 1) << " with " << CAREnum::ActiveLinkAction(nActionType) << " action";
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder("active_link");
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ActiveLinkActionDetails: " << e.what() << endl;
	}
}


void CDocMain::FilterList(string fileName, string searchChar)
{
	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, "Filter List", 1, this->pInside->appConfig);

		CFilterTable *tbl = new CFilterTable(*this->pInside);

		unsigned int filterCount = this->pInside->filterList.GetCount();

		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex )
		{	
			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				bInsert = true;
			}
			else if(searchChar == "#")
			{
				if(CARObject::NameStandardFirstChar(pInside->filterList.FilterGetName(filterIndex)[0]))
					bInsert = true;		
			}
			else
			{
				if(searchChar[0] == tolower(pInside->filterList.FilterGetName(filterIndex)[0]))
					bInsert = true;
			}

			if(bInsert)
			{
				tbl->AddRow(filterIndex, rootLevel);
			}
		}

		stringstream strmTmp;
		strmTmp.str("");
		strmTmp << CWebUtil::LinkToFilterIndex(tbl->NumRows(), rootLevel) << ShortMenu(searchChar);
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder("filter");
	}
	catch(exception& e)
	{
		cout << "EXCEPTION FilterList: " << e.what() << endl;
	}
}

void CDocMain::FilterActionList(string fileName)
{
	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, "Filter Actions", rootLevel, this->pInside->appConfig);

		CTable tbl("filterList", "TblObjectList");

		stringstream strmTmp;
		strmTmp << CWebUtil::ImageTag("filter.gif", rootLevel) << CWebUtil::Link("Filter", CWebUtil::DocName("index"), "", rootLevel) << " with a specified action in If/Else list:";

		tbl.description = strmTmp.str();
		tbl.AddColumn(100, "Filter Action (Items count if/else)");

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750 // Version 7.5 and higher
#define LAST_FILTER_ACTION 12
#else
#define LAST_FILTER_ACTION 11
#endif

		//Search all possible actions
		for(int nActionType=0; nActionType<=LAST_FILTER_ACTION; nActionType++)
		{		
			int nCountIf = 0;
			int nCountElse = 0;

			stringstream linkto;
			linkto << "index_action_" << nActionType;

			// Create a new webpage for every action
			FilterActionDetails(nActionType, nCountIf, nCountElse);

			strmTmp.str("");
			strmTmp << CWebUtil::Link(CAREnum::FilterAction(nActionType), CWebUtil::DocName(linkto.str()), "doc.gif", 1) << " (" << nCountIf << "/" << nCountElse << ")";

			CTableRow row("");
			row.AddCell(CTableCell(strmTmp.str()));		
			tbl.AddRow(row);	
		}

		webPage.AddContent(tbl.ToXHtml());
		tbl.Clear();

		webPage.SaveInFolder("filter");
	}
	catch(exception& e)
	{
		cout << "EXCEPTION FilterActionList: " << e.what() << endl;
	}
}

void CDocMain::FilterActionDetails(int nActionType, int &ifCount, int &elseCount)
{
	try
	{
		int rootLevel = 1;

		stringstream strmTmp;
		strmTmp << "index_action_" << nActionType;
		CWebPage webPage(strmTmp.str(), "Filter Actions", rootLevel, this->pInside->appConfig);

		CFilterTable *tbl = new CFilterTable(*this->pInside);

		unsigned int filterCount = pInside->filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex )
		{	
			int nActionExists = 0;
			const ARFilterActionList &ifActions = pInside->filterList.FilterGetIfActions(filterIndex);
			const ARFilterActionList &elseActions = pInside->filterList.FilterGetElseActions(filterIndex);

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

		strmTmp.str("");
		strmTmp << CWebUtil::Link("Filter", CWebUtil::DocName("index_action"), "filter.gif", rootLevel) << " with " << CAREnum::FilterAction(nActionType) << " action";
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder("filter");
	}
	catch(exception& e)
	{
		cout << "EXCEPTION FilterActionDetails: " << e.what() << endl;
	}
}

void CDocMain::EscalationList(string fileName, string searchChar)
{
	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, "Escalation List", 1, this->pInside->appConfig);

		CEscalTable *tbl = new CEscalTable(*this->pInside);

		unsigned int escalCount = pInside->escalationList.GetCount();
		for (unsigned int escalIndex = 0; escalIndex < escalCount; ++escalIndex)
		{	
			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				bInsert = true;
			}
			else if(searchChar == "#")
			{
				if(!CARObject::NameStandardFirstChar(pInside->escalationList.EscalationGetName(escalIndex)[0]))
					bInsert = true;		
			}
			else
			{
				if(searchChar[0] == tolower(pInside->escalationList.EscalationGetName(escalIndex)[0]))
					bInsert = true;
			}

			if(bInsert)
			{
				tbl->AddRow(escalIndex, rootLevel);			
			}
		}

		stringstream strmTmp;
		strmTmp.str("");
		strmTmp << CWebUtil::LinkToEscalationIndex(tbl->NumRows(), rootLevel) << ShortMenu(searchChar);
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder("escalation");	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION EscalationList: " << e.what() << endl;
	}
}

void CDocMain::EscalationActionList(string fileName)
{
	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, "Escalation Actions", rootLevel, this->pInside->appConfig);

		CTable tbl("escalList", "TblObjectList");

		stringstream strmTmp;
		strmTmp << CWebUtil::ImageTag("escalation.gif", rootLevel) << CWebUtil::Link("Escalation", CWebUtil::DocName("index"), "", rootLevel) << " with a specified action in If/Else list:";

		tbl.description = strmTmp.str();
		tbl.AddColumn(100, "Escalation Action (Items count if/else)");


		//Search all possible actions
		for(int nActionType=0; nActionType<11; nActionType++)
		{		
			int nCountIf = 0;
			int nCountElse = 0;

			//Create a new webpage for every action
			EscalationActionDetails(nActionType, nCountIf, nCountElse);

			stringstream linkto;
			linkto << "index_action_" << nActionType;

			strmTmp.str("");
			strmTmp << CWebUtil::Link(CAREnum::FilterAction(nActionType), CWebUtil::DocName(linkto.str()), "doc.gif", 1) << " (" << nCountIf << "/" << nCountElse << ")";


			CTableRow row("");
			row.AddCell(CTableCell(strmTmp.str()));
			tbl.AddRow(row);
		}

		webPage.AddContent(tbl.ToXHtml());
		tbl.Clear();

		webPage.SaveInFolder("escalation");
	}
	catch(exception& e)
	{
		cout << "EXCEPTION EscalationActionList: " << e.what() << endl;
	}
}


void CDocMain::EscalationActionDetails(int nActionType, int &ifCount, int &elseCount)
{
	try
	{
		int rootLevel = 1;

		stringstream strmTmp;
		strmTmp << "index_action_" << nActionType;
		CWebPage webPage(strmTmp.str(), "Escalation Actions", rootLevel, this->pInside->appConfig);

		CEscalTable *tbl = new CEscalTable(*this->pInside);

		unsigned int escalCount = pInside->escalationList.GetCount();
		for (unsigned int escalIndex = 0; escalIndex < escalCount; ++escalIndex)
		{	
			int nActionExists = 0;
			const ARFilterActionList &ifActions = pInside->escalationList.EscalationGetIfActions(escalIndex);
			const ARFilterActionList &elseActions = pInside->escalationList.EscalationGetElseActions(escalIndex);

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

		strmTmp.str("");
		strmTmp << CWebUtil::Link("Escalation", CWebUtil::DocName("index_action"), "escalation.gif", rootLevel) << " with " << CAREnum::FilterAction(nActionType) << " action";
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder("escalation");
	}
	catch(exception& e)
	{
		cout << "EXCEPTION EscalationActionDetails: " << e.what() << endl;
	}
}


void CDocMain::CharMenuList(string fileName, string searchChar)
{
	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, "CharMenu List", 1, this->pInside->appConfig);

		CMenuTable *tbl = new CMenuTable(*this->pInside);

		list<CARCharMenu>::iterator menuIter;
		CARCharMenu *menu;
		for ( menuIter = this->pInside->menuList.begin(); menuIter != this->pInside->menuList.end(); menuIter++ )
		{	
			menu = &(*menuIter);

			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				bInsert = true;
			}
			else if(searchChar != "#" && searchChar != "*")
			{
				if(menu->GetNameFirstChar() == searchChar)
				{
					bInsert = true;
				}
			}
			else if(searchChar == "#")
			{
				if(!menu->NameStandardFirstChar())
				{
					bInsert = true;		
				}
			}

			if(bInsert)
			{
				tbl->AddRow(*menu, rootLevel);
			}
		}

		stringstream strmTmp;
		strmTmp.str("");
		strmTmp << CWebUtil::LinkToMenuIndex(tbl->NumRows(), rootLevel) << ShortMenu(searchChar);
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.AddContent("(!) Menu is not attached to a character field and no Active Link \"Change Field\" Action sets the menu to a field.");

		webPage.SaveInFolder("menu");
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CharMenuList: " << e.what() << endl;
	}
}

void CDocMain::ContainerList(int nType, string fileName, string title, string searchChar)
{
	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, title, rootLevel, this->pInside->appConfig);

		CContainerTable *tbl = new CContainerTable(*this->pInside);

		list<CARContainer>::iterator contIter;		
		for ( contIter = this->pInside->containerList.begin(); contIter != this->pInside->containerList.end(); contIter++ )
		{	
			CARContainer *cont = &(*contIter);

			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				if(nType == cont->type)  //Only check type
				{
					bInsert = true;
				}
			}
			else if(searchChar != "#" && searchChar != "*")
			{
				if(nType == cont->type && cont->GetNameFirstChar() == searchChar)
				{
					bInsert = true;
				}
			}
			else if(searchChar == "#")
			{
				if(nType == cont->type && !cont->NameStandardFirstChar())
				{
					bInsert = true;		
				}
			}

			if(bInsert)
			{
				tbl->AddRow(*cont, rootLevel);
			}
		}

		stringstream strmTmp;
		strmTmp << CWebUtil::LinkToContainer(tbl->NumRows(), rootLevel, nType);
		strmTmp << ShortMenu(searchChar);
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.AddContent("(!) No Active Link / Filter \"CallGuide\" Action uses this Guide.");

		webPage.SaveInFolder(CAREnum::ContainerDir(nType));
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ContainerList: " << e.what() << endl;
	}
}

void CDocMain::RoleList(string fileName, string searchChar)
{
	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, "Role List", rootLevel, this->pInside->appConfig);

		CRoleTable *tbl = new CRoleTable(*this->pInside);

		list<CARRole>::iterator listIter;				
		for ( listIter = this->pInside->roleList.begin(); listIter != this->pInside->roleList.end(); listIter++ )
		{	
			CARRole *role = &(*listIter);

			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				bInsert = true;
			}
			else if(searchChar != "#" && searchChar != "*")
			{
				if(role->GetNameFirstChar() == searchChar)
				{
					bInsert = true;
				}
			}
			else if(searchChar == "#")
			{
				if(!role->NameStandardFirstChar())
				{
					bInsert = true;		
				}
			}

			if(bInsert)
			{	
				tbl->AddRow(*role, rootLevel);
			}
		}

		stringstream strmTmp;
		strmTmp << CWebUtil::ImageTag("doc.gif", rootLevel) << tbl->NumRows() << " Roles " << ShortMenu(searchChar);
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder("role");	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION GroupList: " << e.what() << endl;
	}
}

void CDocMain::ImageList(string fileName, string searchChar)
{
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	// server version older than 7.5 ?? then there are no files to generate
	if (pInside->CompareServerVersion(7,5) < 0) return;

	if (searchChar.size() != 1) return;

	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, "Image List", rootLevel, this->pInside->appConfig);
		CImageTable imgTable(*this->pInside);

		unsigned int len = this->pInside->imageList.GetCount();
		for (unsigned int idx = 0; idx < len; ++idx)
		{
			bool bInsert = false;
			
			if (searchChar == "*") // All objects
			{
				bInsert = true;
			}
			else if (searchChar == "#")
			{
				if (!CARObject::NameStandardFirstChar(pInside->imageList.ImageGetName(idx)[0]))
					bInsert = true;
			}
			else 
			{
				if (searchChar[0] == tolower(pInside->imageList.ImageGetName(idx)[0]))
					bInsert = true;
			}

			if (bInsert)
				imgTable.AddRow(idx, rootLevel);
		}

		stringstream strmTmp;
		strmTmp << CWebUtil::LinkToImageIndex(imgTable.NumRows(), rootLevel);
		strmTmp << ShortMenu(searchChar);
		
		imgTable.SetDescription(strmTmp.str());
		
		webPage.AddContent(imgTable.Print());
		webPage.SaveInFolder("image");
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ImageList: " << e.what() << endl;
	}
#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
}

void CDocMain::GroupList(string fileName, string searchChar)
{
	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, "Group List", rootLevel, this->pInside->appConfig);

		CGroupTable *tbl = new CGroupTable(*this->pInside);

		list<CARGroup>::iterator listIter;
		list<CARGroup>::iterator endIt = this->pInside->groupList.end();
		for (listIter = this->pInside->groupList.begin(); listIter != endIt; ++listIter)
		{	
			CARGroup *grp = &(*listIter);

			bool bInsert = false;
			if(searchChar == "*")  //All objects
			{
				bInsert = true;
			}
			else if(searchChar != "#" && searchChar != "*")
			{
				if(grp->GetNameFirstChar() == searchChar)
				{
					bInsert = true;
				}
			}
			else if(searchChar == "#")
			{
				if(!grp->NameStandardFirstChar())
				{
					bInsert = true;		
				}
			}

			if(bInsert)
			{	
				tbl->AddRow("", grp->groupId, rootLevel);
			}
		}

		stringstream strmTmp;
		strmTmp << CWebUtil::ImageTag("group.gif", rootLevel) << tbl->NumRows() << " Groups " << ShortMenu(searchChar);
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder("group");	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION GroupList: " << e.what() << endl;
	}
}

void CDocMain::UserList(string fileName, string searchChar)
{
	try
	{
		int rootLevel = 1;
		CWebPage webPage(fileName, "User List", rootLevel, this->pInside->appConfig);

		CUserTable *tbl = new CUserTable(*this->pInside);

		list<CARUser>::iterator listIter;
		list<CARUser>::iterator endIt = this->pInside->userList.end();
		for (listIter = this->pInside->userList.begin(); listIter != endIt; ++listIter)
		{	
			CARUser *user = &(*listIter);

			bool bInsert = false;
			if(searchChar == "*")  //All objecte
			{
				bInsert = true;
			}
			else if(searchChar != "#" && searchChar != "*")
			{
				if(user->GetNameFirstChar() == searchChar)
				{
					bInsert = true;
				}
			}
			else if(searchChar == "#")
			{
				if(!user->NameStandardFirstChar())
				{
					bInsert = true;		
				}
			}

			if(bInsert)
			{	
				tbl->AddRow(*user, rootLevel);
			}
		}

		stringstream strmTmp;
		strmTmp << CWebUtil::ImageTag("user.gif", rootLevel) << tbl->NumRows() << " Users " << ShortMenu(searchChar);;
		tbl->SetDescription(strmTmp.str());

		webPage.AddContent(tbl->Print());
		delete tbl;

		webPage.SaveInFolder("user");
	}
	catch(exception& e)
	{
		cout << "EXCEPTION UserList: " << e.what() << endl;
	}
}

void CDocMain::GlobalFieldList()
{
	try
	{
		int rootLevel = 1;

		//List of unique globoal fields
		list<CARGlobalField> uiGlobalFieldList;

		//Search all global fields
		list<CARGlobalField>::iterator listIter;
		for ( listIter = this->pInside->globalFieldList.begin(); listIter != this->pInside->globalFieldList.end(); listIter++ )
		{	
			CARGlobalField *glField = &(*listIter);

			bool bInList = false;
			list<CARGlobalField>::iterator tmpListIter;
			CARGlobalField *tmpGlField;
			for ( tmpListIter = uiGlobalFieldList.begin(); tmpListIter != uiGlobalFieldList.end(); tmpListIter++ )
			{
				tmpGlField = &(*tmpListIter);
				if(tmpGlField->arGlobalFieldId == glField->arGlobalFieldId)
				{				
					bInList = true;
				}
			}		

			//Add field to ui list if not exist
			if(!bInList)
			{
				CARGlobalField uiGlobalField(glField->schemaInsideId, glField->fieldInsideId, glField->arGlobalFieldId);
				uiGlobalFieldList.push_back(uiGlobalField);
			}
		}


		CWebPage webPage("global_field_list", "Global Fields", rootLevel, this->pInside->appConfig);	

		if(uiGlobalFieldList.size() > 0)
		{
			Sort(uiGlobalFieldList);

			CTable tbl("fieldListAll", "TblObjectList");
			tbl.AddColumn(20, "GlobalFieldId");
			tbl.AddColumn(80, "References");

			list<CARGlobalField>::iterator uiListIter;
			CARGlobalField *uiField;
			for ( uiListIter = uiGlobalFieldList.begin(); uiListIter != uiGlobalFieldList.end(); uiListIter++ )
			{
				uiField = &(*uiListIter);


				CTable innerTbl("refList", "TblObjectList");
				innerTbl.AddColumn(50, "Schema Name");
				innerTbl.AddColumn(50, "Field Name");

				list<CARGlobalField>::iterator glListIter;
				for ( glListIter = this->pInside->globalFieldList.begin(); glListIter != this->pInside->globalFieldList.end(); glListIter++ )
				{	
					CARGlobalField *glField = &(*glListIter);

					if(glField->arGlobalFieldId == uiField->arGlobalFieldId)
					{
						CTableRow innerTblRow("");
						innerTblRow.AddCellList(CTableCell(this->pInside->LinkToSchema(glField->schemaInsideId, rootLevel)),
							CTableCell(this->pInside->LinkToField(glField->schemaInsideId, glField->fieldInsideId, rootLevel) ));

						innerTbl.AddRow(innerTblRow);

					}
				}

				CTableRow tblRow("");
				tblRow.AddCellList(CTableCell(uiField->arGlobalFieldId), CTableCell(innerTbl.ToXHtml()));
				innerTbl.Clear();

				tbl.AddRow(tblRow);
			}

			stringstream strmTmp;
			strmTmp << CWebUtil::ImageTag("doc.gif", rootLevel) << (unsigned int)this->pInside->globalFieldList.size() << " Global Fields" << endl;
			tbl.description = strmTmp.str();

			webPage.AddContent(tbl.ToXHtml());
			tbl.Clear();
		}
		else
		{
			webPage.AddContent("No global fields loaded.");
		}

		webPage.SaveInFolder("other");	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION GlobalFieldList: " << e.what() << endl;
	}
}

void CDocMain::Sort(list<CARGlobalField> &listResult)
{
	listResult.sort(SortByGlobalFieldId);
}

bool CDocMain::SortByGlobalFieldId(const CARGlobalField& t1, const CARGlobalField& t2 )
{	
	return ( t1.arGlobalFieldId < t2.arGlobalFieldId);
}

void CDocMain::MessageList()
{
	try
	{
		int rootLevel = 1;	
		int curMsgNumber = 0;
		list<CMessageItem> listMsgItem;
		listMsgItem.clear();

		//Search all activelinks
		unsigned int alCount = pInside->alList.GetCount();
		for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex)
		{
			const ARActiveLinkActionList &ifActions = pInside->alList.ActiveLinkGetIfActions(alIndex);
			const ARActiveLinkActionList &elseActions = pInside->alList.ActiveLinkGetElseActions(alIndex);

			//actionList
			for(unsigned int nAction=0; nAction < ifActions.numItems; nAction++)
			{
				if(ifActions.actionList[nAction].action == AR_ACTIVE_LINK_ACTION_MESSAGE)
				{
					stringstream strmTmp;
					strmTmp.str("");
					strmTmp << "If-Action "<<nAction;

					ARMessageStruct &msg = ifActions.actionList[nAction].u.message;

					CMessageItem *msgItem = new CMessageItem();
					msgItem->msgDetails = strmTmp.str();
					msgItem->msgNumber = msg.messageNum;
					msgItem->msgText = msg.messageText;
					msgItem->msgType = msg.messageType;
					msgItem->objectLink = pInside->alList.ActiveLinkGetURL(alIndex, rootLevel);
					listMsgItem.push_back(*msgItem);
					delete msgItem;
				}
			}

			//elseList
			for(unsigned int nAction=0; nAction < elseActions.numItems; nAction++)
			{
				if(elseActions.actionList[nAction].action == AR_ACTIVE_LINK_ACTION_MESSAGE)
				{
					stringstream strmTmp;
					strmTmp.str("");
					strmTmp << "Else-Action "<<nAction;

					ARMessageStruct &msg = elseActions.actionList[nAction].u.message;

					CMessageItem *msgItem = new CMessageItem();
					msgItem->msgDetails = strmTmp.str();
					msgItem->msgNumber = msg.messageNum;
					msgItem->msgText = msg.messageText;
					msgItem->msgType = msg.messageType;
					msgItem->objectLink = pInside->alList.ActiveLinkGetURL(alIndex, rootLevel);
					listMsgItem.push_back(*msgItem);
					delete msgItem;
				}
			}
		}


		//Search all filter
		unsigned int filterCount = pInside->filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex )
		{
			const ARFilterActionList &ifActions = pInside->filterList.FilterGetIfActions(filterIndex);
			const ARFilterActionList &elseActions = pInside->filterList.FilterGetElseActions(filterIndex);

			//actionList
			for(unsigned int nAction=0; nAction < ifActions.numItems; ++nAction)
			{
				if(ifActions.actionList[nAction].action == AR_FILTER_ACTION_MESSAGE)
				{
					stringstream strmTmp;
					strmTmp.str("");
					strmTmp << "If-Action " << nAction;

					const ARFilterStatusStruct &msg = ifActions.actionList[nAction].u.message;

					CMessageItem *msgItem = new CMessageItem();
					msgItem->msgDetails = strmTmp.str();
					msgItem->msgNumber = msg.messageNum;
					msgItem->msgText = msg.messageText;
					msgItem->msgType = msg.messageType;
					msgItem->objectLink = pInside->filterList.FilterGetURL(filterIndex, rootLevel);
					listMsgItem.push_back(*msgItem);
					delete msgItem;
				}
			}

			//elseList
			for(unsigned int nAction=0; nAction < elseActions.numItems; ++nAction)
			{
				if(elseActions.actionList[nAction].action == AR_FILTER_ACTION_MESSAGE)
				{
					stringstream strmTmp;
					strmTmp.str("");
					strmTmp << "Else-Action "<<nAction;

					const ARFilterStatusStruct &msg = elseActions.actionList[nAction].u.message;

					CMessageItem *msgItem = new CMessageItem();
					msgItem->msgDetails = strmTmp.str();
					msgItem->msgNumber = msg.messageNum;
					msgItem->msgText = msg.messageText;
					msgItem->msgType = msg.messageType;
					msgItem->objectLink = pInside->filterList.FilterGetURL(filterIndex, rootLevel);
					listMsgItem.push_back(*msgItem);
					delete msgItem;
				}
			}
		}


		CWebPage webPage("message_list", "Messages", rootLevel, this->pInside->appConfig);
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
			tbl.Clear();
			listMsgItem.clear();
		}
		else
		{
			webPage.AddContent("No messages loaded.");
		}

		webPage.SaveInFolder("other");	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION MessageList: " << e.what() << endl;
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

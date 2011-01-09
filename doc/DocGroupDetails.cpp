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
#include "DocGroupDetails.h"

CDocGroupDetails::CDocGroupDetails(CARGroup &arGroup)
{
	this->pGroup = &arGroup;
}

CDocGroupDetails::~CDocGroupDetails(void)
{
}

void CDocGroupDetails::Documentation()
{
	CPageParams file(PAGE_DETAILS, pGroup);
	this->rootLevel = file->GetRootLevel();

	try
	{
		CWebPage webPage(file->GetFileName(), this->pGroup->groupName, this->rootLevel, this->pInside->appConfig);

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToGroupIndex(this->rootLevel) << MenuSeparator << CWebUtil::ObjName(pGroup->groupName) << endl;
		contHeadStrm << " (Id: " << pGroup->groupId << ")";
		webPage.AddContentHead(contHeadStrm.str());

		//Group details
		CTable tbl("groupDetails", "TblObjectList");
		tbl.AddColumn(30, "Description");
		tbl.AddColumn(70, "Value");

		CTableRow tblRow("");
		tblRow.AddCellList(CTableCell("Group Name"), CTableCell(this->pGroup->groupName));
		tbl.AddRow(tblRow);

		tblRow.AddCellList(CTableCell("Long Group Name"), CTableCell(this->pGroup->longGroupName));
		tbl.AddRow(tblRow);

		tblRow.AddCellList(CTableCell("Group Type"), CTableCell(CAREnum::GroupType(this->pGroup->groupType)));
		tbl.AddRow(tblRow);

		tblRow.AddCellList(CTableCell("Group Category"), CTableCell(CAREnum::GroupCategory(this->pGroup->groupCategory)));
		tbl.AddRow(tblRow);

		if (this->pGroup->groupCategory == AR_GROUP_CATEGORY_COMPUTED) {
			tblRow.AddCellList(CTableCell("Computed Group Definition"), CTableCell(this->pGroup->groupDefinition));
			tbl.AddRow(tblRow);
		}

		tblRow.AddCellList(CTableCell("Permissions"), this->GroupPermissions());
		tbl.AddRow(tblRow);

		tblRow.AddCellList(CTableCell("Role Mapping"), this->RoleReferences());
		tbl.AddRow(tblRow);

		webPage.AddContent(tbl.ToXHtml());
		tbl.Clear();

		//Histoy
		webPage.AddContent(this->pInside->DataObjectHistory(this->pGroup, this->rootLevel));

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocGroupDetails_Documentation of '" << pGroup->name << "': " << e.what() << endl;
	}
}

string CDocGroupDetails::RoleReferences()
{
	CTable tbl("roleList", "TblNoSort");
	tbl.AddColumn(30, "Type");
	tbl.AddColumn(70, "Role");

	list<CARRole>::iterator roleIter;		
	for ( roleIter = this->pInside->roleList.begin(); roleIter != this->pInside->roleList.end(); roleIter++ )
	{	
		CARRole *role = &(*roleIter);		
		if(role->productionGroupId == this->pGroup->groupId)
		{
			CTableRow row("");
			row.AddCellList(CTableCell("Production"), CTableCell( role->GetURL(this->rootLevel)));
			tbl.AddRow(row);
		}

		if(role->testGroupId == this->pGroup->groupId)
		{
			CTableRow row("");
			row.AddCellList(CTableCell("Test"), CTableCell( role->GetURL(this->rootLevel)));
			tbl.AddRow(row);
		}
	}

	return tbl.ToXHtml();
}

string CDocGroupDetails::GroupPermissions()
{
	CTable tbl("permissionList", "TblNoSort");
	tbl.AddColumn(100, "Permission Details");

	stringstream strmTmp;
	strmTmp.str("");

	//Form Access
	int nResult = 0;
	this->FormsDoc(nResult, "Form Permission");
	if(nResult == 0)
		strmTmp.str("No Form Access");
	else
		strmTmp << CWebUtil::Link("Form Access", CPageParams(PAGE_GROUP_SCHEMA_LIST, pGroup), "", rootLevel) << " (" << nResult << ")<br/>";

	CTableRow row("");		
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);


	//Field Permission
	strmTmp.str("");
	this->FieldPermissionDoc(nResult, "Field Permission");
	if(nResult == 0)
		strmTmp.str("No Field Permission");
	else
		strmTmp << CWebUtil::Link("Field Permission", CPageParams(PAGE_GROUP_FIELD_LIST, pGroup), "", rootLevel) << " (" << nResult << ")<br/>";

	row.ClearCells();
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);


	//Active Link Permissions
	strmTmp.str("");
	this->AlPermissionDoc(nResult, "Active Link Permission");
	if(nResult == 0)
		strmTmp.str("No Active Link Permission");
	else
		strmTmp << CWebUtil::Link("Active Link Permission", CPageParams(PAGE_GROUP_ACTIVELINK_LIST, pGroup), "", rootLevel) << " (" << nResult << ")<br/>";

	row.ClearCells();
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);


	//Users in group
	strmTmp.str("");
	this->UserDoc(nResult, "Group Members");
	if(nResult == 0)
		strmTmp.str("No Group Members");
	else
		strmTmp << CWebUtil::Link("Group Members", CPageParams(PAGE_GROUP_USER_LIST, pGroup), "", rootLevel) << " (" << nResult << ")<br/>";

	row.ClearCells();
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);


	//Packing List Permissions
	strmTmp.str("");
	this->ContainerPermissionDoc(nResult, "Packing List Permission", ARCON_PACK);
	if(nResult == 0)
		strmTmp.str("No Packing List Permission");
	else
		strmTmp << CWebUtil::Link("Packing List Permission", CPageParams(PAGE_GROUP_CONTAINER_LIST, ARCON_PACK, pGroup), "", rootLevel) << " (" << nResult << ")<br/>";

	row.ClearCells();
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);


	//ActiveLink Guide
	strmTmp.str("");
	this->ContainerPermissionDoc(nResult, "Active Link Guide Permission", ARCON_GUIDE);
	if(nResult == 0)
		strmTmp.str("No Active Link Guide Permission");
	else
		strmTmp << CWebUtil::Link("Active Link Guide Permission", CPageParams(PAGE_GROUP_CONTAINER_LIST, ARCON_GUIDE, pGroup), "", rootLevel) << " (" << nResult << ")<br/>";

	row.ClearCells();
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);


	//Webservice Permissions
	strmTmp.str("");
	this->ContainerPermissionDoc(nResult, "Webservice Permission", ARCON_WEBSERVICE);
	if(nResult == 0)
		strmTmp.str("No Webservice Permission");
	else
		strmTmp << CWebUtil::Link("Webservice Permission", CPageParams(PAGE_GROUP_CONTAINER_LIST, ARCON_WEBSERVICE, pGroup), "", rootLevel) << " (" << nResult << ")<br/>";

	row.ClearCells();
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);	

	return tbl.ToXHtml();
}

void CDocGroupDetails::UserDoc(int &nResult, string title)
{
	CPageParams file(PAGE_GROUP_USER_LIST, pGroup);

	try
	{
		rootLevel = file->GetRootLevel();
		nResult = 0;

		CWebPage webPage(file->GetFileName(), title + " " +this->pGroup->groupName, rootLevel, this->pInside->appConfig);	

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToUserIndex(rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::Link(pGroup->groupName, CPageParams(PAGE_DETAILS, this->pGroup), "", rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::ObjName(title) << endl;
		webPage.AddContentHead(contHeadStrm.str());
		contHeadStrm.str("");

		//Add a table with all users
		CTable tbl("userList", "TblObjectList");
		tbl.AddColumn(60, "Login Name");
		tbl.AddColumn(20, "Modified");
		tbl.AddColumn(20, "By");

		unsigned int userCount = this->pInside->userList.GetCount();
		for (unsigned int userId = 0; userId < userCount; ++userId)
		{	
			CARUser user(userId);

			const CARUser::GroupList& grpList = user.GetGroups();
			CARUser::GroupList::const_iterator findIt = std::find(grpList.begin(), grpList.end(), this->pGroup->groupId);

			if (findIt != grpList.end())
			{
				CTableRow row("");
				row.AddCell(CTableCell(CWebUtil::Link(user.GetName(), CPageParams(PAGE_DETAILS, &user),"", rootLevel)));
				row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(user.GetModifiedDate())));
				row.AddCell(CTableCell(this->pInside->LinkToUser(user.GetLastChanged(), rootLevel)));
				tbl.AddRow(row);

				nResult++;
			}
		}

		webPage.AddContent(tbl.ToXHtml());

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocGroupDetails_FormsDoc of '" << this->pGroup->groupName << "': " << e.what() << endl;
	}
}

void CDocGroupDetails::FormsDoc(int &nResult, string title)
{
	CPageParams file(PAGE_GROUP_SCHEMA_LIST, pGroup);

	try
	{
		rootLevel = file->GetRootLevel();
		nResult = 0;

		CWebPage webPage(file->GetFileName(), title + " " +this->pGroup->groupName, rootLevel, this->pInside->appConfig);	

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToGroupIndex(rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::Link(pGroup->groupName, CPageParams(PAGE_DETAILS, pGroup), "", rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::ObjName(title) << endl;
		webPage.AddContentHead(contHeadStrm.str());
		contHeadStrm.str("");

		//Create table with forms the group can access
		CTable schemaTbl("schemaList", "TblObjectList");
		schemaTbl.description = "Form Permission";
		schemaTbl.AddColumn(5, "Visible");
		schemaTbl.AddColumn(35, "Form Name");
		schemaTbl.AddColumn(5, "Fields");
		schemaTbl.AddColumn(5, "Views");
		schemaTbl.AddColumn(10, "Type");
		schemaTbl.AddColumn(20, "Modified");
		schemaTbl.AddColumn(20, "By");

		//Create table with forms for group subadmin permission
		CTable subadminTbl("schemaListSubadmin", "TblObjectList");
		subadminTbl.description = "Form Subadministrator Permission";
		subadminTbl.AddColumn(35, "Form Name");
		subadminTbl.AddColumn(5, "Fields");
		subadminTbl.AddColumn(5, "Views");
		subadminTbl.AddColumn(10, "Type");
		subadminTbl.AddColumn(20, "Modified");
		subadminTbl.AddColumn(20, "By");

		unsigned int schemaCount = this->pInside->schemaList.GetCount();
		for ( unsigned int schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex )
		{	
			CARSchema schema(schemaIndex);
			const ARPermissionList& groupList = schema.GetPermissions();

			for(unsigned int nGrp = 0; nGrp < groupList.numItems; nGrp++)
			{
				if (this->pGroup->groupId == groupList.permissionList[nGrp].groupId)
				{
					string visibleInfo;
					if (groupList.permissionList[nGrp].permissions == AR_PERMISSIONS_VISIBLE)
						visibleInfo = CWebUtil::ImageTag("visible.gif", rootLevel);
					else
						visibleInfo = CWebUtil::ImageTag("hidden.gif", rootLevel);

					CTableRow row("");
					row.AddCell(CTableCell(visibleInfo));
					row.AddCell(CTableCell(schema.GetURL(rootLevel)));						
					row.AddCell(CTableCell(schema.GetFields()->GetCount()));
					row.AddCell(CTableCell(schema.GetVUIs()->GetCount()));
					row.AddCell(CTableCell(CAREnum::SchemaType(schema.GetCompound().schemaType)));
					row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(schema.GetTimestamp())));
					row.AddCell(CTableCell(this->pInside->LinkToUser(schema.GetLastChanged(), rootLevel)));
					schemaTbl.AddRow(row);
					nResult++;
				}
			}

			const ARInternalIdList& admingrpList = schema.GetSubadmins();

			for(unsigned int nGrp = 0; nGrp < admingrpList.numItems; nGrp++)
			{
				if(this->pGroup->groupId == admingrpList.internalIdList[nGrp])
				{
					CTableRow row("");
					row.AddCell(CTableCell(schema.GetURL(rootLevel)));						
					row.AddCell(CTableCell(schema.GetFields()->GetCount()));
					row.AddCell(CTableCell(schema.GetVUIs()->GetCount()));
					row.AddCell(CTableCell(CAREnum::SchemaType(schema.GetCompound().schemaType)));
					row.AddCell(CTableCell(CUtil::DateTimeToHTMLString(schema.GetTimestamp())));
					row.AddCell(CTableCell(this->pInside->LinkToUser(schema.GetLastChanged(), rootLevel)));
					subadminTbl.AddRow(row);
					nResult++;
				}
			}
		}


		webPage.AddContent(schemaTbl.ToXHtml());

		webPage.AddContent(subadminTbl.ToXHtml());

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocGroupDetails_FormsDoc of '" << this->pGroup->groupName << "': " << e.what() << endl;
	}
}

void CDocGroupDetails::AlPermissionDoc(int &nResult, string title)
{
	CPageParams file(PAGE_GROUP_ACTIVELINK_LIST, pGroup);

	try
	{
		rootLevel = file->GetRootLevel();
		nResult = 0;

		CWebPage webPage(file->GetFileName(), title + " " +this->pGroup->groupName, rootLevel, this->pInside->appConfig);	

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToGroupIndex(rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::Link(pGroup->groupName, CPageParams(PAGE_DETAILS, pGroup), "", rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::ObjName(title) << endl;
		webPage.AddContentHead(contHeadStrm.str());
		contHeadStrm.str("");

		//Create table with ALs the role can access
		CAlTable *alTable = new CAlTable(*this->pInside);

		unsigned int alCount = pInside->alList.GetCount();
		for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex)
		{	
			CARActiveLink al(alIndex);
			for(unsigned int nGrp = 0; nGrp < al.GetGroupList().numItems; nGrp++)
			{
				if(this->pGroup->groupId == al.GetGroupList().internalIdList[nGrp])
				{
					alTable->AddRow(alIndex, this->rootLevel);
					nResult++;
				}
			}
		}


		webPage.AddContent(alTable->Print());
		delete alTable;

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocGroupDetails_ActiveLinkDoc of '" << this->pGroup->groupName << "': " << e.what() << endl;
	}
}

void CDocGroupDetails::ContainerPermissionDoc(int &nResult, string title, int containerType)
{
	CPageParams file(PAGE_GROUP_CONTAINER_LIST, containerType, pGroup);

	try
	{
		rootLevel = file->GetRootLevel();
		nResult = 0;

		CWebPage webPage(file->GetFileName(), title + " " +this->pGroup->groupName, rootLevel, this->pInside->appConfig);	

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToGroupIndex(rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::Link(pGroup->groupName, CPageParams(PAGE_DETAILS, pGroup), "", rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::ObjName(title) << endl;
		webPage.AddContentHead(contHeadStrm.str());
		contHeadStrm.str("");

		//Create table with forms the role can access
		CContainerTable *contTbl = new CContainerTable(*this->pInside);
		CContainerTable *subadminTbl = (containerType == ARCON_PACK ? new CContainerTable(*this->pInside) : NULL);

		contTbl->SetDescription("Container Permission");

		unsigned int cntCount = this->pInside->containerList.GetCount();
		for ( unsigned int cntIndex = 0; cntIndex < cntCount; ++cntIndex )
		{	
			CARContainer cont(cntIndex);
			if(cont.GetType() == containerType)
			{
				const ARPermissionList& groupList = cont.GetPermissions();
				for(unsigned int nGrp = 0; nGrp < groupList.numItems; nGrp++)
				{
					if(this->pGroup->groupId == groupList.permissionList[nGrp].groupId)
					{
						contTbl->AddRow(cont, this->rootLevel);
						nResult++;
					}
				}
			}

			if(containerType == ARCON_PACK)
			{
				//Create table with forms for role subadmin permission
				subadminTbl->SetDescription("Subadministrator Permission");
				
				const ARInternalIdList& admingrpList = cont.GetSubadmins();
				for(unsigned int nGrp = 0; nGrp < admingrpList.numItems; nGrp++)
				{
					if(this->pGroup->groupId == admingrpList.internalIdList[nGrp])
					{
						subadminTbl->AddRow(cont, this->rootLevel);
						nResult++;
					}
				}

			}
		}


		webPage.AddContent(contTbl->Print());
		delete contTbl;

		if (subadminTbl != NULL)
		{
			webPage.AddContent(subadminTbl->Print());
			delete subadminTbl;
		}

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocGroupDetails_FormsDoc of '" << this->pGroup->groupName << "': " << e.what() << endl;
	}
}

int CDocGroupDetails::NumAllowedFields(CARSchema& schema)
{
	int nResult = 0;
	try
	{
		unsigned int fieldCount = schema.GetFields()->GetCount();
		for( unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex )
		{
			CARField field(schema.GetInsideId(), 0, fieldIndex);
			const ARPermissionList& permissions = field.GetPermissions();
			for(unsigned int i = 0; i< permissions.numItems; i++)
			{
				if(permissions.permissionList[i].groupId == this->pGroup->groupId)
				{
					nResult++;
				}
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocGroupDetails_NumAllowedFields of '" << this->pGroup->groupName << "': " << e.what() << endl;
	}
	return nResult;
}

void CDocGroupDetails::FieldPermissionDoc(int &nResult, string title)
{
	CPageParams file(PAGE_GROUP_FIELD_LIST, pGroup);

	try
	{
		rootLevel = file->GetRootLevel();
		nResult = 0;
		CWebPage webPage(file->GetFileName(), title + " " +this->pGroup->groupName, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToGroupIndex(rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::Link(pGroup->name, CPageParams(PAGE_DETAILS, pGroup), "", rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::ObjName("Field Permissions") << endl;
		webPage.AddContentHead(contHeadStrm.str());
		contHeadStrm.str("");

		unsigned int schemaCount = this->pInside->schemaList.GetCount();
		for ( unsigned int schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex )
		{			
			CARSchema schema(schemaIndex);
			if(NumAllowedFields(schema) > 0)
			{
				const ARPermissionList& groupList = schema.GetPermissions();

				//Check if the current group has access to the form
				bool bVisiblePermission = false;		
				bool bInsert = false;

				for(unsigned int nGrp = 0; nGrp < groupList.numItems; nGrp++)
				{
					if(this->pGroup->groupId == groupList.permissionList[nGrp].groupId)
					{
						if (groupList.permissionList[nGrp].permissions == AR_PERMISSIONS_VISIBLE)
						{
							bVisiblePermission = true;
						}
						bInsert = true;
						break;
					}
				}	

				stringstream strmFormDesc;
				strmFormDesc.str("");
				if(bInsert)
				{
					string visibleInfo;
					if(bVisiblePermission)
						visibleInfo = CWebUtil::ImageTag("visible.gif", rootLevel);
					else
						visibleInfo = CWebUtil::ImageTag("hidden.gif", rootLevel);

					strmFormDesc << visibleInfo << schema.GetURL(rootLevel) << endl;
				}
				else
				{
					strmFormDesc << schema.GetURL(rootLevel) << endl;
				}

				//Create a table for every form
				CTable schemaTbl("fieldListAll"+schema.GetInsideId(), "TblObjectList");
				schemaTbl.description = strmFormDesc.str();
				schemaTbl.AddColumn(80, "Field");
				schemaTbl.AddColumn(10, "Permission");
				schemaTbl.AddColumn(10, "Description");

				unsigned int fieldCount = schema.GetFields()->GetCount();
				for( unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
				{
					CARField field(schemaIndex, 0, fieldIndex);
					const ARPermissionList& permissions = field.GetPermissions();
					for(unsigned int i = 0; i < permissions.numItems; i++)
					{
						if(permissions.permissionList[i].groupId == this->pGroup->groupId)
						{
							//We found a field that this group is allowed to access

							string img;
							if(permissions.permissionList[i].permissions == AR_PERMISSIONS_CHANGE)
								img = CWebUtil::ImageTag("edit.gif", rootLevel);
							else
								img = CWebUtil::ImageTag("visible.gif", rootLevel);

							CTableRow row("");			
							row.AddCell(CTableCell(field.GetURL(rootLevel)));
							row.AddCell(CTableCell(img));
							row.AddCell(CTableCell(CAREnum::FieldPermission(permissions.permissionList[i].permissions)));
							schemaTbl.AddRow(row);

							nResult++;
						}
					}
				}

				webPage.AddContent(schemaTbl.ToXHtml());
			}
		}

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocGroupDetails_FieldsDoc of '" << this->pGroup->groupName << "': " << e.what() << endl;
	}
}

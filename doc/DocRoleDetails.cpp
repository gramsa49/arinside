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
#include "DocRoleDetails.h"

CDocRoleDetails::CDocRoleDetails(CARRole &arRole)
{
	this->pRole = &arRole;
	this->rootLevel = 1;
}

CDocRoleDetails::~CDocRoleDetails(void)
{
}

void CDocRoleDetails::Documentation()
{
	CPageParams file(PAGE_DETAILS, pRole);

	try
	{
		rootLevel = file->GetRootLevel();
		CWebPage webPage(file->GetFileName(), this->pRole->roleName, this->rootLevel, this->pInside->appConfig);

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToRoleIndex(this->rootLevel) << MenuSeparator << CWebUtil::ObjName(this->pRole->roleName) << endl;
		contHeadStrm << " (Id: " << this->pRole->roleId << ")";
		webPage.AddContentHead(contHeadStrm.str());

		//User details
		CTable tbl("roleDetails", "TblObjectList");
		tbl.AddColumn(30, "Description");
		tbl.AddColumn(70, "Value");

		CTableRow tblRow("");
		tblRow.AddCellList(CTableCell("Application"), CTableCell(this->pInside->LinkToContainer(this->pRole->applicationName, this->rootLevel)));
		tbl.AddRow(tblRow);

		tblRow.AddCellList(CTableCell("Test Group"), CTableCell(this->pInside->LinkToGroup(this->pRole->applicationName, this->pRole->testGroupId, this->rootLevel)));
		tbl.AddRow(tblRow);

		tblRow.AddCellList(CTableCell("Production Group"), CTableCell(this->pInside->LinkToGroup(this->pRole->applicationName, this->pRole->productionGroupId, this->rootLevel)));
		tbl.AddRow(tblRow);

		tblRow.AddCellList(CTableCell("Permissions"), this->RolePermissions()); // TODO: after this call rootLevel might be incorrect
		tbl.AddRow(tblRow);

		//Add the complete table to the page
		webPage.AddContent(tbl.ToXHtml());
		tbl.Clear();

		//Histoy
		webPage.AddContent(this->pInside->DataObjectHistory(this->pRole, this->rootLevel));

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocRoleDetails_Documentation of '" << this->pRole->roleName << "': " << e.what() << endl;
	}
}

string CDocRoleDetails::RolePermissions()
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
		strmTmp << CWebUtil::Link("Form Access", CPageParams(PAGE_ROLE_SCHEMA_LIST, pRole), "", rootLevel) << " (" << nResult << ")<br/>";

	CTableRow row("");		
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);


	//Field Permission
	strmTmp.str("");

	this->FieldPermissionDoc(nResult, "Field Permission");
	if(nResult == 0)
		strmTmp.str("No Field Permission");
	else
		strmTmp << CWebUtil::Link("Field Permission", CPageParams(PAGE_ROLE_FIELD_LIST, pRole), "", rootLevel) << " (" << nResult << ")<br/>";

	row.ClearCells();
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);


	//Active Link Permissions
	strmTmp.str("");

	this->AlPermissionDoc(nResult, "Active Link Permission");
	if(nResult == 0)
		strmTmp.str("No Active Link Permission");
	else
		strmTmp << CWebUtil::Link("Active Link Permission", CPageParams(PAGE_ROLE_ACTIVELINK_LIST, pRole), "", rootLevel) << " (" << nResult << ")<br/>";

	row.ClearCells();
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);


	//Packing List Permissions
	strmTmp.str("");

	this->ContainerPermissionDoc(nResult, "Packing List Permission", ARCON_PACK);
	if(nResult == 0)
		strmTmp.str("No Packing List Permission");
	else
		strmTmp << CWebUtil::Link("Packing List Permission", CPageParams(PAGE_ROLE_CONTAINER_LIST, ARCON_PACK, pRole), "", rootLevel) << " (" << nResult << ")<br/>";

	row.ClearCells();
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);


	//ActiveLink Guide
	strmTmp.str("");

	this->ContainerPermissionDoc(nResult, "Active Link Guide Permission", ARCON_GUIDE);
	if(nResult == 0)
		strmTmp.str("No Active Link Guide Permission");
	else
		strmTmp << CWebUtil::Link("Active Link Guide Permission", CPageParams(PAGE_ROLE_CONTAINER_LIST, ARCON_GUIDE, pRole), "", rootLevel) << " (" << nResult << ")<br/>";

	row.ClearCells();
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);


	//Webservice Permissions
	strmTmp.str("");

	this->ContainerPermissionDoc(nResult, "Webservice Permission", ARCON_WEBSERVICE);
	if(nResult == 0)
		strmTmp.str("No Webservice Permission");
	else
		strmTmp << CWebUtil::Link("Webservice Permission", CPageParams(PAGE_ROLE_CONTAINER_LIST, ARCON_WEBSERVICE, pRole), "", rootLevel) << " (" << nResult << ")<br/>";

	row.ClearCells();
	row.AddCell(CTableCell(strmTmp.str()));
	tbl.AddRow(row);	

	return tbl.ToXHtml();
}


void CDocRoleDetails::FormsDoc(int &nResult, string title)
{
	CPageParams file(PAGE_ROLE_SCHEMA_LIST, pRole);

	try
	{
		rootLevel = file->GetRootLevel();
		nResult = 0;

		CWebPage webPage(file->GetFileName(), title + " " +this->pRole->roleName, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToRoleIndex(rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::Link(pRole->roleName, CPageParams(PAGE_DETAILS, pRole), "", rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::ObjName(title) << endl;
		webPage.AddContentHead(contHeadStrm.str());
		contHeadStrm.str("");

		//Create table with forms the role can access
		CTable schemaTbl("schemaList", "TblObjectList");
		schemaTbl.description = "Form Permission";
		schemaTbl.AddColumn(5, "Visible");
		schemaTbl.AddColumn(35, "Form Name");
		schemaTbl.AddColumn(5, "Fields");
		schemaTbl.AddColumn(5, "Views");
		schemaTbl.AddColumn(10, "Type");
		schemaTbl.AddColumn(20, "Modified");
		schemaTbl.AddColumn(20, "By");

		//Create table with forms for role subadmin permission
		CTable subadminTbl("schemaListSubadmin", "TblObjectList");
		subadminTbl.description = "Form Subadministrator Permission";
		subadminTbl.AddColumn(35, "Form Name");
		subadminTbl.AddColumn(5, "Fields");
		subadminTbl.AddColumn(5, "Views");
		subadminTbl.AddColumn(10, "Type");
		subadminTbl.AddColumn(20, "Modified");
		subadminTbl.AddColumn(20, "By");

		unsigned int schemaCount = this->pInside->schemaList.GetCount();
		for (unsigned int schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex)
		{	
			CARSchema schema(schemaIndex);

			if(schema.GetAppRefName() == this->pRole->applicationName)
			{
				const ARPermissionList& groupList = schema.GetPermissions();
				for(unsigned int nGrp = 0; nGrp < groupList.numItems; nGrp++)
				{
					if(this->pRole->roleId == groupList.permissionList[nGrp].groupId)
					{
						string visibleInfo;
						if(groupList.permissionList[nGrp].permissions == AR_PERMISSIONS_VISIBLE)
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
					if(this->pRole->roleId == admingrpList.internalIdList[nGrp])
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
		}


		webPage.AddContent(schemaTbl.ToXHtml());
		webPage.AddContent(subadminTbl.ToXHtml());

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocRoleDetails_FormsDoc of '" << this->pRole->roleName << "': " << e.what() << endl;
	}
}

void CDocRoleDetails::AlPermissionDoc(int &nResult, string title)
{
	CPageParams file(PAGE_ROLE_ACTIVELINK_LIST, pRole);

	try
	{
		rootLevel = file->GetRootLevel();
		nResult = 0;
		CWebPage webPage(file->GetFileName(), title + " " +this->pRole->roleName, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToRoleIndex(rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::Link(pRole->roleName, CPageParams(PAGE_DETAILS, pRole), "", rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::ObjName(title) << endl;
		webPage.AddContentHead(contHeadStrm.str());
		contHeadStrm.str("");

		//Create table with ALs the role can access
		CAlTable *alTable = new CAlTable(*this->pInside);

		unsigned int alCount = pInside->alList.GetCount();
		for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex)
		{	
			CARActiveLink al(alIndex);
			if(strcmp(al.GetAppRefName().c_str(), this->pRole->applicationName.c_str())==0)
			{
				for(unsigned int nGrp = 0; nGrp < al.GetGroupList().numItems; nGrp++)
				{
					if(this->pRole->roleId == al.GetGroupList().internalIdList[nGrp])
					{
						alTable->AddRow(alIndex, this->rootLevel);
						nResult++;
					}
				}
			}
		}


		webPage.AddContent(alTable->Print());
		delete alTable;

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocRoleDetails_ActiveLinkDoc of '" << this->pRole->roleName << "': " << e.what() << endl;
	}
}

void CDocRoleDetails::ContainerPermissionDoc(int &nResult, string title, int containerType)
{
	CPageParams file(PAGE_ROLE_CONTAINER_LIST, containerType, pRole);

	try
	{
		rootLevel = file->GetRootLevel();
		nResult = 0;

		CWebPage webPage(file->GetFileName(), title + " " +this->pRole->roleName, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToRoleIndex(rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::Link(pRole->roleName, CPageParams(PAGE_DETAILS, pRole), "", rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::ObjName(title) << endl;
		webPage.AddContentHead(contHeadStrm.str());
		contHeadStrm.str("");

		//Create table with forms the role can access
		CContainerTable *contTbl = new CContainerTable(*this->pInside);
		contTbl->SetDescription("Container Permission");

		unsigned int cntCount = this->pInside->containerList.GetCount();
		for ( unsigned int cntIndex = 0; cntIndex < cntCount; ++cntIndex )
		{	
			CARContainer cont(cntIndex);
			if(cont.GetType() == containerType)
			{
				if(cont.GetAppRefName() == this->pRole->applicationName)
				{
					const ARPermissionList& groupList = cont.GetPermissions();
					for(unsigned int nGrp = 0; nGrp < groupList.numItems; nGrp++)
					{
						if(this->pRole->roleId == groupList.permissionList[nGrp].groupId)
						{
							contTbl->AddRow(cont, this->rootLevel);
							nResult++;
						}
					}
				}
			}
		}


		webPage.AddContent(contTbl->Print());
		delete contTbl;


		if(containerType == ARCON_PACK)
		{						
			//Create table with forms for role subadmin permission
			CContainerTable *subadminTbl = new CContainerTable(*this->pInside);
			subadminTbl->SetDescription("Subadministrator Permission");		

			unsigned int cntCount = this->pInside->containerList.GetCount();
			for ( unsigned int cntIndex = 0; cntIndex < cntCount; ++cntIndex )
			{	
				CARContainer cont(cntIndex);
				if(cont.GetAppRefName() == this->pRole->applicationName)
				{
					const ARInternalIdList& admingrpList = cont.GetSubadmins();
					for(unsigned int nGrp = 0; nGrp < admingrpList.numItems; nGrp++)
					{
						if(this->pRole->roleId == admingrpList.internalIdList[nGrp])
						{
							subadminTbl->AddRow(cont, this->rootLevel);
							nResult++;
						}
					}
				}
			}

			webPage.AddContent(subadminTbl->Print());
			delete subadminTbl;
		}


		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocRoleDetails_FormsDoc of '" << this->pRole->roleName << "': " << e.what() << endl;
	}
}

int CDocRoleDetails::NumAllowedFields(CARSchema& schema)
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
				if(permissions.permissionList[i].groupId == this->pRole->roleId)
				{
					nResult++;
				}
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocRoleDetails_NumAllowedFields of '" << this->pRole->roleName << "': " << e.what() << endl;
	}
	return nResult;
}

void CDocRoleDetails::FieldPermissionDoc(int &nResult, string title)
{
	CPageParams file(PAGE_ROLE_FIELD_LIST, pRole);

	try
	{
		rootLevel = file->GetRootLevel();
		nResult = 0;

		CWebPage webPage(file->GetFileName(), title + " " +this->pRole->roleName, rootLevel, this->pInside->appConfig);

		//ContentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToRoleIndex(rootLevel);
		contHeadStrm << MenuSeparator << CWebUtil::Link(pRole->name, CPageParams(PAGE_DETAILS, pRole), "", rootLevel);
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
				
				for(unsigned int nGrp= 0; nGrp < groupList.numItems; nGrp++)
				{
					if(this->pRole->roleId == groupList.permissionList[nGrp].groupId)
					{
						if (groupList.permissionList[nGrp].permissions == AR_PERMISSIONS_VISIBLE)
						{
							bVisiblePermission = true;
						}
						bInsert = true;
					}
				}	

				stringstream strmFormDesc;
				strmFormDesc.str("");
				if(bInsert)
				{
					string visibleInfo = CWebUtil::ImageTag("hidden.gif", rootLevel);
					if(bVisiblePermission)
					{
						visibleInfo = CWebUtil::ImageTag("visible.gif", rootLevel);
					}				

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
					for(unsigned int i = 0; i< permissions.numItems; i++)
					{
						if(permissions.permissionList[i].groupId == this->pRole->roleId)
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
		cout << "EXCEPTION CDocRoleDetails_FieldsDoc of '" << this->pRole->roleName << "': " << e.what() << endl;
	}
}

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
#include "DocContainerHelper.h"

CDocContainerHelper::CDocContainerHelper(CARContainer &contObj, int rootLevel)
: container(contObj)
{
	this->rootLevel = rootLevel;
}

CDocContainerHelper::~CDocContainerHelper(void)
{
}

string CDocContainerHelper::BaseInfo()
{
	//Container specific properties
	CTable tblProp("specificPropList", "TblObjectList");
	tblProp.AddColumn(20, "Type");
	tblProp.AddColumn(80, "Server Object");

	try
	{
		CTableRow row("");		

		//Label
		if(this->container.GetLabel() != NULL)
		{
			row.AddCellList(CTableCell("Label"), CTableCell(this->container.GetLabel()));
		}
		else
		{
			row.AddCellList(CTableCell("Label"), CTableCell(EmptyValue));
		}
		tblProp.AddRow(row);

		//Description
		if(this->container.GetDescription() != NULL)
		{
			row.AddCellList(CTableCell("Description"), CTableCell(this->container.GetDescription()));
		}
		else
		{
			row.AddCellList(CTableCell("Description"), CTableCell(EmptyValue));	
		}
		tblProp.AddRow(row);

		//Permissions		
		row.AddCellList(CTableCell("Permissions"), CTableCell(this->PermissionList()));	
		tblProp.AddRow(row);	

		//SubAdminList
		unsigned int type = this->container.GetType();
		if(type == ARCON_PACK
			|| type == ARCON_GUIDE
			|| type == ARCON_APP
			)
		{
			row.AddCellList(CTableCell("Subadministrator Permissions"), CTableCell(this->SubadminList()));
			tblProp.AddRow(row);	
		}


		//Forms
		if(type == ARCON_GUIDE || type == ARCON_FILTER_GUIDE)
		{
			row.AddCellList(CTableCell("Owner Object List"), CTableCell(this->ContainerForms()));	
			tblProp.AddRow(row);	
		}

	}
	catch(exception& e)
	{
		cout << "EXCEPTION in CDocContainerHelper::BaseInfo : " << e.what() << endl;
	}

	tblProp.description = "Base Information";
	return tblProp.ToXHtml();

}

string CDocContainerHelper::SubadminList()
{
	stringstream strm;
	strm.str("");
	try
	{
		CGroupTable *grpTbl = new CGroupTable(*this->pInside);

		const ARInternalIdList& admingrpList = this->container.GetSubadmins();
		for(unsigned int i=0; i< admingrpList.numItems; i++)
		{			
			grpTbl->AddRow(this->container.GetAppRefName(), admingrpList.internalIdList[i], this->rootLevel);
		}

		strm << grpTbl->Print();
		delete grpTbl;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in Container SubadminList: " << e.what() << endl;
	}

	return strm.str();
}

string CDocContainerHelper::PermissionList()
{
	CTable tbl("permissionList", "TblObjectList");

	try
	{
		tbl.AddColumn(5, "Permission");
		tbl.AddColumn(10, "Description");
		tbl.AddColumn(75, "Name");
		tbl.AddColumn(10, "Id");

		const ARPermissionList& groupList = this->container.GetPermissions();
		for(unsigned int i=0; i< groupList.numItems; i++)
		{			
			CTableRow row("");			
			string img;

			if(groupList.permissionList[i].permissions == AR_PERMISSIONS_HIDDEN)
				img = CWebUtil::ImageTag("hidden.gif", rootLevel);
			else
				img = CWebUtil::ImageTag("visible.gif", rootLevel);

			string appRefName;
			if(this->container.GetAppRefName().empty())
				appRefName = this->container.GetName();
			else
				appRefName = this->container.GetAppRefName();

			row.AddCell(CTableCell(img));
			row.AddCell(CTableCell(CAREnum::ObjectPermission(groupList.permissionList[i].permissions)));
			row.AddCell(CTableCell(this->pInside->LinkToGroup(appRefName, groupList.permissionList[i].groupId, rootLevel)));
			row.AddCell(CTableCell(groupList.permissionList[i].groupId));			
			tbl.AddRow(row);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ContainerPermissions: " << e.what() << endl; 
	}

	return tbl.ToXHtml();	
}


string CDocContainerHelper::ContainerForms()
{	
	CTable tbl("formList", "TblObjectList");
	tbl.AddColumn(100, "Form Name");

	try
	{
		const ARContainerOwnerObjList& objList = this->container.GetOwnerObjects();
		for(unsigned int i=0; i< objList.numItems; i++)
		{
			CTableRow row("");            
			row.AddCell(CTableCell(this->pInside->LinkToSchema(objList.ownerObjList[i].ownerName, rootLevel)));
			tbl.AddRow(row);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ContainerForms: " << e.what() << endl; 
	}

	tbl.description = "Owner Forms";
	return tbl.ToXHtml();	
}

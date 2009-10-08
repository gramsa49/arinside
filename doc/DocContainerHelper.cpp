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

CDocContainerHelper::CDocContainerHelper(CARInside &arIn, CARContainer &container, int rootLevel)
{
	this->pInside = &arIn;
	this->pContainer = &container;
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
		if(this->pContainer->label != NULL)
		{
			row.AddCellList(CTableCell("Label"), CTableCell(this->pContainer->label));			
		}
		else
		{
			row.AddCellList(CTableCell("Label"), CTableCell(EmptyValue));		
		}
		tblProp.AddRow(row);

		//Description
		if(this->pContainer->description != NULL)
		{
			row.AddCellList(CTableCell("Description"), CTableCell(this->pContainer->description));			
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
		if(this->pContainer->type == ARCON_PACK
			|| this->pContainer->type == ARCON_GUIDE
			|| this->pContainer->type == ARCON_APP
			)
		{
			row.AddCellList(CTableCell("Subadministrator Permissions"), CTableCell(this->SubadminList()));	
			tblProp.AddRow(row);	
		}


		//Forms
		if(this->pContainer->type == ARCON_GUIDE
			|| this->pContainer->type == ARCON_FILTER_GUIDE)
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
		for(unsigned int i=0; i< this->pContainer->admingrpList.numItems; i++)
		{			
			grpTbl->AddRow(this->pContainer->appRefName, this->pContainer->admingrpList.internalIdList[i], this->rootLevel);
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

		for(unsigned int i=0; i< this->pContainer->groupList.numItems; i++)
		{			
			CTableRow row("");			
			string img = CWebUtil::ImageTag("visible.gif", rootLevel);

			if(this->pContainer->groupList.permissionList[i].permissions == AR_PERMISSIONS_HIDDEN)
				img = CWebUtil::ImageTag("hidden.gif", rootLevel);

			string appRefName = this->pContainer->appRefName;
			if(strcmp(appRefName.c_str(), "")==0)
				appRefName = this->pContainer->name;


			row.AddCell(CTableCell(img));
			row.AddCell(CTableCell(CAREnum::ObjectPermission(this->pContainer->groupList.permissionList[i].permissions)));
			row.AddCell(CTableCell(this->pInside->LinkToGroup(appRefName, this->pContainer->groupList.permissionList[i].groupId, rootLevel)));
			row.AddCell(CTableCell(this->pContainer->groupList.permissionList[i].groupId));			
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
		for(unsigned int i=0; i< this->pContainer->ownerObjList.numItems; i++)
		{
			CTableRow row("");            
			row.AddCell(CTableCell(this->pInside->LinkToSchema(this->pContainer->ownerObjList.ownerObjList[i].ownerName, rootLevel)));
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

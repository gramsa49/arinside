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
#include "ARDataFactory.h"
#include "../ARInside.h"

CARDataFactory::CARDataFactory(ARControlStruct &control, ARStatusList &status)
{
	this->pControl = &control;
	this->pStatus = &status;
}

CARDataFactory::~CARDataFactory(void)
{	
	FreeARStatusList(pStatus, false);
}

void CARDataFactory::GetListGroup(AppConfig &appConfig, list<CARGroup> &listResult)
{	
	try
	{
		CARInside* pInside = CARInside::GetInstance();
		ARQualifierStruct	qualifier;

		ARNameType schemaName;
		strncpy(schemaName, appConfig.groupForm.c_str(), AR_MAX_NAME_SIZE);

		char *qualString;
		qualString = new char[appConfig.groupQuery.size() + 1];
		strcpy(qualString, appConfig.groupQuery.c_str());

		AREntryListFieldList fields;
		AREntryListFieldValueList values;

		fields.numItems = 9;
		if (pInside->CompareServerVersion(7,0) >= 0) fields.numItems += 2; // if serv ver >= 7.0 get "Category" and "Computed Group Definition" field

		fields.fieldsList = (AREntryListFieldStruct*)new AREntryListFieldStruct[fields.numItems];
		memset(fields.fieldsList,0,sizeof(AREntryListFieldStruct)*fields.numItems);

		fields.fieldsList[0].fieldId = 1;     //RequestId
		fields.fieldsList[1].fieldId = 105;   //GroupName
		fields.fieldsList[2].fieldId = 106;   //GroupId
		fields.fieldsList[3].fieldId = 107;   //GroupType
		fields.fieldsList[4].fieldId = 8;     //LongGroupName
		fields.fieldsList[5].fieldId = 2;     //CreatedBy
		fields.fieldsList[6].fieldId = 3;     //Created
		fields.fieldsList[7].fieldId = 5;     //ModifiedBy
		fields.fieldsList[8].fieldId = 6;     //Modified
		if (pInside->CompareServerVersion(7,0) >= 0)
		{
			fields.fieldsList[9].fieldId = 120;   //Category
			fields.fieldsList[10].fieldId = 121;   //Computed Group Definition
		}

		for (unsigned int k=0; k<fields.numItems; ++k) { fields.fieldsList[k].columnWidth=1; fields.fieldsList[k].separator[0]='|'; }

		if(ARLoadARQualifierStruct(this->pControl, 
			schemaName,
			NULL, 
			qualString, 
			&qualifier, 
			this->pStatus) == AR_RETURN_OK)
		{
			if (ARGetListEntryWithFields(
				this->pControl,
				schemaName,
				&qualifier,
				&fields,
				NULL,0,0,0,  // sort, first, max, locale
				&values,
				0,this->pStatus) == AR_RETURN_OK)
			{
				for (unsigned int row=0; row<values.numItems; ++row)
				{
					if (values.entryList[row].entryValues->fieldValueList[0].value.u.charVal == NULL) continue;
					
					CARGroup grp(
						values.entryList[row].entryValues->fieldValueList[2].value.u.intVal, /* Group ID */
						values.entryList[row].entryValues->fieldValueList[0].value.u.charVal /* Request ID */ );
					grp.groupName = values.entryList[row].entryValues->fieldValueList[1].value.u.charVal;
					grp.name = grp.groupName;
					grp.comparableName = CUtil::String2Comp(grp.groupName);
					grp.groupId   = values.entryList[row].entryValues->fieldValueList[2].value.u.intVal;
					grp.groupType	= values.entryList[row].entryValues->fieldValueList[3].value.u.intVal;
					grp.longGroupName = values.entryList[row].entryValues->fieldValueList[4].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[5].value.u.charVal != NULL)
						grp.createdBy = values.entryList[row].entryValues->fieldValueList[5].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[6].value.u.timeVal != NULL)
						grp.created = values.entryList[row].entryValues->fieldValueList[6].value.u.timeVal;

					if(values.entryList[row].entryValues->fieldValueList[7].value.u.charVal != NULL)
						grp.modifiedBy = values.entryList[row].entryValues->fieldValueList[7].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[8].value.u.timeVal != NULL)
						grp.modified = values.entryList[row].entryValues->fieldValueList[8].value.u.timeVal;

					if (pInside->CompareServerVersion(7,0) >= 0)
					{
						// the next two fields are only available on 7.0+ servers
						grp.groupCategory = values.entryList[row].entryValues->fieldValueList[9].value.u.intVal;

						if(values.entryList[row].entryValues->fieldValueList[10].value.u.timeVal != NULL)
							grp.groupDefinition = values.entryList[row].entryValues->fieldValueList[10].value.u.charVal;
					}

					LOG << "Group '" << grp.groupName <<"' [OK]" << endl;

					listResult.push_back(grp);
				}
				FreeAREntryListFieldValueList(&values,false);
			}
		}

		delete[] fields.fieldsList;
		delete[] qualString;
	}
	catch(...)
	{		
		throw(AppException("Error loading groups.", "undefined", "DataFactory"));
	}
}

bool CARDataFactory::SortGroupsByName(const CARGroup& t1, const CARGroup& t2 )
{
	return t1.comparableName.compare(t2.comparableName) < 0;
}

void CARDataFactory::Sort(list<CARGroup> &listResult)
{	
	listResult.sort(SortGroupsByName);
}

void CARDataFactory::GetListUser(AppConfig &appConfig, list<CARUser> &listResult)
{
	try
	{
		ARQualifierStruct	qualifier;

		ARNameType schemaName;
		strncpy(schemaName, appConfig.userForm.c_str(), AR_MAX_NAME_SIZE);

		char *qualString;
		qualString = new char[appConfig.userQuery.size() + 1];
		strcpy(qualString, appConfig.userQuery.c_str());

		AREntryListFieldList fields;
		AREntryListFieldValueList values;

		fields.numItems = 12;
		fields.fieldsList = (AREntryListFieldStruct*)new AREntryListFieldStruct[fields.numItems];
		memset(fields.fieldsList,0,sizeof(AREntryListFieldStruct)*fields.numItems);

		fields.fieldsList[0].fieldId = 1;     //RequestId
		fields.fieldsList[1].fieldId = 101;   //LoginName
		fields.fieldsList[2].fieldId = 103;   //Email
		fields.fieldsList[3].fieldId = 104;   //GroupList
		fields.fieldsList[4].fieldId = 8;     //FullName
		fields.fieldsList[5].fieldId = 108;   //DefNorify
		fields.fieldsList[6].fieldId = 109;   //LicType
		fields.fieldsList[7].fieldId = 110;   //FtLicType
		fields.fieldsList[8].fieldId = 2;     //CreatedBy
		fields.fieldsList[9].fieldId = 3;     //Created
		fields.fieldsList[10].fieldId = 5;    //ModifiedBy
		fields.fieldsList[11].fieldId = 6;    //Modified
		
		for (unsigned int k=0; k<fields.numItems; ++k) { fields.fieldsList[k].columnWidth=1; fields.fieldsList[k].separator[0]='|'; }

		if(ARLoadARQualifierStruct(this->pControl, 
			schemaName,
			NULL, 
			qualString, 
			&qualifier, 
			this->pStatus) == AR_RETURN_OK)
		{
			if (ARGetListEntryWithFields(
				this->pControl,
				schemaName,
				&qualifier,
				&fields,
				NULL,0,0,0, // sort, first, max, locale,
				&values,
				0,this->pStatus) == AR_RETURN_OK)
			{
				for (unsigned int row=0; row<values.numItems; ++row)
				{
					if (values.entryList[row].entryValues->fieldValueList[0].value.u.charVal == NULL) continue;

					CARUser arUser(row, values.entryList[row].entryValues->fieldValueList[0].value.u.charVal);

					if(values.entryList[row].entryValues->fieldValueList[1].value.u.charVal != NULL)
					{
						arUser.loginName = values.entryList[row].entryValues->fieldValueList[1].value.u.charVal;
						arUser.name = arUser.loginName;
						arUser.comparableName = CUtil::String2Comp(arUser.loginName);
					}

					if(values.entryList[row].entryValues->fieldValueList[2].value.u.charVal != NULL)
						arUser.email = values.entryList[row].entryValues->fieldValueList[2].value.u.charVal;

					arUser.groupList.clear();
					if(values.entryList[row].entryValues->fieldValueList[3].value.u.charVal != NULL)
					{
						string tmpGroupList = values.entryList[row].entryValues->fieldValueList[3].value.u.charVal;
						CUtil::SplitString(tmpGroupList, arUser.groupList);
					}

					if(values.entryList[row].entryValues->fieldValueList[4].value.u.charVal != NULL)
						arUser.fullName = values.entryList[row].entryValues->fieldValueList[4].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[5].value.u.intVal != NULL)
						arUser.defNotify = values.entryList[row].entryValues->fieldValueList[5].value.u.intVal;

					if(values.entryList[row].entryValues->fieldValueList[6].value.u.intVal != NULL)			
						arUser.licenseType = values.entryList[row].entryValues->fieldValueList[6].value.u.intVal;

					if(values.entryList[row].entryValues->fieldValueList[7].value.u.intVal != NULL)		
						arUser.ftLicenseType = values.entryList[row].entryValues->fieldValueList[7].value.u.intVal;

					if(values.entryList[row].entryValues->fieldValueList[8].value.u.charVal != NULL)
						arUser.createdBy = values.entryList[row].entryValues->fieldValueList[8].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[9].value.u.timeVal != NULL)
						arUser.created = values.entryList[row].entryValues->fieldValueList[9].value.u.timeVal;

					if(values.entryList[row].entryValues->fieldValueList[10].value.u.charVal != NULL)
						arUser.modifiedBy = values.entryList[row].entryValues->fieldValueList[10].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[11].value.u.timeVal != NULL)
						arUser.modified = values.entryList[row].entryValues->fieldValueList[11].value.u.timeVal;

					LOG << "User '" << arUser.loginName <<"' [OK]" << endl;
					
					listResult.push_back(arUser);
				}
				FreeAREntryListFieldValueList(&values,false);
			}
		}

		delete[] fields.fieldsList;
		delete[] qualString;
	}
	catch(exception& e)
	{		
		throw(AppException(e.what(), "Error loading users.", "DataFactory"));
	}
}

bool CARDataFactory::SortUsersByName(const CARUser& t1, const CARUser& t2 )
{
	return t1.comparableName.compare(t2.comparableName) < 0;
}

void CARDataFactory::Sort(list<CARUser> &listResult)
{	
	listResult.sort(SortUsersByName);
}

void CARDataFactory::GetListRoles(AppConfig &appConfig, list<CARRole> &listResult)
{	
	try
	{
		ARQualifierStruct	qualifier;

		ARNameType schemaName;
		strncpy(schemaName, appConfig.roleForm.c_str(), AR_MAX_NAME_SIZE);

		char *qualString;
		qualString = new char[appConfig.roleQuery.size() + 1];
		strcpy(qualString, appConfig.roleQuery.c_str());

		AREntryListFieldList fields;
		AREntryListFieldValueList values;

		fields.numItems = 10;
		fields.fieldsList = new AREntryListFieldStruct[fields.numItems];
		memset(fields.fieldsList,0,sizeof(AREntryListFieldStruct)*fields.numItems);

		fields.fieldsList[0].fieldId = 1;		//RequestId
		fields.fieldsList[1].fieldId = 1700;	//ApplicationName
		fields.fieldsList[2].fieldId = 1701;	//RoleName
		fields.fieldsList[3].fieldId = 1702;	//RoleID
		fields.fieldsList[4].fieldId = 2001;	//GroupName Test
		fields.fieldsList[5].fieldId = 2002;	//GroupName Production
		fields.fieldsList[6].fieldId = 2;		//CreatedBy
		fields.fieldsList[7].fieldId = 3;		//Created
		fields.fieldsList[8].fieldId = 5;		//ModifiedBy
		fields.fieldsList[9].fieldId = 6;		//Modified

		for (unsigned int k=0; k<fields.numItems; ++k) { fields.fieldsList[k].columnWidth=1; fields.fieldsList[k].separator[0]='|'; }

		if(ARLoadARQualifierStruct(this->pControl, 
			schemaName,
			NULL, 
			qualString, 
			&qualifier, 
			this->pStatus) == AR_RETURN_OK)
		{
			if (ARGetListEntryWithFields(
				this->pControl,
				schemaName,
				&qualifier,
				&fields,
				NULL,0,0,0,
				&values,
				0,this->pStatus) == AR_RETURN_OK)
			{
				for (unsigned int row=0; row<values.numItems; ++row)
				{
					if (values.entryList[row].entryValues->fieldValueList[0].value.u.charVal == NULL) continue;

					CARRole role(row, values.entryList[row].entryValues->fieldValueList[0].value.u.charVal);

					if(values.entryList[row].entryValues->fieldValueList[1].value.u.charVal != NULL)
						role.applicationName = values.entryList[row].entryValues->fieldValueList[1].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[2].value.u.charVal != NULL)
					{
						role.roleName = values.entryList[row].entryValues->fieldValueList[2].value.u.charVal;
						role.name = role.roleName;
						role.comparableRoleName = role.roleName;
					}

					if(values.entryList[row].entryValues->fieldValueList[3].value.u.charVal != NULL)
						role.roleId = values.entryList[row].entryValues->fieldValueList[3].value.u.intVal;

					if(values.entryList[row].entryValues->fieldValueList[4].value.u.charVal != NULL)
					{
						string tmp = values.entryList[row].entryValues->fieldValueList[4].value.u.charVal;

						vector<string> tmpGroupList;
						tmpGroupList.clear();

						//				CUtil::SplitString(tmp, ";", tmpGroupList, false);
						CUtil::SplitString(tmp, tmpGroupList);

						role.testGroupId = atoi(tmpGroupList[0].c_str());
					}

					if(values.entryList[row].entryValues->fieldValueList[5].value.u.charVal != NULL)
					{
						string tmp = values.entryList[row].entryValues->fieldValueList[5].value.u.charVal;

						vector<string> tmpGroupList;
						tmpGroupList.clear();

						//				CUtil::SplitString(tmp, ";", tmpGroupList, false);
						CUtil::SplitString(tmp, tmpGroupList);


						role.productionGroupId = atoi(tmpGroupList[0].c_str());
					}

					if(values.entryList[row].entryValues->fieldValueList[6].value.u.charVal != NULL)
						role.createdBy = values.entryList[row].entryValues->fieldValueList[6].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[7].value.u.timeVal != NULL)
						role.created = values.entryList[row].entryValues->fieldValueList[7].value.u.timeVal;

					if(values.entryList[row].entryValues->fieldValueList[8].value.u.charVal != NULL)
						role.modifiedBy = values.entryList[row].entryValues->fieldValueList[8].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[9].value.u.timeVal != NULL)
						role.modified = values.entryList[row].entryValues->fieldValueList[9].value.u.timeVal;

					LOG << "Role '" << role.roleName <<"' [OK]" << endl;	

					listResult.push_back(role);
				}
				FreeAREntryListFieldValueList(&values,false);
			}
		}

		delete[] fields.fieldsList;
		delete[] qualString;
	}
	catch(exception& e)
	{		
		throw(AppException(e.what(), "Error loading roles.", "DataFactory"));
	}
}

bool CARDataFactory::SortRolesByName(const CARRole& t1, const CARRole& t2 )
{
	return t1.comparableRoleName.compare(t2.comparableRoleName) < 0;
}

void CARDataFactory::Sort(list<CARRole> &listResult)
{	
	listResult.sort(SortRolesByName);
}
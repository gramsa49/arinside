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
#include "AppConfig.h"

bool AppConfig::verboseMode = false;

AppConfig::AppConfig(void)
{
	// ARSystem data
	userForm = "";
	groupForm = "";
	roleForm = "";
	userQuery = "";
	groupQuery = "";
	roleQuery = "";
	maxRetrieve = 0;

	// Output and Layout configuration
	companyName = "";
	companyUrl = "";
	targetFolder = "";
	fileMode	= false;
	objListXML	= "";
	oldNaming = false;

	// BlackList
	blackList = "";

	//Restrict ObjectLibrary
	bLoadServerInfoList = true;
	bLoadUserList = true;
	bLoadGroupList = true;
	bLoadRoleList = true;
	bUseUtf8 = false;

	serverName = "";
	userName = "";
	password = "";
	tcpPort = 0;
	rpcPort = 0;
	bCompactFolder = false;
	bDeleteExistingFiles = false;

	//Notes to be displayed on home page
	runNotes = "";
}

AppConfig::~AppConfig(void)
{
}

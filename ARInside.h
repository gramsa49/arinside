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

#pragma once

class IFileNamingFactory;
class IFileStructure;

#include "ARApi.h"
#include "core/ARActiveLink.h"
#include "core/ARFilter.h"
#include "core/AREscalation.h"
#include "core/ARCharMenu.h"
#include "core/ARContainer.h"
#include "core/ARSchema.h"
#include "core/ARField.h"
#include "core/ARVui.h"
#include "core/AREnum.h"
#include "core/ARDataFactory.h"
#include "core/ARServerInfo.h"
#include "core/ARGlobalField.h"
#include "output/WebPage.h"
#include "output/IFileStructure.h"
#include "util/BlackListItem.h"
#include "WindowsUtil.h"
#include "AppConfig.h"
#include "util/RefItem.h"
#include "util/AppTimer.h"
#include "lists/ARSchemaList.h"
#include "lists/ARActiveLinkList.h"
#include "lists/ARFilterList.h"
#include "lists/AREscalationList.h"
#include "lists/ARContainerList.h"
#include "lists/ARMenuList.h"
#include "lists/ARImageList.h"
#include "lists/BlackList.h"

extern const string AppVersion;

class CARInside
{
public:
	CARInside(AppConfig &appConfig);
public:
	~CARInside(void);

	ARControlStruct		arControl;
	ARStatusList		arStatus;
	AppConfig			appConfig;

	int					arXmlVersion;
	string				arServerVersion;
	int					vMajor;
	int					vMinor;
	int					vRevision;

	inline static CARInside* GetInstance() { return CARInside::pInsideInstance; }
	
	int Init(string user, string pw, string server, int port, int rpc);
	void DoWork(int nMode);
	int Terminate(void);
	string GetARStatusError(ARStatusList* status);	

	bool FileExists(string fName);
	int ValidateTargetDir(string targetFolder);
	void LoadServerObjects(int nMode);
	void LoadFromServer(void);
	void LoadFromFile(void);

	bool InBlacklist(int refType, const string &objName) { 	return blackList.Contains(refType, objName.c_str()); }
	void Prepare(void);		
	void Documentation(void);


	string srvHostName;
	string srvFullHostName;
	CARSchemaList schemaList;
	CARFilterList filterList;
	CAREscalationList escalationList;
	CARActiveLinkList alList;
	CARContainerList containerList;
	CARMenuList menuList;
	list<CARUser> userList;
	list<CARGroup> groupList;
	list<CARRole> roleList;
	list<CARServerInfoItem> serverInfoList;
	list<CARGlobalField> globalFieldList;
	CBlackList blackList;
	map<string, vector<CRefItem> > missingMenuReferences;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	CARImageList	imageList;
#endif

	string LinkToServerInfo(string srvName, int rootLevel);
	string LinkToXmlObjType(int arsStructItemType, const string &objName, int subObjId, int rootLevel);
	string LinkToXmlObjType(int arsStructItemType, const string &objName, int rootLevel);
	string XmlObjEnabled(int arsStructItemType, string objName);
	string XmlObjEnabled(CARServerObject *obj);

	int SchemaGetInsideId(string searchObjName);

	string LinkToVui(const string& schemaName, int vuiInsideId, int fromRootLevel);
	string LinkToVui(int schemaInsideId, int vuiInsideId, int fromRootLevel);
	string LinkToField(const string& schemaName, int fieldInsideId, int fromRootLevel);	
	string LinkToField(int schemaInsideId, int fieldInsideId, int fromRootLevel, bool needValidField = true);
	string LinkToMenuField(int schemaInsideId, int fieldInsideId, int fromRootLevel);

	string LinkToContainer(string containerName, int rootLevel);
	string LinkToAl(const string& alName, int rootLevel);
	string LinkToAl(int alInsideId, int rootLevel);
	string LinkToAlRef(const string &alName, int rootLevel);
	string LinkToFilter(string filterName, int rootLevel);	
	string LinkToFilterRef(int filterInsideId, int rootLevel);	
	string LinkToFilterRef(string fltName, int rootLevel);	
	string LinkToFilterRef(CARFilter* filter, int rootLevel);
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	string LinkToImage(unsigned int imageIndex, int rootLevel);
	string LinkToImage(const string &imageName, int rootLevel);
#endif
	string LinkToMenu(string menuName, int rootLevel);
	string LinkToSchema(const string& schemaName, int fromRootLevel);
	string LinkToSchema(int insideId, int fromRootLevel);
	string LinkToSchemaIndex(string indexName, int schemaInsideId, int fromRootLevel);	
	string LinkToEscalation(string escalationName, int rootLevel);
	string LinkToUser(string loginName, int rootLevel);	
	string LinkToSchemaTypeList(int schemaType, int rootLevel);	
	string LinkToGroup(const string& appRefName, int permissionId, int rootLevel);
	string GetFieldEnumValue(int schemaInsideId, int fieldInsideId, int enumPosition);

	void AddFieldReference(int schemaId, int fieldId, const CRefItem& ref);
	void AddMenuReference(const string& menuName, const CRefItem& ref);

	string TextFindFields(string inText, string fieldSeparator, int schemaInsideId, int rootLevel, bool findKeywords, const CRefItem *refItem);
	string TextFindKeywords(string inText, string fieldSeparator);	
	string XMLFindFields(string inText, int schemaInsideId, int rootLevel, const CRefItem *refItem);

	bool FieldreferenceExists(int schemaInsideId, int fieldInsideId, const CRefItem &refItem);

	string ServerObjectHistory(CARServerObject *obj, int rootLevel);
	string DataObjectHistory(CARDataObject *obj, int rootLevel);
	bool ValidateGroup(const string& appRefName, int permissionId);
	int CompareServerVersion(int major, int minor = -1, int revision = -1);

	float nDurationLoad;
	float nDurationDocumentation;
private:
	static CARInside*	pInsideInstance;
private:	
	string GetARStatusError();	
	static bool SortByName(const CARServerObject& t1, const CARServerObject& t2 );	
	string ObjListFilename(string firstChar);

	void Sort(list<CARContainer> &listResult);
	void Sort(list<CARCharMenu> &listResult);

	int LoadActiveLinks(void);
	int LoadFilters(void);
	int LoadEscalations(void);
	int LoadCharMenus(void);
	int LoadContainer(void);
	int LoadForms(void);
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	int LoadImages(void);
#endif

	void BuildReferences();
	void SortReferences();

	void SearchCustomFieldReferences();
	void SearchActiveLinkReferences();
	void SearchFilterReferences();
	void SearchEscalationReferences();
	void SearchContainerReferences();
	void CustomFieldReferences(const CARSchema &schema, const CARField &obj);

	void ParseVersionString(string version);
	void ParseVersionString(int xmlVersion);

	bool getPos(const string& inText, const string& findText);
	string processOneField(const string& command, const string& inText, int schemaInsideId, int rootLevel, const CRefItem *refItem);
	string processTwoFields(const string& command, const string& inText, int schemaInsideId, int rootLevel, const CRefItem *refItem);
	string processForm(const string& command, const string& inText, int schemaInsideId, int rootLevel, const CRefItem *refItem);
	string processSecondParameter(const string& command, const string& inText, int schemaInsideId, int rootLevel, const CRefItem *refItem);
	string refFieldID(int iFieldID, int schemaInsideId, int rootLevel, const CRefItem *refItem);

	bool WriteHTAccess();
};

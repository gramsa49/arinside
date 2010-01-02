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
#include "ARApi.h"
#include "core/ARActiveLink.h"
#include "core/ARFilter.h"
#include "core/AREscalation.h"
#include "core/ARCharMenu.h"
#include "core/ARContainer.h"
#include "core/ARSchema.h"
#include "core/AREnum.h"
#include "core/ARDataFactory.h"
#include "core/ARServerInfo.h"
#include "core/ARGlobalField.h"
#include "core/ChangeHistoryEntry.h"
#include "output/WebPage.h"
#include "util/BlackListItem.h"
#include "WindowsUtil.h"
#include "AppConfig.h"
#include "util/FieldRefItem.h"
#include "util/MissingMenuRefItem.h"
#include "util/AppTimer.h"
#include "lists/ARActiveLinkList.h"
#include "lists/ARFilterList.h"
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

	static CARInside* GetInstance();
	
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


	CARSchema* FindSchema(int schemaInsideId);
	CARSchema* FindSchema(string schemaName);	
	CARField* FindField(CARSchema* schema, int fieldId);

	string srvHostName;
	string srvFullHostName;
	list<CARSchema> schemaList;
	CARFilterList filterList;
	list<CAREscalation> escalList;
	CARActiveLinkList alList;
	list<CARContainer> containerList;
	list<CARCharMenu> menuList;
	list<CARUser> userList;
	list<CARGroup> groupList;
	list<CARRole> roleList;
	list<CARServerInfoItem> serverInfoList;
	list<CARGlobalField> globalFieldList;
	CBlackList blackList;
	list<CFieldRefItem> listFieldRefItem;
	list<CMissingMenuRefItem> listMenuRefItem;
	list<CFieldRefItem> listFieldNotFound;
	list<CMissingMenuRefItem> listMenuNotFound;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	CARImageList	imageList;
#endif

	string LinkToServerInfo(string srvName, int rootLevel);
	string LinkToXmlObjType(int arsStructItemType, const string &objName, int subObjId, int rootLevel);
	string LinkToXmlObjType(int arsStructItemType, const string &objName, int rootLevel);
	string XmlObjEnabled(int arsStructItemType, string objName);
	string XmlObjEnabled(CARServerObject *obj);

	int SchemaGetInsideId(string searchObjName);

	string LinkToField(string schemaName, int fieldInsideId, int fromRootLevel);	
	string LinkToField(int schemaInsideId, int fieldInsideId, int fromRootLevel);
	string LinkToMenuField(int schemaInsideId, int fieldInsideId, int fromRootLevel);	

	string LinkToContainer(string containerName, int rootLevel);
	string LinkToAl(string alName, int rootLevel);
	string LinkToAl(int alInsideId, int rootLevel);
	string LinkToAlRef(int alInsideId, int rootLevel);
	string LinkToAlRef(const string &alName, int rootLevel);
	string LinkToAlRef(CARActiveLink &al, int rootLevel);
	string LinkToFilter(string filterName, int rootLevel);	
	string LinkToFilterRef(int filterInsideId, int rootLevel);	
	string LinkToFilterRef(string fltName, int rootLevel);	
	string LinkToFilterRef(CARFilter* filter, int rootLevel);
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	string LinkToImage(unsigned int imageIndex, int rootLevel);
	string LinkToImage(const string &imageName, int rootLevel);
#endif
	string LinkToMenu(string menuName, int rootLevel, bool* bFound = NULL);
	string LinkToSchema(string schemaName, int fromRootLevel);
	string LinkToSchema(int insideId, int fromRootLevel);
	string LinkToSchemaIndex(string indexName, int schemaInsideId, int fromRootLevel);	
	string LinkToEscalation(string escalationName, int rootLevel);
	string LinkToUser(string loginName, int rootLevel);	
	string LinkToSchemaTypeList(int schemaType, int rootLevel);	
	string LinkToGroup(string appRefName, int permissionId, int rootLevel);
	string GetFieldEnumValue(int schemaInsideId, int fieldInsideId, int enumPosition);

	void AddReferenceItem(CFieldRefItem *refItem);
	void AddMissingMenu(const CMissingMenuRefItem& refItem);
	string TextFindFields(string inText, string fieldSeparator, int schemaInsideId, int rootLevel, bool findKeywords, CFieldRefItem *refItem);
	string TextFindKeywords(string inText, string fieldSeparator);	
	string XMLFindFields(string inText, int schemaInsideId, int rootLevel, CFieldRefItem *refItem);

	bool FieldreferenceExists(int schemaInsideId, int fieldInsideId, const CFieldRefItem &refItem);

	string ServerObjectHistory(CARServerObject *obj, int rootLevel);
	string DataObjectHistory(CARDataObject *obj, int rootLevel);
	bool ValidateGroup(string appRefName, int permissionId);
	int CompareServerVersion(int major, int minor = -1, int revision = -1);

	float nDurationLoad;
	float nDurationDocumentation;
private:
	static CARInside*	pInsideInstance;
private:	
	string GetARStatusError();	
	static bool SortByName(const CARServerObject& t1, const CARServerObject& t2 );	
	string ObjListFilename(string firstChar);

	int ContainerGetInsideId(string searchObjName);	
	int AlGetInsideId(string searchObjName);	
	int FilterGetInsideId(string searchObjName);	
	int EscalationGetInsideId(string searchObjName);	
	int MenuGetInsideId(string searchObjName);	

	void Sort(list<CARSchema> &listResult);
	void Sort(list<CARFilter> &listResult);
	void Sort(list<CAREscalation> &listResult);
	void Sort(list<CARActiveLink> &listResult);
	void Sort(list<CARContainer> &listResult);
	void Sort(list<CARCharMenu> &listResult);
	void Sort(list<CARField> &listResult);

	int LoadActiveLinks(void);
	int LoadFilters(void);
	int LoadEscalations(void);
	int LoadCharMenus(void);
	int LoadContainer(void);
	int LoadForms(int nType, int &schemaInsideId);
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	int LoadImages(void);
#endif

	void BuildReferences();

	void SearchCustomFieldReferences();
	void SearchFilterReferences();
	void CustomFieldReferences(CARSchema &schema, CARField &obj);

	void ParseVersionString(string version);
	void ParseVersionString(int xmlVersion);

	bool getPos(string inText, string findText);
	string processOneField(string command, string inText, int schemaInsideId, int rootLevel, CFieldRefItem *refItem);
	string processTwoFields(string command, string inText, int schemaInsideId, int rootLevel, CFieldRefItem *refItem);
	string processForm(string command, string inText, int schemaInsideId, int rootLevel, CFieldRefItem *refItem);
	string processSecondParameter(string command, string inText, int schemaInsideId, int rootLevel, CFieldRefItem *refItem);
	string refFieldID(int iFieldID, int schemaInsideId, int rootLevel, CFieldRefItem *refItem);
};

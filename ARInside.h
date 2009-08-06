
/****************************************************************************** 
 * 
 *  file:  ARInside.h
 * 
 *  Copyright (c) 2007, Stefan Nerlich | stefan.nerlich@hotmail.com 
 *  All rights reverved.
 * 
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *  
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.  
 *  
 *****************************************************************************/

#pragma once
#include "arapi.h"
#include "core\aractivelink.h"
#include "core\arfilter.h"
#include "core\arescalation.h"
#include "core\archarmenu.h"
#include "core\arcontainer.h"
#include "core\arschema.h"
#include "core\arenum.h"
#include "core\ardatafactory.h"
#include "core\arserverinfo.h"
#include "core\arglobalfield.h"
#include "core\changehistoryentry.h"
#include "output\webpage.h"
#include "util\blacklistitem.h"
#include "windowsutil.h"
#include "appconfig.h"
#include "util\fieldrefitem.h"
#include "util\menurefitem.h"
#include "util\apptimer.h"

extern bool verboseMode;

class CARInside
{
public:
	CARInside(AppConfig &appConfig);
public:
	~CARInside(void);

	ARControlStruct		arControl;
	ARStatusList		arStatus;
	AppConfig			appConfig;	

	int Init(string user, string pw, string server, int port, int rpc);
	int Terminate(void);

	bool FileExists(string fName);
	int ValidateTargetDir(string targetFolder);
	void LoadServerObjects(int nMode);
	void LoadFromServer(void);
	void LoadFromFile(void);

	bool InBlacklist(int refType, string objName);
	void LoadBlackList(void);	
	void Prepare(void);		
	void Documentation(void);


	string srvHostName;
	string srvFullHostName;
	list<CARSchema> schemaList;
	list<CARFilter> filterList;
	list<CAREscalation> escalList;
	list<CARActiveLink> alList;
	list<CARContainer> containerList;
	list<CARCharMenu> menuList;
	list<CARUser> userList;
	list<CARGroup> groupList;
	list<CARRole> roleList;
	list<CARServerInfoItem> serverInfoList;
	list<CARGlobalField> globalFieldList;
	list<CBlackListItem> blackList;
	list<CFieldRefItem> listFieldRefItem;
	list<CMenuRefItem> listMenuRefItem;
	list<CFieldRefItem> listFieldNotFound;
		
	string LinkToServerInfo(string srvName, int rootLevel);
	string LinkToXmlObjType(int arsStructItemType, string objName, int rootLevel);
	string XmlObjEnabled(int arsStructItemType, string objName);
	
	int SchemaGetInsideId(string searchObjName);

	string LinkToField(int schemaInsideId, int fieldInsideId, int fromRootLevel);
	string LinkToField(string schemaName, int fieldInsideId, int fromRootLevel);	
	string LinkToMenuField(int schemaInsideId, int fieldInsideId, int fromRootLevel);	
	
	string LinkToContainer(string containerName, int rootLevel);
	string LinkToAl(string alName, int rootLevel);
	string LinkToAlRef(int alInsideId, int rootLevel);
	string LinkToFilter(string filterName, int rootLevel);	
	string LinkToFilterRef(int filterInsideId, int rootLevel);	
	string LinkToMenu(string menuName, int rootLevel);
	string LinkToSchema(string schemaName, int fromRootLevel);
	string LinkToSchema(int insideId, int fromRootLevel);
	string LinkToSchemaIndex(string indexName, int schemaInsideId, int fromRootLevel);	
	string LinkToEscalation(string escalationName, int rootLevel);
	string LinkToUser(string loginName, int rootLevel);	
	string LinkToSchemaTypeList(int schemaType, int rootLevel);	
	string LinkToGroup(string appRefName, int permissionId, int rootLevel);
	string GetFieldEnumValue(int schemaInsideId, int fieldInsideId, int enumPosition);

	void AddReferenceItem(CFieldRefItem *refItem);
	string TextFindFields(string inText, string fieldSeparator, int schemaInsideId, int rootLevel, bool findKeywords, CFieldRefItem *refItem);
	string TextFindKeywords(string inText, string fieldSeparator);	
	string XMLFindFields(string inText, int schemaInsideId, int rootLevel, CFieldRefItem *refItem);

	bool FieldreferenceExists(int schemaInsideId, int fieldInsideId, CFieldRefItem &refItem);

	string ServerObjectHistory(CARServerObject *obj, int rootLevel);
	string DataObjectHistory(CARDataObject *obj, int rootLevel);
	bool ValidateGroup(string appRefName, int permissionId);

	float nDurationLoad;
	float nDurationDocumentation;
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
			
	void SearchCustomFieldReferences();
	void CustomFieldReferences(CARSchema &schema, CARField &obj);
};

#include "ARInsideTest.h"
#include "AppConfigReader.h"
#include "AppConfig.h"

TEST(AppConfigReader, CheckDefaults)
{
	AppConfigReader cfgReader("mytest.file");
	AppConfig myConfig;

	cfgReader.LoadTo(myConfig);

	ASSERT_EQ("", myConfig.serverName);
	ASSERT_EQ("", myConfig.userName);
	ASSERT_EQ("", myConfig.password);
	ASSERT_EQ(0, myConfig.tcpPort);
	ASSERT_EQ(0, myConfig.rpcPort);

	ASSERT_EQ("", myConfig.targetFolder);
	ASSERT_EQ(false, myConfig.fileMode);
	ASSERT_EQ("", myConfig.objListXML);
	ASSERT_EQ(false, myConfig.bUseUtf8);
	ASSERT_EQ("", myConfig.blackList);
	ASSERT_EQ(true, myConfig.bLoadServerInfoList);
	ASSERT_EQ(false, myConfig.bCompactFolder);
	ASSERT_EQ(false, myConfig.bGZCompression);
	ASSERT_EQ(false, myConfig.bDeleteExistingFiles);
	ASSERT_EQ(false, myConfig.oldNaming);
	ASSERT_EQ("TRUE", myConfig.overlayMode);

	ASSERT_EQ(true, myConfig.bLoadUserList);
	ASSERT_EQ("User", myConfig.userForm);
	ASSERT_EQ("1=1", myConfig.userQuery);

	ASSERT_EQ(true, myConfig.bLoadGroupList);
	ASSERT_EQ("Group", myConfig.groupForm);
	ASSERT_EQ("1=1", myConfig.groupQuery);

	ASSERT_EQ(true, myConfig.bLoadRoleList);
	ASSERT_EQ("Roles", myConfig.roleForm);
	ASSERT_EQ("1=1", myConfig.roleQuery);

	ASSERT_EQ("", myConfig.companyName);
	ASSERT_EQ("", myConfig.companyUrl);

	ASSERT_EQ("", myConfig.runNotes);
}


/****************************************************************************** 
 * 
 *  file:  AppConfig.h
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

using namespace std;

class AppConfig
{
public:
	AppConfig(void);
	~AppConfig(void);

	// ARSystem data
	string userForm;
	string groupForm;
	string roleForm;
	string userQuery;
	string groupQuery;
	string roleQuery;
	int maxRetrieve;
		
	// Output and Layout configuration
	string companyName;
	string companyUrl;
	string targetFolder;
	bool fileMode;
	string objListXML;
	
	// BlackList
	string blackList;

	//Restrict ObjectLibrary
	bool bLoadServerInfoList;
	bool bLoadUserList;
	bool bLoadGroupList;
	bool bLoadRoleList;
	bool bUseUtf8;
	bool bCompactFolder;
	bool bDeleteExistingFiles;

	string serverName;
};

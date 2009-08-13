
/****************************************************************************** 
 * 
 *  file:  WebPage.h
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
#include "..\appconfig.h"
#include "..\appexception.h"

namespace OUTPUT
{

class CWebPage
{
public:
	CWebPage(string fileName, string title, int dirLevel, AppConfig &appConfig);
	~CWebPage(void);

	void AddContent(string content);	
	void AddContentHead(string description);
	void AddNavigation(string nav);
	int SaveInFolder(string path);	

private:
	string PageHeader();
	string ContentOpen();
	string ContentClose();
	stringstream bodyStrm;
	stringstream contentStrm;
	stringstream navStrm;
	string fileName;
	string title;
	int rootLevel;

	string GetFileContent();
	AppConfig appConfig;
	string DynamicHeaderText();
	string DynamicFooterText();
	string CurrentDateTime();
};
}

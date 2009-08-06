
/****************************************************************************** 
 * 
 *  file:  CsvPage.cpp
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

#include "StdAfx.h"
#include "CsvPage.h"
#include "webutil.h"
#include "..\arinside.h"

using namespace OUTPUT;

#define LOG if(verboseMode) cout

extern int nFilesCreated;

CCsvPage::CCsvPage(string fileName, AppConfig &appConfig)
{
	this->fileName = fileName;
	this->appConfig = appConfig;
}

CCsvPage::~CCsvPage(void)
{
	
}

int CCsvPage::SaveInFolder(string path, string content)
{
	int result = 0;

	stringstream strm;
	strm.str("");

	if(!path.empty())
		strm << this->appConfig.targetFolder << "\\" << path << "\\" << CWebUtil::CsvDocName(this->fileName);
	else
		strm << this->appConfig.targetFolder << "\\" << CWebUtil::CsvDocName(this->fileName);

	try
	{	
		LOG << "Save file '" << strm.str();

		ofstream fout( strm.str().c_str(), ios::out);
		fout << content << endl;
		fout.close();

		LOG << "' [OK]" << endl;
		
		nFilesCreated++;
		result = 1;
	}
	catch(...)
	{		
		stringstream erStrm;
		erStrm << "Error saving file '" << strm.str() << "' to disk. ";
		throw(AppException(erStrm.str(), "undefined", "FileIo"));
	}

	return result;
}
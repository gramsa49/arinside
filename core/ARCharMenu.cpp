
/****************************************************************************** 
 * 
 *  file:  ARCharMenu.cpp
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
#include ".\archarmenu.h"

CARCharMenu::CARCharMenu(string name, int insideId)
{
	this->name = name;
	this->insideId = insideId;

	this->refreshCode = 0;

	this->objPropList.props = NULL;
}

CARCharMenu::~CARCharMenu(void)
{
	try
	{
		FreeARCharMenuStruct(&menuDefn, false);
		
		if(objPropList.props != NULL)
			FreeARPropList(&objPropList, false);
	}
	catch(...)
	{
	}
}

string CARCharMenu::GetURL(int rootLevel)
{
	return CWebUtil::Link(this->name, CWebUtil::RootPath(rootLevel)+"menu/"+this->FileID()+"/"+CWebUtil::DocName("index"), "menu.gif", rootLevel);
}
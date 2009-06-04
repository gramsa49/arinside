
/****************************************************************************** 
 * 
 *  file:  UlList.cpp
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
#include "ulist.h"

using namespace OUTPUT;

CUList::CUList(int rootLevel, string cssClass)
{
	this->cssClass = cssClass;
	this->rootLevel = rootLevel;
}

CUList::~CUList(void)
{
	this->listItem.clear();
}

void CUList::AddItem(CUListItem *li)
{
	this->listItem.insert(this->listItem.end(), *li);
}

string CUList::ToXHtml(string title, bool bItemImage)
{
	stringstream strm;

	if(title.size() > 0)
		strm << title << endl;


	strm << "<ul>" << endl;
	list<CUListItem>::iterator iter;
	CUListItem *listItem;	

	for ( iter = this->listItem.begin(); iter != this->listItem.end(); iter++ )
	{	
		listItem = &(*iter);

		string liImage = "";

		if(bItemImage)
			liImage = CWebUtil::ImageTag("doc.gif", rootLevel);

		strm << "<li>" << liImage << listItem->itemText << "</li>" << endl;
	}
	strm << "</ul>" << endl;
	return strm.str();
}
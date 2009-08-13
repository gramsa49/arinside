
/****************************************************************************** 
 * 
 *  file:  TableColumn.cpp
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
#include ".\tablecolumn.h"

using namespace OUTPUT;

CTableColumn::CTableColumn(int width, string title)
{
	this->width = width;
	this->title = title;
}

CTableColumn::CTableColumn(int width, string title, string cssClass)
{
	this->width = width;
	this->title = title;
	this->cssClass = cssClass;
}

CTableColumn::~CTableColumn(void)
{
}

string CTableColumn::ColToXHtml()
{
	stringstream strm;		
	strm << "<col width=\"" << this->width << "%\"/>" << endl;		
	return strm.str();
}

string CTableColumn::HeaderCellToXHtml()
{
	stringstream strm;	
	if(this->cssClass.length() == 0)
	{
		strm << "<th>" << this->title << "</th>" << endl;
	}
	else
	{
		strm << "<th class=\"" << this->cssClass << "\" width=\""<< this->width<<"%\">" << this->title << "</th>" << endl;
	}
	return strm.str();
}

string CTableColumn::GetTitle()
{
	return this->title;
}
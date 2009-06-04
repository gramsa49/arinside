
/****************************************************************************** 
 * 
 *  file:  TableCell.cpp
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
#include ".\tablecell.h"

using namespace OUTPUT;

CTableCell::CTableCell()
{
	this->content = "";
	this->cssClass = "";
}

CTableCell::CTableCell(int content)
{
	stringstream strm;
	strm << content;
	this->content = strm.str();
}

CTableCell::CTableCell(string content)
{
	this->content = content;
}

CTableCell::CTableCell(string content, string cssClass)
{
	this->content = content;
	
	if(!cssClass.empty())
		this->cssClass = cssClass;
}

CTableCell::CTableCell(int content, string cssClass)
{
	stringstream strm;
	strm << content;
	this->content = strm.str();
	
	if(!cssClass.empty())
		this->cssClass = cssClass;
}


CTableCell::~CTableCell()
{
	this->content = "";
	this->cssClass = "";
}

string CTableCell::ToXHtml(void)
{
	stringstream strm;	
	if(this->cssClass.empty())
	{
		strm << "<td>" << this->content << "</td>" << endl;
	}
	else
	{
		strm << "<td class=\"" << this->cssClass << "\">" << this->content << "</td>" << endl;
	}
	return strm.str();
}

string CTableCell::ToCsv(void)
{
	stringstream strm;	
	strm.str("");

	strm << this->content << "\t";

	return strm.str();
}
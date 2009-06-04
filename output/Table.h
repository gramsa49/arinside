
/****************************************************************************** 
 * 
 *  file:  Table.h
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
#include ".\webcontrol.h"
#include ".\tablerow.h"
#include ".\tablecolumn.h"

namespace OUTPUT
{
class CTable :
	public CWebControl
{
public:
	CTable();
	CTable(string htmId, string cssClass);
	~CTable(void);
	
	string description;	
	
	string ToXHtml();
	string ToXHtmlNoHeader();
	string ToXHtml(string styleTag);
	string ToCsv();

	void Clear();
	void ClearRows();
	void ClearColumns();
	void SetHtmId(string cssClass);
	void SetCssClass(string htmId);
	int NumRows();
	void AddColumn(int width, string text);
	void AddColumn(int width, string text, string cssClass);	
	void AddRow(CTableRow tableRow);

private:	
	list<CTableRow> listRows;
	list<CTableColumn> listColumns;
	string htmId;
	string GetHtmlRows();
	string GetCsvRows();
	string GetColumnDefinition();
	string GetHeaderDefinition();
	string GetCsvHeaderDefinition();
};
}
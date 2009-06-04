
/****************************************************************************** 
 * 
 *  file:  TableRow.h
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
#include "webcontrol.h"
#include "tablecell.h"

namespace OUTPUT
{
class CTableRow :
	public CWebControl
{
public:
	CTableRow();
	CTableRow(string cssClass);
	~CTableRow(void);
	
	string ToXHtml();
	string ToCsv();

	string Name;
	void AddCell(string cellText);
	void AddCell(CTableCell cell);

	void AddCellList(string cellText0, string cellText1);
	void AddCellList(CTableCell cell0, CTableCell cell1);

	void AddCellList(CTableCell cell0, CTableCell cell1, CTableCell cell2);	
	void AddCellList(string cellText0, string cellText1, string cellText2);
	void ClearCells();
private:
	string GetCells();	
	list<CTableCell> listCells;
	
};
}

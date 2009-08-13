
/****************************************************************************** 
 * 
 *  file:  DocApplicationDetails.h
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
#include ".\docbase.h"
#include ".\doccontainerhelper.h"

class CDocApplicationDetails :
	public CDocBase
{
public:
	CDocApplicationDetails(CARInside &arIn, CARContainer &application);
	~CDocApplicationDetails(void);

	void Documentation();

private:
	CARContainer *pApp;

	string GetPrimaryForm();
	string ApplicationInformation();

	string SearchActiveLinks(int &nResult);
	string SearchFilters(int &nResult);
	string SearchEscalations(int &nResult);
	string SearchForms(int &nResult);	
	string SearchContainer(int &nResult, int nType);
	string SearchMenus(int &nResult);
	bool InList(string searchName, int objType);
};

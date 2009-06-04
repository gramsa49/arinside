
/****************************************************************************** 
 * 
 *  file:  DocCharMenuDetails.h
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
#include "..\core\arproplisthelper.h"

class CDocCharMenuDetails :
	public CDocBase
{
public:
	CDocCharMenuDetails(CARInside &arInside, CARCharMenu &arCharMenu, string path, int rootLevel);
	~CDocCharMenuDetails(void);

	void Documentation();
private:
	CARCharMenu *pMenu;
	
	string CharMenuDetails();
	string SearchMenuDetails();
	string FileMenuDetails();
	string SqlMenuDetails();
	string DataDictMenuDetails();
	string GetFieldTypes(unsigned int fieldMask);
	string RelatedFields();
	string RelatedActiveLinks();
};

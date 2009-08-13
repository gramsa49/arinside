
/****************************************************************************** 
 * 
 *  file:  DocAlDetails.h
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
#include ".\docalactionstruct.h"
#include "..\output\grouptable.h"
#include "..\output\containertable.h"
#include "..\core\arproplisthelper.h"

class CDocAlDetails :
	public CDocBase
{
public:
	CDocAlDetails(CARInside &arInside, CARActiveLink &arActiveLink, string path, int rootLevel);
	~CDocAlDetails(void);

	void Documentation();
private:
	CARActiveLink *pAl;

	string Permissions();
	string ContainerReferences();
	string CreateSpecific(string schemaName);
	
};

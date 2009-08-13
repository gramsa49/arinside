
/****************************************************************************** 
 * 
 *  file:  ARContainer.cpp
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
#include ".\arcontainer.h"

CARContainer::CARContainer(string name, int insideId)
{
	this->name = name;
	this->insideId = insideId;	
	this->type = 0;

	this->objPropList.props = NULL;
}

CARContainer::~CARContainer(void)
{
	try
	{
		FreeARReferenceList(&references, false);
		FreeARPermissionList(&groupList, false);
		FreeARInternalIdList(&admingrpList, false);
		FreeARContainerOwnerObjList(&ownerObjList, false);
		FreeARReferenceList(&references, false);

		if(objPropList.props != NULL)
			FreeARPropList(&objPropList, false);
	}
	catch(...)
	{
	}
}

string CARContainer::GetURL(int rootLevel)
{
	string tmp = CWebUtil::RootPath(rootLevel)+CAREnum::ContainerDir(this->type)+"/"+this->FileID()+"/"+CWebUtil::DocName("index");
	return CWebUtil::Link(this->name, tmp, CAREnum::ContainerImage(this->type), rootLevel);
}
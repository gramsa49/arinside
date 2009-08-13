
/****************************************************************************** 
 * 
 *  file:  ARField.cpp
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
#include ".\arfield.h"

CARField::CARField()
{
	this->fieldId = 0;
	this->dataType = 0;
	this->option = 0;
	this->createMode = 0;
}


CARField::CARField(int insideId)
{	
	this->insideId = insideId;
	this->fieldId = 0;
	this->dataType = 0;
	this->option = 0;
	this->createMode = 0;
}

CARField::~CARField(void)
{ 
	try
	{
		FreeARValueStruct (&defaultVal, false);			
		FreeARFieldLimitStruct (&limit, false);	
		FreeARPermissionList (&permissions, false);	
		FreeARDisplayInstanceList (&dInstanceList, false);
	}
	catch(...)
	{
	}
}

string CARField::GetURL(int rootLevel)
{
	stringstream tmp;
	tmp.str("");
	
	tmp << CWebUtil::RootPath(rootLevel) << "schema/" << this->schemaInsideId << "/" << CWebUtil::DocName(this->FileID());
	return CWebUtil::Link(this->name, tmp.str(), "", rootLevel);
}
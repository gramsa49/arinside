
/****************************************************************************** 
 * 
 *  file:  ARObject.cpp
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
#include "ARObject.h"
#include "..\output\webpage.h"

using namespace OUTPUT;

CARObject::CARObject(void)
{
	this->insideId = 0;
	this->name = "";
}

CARObject::~CARObject(void)
{
}

string CARObject::FileID()
{
	char buffer[20];
	return _itoa(this->insideId, buffer, 10);
}

string CARObject::GetNameFirstChar()
{
	return CUtil::String2Comp(std::string(1, name.at(0)));
}


bool CARObject::NameStandardFirstChar()
{
	string strValue = "abcdefghijklmnopqrstuvwxyz0123456789";
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{
		if(this->GetNameFirstChar() == std::string(1, strValue.at(i)))
		{
			return true;
		}
	}

	return false;
}
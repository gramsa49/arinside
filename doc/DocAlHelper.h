
/****************************************************************************** 
 * 
 *  file:  DocAlHelper.h
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
#include "..\arinside.h"
#include "..\core\arqualification.h"

class CDocAlHelper
{
public:
	CDocAlHelper(CARInside &arInside, CARActiveLink &arActiveLink);
	~CDocAlHelper(void);

	void SetFieldsGetSecondaryForm(ARSetFieldsActionStruct sFieldStruct, string fromSchema, int rootLevel, stringstream &strmSchema, stringstream &strmSchemaDisplay, stringstream &strmServer, stringstream &strmQual);
	
private:
	CARInside *pInside;
	CARActiveLink *pAl;

	void FieldAssignement(ARAssignStruct &assignment, string fromSchema, int rootLevel, stringstream &assignSchema, stringstream &assignSchemaDisplay, stringstream &assignServer, stringstream &assignQual, string refItemDesc);
	void CheckAssignment(ARAssignStruct &assignment, string fromSchema, int rootLevel, stringstream &assignSchema, stringstream &assignSchemaDisplay, stringstream &assignServer, stringstream &assignQual, string refItemDesc);
};

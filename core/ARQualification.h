
/****************************************************************************** 
 * 
 *  file:  ARQualification.h
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

class CARQualification
{
public:
	CARQualification(CARInside &arIn);
	~CARQualification(void);

	void CheckQuery(ARQualifierStruct *query, CFieldRefItem refItem, int depth, int pFormId, int sformId, stringstream &qText, int rootLevel);	

	int arsStructItemType;;

private:
	CARInside *arIn;
	void CheckOperand(ARFieldValueOrArithStruct *operand, CFieldRefItem refItem, int pFormId, int sFormId, stringstream &qText, int rootLevel);
	
	int tmpFieldId;
	int tmpFormId;
};

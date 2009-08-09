
/****************************************************************************** 
 * 
 *  file:  ARAssignHelper.h
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
#include ".\arvalue.h"
#include "..\output\table.h"
#include "..\output\tablerow.h"
#include "..\arinside.h"

using namespace OUTPUT;

class CARAssignHelper
{
public:
	CARAssignHelper(CARInside &arIn, string dir, int rootLevel, string objName, int objType, string schemaName1, string schemaName2);
	~CARAssignHelper(void);

	bool pushFieldFlag;
	bool openWindowFlag;

	string PushFieldsAssignment(ARPushFieldsActionStruct &action, int nAction, string ifElse);
	string SetFieldsAssignment(ARSetFieldsActionStruct &action, int nAction, string ifElse);
	string OpenWindowAssignment(ARFieldAssignList &action, int nAction, string ifElse, string openCloseInfo);
	string CloseWindowAssignment(ARFieldAssignList &action, int nAction, string ifElse, string openCloseInfo);
	string ServiceAssignment(ARFieldAssignList &action, int nAction, string ifElse, string serviceInfo);
	void CheckAssignment(int targetFieldId, string ifElse, int nAction, ARAssignStruct &assignment, stringstream &assignText, string refItemDesc);
	
	void AssignValue(int targetFieldId, string ifElse, ARValueStruct &v, stringstream &assignText, string refItemDesc);	
	void AssignField(string ifElse, int nAction, ARAssignFieldStruct &v, stringstream &assignText, string refItemDesc);
	void AssignProcess(string ifElse, char *v, stringstream &assignText, string refItemDesc);
	void AssignFunction(int targetFieldId, string ifElse, int nAction, ARFunctionAssignStruct &v, stringstream &assignText, string refItemDesc);	
	void AssignDDE(string ifElse, ARDDEStruct &v, stringstream &assignText, string refItemDesc);	
	void AssignSQL(string ifElse, ARAssignSQLStruct &v, stringstream &assignText, string refItemDesc);	
	void AssignFilterApi(string ifElse, ARAssignFilterApiStruct &v, stringstream &assignText, string refItemDesc);	

private:
	CARInside *arIn;
	string dir;
	string schemaName1;
	string schemaName2;
	int schemaInsideId1;
	int schemaInsideId2;
	int rootLevel;
    
	string objName;
	int objType;
};


/****************************************************************************** 
 * 
 *  file:  DocSchemaDetails.h
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
#include "..\output\table.h"
#include "..\output\tablerow.h"
#include "..\output\filtertable.h"
#include "..\output\altable.h"
#include "..\output\escaltable.h"
#include "..\output\containertable.h"
#include "..\output\menutable.h"
#include "..\output\schematable.h"

class CDocSchemaDetails :
	public CDocBase
{
public:
	CDocSchemaDetails(CARInside &arInside, CARSchema &schema, string path, int rootLevel);
	~CDocSchemaDetails(void);

	void Documentation();
private:
	CARSchema *pSchema;
	
	void IndexDoc();
	void ResultListDoc();
	void SortListDoc();
	void VuiListDoc();
	void SchemaFilterDoc();
	void SchemaAlDoc();
	void SchemaEscalDoc();
	void SchemaPermissionDoc();
	void SchemaSubadminDoc();
	void WorkflowDoc();


	string AllFields(string fName);
	void AllFieldsCsv(string fName);
	string AllFieldsJoin(string fName);
	void AllFieldsJoinCsv(string fName);

	string TypeDetails();
	string ContainerReferences();
	string TableFieldReferences();
	string AlPushFieldsReferences();
	string FilterPushFieldsReferences();
	string AlWindowOpenReferences();
	string JoinFormReferences();
	string SearchMenuReferences();
	
	string SchemaNavigation();	
	string FormPageHeader(string description);

	bool InAlList(string objName);
	bool InFilterList(string objName);
	bool InEscalList(string objName);
	list<string> uniqueAlList;
	list<string> uniqueFilterList;
	list<string> uniqueEscalList;
	CTableRow UniqueTblRow(string name, int itemType);
};

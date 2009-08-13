
/****************************************************************************** 
 * 
 *  file:  DocValidator.h
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

class CDocValidator :
	public CDocBase
{
public:
	CDocValidator(CARInside &arIn, string path);
	~CDocValidator(void);

	void Main();
private:
	int NumReferences(int searchSchemaId, int searchFieldId);
	void BuildUniqueFieldList();
	bool InList(int fieldId, int schemaId);
	list<CFieldRefItem> uniqueMissingFieldList;

	void FieldReferenceValidator();

	//Permission check
	void FieldGroupValidator();
	void FieldGroupValidatorDetails(CARSchema &schema, string fName);

	void FormGroupValidator();
	void AlGroupValidator();
	void ContainerGroupValidator();
};
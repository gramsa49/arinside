
/****************************************************************************** 
 * 
 *  file:  WebUtil.h
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

namespace OUTPUT
{
class CWebUtil
{
public:
	CWebUtil(void);
	~CWebUtil(void);

	static string RootPath(int level);	
	static string Validate(string text);
	static string Replace(string text);	
	static string Link(string caption, string linkTo, string imgName, int rootLevel);
	static string Link(string caption, string linkTo, string imgName, int rootLevel, bool validate);
	static string WebPageSuffix();
	static string CsvPageSuffix();
	static string ImageTag(string imgName, int rootLevel);
	static string DocName(string fName);
	static string CsvDocName(string fName);
	static string ObjName(string objName);

	static string LinkToActiveLinkIndex(int rootLevel);
	static string LinkToActiveLinkIndex(int objectCount, int rootLevel);
	static string LinkToActiveLinkGuideIndex(int rootLevel);
	static string LinkToActiveLinkGuideIndex(int objectCount, int rootLevel);
	static string LinkToApplicationIndex(int rootLevel);
	static string LinkToApplicationIndex(int objectCount, int rootLevel);
	static string LinkToEscalationIndex(int rootLevel);
	static string LinkToEscalationIndex(int objectCount, int rootLevel);
	static string LinkToFilterIndex(int rootLevel);
	static string LinkToFilterIndex(int objectCount, int rootLevel);
	static string LinkToFilterGuideIndex(int rootLevel);
	static string LinkToFilterGuideIndex(int objectCount, int rootLevel);
	static string LinkToGroupIndex(int rootLevel);
	static string LinkToGroupIndex(int objectCount, int rootLevel);
	static string LinkToMenuIndex(int rootLevel);
	static string LinkToMenuIndex(int objectCount, int rootLevel);
	static string LinkToPackingListIndex(int rootLevel);
	static string LinkToPackingListIndex(int objectCount, int rootLevel);
	static string LinkToRoleIndex(int rootLevel);
	static string LinkToRoleIndex(int objectCount, int rootLevel);
	static string LinkToSchemaIndex(int rootLevel);
	static string LinkToSchemaIndex(int objectCount, int rootLevel);
	static string LinkToUserIndex(int rootLevel);
	static string LinkToUserIndex(int objectCount, int rootLevel);
	static string LinkToWebServiceIndex(int rootLevel);
	static string LinkToWebServiceIndex(int objectCount, int rootLevel);	
	static string LinkToContainer(int objectCount, int rootLevel, int containerType);

	static string LinkToHelper(string name, int objectCount, string folderName, string image, int rootLevel);
};
}

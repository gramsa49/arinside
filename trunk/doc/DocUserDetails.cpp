
/****************************************************************************** 
 * 
 *  file:  DocUserDetails.cpp
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
#include ".\docuserdetails.h"

CDocUserDetails::CDocUserDetails(CARInside &arIn, CARUser &arUser)
{
	this->pInside = &arIn;
	this->pUser = &arUser;
	this->rootLevel = 1;
}

CDocUserDetails::~CDocUserDetails(void)
{
}

void CDocUserDetails::Documentation()
{
	try
	{
		CWebPage webPage(this->pUser->FileID(), this->pUser->loginName, this->rootLevel, this->pInside->appConfig);

		//ContentHead informations
		webPage.AddContentHead(CWebUtil::LinkToUserIndex(this->rootLevel) + MenuSeparator + CWebUtil::ObjName(this->pUser->loginName));

		//User details
		CTable tbl("userDetails", "TblObjectList");
		tbl.AddColumn(30, "Description");
		tbl.AddColumn(70, "Value");
				
		CTableRow tblRow("");
		tblRow.AddCellList(CTableCell("Full Name"), CTableCell(this->pUser->fullName));
		tbl.AddRow(tblRow);

		tblRow.AddCellList(CTableCell("Email"), CTableCell(this->pUser->email));
		tbl.AddRow(tblRow);

		tblRow.AddCellList(CTableCell("License Type"), CTableCell(CAREnum::UserGetFTLicType(this->pUser->licenseType)));
		tbl.AddRow(tblRow);
		
		tblRow.AddCellList(CTableCell("Full Text License Type"), CTableCell(CAREnum::UserGetFTLicType(this->pUser->ftLicenseType)));
		tbl.AddRow(tblRow);

		tblRow.AddCellList(CTableCell("Default Notify Mechanism"), CTableCell(CAREnum::UserGetDefNotify(this->pUser->defNotify)));
		tbl.AddRow(tblRow);

		//Groups
		CTable tblGrplist("userGroups", "TblObjectList");
		tblGrplist.AddColumn(10, "Group ID");
		tblGrplist.AddColumn(90, "Group Name");

		for(unsigned int i=0; i< this->pUser->groupList.size(); i++)
		{
			string grpIdString = CUtil::ClearSpaces(this->pUser->groupList[i]);
			if(grpIdString.size() > 0)
			{
				int grpId = atoi(grpIdString.c_str());
	            
				CTableRow rowGrp("cssStdRow");	
				rowGrp.AddCell(CTableCell(grpId));
				rowGrp.AddCell(CTableCell(this->pInside->LinkToGroup("", grpId, this->rootLevel)));
				tblGrplist.AddRow(rowGrp);			
			}
		}

		tblRow.AddCellList(CTableCell("Group List"), CTableCell(tblGrplist.ToXHtml()));
		tbl.AddRow(tblRow);

		//Add the complete table to the page
		webPage.AddContent(tbl.ToXHtml());
		tbl.Clear();

		//Histoy
		webPage.AddContent(this->pInside->DataObjectHistory(this->pUser, this->rootLevel));

		webPage.SaveInFolder("user");	
	}
	catch(...)
	{
		cout << "EXCEPTION user details documentation"<< endl;
	}
}
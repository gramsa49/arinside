
/****************************************************************************** 
 * 
 *  file:  MenuTable.cpp
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
#include ".\menutable.h"

using namespace OUTPUT;

CMenuTable::CMenuTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("charmenuList", "TblObjectList");
	tbl->AddColumn(40, "CharMenu Name");
	tbl->AddColumn(10, "Type");
	tbl->AddColumn(10, "Refresh On");
	tbl->AddColumn(20, "Changed");
	tbl->AddColumn(20, "By");
}

CMenuTable::~CMenuTable(void)
{
}

void CMenuTable::AddRow(CARCharMenu &menu, int rootLevel)
{
	CTableRow tblRow("");

	string cellNameValue = menu.GetURL(rootLevel);
	if(NumRelatedFields(menu) + NumRelatedActiveLinks(menu) ==0)
	{
		cellNameValue += " (<b>!</b>)";
	}

	tblRow.AddCell( CTableCell(cellNameValue));		
	tblRow.AddCell( CTableCell(CAREnum::MenuType(menu.menuDefn.menuType)));
	tblRow.AddCell( CTableCell(CAREnum::MenuRefresh(menu.refreshCode)));
	tblRow.AddCell( CTableCell(CUtil::TimeToString(menu.timestamp)));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(menu.lastChanged, rootLevel)));

	this->tbl->AddRow(tblRow);
}

int CMenuTable::NumRelatedFields(CARCharMenu &obj)
{	
	int nCnt = 0;

	list<CARSchema>::iterator schemaIter;	
	for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
	{			
		CARSchema *schema = &(*schemaIter);
		
		list<CARField>::iterator fieldIter;		
		CARField *field;

		for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
		{
			field = &(*fieldIter);

			if(field->dataType == AR_DATA_TYPE_CHAR)
			{
				if(strcmp(field->limit.u.charLimits.charMenu, obj.name.c_str())==0)
				{
					nCnt++;
				}
			}
		}		
	}
	return nCnt;
}

int CMenuTable::NumRelatedActiveLinks(CARCharMenu &obj)
{
	int nCnt = 0;

	list<CARActiveLink>::iterator alIter;			
	for ( alIter = this->pInside->alList.begin(); alIter != this->pInside->alList.end(); alIter++ )
	{			
		CARActiveLink *al = &(*alIter);
		for(unsigned int nAction = 0; nAction < al->actionList.numItems; nAction++)
		{
			ARActiveLinkActionStruct action = al->actionList.actionList[nAction];
			if(action.action == AR_ACTIVE_LINK_ACTION_SET_CHAR)
			{
				if(action.u.characteristics.charMenu != NULL)
				{
					if(strcmp(action.u.characteristics.charMenu, obj.name.c_str())==0)
					{
						nCnt++;
					}
				}
			}
		}		

		for(unsigned int nAction = 0; nAction < al->elseList.numItems; nAction++)
		{
			ARActiveLinkActionStruct action = al->elseList.actionList[nAction];
			if(action.action == AR_ACTIVE_LINK_ACTION_SET_CHAR)
			{
				if(action.u.characteristics.charMenu != NULL)
				{
					if(strcmp(action.u.characteristics.charMenu, obj.name.c_str())==0)
					{
						nCnt++;
					}
				}
			}
		}		
	}
	return nCnt;
}
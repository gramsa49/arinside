//Copyright (C) 2014 John Luthgers | jls17
//
//This file is part of ARInside.
//
//    ARInside is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    ARInside is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with ARInside.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "WorkflowReferenceTable.h"
#include "../core/ARContainer.h"
#include "../core/AREnum.h"
#include "../core/ARImage.h"
#include "../ARInside.h"


using namespace OUTPUT;

WorkflowReferenceTable::WorkflowReferenceTable(const CARImage &image)
: obj(image)
{
	InitIterators(image.GetReferences());
}

WorkflowReferenceTable::WorkflowReferenceTable(const CARContainer& container)
:obj(container)
{
	InitIterators(container.GetReferences());
}

void WorkflowReferenceTable::InitIterators(const CRefItemList &refs)
{
	curIt = refs.begin();
	endIt = refs.end();
}

string WorkflowReferenceTable::ToString(int rootLevel)
{
	stringstream strm;
	strm.str("");
	CARInside *pInside = CARInside::GetInstance();

	try
	{
		CTable tblRef("referenceList", "TblObjectList");
		tblRef.AddColumn(10, "Type");
		tblRef.AddColumn(45, "Server object");
		tblRef.AddColumn(5, "Enabled");
		tblRef.AddColumn(40, "Description");

		for ( ; curIt != endIt; ++curIt)
		{			
			CTableRow row("cssStdRow");		
			row.AddCell(CAREnum::XmlStructItem(curIt->GetObjectType()));				
			row.AddCell(pInside->LinkToObjByRefItem(*curIt, rootLevel));

			string tmpEnabled = "";
			string tmpCssEnabled = "";

			bool enabledSupported = false;
			int enabled = curIt->GetObjectEnabled(enabledSupported);

			if (enabledSupported)
			{
				tmpEnabled = CAREnum::ObjectEnable(enabled);
				if (!enabled) { tmpCssEnabled = "objStatusDisabled"; }
			}

			row.AddCell(CTableCell(tmpEnabled, tmpCssEnabled));
			row.AddCell(curIt->GetDescription(rootLevel));
			tblRef.AddRow(row);
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Workflow Reference";

		tblRef.description = tblDesc.str();

		strm << tblRef;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating workflow references for object: " << obj.GetName() << " -- " << e.what() << endl;
	}	

	return strm.str();
}

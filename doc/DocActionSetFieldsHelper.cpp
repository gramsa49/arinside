//Copyright (C) 2009 John Luthgers | jls17
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
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "DocActionSetFieldsHelper.h"
#include "../core/ARSetFieldHelper.h"

CDocActionSetFieldsHelper::CDocActionSetFieldsHelper(CARInside &arInside, CARServerObject &arServerObject, const ARSetFieldsActionStruct& sFieldStruct, int structItemType, IfElseState ifElseMode, int numAction)
: arIn(arInside), obj(arServerObject), setFieldsStruct(sFieldStruct), ifElse(ifElseMode)
{
	arStructItemType = structItemType;
	nAction = numAction;
}

CDocActionSetFieldsHelper::~CDocActionSetFieldsHelper(void)
{
}

void CDocActionSetFieldsHelper::SetFieldsGetSecondaryForm(const string& fromSchema, int rootLevel, stringstream &strmSchema, stringstream &strmSchemaDisplay, stringstream &strmServer, stringstream &strmQual)
{
	CARSchema wfSchema(fromSchema);
	if (wfSchema.Exists())
	{
		CARSetFieldHelper sfh(*CARInside::GetInstance(), wfSchema, setFieldsStruct, ifElse, nAction);

		switch (sfh.GetType())
		{
		case SFT_CURRENT:
			strmServer << arIn.LinkToServerInfo(arIn.appConfig.serverName, rootLevel);
			strmSchemaDisplay << "CURRENT SCREEN";
			strmSchema << sfh.GetSchemaName();
			break;
		case SFT_SERVER:
		case SFT_SAMPLEDATA:
			{
				int pFormId = arIn.SchemaGetInsideId(fromSchema);
				int sFormId = -1;
				string readServer = sfh.GetServerName();
				string readSchema = sfh.GetSchemaName();

				if (readSchema.compare("@") == 0)
				{
					readSchema = fromSchema;
				}

				if (sfh.GetType() == SFT_SAMPLEDATA)
				{
					int fieldId = sfh.GetServerFieldId();
					strmServer << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn.LinkToField(fromSchema, fieldId, rootLevel)) << "$ (Sample Server: " << arIn.LinkToServerInfo(readServer, rootLevel) << ")";

					CRefItem refItemServer(obj, ifElse, nAction, REFM_SETFIELDS_SERVER);
					arIn.AddFieldReference(pFormId, fieldId, refItemServer);

					fieldId = sfh.GetSchemaFieldId();
					strmSchemaDisplay << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn.LinkToField(fromSchema, fieldId, rootLevel)) << "$ (Sample Form: " << arIn.LinkToSchema(readSchema, rootLevel) << ")";
					strmSchema << readSchema;

					CRefItem refItemForm(obj, ifElse, nAction, REFM_SETFIELDS_FORM);
					arIn.AddFieldReference(pFormId, fieldId, refItemForm);
				}
				else
				{
					strmServer << arIn.LinkToServerInfo(readServer, rootLevel);

					strmSchemaDisplay << readSchema;
					strmSchema << readSchema;
				}

				// *************************************************************************

				sFormId = arIn.SchemaGetInsideId(readSchema);

				strmQual << "<br/>Set Field If<br/>" << endl;
				stringstream strmTmpQual;
				CRefItem refItem(obj, ifElse, nAction, REFM_SETFIELDS_QUALIFICATION);

				CARQualification arQual(arIn);
				arQual.arsStructItemType = arStructItemType;
				arQual.CheckQuery(sfh.GetQualifier(), refItem, 0, pFormId, sFormId, strmTmpQual, rootLevel);

				if(strmTmpQual.str().length() > 0)
				{
					strmQual << strmTmpQual.str() << "<br/><br/>" << endl;
				}
				else
				{
					strmQual << EmptyValue << "<br/><br/>" << endl;
				}

				strmQual << "If No Requests Match: " << CAREnum::NoMatchRequest(sfh.GetNoMatchOption()) << "<br/>" << endl;
				strmQual << "If Multiple Requests Match: " << CAREnum::MultiMatchRequest(sfh.GetMultiMatchOption()) << "<br/><br/>" << endl;
			}
			break;
		case SFT_SQL:
			{
				strmServer << arIn.LinkToServerInfo(sfh.GetServerName(), rootLevel);
				strmSchema << AR_ASSIGN_SQL_SCHEMA_NAME;
				strmSchemaDisplay << "SQL";

				strmQual << "<br/>SQL command<br/>" << endl;

				if(!sfh.GetSqlCommand().empty())
				{
					CRefItem refItem(obj, ifElse, nAction, REFM_SETFIELDS_SQL_QUALIFICATION);
					strmQual << arIn.TextFindFields(sfh.GetSqlCommand(), "$", arIn.SchemaGetInsideId(fromSchema), rootLevel, true, &refItem) << "<br/><br/>" << endl;
				}
				else
					strmQual << EmptyValue << "<br/><br/>" << endl;

				strmQual << "If No Requests Match: " << CAREnum::NoMatchRequest(sfh.GetNoMatchOption()) << "<br/>" << endl;
				strmQual << "If Multiple Requests Match: " << CAREnum::MultiMatchRequest(sfh.GetMultiMatchOption()) << "<br/><br/>" << endl;	
			}
			break;
		case SFT_FILTERAPI:
		case SFT_WEBSERVICE:
		case SFT_ATRIUM_ORCHESTRATOR:
			{
				// TODO: implement documenting of filter-api actions correctly
				// normally we should set assignSchemaDisplay to "FILTER API" but CDocFilterActionStruct checks 
				// for "ARSYS.ARF.WEBSERVICE". To avoid breaking this implementation we leave it as it is until
				// there is enough time to re-work both (filter api & webservices) at the same time.
				//
				// Note: serviceName can hold a fieldId which is used to read the plugin-name from. add a 
				// reference tho this field later.
				strmServer << arIn.LinkToServerInfo(AR_CURRENT_SERVER_TAG, rootLevel);
				strmSchema << sfh.GetSchemaName();
				strmSchemaDisplay << sfh.GetSchemaName();
			}
			break;
		}
	}
}

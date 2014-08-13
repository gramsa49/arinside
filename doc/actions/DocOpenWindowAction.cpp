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
#include "DocAlMessageAction.h"
#include "DocOpenWindowAction.h"
#include "../DocActionOpenWindowHelper.h"
#include "../../core/ARQualification.h"
#include "../../core/ARAssignHelper.h"
#include "../../output/ObjNotFound.h"
#include "../../output/URLLink.h"
#include "../../util/Context.h"

DocOpenWindowAction::DocOpenWindowAction(Context &theContext, const AROpenDlgStruct &theAction)
:	context(theContext), action(theAction)
{
}

void DocOpenWindowAction::ToStream(std::ostream& strm)
{
	try
	{
		CARInside* arIn = &context.getInside();
		string openWindowServer;
		string openWindowSchemaName;
		CARSchema attachedSchema(context.getCurrentSchemaId());

		// check if we need to get the sample data
		if(action.serverName[0] == '$')
		{
			CDocActionOpenWindowHelper::GetSampleData(context.getActLink(), context.getIfElse(), context.getActionIndex(), openWindowServer, openWindowSchemaName);
		}

		// window type
		int windowMode = CAREnum::OpenWindowModeMapped(action.windowMode);
		strm << "<p>Window Type: " << CAREnum::OpenWindowMode(windowMode) << endl;

		// display type
		if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_MODIFY || windowMode == AR_ACTIVE_LINK_ACTION_OPEN_DSPLY)
		{
			strm << "<br/>Display Type: " << CAREnum::OpenWindowDisplayType(action.windowMode) << endl;
		}

		// target location
		if (ActionOpenDlgTargetLocation(windowMode))
		{
			strm << "<br/>Target Location: ";

			if (action.targetLocation != NULL) 
			{
				if (action.targetLocation[0] == '$')
				{
					int fieldId = atoi(&action.targetLocation[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(context.getCurrentSchemaId(), fieldId, context.getRootLevel())) << "$";

					if (fieldId > 0)
					{
						CRefItem refItem(context, REFM_OPENWINDOW_LOCATION);
						arIn->AddFieldReference(context.getCurrentSchemaId(), fieldId, refItem);
					}
				}
				else
				{
					strm << action.targetLocation;
				}
			}
			strm << endl;
		}
		strm << "</p>" << endl;

		// ****************************************************
		// server
		strm << "<p>Server Name: ";
		if(action.serverName[0] == '$' && !openWindowServer.empty())
		{
			int fieldId = atoi(&action.serverName[1]);
			strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(context, fieldId)) << "$ (Sample Server: " << arIn->LinkToServerInfo(context, openWindowServer) << ")";

			if (fieldId > 0)
			{
				CRefItem refItem(context, REFM_OPENWINDOW_SERVER);
				arIn->AddFieldReference(context.getCurrentSchemaId(), fieldId, refItem);
			}
		}
		else
		{
			openWindowServer = action.serverName;
			strm << arIn->LinkToServerInfo(context, openWindowServer);
		}

		strm << "<br/>Form Name: ";
		if (action.schemaName[0] == '$')
		{
			int fieldId = atoi(&action.schemaName[1]);
			strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(context, fieldId)) << "$ (Sample Schema: " << arIn->LinkToSchema(openWindowSchemaName, context.getRootLevel()) << ")";

			if (fieldId > 0)
			{
				CRefItem refItem(context, REFM_OPENWINDOW_FORM);
				arIn->AddFieldReference(context.getCurrentSchemaId(), fieldId, refItem);
			}

			if (fieldId == (-AR_KEYWORD_SCHEMA)) openWindowSchemaName = attachedSchema.GetName();
		}
		else
		{
			openWindowSchemaName = action.schemaName;
			strm << arIn->LinkToSchema(openWindowSchemaName, context.getRootLevel()) << endl;
		}

		LookupFormContext openWindowContext(context, openWindowSchemaName);
		CARSchema openWindowSchema(openWindowContext.getLookupSchemaId());

		strm << "<br/>View Name: ";
		if(action.vuiLabel[0] == '$')// == NULL || strcmp(action.vuiLabel, "")==0)
		{
			int fieldId = atoi(&action.vuiLabel[1]);
			strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(context, fieldId)) << "$";

			if (fieldId > 0)
			{
				CRefItem refItem(context, REFM_OPENWINDOW_VIEW);
				arIn->AddFieldReference(context.getCurrentSchemaId(), fieldId, refItem);
			}
		}
		else
		{
			if (action.vuiLabel[0] == 0)
			{ 
				strm << "(Clear)" << endl;
			}
			else
			{
				if (!openWindowSchema.Exists())
				{
					ObjNotFound notFound(strm);
					notFound << action.vuiLabel;
					notFound.End();
				}
				else
				{
					CARVui vui(openWindowSchema.GetInsideId(), action.vuiLabel);
					if (vui.Exists())
						strm << URLLink(vui, context.getRootLevel());
					else
					{
						ObjNotFound notFound(strm);
						notFound << action.vuiLabel;
						notFound.End();
					}
				}
			}
		}
		strm << "</p>" << endl;

		// report details here
		string *entryIDs = NULL;
		string *queryOverride = NULL;
		string *reportOperation = NULL;
		string *charEncoding = NULL;
		if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_REPORT)
		{
			string reportType;
			string reportLocation;
			string reportName;
			string reportDestination;
			entryIDs = new string();
			queryOverride = new string();
			reportOperation = new string();
			charEncoding = new string();

			if (!CDocActionOpenWindowHelper::GetReportData(action.reportString, reportType, reportLocation, reportName, reportDestination, *entryIDs, *queryOverride, *reportOperation, *charEncoding))
			{
				strm << "<p>";
				ObjNotFound notFound(strm);
				notFound << "Could not load report informations!";
				notFound.End();
				strm << "</p>";
			}
			else
			{
				strm << "<p>Report Type: ";
				if (!reportType.empty() && reportType[0] == '$')
				{
					int fieldId = atoi(&reportType.c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(context, fieldId)) << "$";
					if (fieldId > 0)
					{
						CRefItem refItem(context, REFM_OPENWINDOW_REPORTTYPE);
						arIn->AddFieldReference(context.getCurrentSchemaId(), fieldId, refItem);
					}
				}
				else
				{
					strm << reportType;
				}
				strm <<  "<br/>";

				int iReportLocation = atoi(reportLocation.c_str());
				strm << "Report Location: " << CAREnum::ReportLocation(iReportLocation);
				
				strm << "<br/>Report Name: ";
				if (!reportName.empty() && reportName[0] == '$')
				{
					int fieldId = atoi(&reportName.c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(context, fieldId)) << "$";
					if (fieldId > 0)
					{
						CRefItem refItem(context, REFM_OPENWINDOW_REPORTNAME);
						arIn->AddFieldReference(context.getCurrentSchemaId(), fieldId, refItem);
					}
				}
				else
				{
					strm << reportName;
				}
				strm << "<br/>";
				
				// to-screen:
				// to-print:
				// to-file:
				strm << "Report Destination: ";
				if (!reportDestination.empty() && reportDestination[0] == '$')
				{
					int fieldId = atoi(&reportDestination.c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(context, fieldId)) << "$";
					if (fieldId > 0)
					{
						CRefItem refItem(context, REFM_OPENWINDOW_REPORTDESTINATION);
						arIn->AddFieldReference(context.getCurrentSchemaId(), fieldId, refItem);
					}
				}
				else
				{
					// map the string value to the displayed combobox item of dev studio
					strm << reportDestination;
				}
				strm << "</p>";
			}
		}

		// qualification
		if(ActionOpenDlgQualifier(action.windowMode))
		{
			if(action.query.operation != AR_COND_OP_NONE)
			{
				stringstream strmTmpQual;

				CRefItem refItem(context, REFM_OPENWINDOW_QUALIFICATION);
				MappingContext qualContext(context, openWindowSchema.GetInsideId());

				strmTmpQual.str("");
				CARQualification arQual(context, refItem);
				arQual.CheckQuery(&action.query, strmTmpQual);

				strm << "<p>Qualification:<br/>" << endl;
				strm << strmTmpQual.str() << "</p>";
			}	
		}

		strm << "<p>";
		// show close button
		if (ActionOpenDlgCloseButton(windowMode))
		{
			strm << "<input type=\"checkbox\" name=\"closeWnd\" value=\"closeWndAll\" " << (action.closeBox ? "checked" : "") << ">Show Close Button in Dialog<br/>" << endl;
		}

		// suppres empty list
		if (ActionOpenDlgSuppressEmptyList(windowMode))
		{
			strm << "<input type=\"checkbox\" name=\"suprEmptyLst\" value=\"suprEmptyLstVal\" " << (action.suppressEmptyLst ? "checked" : "") << ">Suppress Empty List<br/>" << endl;		
		}

		// set fields to defaults
		bool setToDefault = false;
		if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_SEARCH || windowMode == AR_ACTIVE_LINK_ACTION_OPEN_SUBMIT)
		{

			for(unsigned int i= 0; i < action.inputValueFieldPairs.numItems; ++i)
			{
				if (action.inputValueFieldPairs.fieldAssignList[i].fieldId == AR_SET_DEFAULTS_ID)
				{
					setToDefault = true;
					break;
				}
			}

			strm << "<input type=\"checkbox\" name=\"setToDefault\" value=\"setToDefaultVal\" " << (setToDefault ? "checked" : "") << ">Set Fields To Defaults<br/>" << endl;		
		}

		// input mapping
		if (ActionOpenDlgInputMapping(windowMode) && !setToDefault)
		{
			MappingContext mappingContext(context);
			mappingContext.setPrimarySchemaId(openWindowSchema.GetInsideId());
			mappingContext.setSecondarySchemaId(context.getCurrentSchemaId());

			CARAssignHelper assignHelper(mappingContext);
			strm << assignHelper.OpenWindowAssignment(action.inputValueFieldPairs, mappingContext.getActionIndex(), mappingContext.getIfElse(), OWM_OPEN);
		}

		// output mapping (dialog on close)
		if (ActionOpenDlgOutputMapping(windowMode))
		{
			strm << "On Dialog Close Action:<br/>";

			MappingContext mappingContext(context, openWindowSchema.GetInsideId());

			CARAssignHelper assignHelper(mappingContext);
			strm << assignHelper.OpenWindowAssignment(action.outputValueFieldPairs, mappingContext.getActionIndex(), mappingContext.getIfElse(), OWM_CLOSE);
		}
		strm << "</p>";

		if (ActionOpenDlgMessage(windowMode))
		{
			// message
			strm << "<p>If No Request Match: ";
			if (action.noMatchContinue)
			{
				strm << "Do not show any message";
			}
			else if (action.msg.messageText == NULL || action.msg.messageText[0] == 0)	// dont know, if thats the correct criteria for the default message
			{
				strm << "Show default message";
			}
			else if (action.msg.messageType > 0)
			{
				strm << "Show message<br/>";
				
				DocAlMessageAction docMessage(context, action.msg);
				docMessage.ToStream(strm);
			}
			strm << "</p>" << endl;

			// sorting
			if (action.sortOrderList.numItems > 0)	
			{
				CTable tblSortList("sortList", "TblObjectList");
				tblSortList.AddColumn(40, "Field Name");
				tblSortList.AddColumn(15, "Field Id");
				tblSortList.AddColumn(15, "Field Type");
				tblSortList.AddColumn(30, "Sort Order");
				
				char strFieldId[20];

				for (unsigned int i = 0; i < action.sortOrderList.numItems; ++i)
				{
					CARField rField(openWindowSchema.GetInsideId(), action.sortOrderList.sortList[i].fieldId);

					strFieldId[0] = 0;
					sprintf(strFieldId, "%d", static_cast<int>(action.sortOrderList.sortList[i].fieldId));

					CTableRow row("cssStdRow");
					row.AddCell(arIn->LinkToField(openWindowContext, action.sortOrderList.sortList[i].fieldId));
					row.AddCell(strFieldId);
					row.AddCell((rField.Exists() ? CAREnum::DataType(rField.GetDataType()) : "Unknown"));
					row.AddCell(CAREnum::SchemaSortOrder(action.sortOrderList.sortList[i].sortOrder));
					
					tblSortList.AddRow(row);

					if (openWindowSchema.Exists())
					{
						CRefItem refItem(context, REFM_OPENWINDOW_SORTBY);
						arIn->AddFieldReference(openWindowSchema.GetInsideId(), action.sortOrderList.sortList[i].fieldId, refItem);
					}
				}

				strm << "<p>Sort Order" << tblSortList << "</p>";
			}

			// additional report informations
			if (windowMode == AR_ACTIVE_LINK_ACTION_OPEN_REPORT)
			{
				strm << "<p>EntryIDs: ";
				if (!entryIDs->empty() && entryIDs->operator[](0) == '$')
				{
					int fieldId = atoi(&entryIDs->c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(context, fieldId)) << "$";
					if (fieldId > 0)
					{
						CRefItem refItem(context, REFM_OPENWINDOW_ENTRYIDS);
						arIn->AddFieldReference(context.getCurrentSchemaId(), fieldId, refItem);
					}
				}
				else
				{
					strm << *entryIDs;
				}
				
				strm << "<br/>Query Override: ";
				if (!queryOverride->empty() && queryOverride->operator[](0) == '$')
				{
					int fieldId = atoi(&entryIDs->c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(context, fieldId)) << "$";
					if (fieldId > 0)
					{
						CRefItem refItem(context, REFM_OPENWINDOW_QUERYOVERRIDE);
						arIn->AddFieldReference(context.getCurrentSchemaId(), fieldId, refItem);
					}
				}
				else
				{
					strm << *queryOverride;
				}
				
				strm << "<br/>Report Operation: ";
				int iReportOperation = atoi(reportOperation->c_str());
				if (iReportOperation == 0) iReportOperation = 2;	// set default to Run
				strm << CAREnum::ReportOperation(iReportOperation);

				strm << "<br/>Character Encoding: ";
				if (!charEncoding->empty() && charEncoding->operator[](0) == '$')
				{
					int fieldId = atoi(&charEncoding->c_str()[1]);
					strm << "$" << (fieldId < 0 ? CAREnum::Keyword(abs(fieldId)) : arIn->LinkToField(context, fieldId)) << "$";
					if (fieldId > 0)
					{
						CRefItem refItem(context, REFM_OPENWINDOW_CHARENC);
						arIn->AddFieldReference(context.getCurrentSchemaId(), fieldId, refItem);
					}
				}
				else
				{
					strm << *charEncoding;
				}
				strm << "</p>" << endl;

				// clean up
				delete entryIDs;
				delete queryOverride;
				delete reportOperation;
				delete charEncoding;
			}

			// polling interval
			if (ActionOpenDlgPoolingInterval(windowMode) && action.pollinginterval > 0)
			{
				strm << "<p>Polling interval: " << action.pollinginterval << "</p>" << endl;
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in AlActionOpenDlg of '" << context.getCurrentObject().GetName() << "': " << e.what() << endl;
	}
}

bool DocOpenWindowAction::ActionOpenDlgQualifier(int nWindowType)
{
	switch(nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DETAIL:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_LST:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_SPLIT:
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY_LST:
	case AR_ACTIVE_LINK_ACTION_OPEN_REPORT:
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY_DETAIL:
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY_SPLIT:
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT_LST:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT_DETAIL:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT_SPLIT:
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT:
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT_LST:
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT_DETAIL:
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT_SPLIT:
#endif
		return true;
	default: return false;
	}
}

bool DocOpenWindowAction::ActionOpenDlgTargetLocation(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_SEARCH:
	case AR_ACTIVE_LINK_ACTION_OPEN_SUBMIT:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY:
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY:
	case AR_ACTIVE_LINK_ACTION_OPEN_REPORT:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT: 
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT:
#endif
		return true;
	default: return false;
	}
}

bool DocOpenWindowAction::ActionOpenDlgCloseButton(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_DLG:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
	case AR_ACTIVE_LINK_ACTION_OPEN_POPUP:
#endif
		return true;
	default: return false;
	}
}

bool DocOpenWindowAction::ActionOpenDlgSuppressEmptyList(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY:
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT:
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT:
#endif
		return true;
	default: return false;
			
	}
}

bool DocOpenWindowAction::ActionOpenDlgInputMapping(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_DLG:
	case AR_ACTIVE_LINK_ACTION_OPEN_SEARCH:
	case AR_ACTIVE_LINK_ACTION_OPEN_SUBMIT:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
	case AR_ACTIVE_LINK_ACTION_OPEN_POPUP:
#endif
		return true;
	default: return false;
	}
}

bool DocOpenWindowAction::ActionOpenDlgOutputMapping(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_DLG:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
	case AR_ACTIVE_LINK_ACTION_OPEN_POPUP:
#endif
		return true;
	default: return false;
	}
}

bool DocOpenWindowAction::ActionOpenDlgPoolingInterval(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT:
#endif
		return true;
	default: return false;
	}
}

bool DocOpenWindowAction::ActionOpenDlgMessage(int nWindowType)
{
	switch (nWindowType)
	{
	case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY:
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	case AR_ACTIVE_LINK_ACTION_OPEN_DISPLAY_DIRECT:
	case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DIRECT:
#endif
	case AR_ACTIVE_LINK_ACTION_OPEN_REPORT:
		return true;
	default: return false;
	}
}
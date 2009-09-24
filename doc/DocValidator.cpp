//Copyright (C) 2009 Stefan Nerlich | stefan.nerlich@hotmail.com
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

#include "StdAfx.h"
#include "docvalidator.h"

CDocValidator::CDocValidator(CARInside &arIn, string path)
{
	this->pInside = &arIn;
	this->path = path;
	this->rootLevel = 1;
	uniqueMissingFieldList.clear();
}

CDocValidator::~CDocValidator(void)
{
	uniqueMissingFieldList.clear();
}


void CDocValidator::Main()
{
	try
	{		
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage("validation_main", "Validator", rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << "Server object validation:" << endl;
			webPage.AddContentHead(contHeadStrm.str());

			//cout << "Field reference validation" << endl;
			//webPage.AddContent(CWebUtil::Link("List of missing fields that are referenced in ARSystem workflow", CWebUtil::DocName("validation_field"), "doc.gif", rootLevel, true));
			//this->FieldReferenceValidator();

			//Field reference check
			webPage.AddContent("Missing Objects referenced by Workflow:<br/>");
			cout << "Field reference validation" << endl;
			webPage.AddContent(CWebUtil::Link("Fields", CWebUtil::DocName("validation_field_references"), "doc.gif", rootLevel, true));
			this->FieldReferenceValidator();
			webPage.AddContent("<br/>");

			//Menu reference check
			cout << "Menu reference validation" << endl;
			webPage.AddContent(CWebUtil::Link("Menus", CWebUtil::DocName("validation_menu_references"), "doc.gif", rootLevel, true));
			this->MenuReferenceValidator();

			//Group permission check
			webPage.AddContent("<br/><br/>List of server objects with no specified access groups:<br/>");

			cout << "Form access group validation" << endl;
			webPage.AddContent(CWebUtil::Link("Forms", CWebUtil::DocName("validation_group_form"), "doc.gif", rootLevel, true));
			this->FormGroupValidator();

			cout << "Field access group validation" << endl;
			webPage.AddContent("<br/>");
			webPage.AddContent(CWebUtil::Link("Fields", CWebUtil::DocName("validation_group_field"), "doc.gif", rootLevel, true));
			this->FieldGroupValidator();

			cout << "Active Link access group validation" << endl;
			webPage.AddContent("<br/>");
			webPage.AddContent(CWebUtil::Link("Active Links", CWebUtil::DocName("validation_group_al"), "doc.gif", rootLevel, true));
			this->AlGroupValidator();

			cout << "Container access group validation" << endl;
			webPage.AddContent("<br/>");
			webPage.AddContent(CWebUtil::Link("Container", CWebUtil::DocName("validation_group_container"), "doc.gif", rootLevel, true));
			this->ContainerGroupValidator();

			webPage.SaveInFolder(this->path);	
		}
	}
	catch(...)
	{
		cout << "EXCEPTION Group_Validation" << endl;
	}
}

void CDocValidator::ContainerGroupValidator()
{
	try
	{		
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage("validation_group_container", "Container access group validation", rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << CWebUtil::Link("Validation", CWebUtil::DocName("validation_main"), "", 0) << MenuSeparator;
			contHeadStrm << "Container with no or unknown group permission:" << endl;
			webPage.AddContentHead(contHeadStrm.str());

			//Check forms	
			CTable tblObj("ContainerNoPermission", "TblObjectList");
			tblObj.AddColumn(10, "Type");
			tblObj.AddColumn(90, "Container");

			list<CARContainer>::iterator contIter;		
			for ( contIter = this->pInside->containerList.begin(); contIter != this->pInside->containerList.end(); contIter++ )
			{			
				CARContainer *cont = &(*contIter);

				if(cont->type == ARCON_WEBSERVICE
					|| cont->type == ARCON_GUIDE
					|| cont->type == ARCON_APP)
				{
					if(cont->groupList.permissionList == NULL		//Check if the container has no access group
						|| cont->groupList.numItems == 0 )
					{
						//Container has no access group
						CTableRow row("");	
						row.AddCell(CAREnum::ContainerType(cont->type));
						row.AddCell(pInside->LinkToContainer(cont->name, this->rootLevel));
						tblObj.AddRow(row);
					}
				}
			}

			webPage.AddContent(tblObj.ToXHtml());
			tblObj.Clear();

			webPage.SaveInFolder(this->path);	
		}
	}
	catch(...)
	{
		cout << "EXCEPTION Container_Group_Validation" << endl;
	}
}

void CDocValidator::AlGroupValidator()
{
	try
	{		
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage("validation_group_al", "Active Link access group validation", rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << CWebUtil::Link("Validation", CWebUtil::DocName("validation_main"), "", 0) << MenuSeparator;
			contHeadStrm << "Active Links with no or unknown group permission:" << endl;
			webPage.AddContentHead(contHeadStrm.str());

			//Check forms	
			CTable tblObj("AlNoPermission", "TblObjectList");
			tblObj.AddColumn(100, "Active Link");

			list<CARActiveLink>::iterator alIter;		
			for ( alIter = this->pInside->alList.begin(); alIter != this->pInside->alList.end(); alIter++ )
			{			
				CARActiveLink *al = &(*alIter);

				if(al->groupList.internalIdList == NULL		//Check if the al has no access group
					|| al->groupList.numItems == 0 )
				{
					//Form has no access group
					CTableRow row("");		
					row.AddCell(pInside->LinkToAl(al->name, this->rootLevel));
					tblObj.AddRow(row);
				}
			}

			webPage.AddContent(tblObj.ToXHtml());
			tblObj.Clear();

			webPage.SaveInFolder(this->path);	
		}
	}
	catch(...)
	{
		cout << "EXCEPTION ActiveLink_Group_Validation" << endl;
	}
}

void CDocValidator::FieldGroupValidatorDetails(CARSchema &schema, string fName)
{
	try
	{		
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage(fName, "Field access group validation", rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << CWebUtil::Link("Validation", CWebUtil::DocName("validation_main"), "", 0) << MenuSeparator;
			contHeadStrm << "Fields with no or unknown group permission:" << endl;
			webPage.AddContentHead(contHeadStrm.str());

			CTable tblObj("FieldsNoPermission", "TblObjectList");
			tblObj.AddColumn(100, "Field Name");

			list<CARField>::iterator fieldIter;			
			for( fieldIter = schema.fieldList.begin(); fieldIter != schema.fieldList.end(); fieldIter++)
			{
				CARField *field = &(*fieldIter);

				if(field->permissions.permissionList== NULL  //First check if the field has any access group
					|| field->permissions.numItems == 0)
				{
					//field has no access group
					CTableRow row("");		
					row.AddCell(pInside->LinkToField(schema.insideId, field->insideId, this->rootLevel));
					tblObj.AddRow(row);					
				}
			}

			tblObj.description = pInside->LinkToSchema(schema.insideId, this->rootLevel);
			webPage.AddContent(tblObj.ToXHtml());
			tblObj.Clear();

			webPage.SaveInFolder(this->path);	
		}
	}
	catch(...)
	{
		cout << "EXCEPTION Field_Group_Validation_Details" << endl;
	}
}

void CDocValidator::FieldGroupValidator()
{
	try
	{		
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage("validation_group_field", "Field access group validation", rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << CWebUtil::Link("Validation", CWebUtil::DocName("validation_main"), "", 0) << MenuSeparator;
			contHeadStrm << "Fields with no or unknown group permission:" << endl;
			webPage.AddContentHead(contHeadStrm.str());

			//Check fields
			CTable tblObj("FieldsNoPermission", "TblObjectList");
			tblObj.AddColumn(10, "Num. Fields");
			tblObj.AddColumn(90, "Form Name");

			list<CARSchema>::iterator schemaIter;		
			for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
			{			
				CARSchema *schema = &(*schemaIter);

				int nEmptyFields = 0;
				list<CARField>::iterator fieldIter;			
				for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
				{
					CARField *field = &(*fieldIter);

					if(field->permissions.permissionList== NULL  //First check if the field has any access group
						|| field->permissions.numItems == 0)
					{
						nEmptyFields++;					
					}
				}

				if(nEmptyFields > 0)
				{
					string fName = "validation_group_field_"+schema->FileID();

					CTableRow row("");		
					row.AddCell(nEmptyFields);
					row.AddCell(CWebUtil::Link(schema->name, CWebUtil::DocName(fName), "", 0));
					tblObj.AddRow(row);			

					FieldGroupValidatorDetails(*schema, fName);
				}
			}

			webPage.AddContent(tblObj.ToXHtml());
			webPage.SaveInFolder(this->path);	
		}
	}
	catch(...)
	{
		cout << "EXCEPTION Field_Group_Validation" << endl;
	}
}

void CDocValidator::FormGroupValidator()
{
	try
	{		
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage("validation_group_form", "Form access group validation", rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << CWebUtil::Link("Validation", CWebUtil::DocName("validation_main"), "", 0) << MenuSeparator;
			contHeadStrm << "Forms with no or unknown group permission:" << endl;
			webPage.AddContentHead(contHeadStrm.str());

			//Check forms	
			CTable tblForms("FormNoPermission", "TblObjectList");
			tblForms.AddColumn(100, "Form Name");

			list<CARSchema>::iterator schemaIter;		
			for ( schemaIter = this->pInside->schemaList.begin(); schemaIter != this->pInside->schemaList.end(); schemaIter++ )
			{			
				CARSchema *schema = &(*schemaIter);

				if(schema->groupList.permissionList == NULL		//Check if the form has no access group
					|| schema->groupList.numItems == 0 )
				{
					//Form has no access group
					CTableRow row("");		
					row.AddCell(pInside->LinkToSchema(schema->name, this->rootLevel));
					tblForms.AddRow(row);
				}
			}

			webPage.AddContent(tblForms.ToXHtml());
			tblForms.Clear();

			webPage.SaveInFolder(this->path);	
		}
	}
	catch(...)
	{
		cout << "EXCEPTION Form_Group_Validation" << endl;
	}
}

void CDocValidator::FieldReferenceValidator()
{
	try
	{		
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage("validation_field_references", "Field validator", rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << CWebUtil::Link("Validation", CWebUtil::DocName("validation_main"), "", 0) << MenuSeparator;
			contHeadStrm << "List of missing fields that are referenced in ARSystem workflow:" << endl;
			webPage.AddContentHead(contHeadStrm.str());

			BuildUniqueFieldList();

			if(this->uniqueMissingFieldList.size() > 0)
			{
				//Outer table
				CTable tbl("fieldListAll", "TblObjectList");
				tbl.AddColumn(10, "Missing Field");
				tbl.AddColumn(30, "Form");		
				tbl.AddColumn(60, "Workflow Items");


				list<CFieldRefItem>::iterator missingFieldIter;			
				for(missingFieldIter = this->uniqueMissingFieldList.begin(); missingFieldIter != this->uniqueMissingFieldList.end(); missingFieldIter++)
				{
					CFieldRefItem *missingFieldItem = &(*missingFieldIter);
					if(NumReferences(missingFieldItem->schemaInsideId, missingFieldItem->fieldInsideId) > 0)
					{
						//Field references
						CTable tblRef("referenceList", "TblObjectList");
						tblRef.AddColumn(15, "Type");
						tblRef.AddColumn(25, "Server object");
						tblRef.AddColumn(10, "Enabled");
						tblRef.AddColumn(50, "Description");

						list<CFieldRefItem>::iterator iter;
						for ( iter = this->pInside->listFieldRefItem.begin(); iter != this->pInside->listFieldRefItem.end(); iter++ )
						{	
							CFieldRefItem *item = &(*iter);
							if(item->schemaInsideId == missingFieldItem->schemaInsideId && item->fieldInsideId == missingFieldItem->fieldInsideId)
							{			
								if(strcmp(item->fromName.c_str(), EmptyValue.c_str())!=0)
								{
									CTableRow row("cssStdRow");		
									row.AddCell(CTableCell(CAREnum::XmlStructItem(item->arsStructItemType)));				
									row.AddCell(CTableCell(this->pInside->LinkToXmlObjType(item->arsStructItemType, item->fromName, rootLevel)));

									string tmpEnabled = this->pInside->XmlObjEnabled(item->arsStructItemType, item->fromName);
									string tmpCssEnabled = "";

									if(strcmp(tmpEnabled.c_str(), "Disabled")==0)
										tmpCssEnabled = "objStatusDisabled";

									row.AddCell(CTableCell(tmpEnabled, tmpCssEnabled));
									row.AddCell(CTableCell(item->description));
									tblRef.AddRow(row);		
								}
							}
						}


						CTableRow row("");				
						row.AddCell(CTableCell(missingFieldItem->fieldInsideId));
						row.AddCell(CTableCell(this->pInside->LinkToSchema(missingFieldItem->schemaInsideId, rootLevel)));
						row.AddCell(CTableCell(tblRef.ToXHtml()));	
						tbl.AddRow(row);					
					}
				}

				webPage.AddContent(tbl.ToXHtml());
				tbl.ClearRows();
			}
			else
			{
				webPage.AddContent("Integrity check found no missing fields in workflow.");
			}

			webPage.SaveInFolder(this->path);	
		}
	}
	catch(...)
	{
		cout << "EXCEPTION Field_Reference_Validation" << endl;
	}
}

bool CDocValidator::InList(int fieldId, int schemaId)
{
	try
	{
		list<CFieldRefItem>::iterator fIter;
		CFieldRefItem *fItem;
		for(fIter = uniqueMissingFieldList.begin(); fIter != uniqueMissingFieldList.end(); fIter++)
		{
			fItem = &(*fIter);

			if(fItem->fieldInsideId == fieldId && fItem->schemaInsideId == schemaId)
			{
				return true;
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION Validation_InList" << endl;
	}

	return false;
}


void CDocValidator::BuildUniqueFieldList()
{
	try
	{
		uniqueMissingFieldList.clear();

		list<CFieldRefItem>::iterator missingFieldIter;
		CFieldRefItem *missingFieldItem;
		for(missingFieldIter = this->pInside->listFieldNotFound.begin(); missingFieldIter != this->pInside->listFieldNotFound.end(); missingFieldIter++)
		{
			missingFieldItem = &(*missingFieldIter);

			if(!InList(missingFieldItem->fieldInsideId, missingFieldItem->schemaInsideId))
			{
				uniqueMissingFieldList.push_back(*missingFieldItem);
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION Validation_BuildUniqueFieldList" << endl;
	}
}

int CDocValidator::NumReferences(int searchSchemaId, int searchFieldId)
{
	int nResult = 0;

	try
	{
		list<CFieldRefItem>::iterator iter;
		CFieldRefItem *item;

		for ( iter = this->pInside->listFieldRefItem.begin(); iter != this->pInside->listFieldRefItem.end(); iter++ )
		{	
			item = &(*iter);
			if(item->schemaInsideId == searchSchemaId  && item->fieldInsideId == searchFieldId)
			{			
				if(strcmp(item->fromName.c_str(), EmptyValue.c_str())!=0)
				{
					nResult++;
				}
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION Validation_NumReferences" << endl;
	}

	return nResult;
}

void CDocValidator::MenuReferenceValidator()
{
	try
	{
		CWindowsUtil winUtil(this->pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)>=0)
		{
			stringstream pgStream;
			CWebPage webPage("validation_menu_references", "Menu validator", rootLevel, this->pInside->appConfig);

			//ContentHead informations
			stringstream contHeadStrm;
			contHeadStrm << CWebUtil::Link("Validation", CWebUtil::DocName("validation_main"), "", 0) << MenuSeparator;
			contHeadStrm << "List of missing menus that are referenced in ARSystem workflow:" << endl;
			webPage.AddContentHead(contHeadStrm.str());

			if (pInside->listMenuNotFound.size() > 0)
			{
				pInside->listMenuNotFound.sort();

				CTable tbl("missingMenus", "TblObjectList");
				tbl.AddColumn(40, "Menu Name");
				tbl.AddColumn(10, "Object Type");
				tbl.AddColumn(40, "Server object");
				tbl.AddColumn(10, "Details");

				list<CMissingMenuRefItem>::iterator mIter = pInside->listMenuNotFound.begin();
				list<CMissingMenuRefItem>::iterator endIt = pInside->listMenuNotFound.end();

				for ( ; mIter != endIt; ++mIter)
				{
					CMissingMenuRefItem mnuRefItem = *mIter;
					CTableRow row("cssStdRow");

					string tmpEnabled = pInside->XmlObjEnabled(mnuRefItem.arsStructItemType, mnuRefItem.fromName);
					string tmpCssClass = "";

					if (tmpEnabled.compare("Disabled")==0)
						tmpCssClass = "objStatusDisabled";

					row.AddCell(mnuRefItem.menuName);				
					row.AddCell(CAREnum::XmlStructItem(mnuRefItem.arsStructItemType));

					if (mnuRefItem.arsStructItemType == AR_STRUCT_ITEM_XML_FIELD)
					{
						row.AddCell(pInside->LinkToField(mnuRefItem.schemaInsideId, mnuRefItem.fieldInsideId, rootLevel));
						row.AddCell(pInside->LinkToSchema(mnuRefItem.schemaInsideId, rootLevel));
					}
					else
					{
						row.AddCell(pInside->LinkToXmlObjType(mnuRefItem.arsStructItemType, mnuRefItem.fromName, rootLevel));
						row.AddCell(CTableCell(tmpEnabled, tmpCssClass));
					}

					
					tbl.AddRow(row);
				}
				webPage.AddContent(tbl.ToXHtml());
				tbl.ClearRows();
			}
			else
			{
				webPage.AddContent("Integrity check found no missing menus in workflow.");
			}

			webPage.SaveInFolder(this->path);	
		}
	}
	catch (...)
	{
		cout << "EXCEPTION in MenuReferenceValidator" << endl;
	}
}
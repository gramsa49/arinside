//Copyright (C) 2010 John Luthgers | jls17
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
#include "IFileStructure.h"
#include "FileNaming.h"
#include <assert.h>

// special ar-object includes
#include "../ARInside.h"
#include "../core/ARImage.h"

// include utils
#include "WebUtil.h"

////////////////////////////////////////////////////////////////////
// namespaces
namespace OUTPUT { namespace FileNaming {


///////////////////////////////////////////////////////////////////////////////
// constants
const char* DIR_SCHEMA = "schema";
const char* DIR_ACTLINK = "active_link";
const char* DIR_FILTER = "filter";
const char* DIR_ESCALATION = "escalation";
const char* DIR_MENU = "menu";
const char* DIR_ALGUIDE = "active_link_guide";
const char* DIR_APPLICATION = "application";
const char* DIR_FLTGUIDE = "filter_guide";
const char* DIR_PACKINGLIST = "packing_list";
const char* DIR_WEBSERVICE = "webservice";
const char* DIR_IMAGE = "image";
const char* DIR_USER = "user";
const char* DIR_GROUP = "group";
const char* DIR_ROLE = "role";
const char* DIR_OTHER = "other";
const char* DIR_OVERVIEW = "overview";

const char* FILE_INDEX = "index";
const char* FILE_INDEX_REGULAR = "index_regular";
const char* FILE_INDEX_JOIN = "index_join";
const char* FILE_INDEX_VIEW = "index_view";
const char* FILE_INDEX_DIALOG = "index_dialog";
const char* FILE_INDEX_VENDOR = "index_vendor";

const char* FILE_INDEX_LETTER_PREFIX = "index_";
const char* FILE_INDEX_OTHER = "index_other";

const char* FILE_INDEX_ACTION = "index_action";
const char* FILE_INDEX_ACTION_PREFIX = "index_action_";

const char* FILE_SERVER = "server";

////////////////////////////////////////////////////////////////////
// for Schemas                                                    //
class SchemaOverview : public IFileStructure
{
public:
	SchemaOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_SCHEMA; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class SchemaDetail : public IFileStructure
{
public:
	SchemaDetail(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class SchemaPermissions : public IFileStructure
{
public:
	SchemaPermissions(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_permission_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class SchemaWorkflow : public IFileStructure
{
public:
	SchemaWorkflow(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_workflow"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class SchemaIndexes : public IFileStructure
{
public:
	SchemaIndexes(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_index_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class SchemaSortList : public IFileStructure
{
public:
	SchemaSortList(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_sort_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class SchemaResultList : public IFileStructure
{
public:
	SchemaResultList(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_result_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class SchemaSubadmins : public IFileStructure
{
public:
	SchemaSubadmins(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_subadmin_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class SchemaActiveLinks : public IFileStructure
{
public:
	SchemaActiveLinks(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_al_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class SchemaFilters : public IFileStructure
{
public:
	SchemaFilters(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_filter_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class SchemaEscalations : public IFileStructure
{
public:
	SchemaEscalations(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_escal_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class SchemaRegular : public IFileStructure
{
public:
	SchemaRegular() { }
	virtual string GetFileName() const { return FILE_INDEX_REGULAR; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_SCHEMA; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class SchemaJoin : public IFileStructure
{
public:
	SchemaJoin() { }
	virtual string GetFileName() const { return FILE_INDEX_JOIN; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_SCHEMA; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class SchemaView : public IFileStructure
{
public:
	SchemaView() { }
	virtual string GetFileName() const { return FILE_INDEX_VIEW; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_SCHEMA; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class SchemaDialog : public IFileStructure
{
public:
	SchemaDialog() { }
	virtual string GetFileName() const { return FILE_INDEX_DIALOG; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_SCHEMA; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class SchemaVendor : public IFileStructure
{
public:
	SchemaVendor() { }
	virtual string GetFileName() const { return FILE_INDEX_VENDOR; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_SCHEMA; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class SchemaLetterOverview : public IFileStructure
{
public:
	SchemaLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return FILE_INDEX_OTHER; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_SCHEMA; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class SchemaFieldsCSV : public IFileStructure
{
public:
	SchemaFieldsCSV(const CARSchema* vui) : obj(vui) {  }
	virtual string GetFileName() const { return obj->FileID() + "_fields"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::CsvDocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARSchema* obj;
};

////////////////////////////////////////////////////////////////////
// for schema VUIs                                                //
class SchemaVUIOverview : public IFileStructure
{
public:
	SchemaVUIOverview(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_vui_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class SchemaVUIDetail : public IFileStructure
{
public:
	SchemaVUIDetail(const CARVui* vui) : obj(vui) {  }
	virtual string GetFileName() const { return "vui_"+obj->FileID(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->GetSchema().FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARVui* obj;
};

class SchemaVUIFieldsCSV : public IFileStructure
{
public:
	SchemaVUIFieldsCSV(const CARVui* vui) : obj(vui) {  }
	virtual string GetFileName() const { return "vui_"+obj->FileID(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::CsvDocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->GetSchema().FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARVui* obj;
};

////////////////////////////////////////////////////////////////////
// for schema fields                                              //
class SchemaFieldDetail : public IFileStructure
{
public:
	SchemaFieldDetail(const CARField* fld) : obj(fld) {  }
	virtual string GetFileName() const { return obj->FileID(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + obj->GetSchema().FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARField* obj;
};


////////////////////////////////////////////////////////////////////
// for Active Links                                               //
class ActiveLinkOverview : public IFileStructure
{
public:
	ActiveLinkOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ACTLINK; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ActiveLinkDetail : public IFileStructure
{
public:
	ActiveLinkDetail(const CARObject* al) : obj(al) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_ACTLINK) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ActiveLinkActionOverview : public IFileStructure
{
public:
	ActiveLinkActionOverview() { }
	virtual string GetFileName() const { return FILE_INDEX_ACTION; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ACTLINK; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ActiveLinkActionDetail : public IFileStructure
{
public:
	ActiveLinkActionDetail(unsigned int action) { nAction = action; }
	virtual string GetFileName() const { stringstream strmTmp; strmTmp << FILE_INDEX_ACTION_PREFIX << nAction; return strmTmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ACTLINK; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int nAction;
};

class ActiveLinkLetterOverview : public IFileStructure
{
public:
	ActiveLinkLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ACTLINK; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

////////////////////////////////////////////////////////////////////
// for Filters                                                    //
class FilterOverview : public IFileStructure
{
public:
	FilterOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_FILTER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class FilterDetail : public IFileStructure
{
public:
	FilterDetail(const CARObject* flt) : obj(flt) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_FILTER) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class FilterActionOverview : public IFileStructure
{
public:
	FilterActionOverview() { }
	virtual string GetFileName() const { return FILE_INDEX_ACTION; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_FILTER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class FilterActionDetail : public IFileStructure
{
public:
	FilterActionDetail(unsigned int action) { nAction = action; }
	virtual string GetFileName() const { stringstream strmTmp; strmTmp << FILE_INDEX_ACTION_PREFIX << nAction; return strmTmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_FILTER; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int nAction;
};

class FilterLetterOverview : public IFileStructure
{
public:
	FilterLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_FILTER; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

////////////////////////////////////////////////////////////////////
// for Escalations                                                //
class EscalationOverview : public IFileStructure
{
public:
	EscalationOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ESCALATION; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class EscalationDetail : public IFileStructure
{
public:
	EscalationDetail(const CARObject* esc) : obj(esc) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_ESCALATION) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class EscalationActionOverview : public IFileStructure
{
public:
	EscalationActionOverview() { }
	virtual string GetFileName() const { return FILE_INDEX_ACTION; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ESCALATION; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class EscalationActionDetail : public IFileStructure
{
public:
	EscalationActionDetail(unsigned int action) { nAction = action; }
	virtual string GetFileName() const { stringstream strmTmp; strmTmp << FILE_INDEX_ACTION_PREFIX << nAction; return strmTmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ESCALATION; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int nAction;
};

class EscalationLetterOverview : public IFileStructure
{
public:
	EscalationLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ESCALATION; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

////////////////////////////////////////////////////////////////////
// for Menus                                                      //
class MenuOverview : public IFileStructure
{
public:
	MenuOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_MENU; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class MenuDetail : public IFileStructure
{
public:
	MenuDetail(const CARObject* esc) : obj(esc) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_MENU) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class MenuLetterOverview : public IFileStructure
{
public:
	MenuLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_MENU; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

////////////////////////////////////////////////////////////////////
// for al-guides                                                  //
class ALGuideOverview : public IFileStructure
{
public:
	ALGuideOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ALGUIDE; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ALGuideDetail : public IFileStructure
{
public:
	ALGuideDetail(const CARObject* alg) : obj(alg) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_ALGUIDE) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ALGuideLetterOverview : public IFileStructure
{
public:
	ALGuideLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ALGUIDE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

////////////////////////////////////////////////////////////////////
// for applications                                               //
class ApplicationOverview : public IFileStructure
{
public:
	ApplicationOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_APPLICATION; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ApplicationDetail : public IFileStructure
{
public:
	ApplicationDetail(const CARObject* alg) : obj(alg) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_APPLICATION) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ApplicationLetterOverview : public IFileStructure
{
public:
	ApplicationLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_APPLICATION; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

////////////////////////////////////////////////////////////////////
// for filter-guides                                              //
class FilterGuideOverview : public IFileStructure
{
public:
	FilterGuideOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_FLTGUIDE; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class FilterGuideDetail : public IFileStructure
{
public:
	FilterGuideDetail(const CARObject* alg) : obj(alg) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_FLTGUIDE) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class FilterGuideLetterOverview : public IFileStructure
{
public:
	FilterGuideLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_FLTGUIDE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

////////////////////////////////////////////////////////////////////
// for packinglist                                                //
class PackingListOverview : public IFileStructure
{
public:
	PackingListOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_PACKINGLIST; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class PackingListDetail : public IFileStructure
{
public:
	PackingListDetail(const CARObject* alg) : obj(alg) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_PACKINGLIST) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class PackingListLetterOverview : public IFileStructure
{
public:
	PackingListLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_PACKINGLIST; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

////////////////////////////////////////////////////////////////////
// for webservice                                                 //
class WebserviceOverview : public IFileStructure
{
public:
	WebserviceOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_WEBSERVICE; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class WebserviceDetail : public IFileStructure
{
public:
	WebserviceDetail(const CARObject* alg) : obj(alg) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_WEBSERVICE) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class WebserviceLetterOverview : public IFileStructure
{
public:
	WebserviceLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_WEBSERVICE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

////////////////////////////////////////////////////////////////////
// for users                                                      //
class UserOverview : public IFileStructure
{
public:
	UserOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_USER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class UserDetail : public IFileStructure
{
public:
	UserDetail(const CARObject* usr) : obj(usr) { }
	virtual string GetFileName() const { return obj->FileID(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_USER; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class UserLetterOverview : public IFileStructure
{
public:
	UserLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_USER; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

////////////////////////////////////////////////////////////////////
// for group                                                      //
class GroupOverview : public IFileStructure
{
public:
	GroupOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class GroupDetail : public IFileStructure
{
public:
	GroupDetail(const CARObject* usr) : obj(usr) { }
	virtual string GetFileName() const { return obj->FileID(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class GroupLetterOverview : public IFileStructure
{
public:
	GroupLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class GroupSchemaList : public IFileStructure
{
public:
	GroupSchemaList(const CARObject* usr) : obj(usr) { }
	virtual string GetFileName() const { return obj->FileID() + "_list_form"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class GroupFieldList : public IFileStructure
{
public:
	GroupFieldList(const CARObject* usr) : obj(usr) { }
	virtual string GetFileName() const { return obj->FileID() + "list_field"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class GroupALList : public IFileStructure
{
public:
	GroupALList(const CARObject* usr) : obj(usr) { }
	virtual string GetFileName() const { return obj->FileID() + "list_active_link"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class GroupUserList : public IFileStructure
{
public:
	GroupUserList(const CARObject* usr) : obj(usr) { }
	virtual string GetFileName() const { return obj->FileID() + "list_user"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class GroupPackListList : public IFileStructure
{
public:
	GroupPackListList(const CARObject* usr) : obj(usr) { }
	virtual string GetFileName() const { return obj->FileID() + "list_packing_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class GroupALGuideList : public IFileStructure
{
public:
	GroupALGuideList(const CARObject* usr) : obj(usr) { }
	virtual string GetFileName() const { return obj->FileID() + "list_al_guide"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class GroupWebserviceList : public IFileStructure
{
public:
	GroupWebserviceList(const CARObject* usr) : obj(usr) { }
	virtual string GetFileName() const { return obj->FileID() + "list_webservice"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

////////////////////////////////////////////////////////////////////
// for roles                                                      //
class RoleOverview : public IFileStructure
{
public:
	RoleOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ROLE; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class RoleDetail : public IFileStructure
{
public:
	RoleDetail(const CARObject* role) : obj(role) { }
	virtual string GetFileName() const { return obj->FileID(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ROLE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class RoleLetterOverview : public IFileStructure
{
public:
	RoleLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ROLE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class RoleSchemaList : public IFileStructure
{
public:
	RoleSchemaList(const CARObject* role) : obj(role) { }
	virtual string GetFileName() const { return obj->FileID() + "_list_form"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ROLE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class RoleFieldList : public IFileStructure
{
public:
	RoleFieldList(const CARObject* role) : obj(role) { }
	virtual string GetFileName() const { return obj->FileID() + "list_field"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ROLE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class RoleALList : public IFileStructure
{
public:
	RoleALList(const CARObject* role) : obj(role) { }
	virtual string GetFileName() const { return obj->FileID() + "list_active_link"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ROLE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class RolePackListList : public IFileStructure
{
public:
	RolePackListList(const CARObject* role) : obj(role) { }
	virtual string GetFileName() const { return obj->FileID() + "list_packing_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ROLE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class RoleALGuideList : public IFileStructure
{
public:
	RoleALGuideList(const CARObject* role) : obj(role) { }
	virtual string GetFileName() const { return obj->FileID() + "list_al_guide"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ROLE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class RoleWebserviceList : public IFileStructure
{
public:
	RoleWebserviceList(const CARObject* role) : obj(role) { }
	virtual string GetFileName() const { return obj->FileID() + "list_webservice"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ROLE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

////////////////////////////////////////////////////////////////////
// for Images                                                     //
class ImageOverview : public IFileStructure
{
public:
	ImageOverview() { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_IMAGE; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ImageDetail : public IFileStructure
{
public:
	ImageDetail(const CARObject* img) : obj(img) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_IMAGE) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ImageLetterOverview : public IFileStructure
{
public:
	ImageLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << FILE_INDEX_LETTER_PREFIX << ((char)page); return strmTmp.str(); } else return "index_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_IMAGE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
class ImageData : public IFileStructure
{
public:
	ImageData(const CARObject* img) : obj(img) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + "image." + ((CARImage*)obj)->GetType(); }
	virtual string GetPath() const { return string(DIR_IMAGE) + "/" + obj->FileID(); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};
#endif

////////////////////////////////////////////////////////////////////
// for server info page                                           //
class ServerInfo : public IFileStructure
{
public:
	ServerInfo() { }
	virtual string GetFileName() const { return FILE_SERVER; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

////////////////////////////////////////////////////////////////////
// for global fields page                                           //
class GlobalFields : public IFileStructure
{
public:
	GlobalFields() { }
	virtual string GetFileName() const { return "global_field_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

////////////////////////////////////////////////////////////////////
// for messages page                                              //
class Messages : public IFileStructure
{
public:
	Messages() { }
	virtual string GetFileName() const { return "message_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

////////////////////////////////////////////////////////////////////
// analyzer main page                                             //
class AnalyzerMain : public IFileStructure
{
public:
	AnalyzerMain() { }
	virtual string GetFileName() const { return "analyzer_main"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

////////////////////////////////////////////////////////////////////
// analyzer qbe check page                                             //
class AnalyzerQBEChecker : public IFileStructure
{
public:
	AnalyzerQBEChecker() { }
	virtual string GetFileName() const { return "analyzer_schema_index"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

////////////////////////////////////////////////////////////////////
// validator main page                                            //
class ValidatorMain : public IFileStructure
{
public:
	ValidatorMain() { }
	virtual string GetFileName() const { return "validation_main"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

////////////////////////////////////////////////////////////////////
// validator container groups                                     //
class ValidatorContainerGroups : public IFileStructure
{
public:
	ValidatorContainerGroups() { }
	virtual string GetFileName() const { return "validation_group_container"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

////////////////////////////////////////////////////////////////////
// validator activelink groups                                    //
class ValidatorActiveLinkGroups : public IFileStructure
{
public:
	ValidatorActiveLinkGroups() { }
	virtual string GetFileName() const { return "validation_group_al"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

////////////////////////////////////////////////////////////////////
// validator field groups                                         //
class ValidatorFieldGroups : public IFileStructure
{
public:
	ValidatorFieldGroups() { }
	virtual string GetFileName() const { return "validation_group_field"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ValidatorFieldGroupDetails : public IFileStructure
{
public:
	ValidatorFieldGroupDetails(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "validation_group_field_" + obj->FileID(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

////////////////////////////////////////////////////////////////////
// validator field groups                                         //
class ValidatorFormGroups : public IFileStructure
{
public:
	ValidatorFormGroups() { }
	virtual string GetFileName() const { return "validation_group_form"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

////////////////////////////////////////////////////////////////////
// validator missing fields                                       //
class ValidatorMissingFields : public IFileStructure
{
public:
	ValidatorMissingFields() { }
	virtual string GetFileName() const { return "validation_field_references"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

////////////////////////////////////////////////////////////////////
// validator missing fields                                       //
class ValidatorMissingMenus : public IFileStructure
{
public:
	ValidatorMissingMenus() { }
	virtual string GetFileName() const { return "validation_menu_references"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OTHER; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

///////////////////////////////////////////////////////////////////////////////
// DefaultFileNamingStrategy implementation

void DefaultFileNamingStrategy::InitStructure()
{
	CWindowsUtil wUtil(CARInside::GetInstance()->appConfig);

	wUtil.CreateSubDirectory(DIR_ACTLINK);
	wUtil.CreateSubDirectory(DIR_ALGUIDE);
	wUtil.CreateSubDirectory(DIR_APPLICATION);
	wUtil.CreateSubDirectory(DIR_ESCALATION);
	wUtil.CreateSubDirectory(DIR_FILTER);
	wUtil.CreateSubDirectory(DIR_FLTGUIDE);
	wUtil.CreateSubDirectory(DIR_GROUP);
	wUtil.CreateSubDirectory(DIR_IMAGE);
	wUtil.CreateSubDirectory("img");
	wUtil.CreateSubDirectory(DIR_MENU);
	wUtil.CreateSubDirectory(DIR_OTHER);
	wUtil.CreateSubDirectory(DIR_PACKINGLIST);
	wUtil.CreateSubDirectory(DIR_SCHEMA);
	wUtil.CreateSubDirectory("template");
	wUtil.CreateSubDirectory(DIR_USER);
	wUtil.CreateSubDirectory(DIR_ROLE);
	wUtil.CreateSubDirectory(DIR_WEBSERVICE);
}

IFileStructure* DefaultFileNamingStrategy::GetFileNameOf(CPageParams &params)
{
	switch (params.page)
	{
		case PAGE_DETAILS:
		{
			assert(params.obj1 != NULL || params.data1 != NULL);

			if (params.obj1 != NULL)
			{
				switch (params.obj1->GetServerObjectTypeXML())
				{
					case AR_STRUCT_ITEM_XML_SCHEMA: return new SchemaDetail(params.obj1);
					case AR_STRUCT_ITEM_XML_FIELD: assert(params.obj1 != NULL && params.obj1->GetServerObjectTypeXML() == AR_STRUCT_ITEM_XML_FIELD); return new SchemaFieldDetail(static_cast<CARField*>(params.obj1));
					case AR_STRUCT_ITEM_XML_VUI: assert(params.obj1 != NULL && params.obj1->GetServerObjectTypeXML() == AR_STRUCT_ITEM_XML_VUI); return new SchemaVUIDetail(static_cast<CARVui*>(params.obj1));
					case AR_STRUCT_ITEM_XML_ACTIVE_LINK: return new ActiveLinkDetail(params.obj1);
					case AR_STRUCT_ITEM_XML_FILTER: return new FilterDetail(params.obj1);
					case AR_STRUCT_ITEM_XML_ESCALATION: return new EscalationDetail(params.obj1);
					case AR_STRUCT_ITEM_XML_CHAR_MENU: return new MenuDetail(params.obj1);
					case AR_STRUCT_ITEM_XML_CONTAINER:
						{
							CARContainer* cont = (CARContainer*)params.obj1;
							switch (cont->GetType())
							{
							case ARCON_GUIDE: return new ALGuideDetail(params.obj1);
							case ARCON_APP: return new ApplicationDetail(params.obj1);
							case ARCON_FILTER_GUIDE: return new FilterGuideDetail(params.obj1);
							case ARCON_PACK: return new PackingListDetail(params.obj1);
							case ARCON_WEBSERVICE: return new WebserviceDetail(params.obj1);
							}
						}
						break;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
					case AR_STRUCT_ITEM_XML_IMAGE: return new ImageDetail(params.obj1);
#endif
				}
			}
			else if (params.data1 != NULL)
			{
				switch (params.data1->GetDataType())
				{
				case DATA_TYPE_USER: return new UserDetail(params.data1);
				case DATA_TYPE_GROUP: return new GroupDetail(params.data1);
				case DATA_TYPE_ROLE: return new RoleDetail(params.data1);
				}
			}
		}
		break;

		case PAGE_OVERVIEW: 
		// object overview pages (except form, bc they have special page handling)
		{
			switch (params.uint1)
			{
				case AR_STRUCT_ITEM_XML_NONE:
					{
						switch (params.uint2)
						{
						case DATA_TYPE_USER: return new UserOverview();
						case DATA_TYPE_GROUP: return new GroupOverview();
						case DATA_TYPE_ROLE: return new RoleOverview();
						}
					}
					break;
				case AR_STRUCT_ITEM_XML_SCHEMA: return new SchemaOverview();
				case AR_STRUCT_ITEM_XML_ACTIVE_LINK: return new ActiveLinkOverview();
				case AR_STRUCT_ITEM_XML_FILTER: return new FilterOverview();
				case AR_STRUCT_ITEM_XML_ESCALATION: return new EscalationOverview();
				case AR_STRUCT_ITEM_XML_CHAR_MENU: return new MenuOverview();
				case AR_STRUCT_ITEM_XML_CONTAINER: 
					{
						// switch container type
						switch (params.uint2)
						{
						case ARCON_GUIDE: return new ALGuideOverview();
						case ARCON_APP: return new ApplicationOverview();
						case ARCON_FILTER_GUIDE: return new FilterGuideOverview();
						case ARCON_PACK: return new PackingListOverview();
						case ARCON_WEBSERVICE: return new WebserviceOverview();
						}
					}
					break;
				case AR_STRUCT_ITEM_XML_VUI: assert(params.obj1 != NULL && params.obj1->GetServerObjectTypeXML() == AR_STRUCT_ITEM_XML_SCHEMA); return new SchemaVUIOverview(static_cast<CARSchema*>(params.obj1));
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
				case AR_STRUCT_ITEM_XML_IMAGE: return new ImageOverview();
#endif
				default: return NULL;
			}
		}
		break;
		
		case PAGE_SCHEMA_PERMISSIONS: return new SchemaPermissions(params.obj1);
		case PAGE_SCHEMA_WORKFLOW: return new SchemaWorkflow(params.obj1);
		case PAGE_SCHEMA_INDEXES: return new SchemaIndexes(params.obj1);
		case PAGE_SCHEMA_SORTLIST: return new SchemaSortList(params.obj1);
		case PAGE_SCHEMA_RESULTLIST: return new SchemaResultList(params.obj1);
		case PAGE_SCHEMA_SUBADMINS: return new SchemaSubadmins(params.obj1);
		case PAGE_SCHEMA_ACTIVELINKS: return new SchemaActiveLinks(params.obj1);
		case PAGE_SCHEMA_FILTERS: return new SchemaFilters(params.obj1);
		case PAGE_SCHEMA_ESCALATIONS: return new SchemaEscalations(params.obj1);

		case PAGE_SCHEMA_REGULAR: return new SchemaRegular();
		case PAGE_SCHEMA_JOIN: return new SchemaJoin();
		case PAGE_SCHEMA_VIEW: return new SchemaView();
		case PAGE_SCHEMA_DIALOG: return new SchemaDialog();
		case PAGE_SCHEMA_VENDOR: return new SchemaVendor();

		case PAGE_SCHEMA_FIELDS_CSV: assert(params.obj1 != NULL && params.obj1->GetServerObjectTypeXML() == AR_STRUCT_ITEM_XML_SCHEMA); return new SchemaFieldsCSV(static_cast<CARSchema*>(params.obj1));
		case PAGE_SCHEMA_VUIFIELDS_CSV: assert(params.obj1 != NULL && params.obj1->GetServerObjectTypeXML() == AR_STRUCT_ITEM_XML_VUI); return new SchemaVUIFieldsCSV(static_cast<CARVui*>(params.obj1));
		
		case PAGE_ACTION_OVERVIEW: 
		{
			switch (params.uint1)
			{
			case AR_STRUCT_ITEM_XML_ACTIVE_LINK: return new ActiveLinkActionOverview();
			case AR_STRUCT_ITEM_XML_FILTER: return new FilterActionOverview();
			case AR_STRUCT_ITEM_XML_ESCALATION: return new EscalationActionOverview();
			}
		}
		break;

		case PAGE_ACTION_OBJLIST:
		{
			switch (params.uint1)
			{
				case AR_STRUCT_ITEM_XML_ACTIVE_LINK: return new ActiveLinkActionDetail(params.uint2);
				case AR_STRUCT_ITEM_XML_FILTER: return new FilterActionDetail(params.uint2);
				case AR_STRUCT_ITEM_XML_ESCALATION: return new EscalationActionDetail(params.uint2);
			}
		}
		break;

		case PAGE_GROUP_SCHEMA_LIST: assert(params.data1 != NULL); return new GroupSchemaList(params.data1);
		case PAGE_GROUP_FIELD_LIST: assert(params.data1 != NULL); return new GroupFieldList(params.data1);
		case PAGE_GROUP_ACTIVELINK_LIST: assert(params.data1 != NULL); return new GroupALList(params.data1);
		case PAGE_GROUP_USER_LIST: assert(params.data1 != NULL); return new GroupUserList(params.data1);
		case PAGE_GROUP_CONTAINER_LIST: 
			{
				switch (params.uint1)
				{
				case ARCON_PACK: assert(params.data1 != NULL); return new GroupPackListList(params.data1);
				case ARCON_GUIDE: assert(params.data1 != NULL); return new GroupALGuideList(params.data1);
				case ARCON_WEBSERVICE: assert(params.data1 != NULL); return new GroupWebserviceList(params.data1);
				}
			}
			break;
		
		case PAGE_ROLE_SCHEMA_LIST: assert(params.data1 != NULL); return new RoleSchemaList(params.data1);
		case PAGE_ROLE_FIELD_LIST: assert(params.data1 != NULL); return new RoleFieldList(params.data1);
		case PAGE_ROLE_ACTIVELINK_LIST: assert(params.data1 != NULL); return new RoleALList(params.data1);
		case PAGE_ROLE_CONTAINER_LIST:
			{
				switch(params.uint1)
				{
				case ARCON_PACK: assert(params.data1 != NULL); return new RolePackListList(params.data1);
				case ARCON_GUIDE: assert(params.data1 != NULL); return new RoleALGuideList(params.data1);
				case ARCON_WEBSERVICE: assert(params.data1 != NULL); return new RoleWebserviceList(params.data1);
				}
			}
			break;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		case PAGE_IMAGE_DATA: assert(params.obj1 != NULL); return new ImageData(params.obj1);
#endif		
		case PAGE_SERVER_INFO: return new ServerInfo();
		case PAGE_GLOBALFIELS: return new GlobalFields();
		case PAGE_MESSAGES: return new Messages();

		case PAGE_ANALYZER_MAIN: return new AnalyzerMain();
		case PAGE_ANALYZER_QBE_CHECK: return new AnalyzerQBEChecker();

		case PAGE_VALIDATOR_MAIN: return new ValidatorMain();
		case PAGE_VALIDATOR_MISSING_FIELDS: return new ValidatorMissingFields();
		case PAGE_VALIDATOR_MISSING_MENUS: return new ValidatorMissingMenus();
		case PAGE_VALIDATOR_FORM_GROUPS: return new ValidatorFormGroups();
		case PAGE_VALIDATOR_FIELD_GROUPS: return new ValidatorFieldGroups();
		case PAGE_VALIDATOR_FIELD_GROUP_DETAILS: assert(params.obj1 != NULL); return new ValidatorFieldGroupDetails(params.obj1);
		case PAGE_VALIDATOR_ACTIVELINKS_GROUPS: return new ValidatorActiveLinkGroups();
		case PAGE_VALIDATOR_CONTAINER_GROUPS: return new ValidatorContainerGroups();
	}

	if (params.page >= PAGE_OVERVIEW_LETTER_A && params.page <= PAGE_OVERVIEW_LETTER_Z || 
		  params.page >= PAGE_OVERVIEW_LETTER_0 && params.page <= PAGE_OVERVIEW_LETTER_9 ||
			params.page == PAGE_OVERVIEW_OTHER)
	{
		switch (params.uint1)
		{
			case AR_STRUCT_ITEM_XML_NONE:
				{
					switch (params.uint2)
					{
					case DATA_TYPE_USER: return new UserLetterOverview(params.page);
					case DATA_TYPE_GROUP: return new GroupLetterOverview(params.page);
					case DATA_TYPE_ROLE: return new RoleLetterOverview(params.page);
					}
				}
				break;
			case AR_STRUCT_ITEM_XML_SCHEMA: return new SchemaLetterOverview(params.page);
			case AR_STRUCT_ITEM_XML_ACTIVE_LINK: return new ActiveLinkLetterOverview(params.page);
			case AR_STRUCT_ITEM_XML_FILTER: return new FilterLetterOverview(params.page);
			case AR_STRUCT_ITEM_XML_ESCALATION: return new EscalationLetterOverview(params.page);
			case AR_STRUCT_ITEM_XML_CHAR_MENU: return new MenuLetterOverview(params.page);
			case AR_STRUCT_ITEM_XML_CONTAINER:
				{
					switch (params.uint2)
					{
					case ARCON_GUIDE: return new ALGuideLetterOverview(params.page);
					case ARCON_APP: return new ApplicationLetterOverview(params.page);
					case ARCON_FILTER_GUIDE: return new FilterGuideLetterOverview(params.page);
					case ARCON_PACK: return new PackingListLetterOverview(params.page);
					case ARCON_WEBSERVICE: return new WebserviceLetterOverview(params.page);
					}
				}
				break;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
			case AR_STRUCT_ITEM_XML_IMAGE: return new ImageLetterOverview(params.page);
#endif
		}
	}

	assert(false); /// whoops, this shouldn't happen ... maybe a needed implementation is missing!
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// object name file name generator
string GetFileNameOfObjectName(const string &objName)
{
	stringstream strmTmp;
	strmTmp << hex;

	size_t len = objName.length();
	for (size_t pos = 0; pos < len; ++pos) 
	{
		char c = objName[pos];
		if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9' || c == '-' || c == '.' || c == ',' || c == ' ' || c == '_' || c == '+')
		{
			strmTmp << c;
		}
		else {
			strmTmp << '~' << (int)c;
		}
	}
	return strmTmp.str();
}

///////////////////////////////////////////////////////////////////////////////
// object name file naming classes
class ObjectNameSchemaDetail : public IFileStructure
{
public:
	ObjectNameSchemaDetail(const CARSchema* schema) : obj(schema) { }
	virtual string GetFileName() const { return FILE_INDEX; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARSchema* obj;
};

class ObjectNameSchemaFieldDetail : public IFileStructure
{
public:
	ObjectNameSchemaFieldDetail(const CARField* fld) : obj(fld) {  }
	virtual string GetFileName() const { return "fld_" + obj->FileID(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetSchema().GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARField* obj;
};

class ObjectNameSchemaVUIOverview : public IFileStructure
{
public:
	ObjectNameSchemaVUIOverview(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_vui_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ObjectNameSchemaVUIDetail : public IFileStructure
{
public:
	ObjectNameSchemaVUIDetail(const CARVui* vui) : obj(vui) {  }
	virtual string GetFileName() const { return "vui_"+obj->FileID(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetSchema().GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARVui* obj;
};

class ObjectNameSchemaVUIFieldsCSV : public IFileStructure
{
public:
	ObjectNameSchemaVUIFieldsCSV(const CARVui* vui) : obj(vui) {  }
	virtual string GetFileName() const { return "vui_"+obj->FileID(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::CsvDocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetSchema().GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARVui* obj;
};

class ObjectNameSchemaActiveLinks : public IFileStructure
{
public:
	ObjectNameSchemaActiveLinks(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_al_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ObjectNameSchemaEscalations : public IFileStructure
{
public:
	ObjectNameSchemaEscalations(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_escal_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ObjectNameSchemaFilters : public IFileStructure
{
public:
	ObjectNameSchemaFilters(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_filter_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ObjectNameSchemaIndexes : public IFileStructure
{
public:
	ObjectNameSchemaIndexes(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_index_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ObjectNameSchemaPermissions : public IFileStructure
{
public:
	ObjectNameSchemaPermissions(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_permission_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ObjectNameSchemaResultList : public IFileStructure
{
public:
	ObjectNameSchemaResultList(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_result_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ObjectNameSchemaSortList : public IFileStructure
{
public:
	ObjectNameSchemaSortList(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_sort_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ObjectNameSchemaSubadmins : public IFileStructure
{
public:
	ObjectNameSchemaSubadmins(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_subadmin_list"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ObjectNameActiveLinkDetail : public IFileStructure
{
public:
	ObjectNameActiveLinkDetail(const CARActiveLink* al) : obj(al) { }
	virtual string GetFileName() const { return GetFileNameOfObjectName(obj->GetName()); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ACTLINK; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARActiveLink* obj;
};

class ObjectNameSchemaWorkflow : public IFileStructure
{
public:
	ObjectNameSchemaWorkflow(const CARObject* schema) : obj(schema) { }
	virtual string GetFileName() const { return "form_workflow"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARObject* obj;
};

class ObjectNameSchemaFieldsCSV : public IFileStructure
{
public:
	ObjectNameSchemaFieldsCSV(const CARSchema* vui) : obj(vui) {  }
	virtual string GetFileName() const { return "form_fields"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::CsvDocName(GetFileName()); }
	virtual string GetPath() const { return string(DIR_SCHEMA) + "/" + GetFileNameOfObjectName(obj->GetName()); }
	virtual unsigned int GetRootLevel() const { return 2; }
private:
	const CARSchema* obj;
};

class ObjectNameFilterDetail : public IFileStructure
{
public:
	ObjectNameFilterDetail(const CARFilter* flt) : obj(flt) { }
	virtual string GetFileName() const { return GetFileNameOfObjectName(obj->GetName()); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_FILTER; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARFilter* obj;
};

class ObjectNameEscalationDetail : public IFileStructure
{
public:
	ObjectNameEscalationDetail(const CAREscalation* esc) : obj(esc) { }
	virtual string GetFileName() const { return GetFileNameOfObjectName(obj->GetName()); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ESCALATION; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CAREscalation* obj;
};

class ObjectNameMenuDetail : public IFileStructure
{
public:
	ObjectNameMenuDetail(const CARCharMenu* mnu) : obj(mnu) { }
	virtual string GetFileName() const { return GetFileNameOfObjectName(obj->GetName()); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_MENU; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARCharMenu* obj;
};

class ObjectNameContainerDetail : public IFileStructure
{
public:
	ObjectNameContainerDetail(const CARServerObject* alg, const char* path) : obj(alg), dir(path)  { }
	virtual string GetFileName() const { return GetFileNameOfObjectName(obj->GetName()); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return dir; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
	const char* dir;
};

class ObjectNameGroupDetail : public IFileStructure
{
public:
	ObjectNameGroupDetail(const CARGroup* grp) : obj(grp) { }
	virtual string GetFileName() const { stringstream tmp; tmp << obj->groupId; return tmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARGroup* obj;
};

class ObjectNameGroupSchemaList : public IFileStructure
{
public:
	ObjectNameGroupSchemaList(const CARGroup* grp) : obj(grp) { }
	virtual string GetFileName() const { stringstream tmp; tmp << obj->groupId << "_list_form"; return tmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARGroup* obj;
};

class ObjectNameGroupFieldList : public IFileStructure
{
public:
	ObjectNameGroupFieldList(const CARGroup* grp) : obj(grp) { }
	virtual string GetFileName() const { stringstream tmp; tmp << obj->groupId << "_list_field"; return tmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARGroup* obj;
};

class ObjectNameGroupALList : public IFileStructure
{
public:
	ObjectNameGroupALList(const CARGroup* grp) : obj(grp) { }
	virtual string GetFileName() const { stringstream tmp; tmp << obj->groupId << "_list_active_link"; return tmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARGroup* obj;
};

class ObjectNameGroupUserList : public IFileStructure
{
public:
	ObjectNameGroupUserList(const CARGroup* grp) : obj(grp) { }
	virtual string GetFileName() const { stringstream tmp; tmp << obj->groupId << "_list_user"; return tmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARGroup* obj;
};

class ObjectNameGroupPackListList : public IFileStructure
{
public:
	ObjectNameGroupPackListList(const CARGroup* grp) : obj(grp) { }
	virtual string GetFileName() const { stringstream tmp; tmp << obj->groupId << "_list_packing_list"; return tmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARGroup* obj;
};

class ObjectNameGroupALGuideList : public IFileStructure
{
public:
	ObjectNameGroupALGuideList(const CARGroup* grp) : obj(grp) { }
	virtual string GetFileName() const { stringstream tmp; tmp << obj->groupId << "_list_al_guide"; return tmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARGroup* obj;
};

class ObjectNameGroupWebserviceList : public IFileStructure
{
public:
	ObjectNameGroupWebserviceList(const CARGroup* grp) : obj(grp) { }
	virtual string GetFileName() const { stringstream tmp; tmp << obj->groupId << "_list_webservice"; return tmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_GROUP; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARGroup* obj;
};

class ObjectNameUserDetail : public IFileStructure
{
public:
	ObjectNameUserDetail(const CARObject* usr) : obj(usr) { }
	virtual string GetFileName() const { return GetFileNameOfObjectName(obj->GetName()); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_USER; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

class ObjectNameRoleDetail : public IFileStructure
{
public:
	ObjectNameRoleDetail(const CARRole* role) : obj(role) { }
	virtual string GetFileName() const { stringstream tmp; tmp << obj->roleId; return tmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_ROLE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARRole* obj;
};

class ObjectNameImageDetail : public IFileStructure
{
public:
	ObjectNameImageDetail(const CARObject* img) : obj(img) { }
	virtual string GetFileName() const { return GetFileNameOfObjectName(obj->GetName()); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_IMAGE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARObject* obj;
};

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
class ObjectNameImageData : public IFileStructure
{
public:
	ObjectNameImageData(const CARImage* img) : obj(img) { }
	virtual string GetFileName() const { return GetFileNameOfObjectName(obj->GetName()); }
	virtual string GetFullFileName() const { stringstream tmp; tmp << GetPath() << "/" << GetFileName() << "." << obj->GetType(); return tmp.str(); }
	virtual string GetPath() const { return DIR_IMAGE; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	const CARImage* obj;
};
#endif

class ObjectNameActiveLinkOverview : public IFileStructure
{
public:
	ObjectNameActiveLinkOverview() { }
	virtual string GetFileName() const { return "actlinks"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameActiveLinkLetterOverview : public IFileStructure
{
public:
	ObjectNameActiveLinkLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "actlinks_" << ((char)page); return strmTmp.str(); } else return "actlinks_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class ObjectNameActiveLinkActionOverview : public IFileStructure
{
public:
	ObjectNameActiveLinkActionOverview() { }
	virtual string GetFileName() const { return "actlinks_action"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameActiveLinkActionDetail : public IFileStructure
{
public:
	ObjectNameActiveLinkActionDetail(unsigned int action) { nAction = action; }
	virtual string GetFileName() const { stringstream strmTmp; strmTmp << "actlinks_action_" << nAction; return strmTmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int nAction;
};

class ObjectNameFilterOverview : public IFileStructure
{
public:
	ObjectNameFilterOverview() { }
	virtual string GetFileName() const { return "filters"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameFilterActionOverview : public IFileStructure
{
public:
	ObjectNameFilterActionOverview() { }
	virtual string GetFileName() const { return "filters_action"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameFilterActionDetail : public IFileStructure
{
public:
	ObjectNameFilterActionDetail(unsigned int action) { nAction = action; }
	virtual string GetFileName() const { stringstream strmTmp; strmTmp << "filters_action_" << nAction; return strmTmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int nAction;
};

class ObjectNameFilterLetterOverview : public IFileStructure
{
public:
	ObjectNameFilterLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "filters_" << ((char)page); return strmTmp.str(); } else return "filters_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class ObjectNameEscalationOverview : public IFileStructure
{
public:
	ObjectNameEscalationOverview() { }
	virtual string GetFileName() const { return "escalations"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameEscalationActionOverview : public IFileStructure
{
public:
	ObjectNameEscalationActionOverview() { }
	virtual string GetFileName() const { return "escalations_action"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameEscalationActionDetail : public IFileStructure
{
public:
	ObjectNameEscalationActionDetail(unsigned int action) { nAction = action; }
	virtual string GetFileName() const { stringstream strmTmp; strmTmp << "escalations_action_" << nAction; return strmTmp.str(); }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int nAction;
};

class ObjectNameEscalationLetterOverview : public IFileStructure
{
public:
	ObjectNameEscalationLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "escalations_" << ((char)page); return strmTmp.str(); } else return "escalations_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class ObjectNameALGuideOverview : public IFileStructure
{
public:
	ObjectNameALGuideOverview() { }
	virtual string GetFileName() const { return "actlink_guides"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameALGuideLetterOverview : public IFileStructure
{
public:
	ObjectNameALGuideLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "actlink_guides_" << ((char)page); return strmTmp.str(); } else return "actlinks_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class ObjectNameFilterGuideOverview : public IFileStructure
{
public:
	ObjectNameFilterGuideOverview() { }
	virtual string GetFileName() const { return "filter_guides"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameFilterGuideLetterOverview : public IFileStructure
{
public:
	ObjectNameFilterGuideLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "filter_guides_" << ((char)page); return strmTmp.str(); } else return "filter_guides_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class ObjectNamePackingListOverview : public IFileStructure
{
public:
	ObjectNamePackingListOverview() { }
	virtual string GetFileName() const { return "packlists"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNamePackingListLetterOverview : public IFileStructure
{
public:
	ObjectNamePackingListLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "packlists_" << ((char)page); return strmTmp.str(); } else return "packlists_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class ObjectNameApplicationOverview : public IFileStructure
{
public:
	ObjectNameApplicationOverview() { }
	virtual string GetFileName() const { return "apps"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameApplicationLetterOverview : public IFileStructure
{
public:
	ObjectNameApplicationLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "apps_" << ((char)page); return strmTmp.str(); } else return "apps_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class ObjectNameWebserviceOverview : public IFileStructure
{
public:
	ObjectNameWebserviceOverview() { }
	virtual string GetFileName() const { return "webservices"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameWebserviceLetterOverview : public IFileStructure
{
public:
	ObjectNameWebserviceLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "webservices_" << ((char)page); return strmTmp.str(); } else return "webservices_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class ObjectNameMenuOverview : public IFileStructure
{
public:
	ObjectNameMenuOverview() { }
	virtual string GetFileName() const { return "menus"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameMenuLetterOverview : public IFileStructure
{
public:
	ObjectNameMenuLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "menus_" << ((char)page); return strmTmp.str(); } else return "menus_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class ObjectNameUserOverview : public IFileStructure
{
public:
	ObjectNameUserOverview() { }
	virtual string GetFileName() const { return "users"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameUserLetterOverview : public IFileStructure
{
public:
	ObjectNameUserLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "users_" << ((char)page); return strmTmp.str(); } else return "users_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class ObjectNameGroupOverview : public IFileStructure
{
public:
	ObjectNameGroupOverview() { }
	virtual string GetFileName() const { return "groups"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameGroupLetterOverview : public IFileStructure
{
public:
	ObjectNameGroupLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "groups" << ((char)page); return strmTmp.str(); } else return "groups_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class ObjectNameRoleOverview : public IFileStructure
{
public:
	ObjectNameRoleOverview() { }
	virtual string GetFileName() const { return "roles"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameRoleLetterOverview : public IFileStructure
{
public:
	ObjectNameRoleLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "roles_" << ((char)page); return strmTmp.str(); } else return "roles_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

class ObjectNameImageOverview : public IFileStructure
{
public:
	ObjectNameImageOverview() { }
	virtual string GetFileName() const { return "images"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
};

class ObjectNameImageLetterOverview : public IFileStructure
{
public:
	ObjectNameImageLetterOverview(unsigned int dwPage) { page = dwPage; }
	virtual string GetFileName() const { if (page != PAGE_OVERVIEW_OTHER) { stringstream strmTmp; strmTmp << "images_" << ((char)page); return strmTmp.str(); } else return "images_other"; }
	virtual string GetFullFileName() const { return GetPath() + "/" + CWebUtil::DocName(GetFileName()); }
	virtual string GetPath() const { return DIR_OVERVIEW; }
	virtual unsigned int GetRootLevel() const { return 1; }
private:
	unsigned int page;
};

///////////////////////////////////////////////////////////////////////////////
// object name file naming implementation

void ObjectNameFileNamingStrategy::InitStructure()
{
	CWindowsUtil wUtil(CARInside::GetInstance()->appConfig);

	wUtil.CreateSubDirectory(DIR_ACTLINK);
	wUtil.CreateSubDirectory(DIR_ALGUIDE);
	wUtil.CreateSubDirectory(DIR_APPLICATION);
	wUtil.CreateSubDirectory(DIR_ESCALATION);
	wUtil.CreateSubDirectory(DIR_FILTER);
	wUtil.CreateSubDirectory(DIR_FLTGUIDE);
	wUtil.CreateSubDirectory(DIR_GROUP);
	wUtil.CreateSubDirectory(DIR_IMAGE);
	wUtil.CreateSubDirectory("img");
	wUtil.CreateSubDirectory(DIR_MENU);
	wUtil.CreateSubDirectory(DIR_OTHER);
	wUtil.CreateSubDirectory(DIR_PACKINGLIST);
	wUtil.CreateSubDirectory(DIR_SCHEMA);
	wUtil.CreateSubDirectory("template");
	wUtil.CreateSubDirectory(DIR_USER);
	wUtil.CreateSubDirectory(DIR_ROLE);
	wUtil.CreateSubDirectory(DIR_WEBSERVICE);
	wUtil.CreateSubDirectory(DIR_OVERVIEW);
}

IFileStructure* ObjectNameFileNamingStrategy::GetFileNameOf(CPageParams &params)
{
	switch (params.page)
	{
		case PAGE_DETAILS:
		{
			assert(params.obj1 != NULL || params.data1 != NULL);

			if (params.obj1 != NULL)
			{
				switch (params.obj1->GetServerObjectTypeXML())
				{
					case AR_STRUCT_ITEM_XML_SCHEMA:      return new ObjectNameSchemaDetail(static_cast<CARSchema*>(params.obj1));
					case AR_STRUCT_ITEM_XML_FIELD:       return new ObjectNameSchemaFieldDetail(static_cast<CARField*>(params.obj1));
					case AR_STRUCT_ITEM_XML_VUI:         return new ObjectNameSchemaVUIDetail(static_cast<CARVui*>(params.obj1));
					case AR_STRUCT_ITEM_XML_ACTIVE_LINK: return new ObjectNameActiveLinkDetail(static_cast<CARActiveLink*>(params.obj1));
					case AR_STRUCT_ITEM_XML_FILTER:      return new ObjectNameFilterDetail(static_cast<CARFilter*>(params.obj1));
					case AR_STRUCT_ITEM_XML_ESCALATION:  return new ObjectNameEscalationDetail(static_cast<CAREscalation*>(params.obj1));
					case AR_STRUCT_ITEM_XML_CHAR_MENU:   return new ObjectNameMenuDetail(static_cast<CARCharMenu*>(params.obj1));
					case AR_STRUCT_ITEM_XML_CONTAINER:
						{
							CARContainer* cont = static_cast<CARContainer*>(params.obj1);
							switch (cont->GetType())
							{
							case ARCON_GUIDE: return new ObjectNameContainerDetail(params.obj1, DIR_ALGUIDE);
							case ARCON_APP: return new ObjectNameContainerDetail(params.obj1, DIR_APPLICATION);
							case ARCON_FILTER_GUIDE: return new ObjectNameContainerDetail(params.obj1, DIR_FLTGUIDE);
							case ARCON_PACK: return new ObjectNameContainerDetail(params.obj1, DIR_PACKINGLIST);
							case ARCON_WEBSERVICE: return new ObjectNameContainerDetail(params.obj1, DIR_WEBSERVICE);
							}
						}
						break;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
					case AR_STRUCT_ITEM_XML_IMAGE: return new ObjectNameImageDetail(params.obj1);
#endif
				}
			}
			else if (params.data1 != NULL)
			{
				switch (params.data1->GetDataType())
				{
				case DATA_TYPE_USER: return new ObjectNameUserDetail(params.data1);
				case DATA_TYPE_GROUP: return new ObjectNameGroupDetail(static_cast<CARGroup*>(params.data1));
				case DATA_TYPE_ROLE: return new ObjectNameRoleDetail(static_cast<CARRole*>(params.data1));
				}
			}
		}
		break;

		case PAGE_OVERVIEW: 
		// object overview pages (except form, bc they have special page handling)
		{
				switch (params.uint1)
				{
				case AR_STRUCT_ITEM_XML_NONE:
					{
						switch (params.uint2)
						{
						case DATA_TYPE_USER: return new ObjectNameUserOverview();
						case DATA_TYPE_GROUP: return new GroupOverview();
						case DATA_TYPE_ROLE: return new RoleOverview();
						}
					}
					break;
				case AR_STRUCT_ITEM_XML_SCHEMA: return new SchemaOverview();
				case AR_STRUCT_ITEM_XML_ACTIVE_LINK: return new ObjectNameActiveLinkOverview();
				case AR_STRUCT_ITEM_XML_FILTER: return new ObjectNameFilterOverview();
				case AR_STRUCT_ITEM_XML_ESCALATION: return new ObjectNameEscalationOverview();
				case AR_STRUCT_ITEM_XML_CHAR_MENU: return new ObjectNameMenuOverview();
				case AR_STRUCT_ITEM_XML_CONTAINER: 
					{
						// switch container type
						switch (params.uint2)
						{
						case ARCON_GUIDE: return new ObjectNameALGuideOverview();
						case ARCON_APP: return new ObjectNameApplicationOverview();
						case ARCON_FILTER_GUIDE: return new ObjectNameFilterGuideOverview();
						case ARCON_PACK: return new ObjectNamePackingListOverview();
						case ARCON_WEBSERVICE: return new ObjectNameWebserviceOverview();
						}
					}
					break;
				case AR_STRUCT_ITEM_XML_VUI: return new ObjectNameSchemaVUIOverview(static_cast<CARSchema*>(params.obj1));
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
				case AR_STRUCT_ITEM_XML_IMAGE: return new ObjectNameImageOverview();
#endif
				default: return NULL;
				}
		}
		break;
		
		case PAGE_SCHEMA_PERMISSIONS: return new ObjectNameSchemaPermissions(params.obj1);
		case PAGE_SCHEMA_WORKFLOW: return new ObjectNameSchemaWorkflow(params.obj1);
		case PAGE_SCHEMA_INDEXES: return new ObjectNameSchemaIndexes(params.obj1);
		case PAGE_SCHEMA_SORTLIST: return new ObjectNameSchemaSortList(params.obj1);
		case PAGE_SCHEMA_RESULTLIST: return new ObjectNameSchemaResultList(params.obj1);
		case PAGE_SCHEMA_SUBADMINS: return new ObjectNameSchemaSubadmins(params.obj1);
		case PAGE_SCHEMA_ACTIVELINKS: return new ObjectNameSchemaActiveLinks(params.obj1);
		case PAGE_SCHEMA_FILTERS: return new ObjectNameSchemaFilters(params.obj1);
		case PAGE_SCHEMA_ESCALATIONS: return new ObjectNameSchemaEscalations(params.obj1);

		case PAGE_SCHEMA_REGULAR: return new SchemaRegular();
		case PAGE_SCHEMA_JOIN: return new SchemaJoin();
		case PAGE_SCHEMA_VIEW: return new SchemaView();
		case PAGE_SCHEMA_DIALOG: return new SchemaDialog();
		case PAGE_SCHEMA_VENDOR: return new SchemaVendor();

		case PAGE_SCHEMA_FIELDS_CSV: assert(params.obj1 != NULL && params.obj1->GetServerObjectTypeXML() == AR_STRUCT_ITEM_XML_SCHEMA); return new ObjectNameSchemaFieldsCSV(static_cast<CARSchema*>(params.obj1));
		case PAGE_SCHEMA_VUIFIELDS_CSV: assert(params.obj1 != NULL && params.obj1->GetServerObjectTypeXML() == AR_STRUCT_ITEM_XML_VUI); return new ObjectNameSchemaVUIFieldsCSV(static_cast<CARVui*>(params.obj1));
		
		case PAGE_ACTION_OVERVIEW: 
		{
			switch (params.uint1)
			{
			case AR_STRUCT_ITEM_XML_ACTIVE_LINK: return new ObjectNameActiveLinkActionOverview();
			case AR_STRUCT_ITEM_XML_FILTER: return new ObjectNameFilterActionOverview();
			case AR_STRUCT_ITEM_XML_ESCALATION: return new ObjectNameEscalationActionOverview();
			}
		}
		break;

		case PAGE_ACTION_OBJLIST:
		{
			switch (params.uint1)
			{
				case AR_STRUCT_ITEM_XML_ACTIVE_LINK: return new ObjectNameActiveLinkActionDetail(params.uint2);
				case AR_STRUCT_ITEM_XML_FILTER: return new ObjectNameFilterActionDetail(params.uint2);
				case AR_STRUCT_ITEM_XML_ESCALATION: return new ObjectNameEscalationActionDetail(params.uint2);
			}
		}
		break;

		case PAGE_GROUP_SCHEMA_LIST: assert(params.data1 != NULL && params.data1->GetDataType() == DATA_TYPE_GROUP); return new ObjectNameGroupSchemaList(static_cast<CARGroup*>(params.data1));
		case PAGE_GROUP_FIELD_LIST: assert(params.data1 != NULL && params.data1->GetDataType() == DATA_TYPE_GROUP); return new ObjectNameGroupFieldList(static_cast<CARGroup*>(params.data1));
		case PAGE_GROUP_ACTIVELINK_LIST: assert(params.data1 != NULL && params.data1->GetDataType() == DATA_TYPE_GROUP); return new ObjectNameGroupALList(static_cast<CARGroup*>(params.data1));
		case PAGE_GROUP_USER_LIST: assert(params.data1 != NULL && params.data1->GetDataType() == DATA_TYPE_GROUP); return new ObjectNameGroupUserList(static_cast<CARGroup*>(params.data1));
		case PAGE_GROUP_CONTAINER_LIST: 
			{
				switch (params.uint1)
				{
				case ARCON_PACK: assert(params.data1 != NULL && params.data1->GetDataType() == DATA_TYPE_GROUP); return new GroupPackListList(static_cast<CARGroup*>(params.data1));
				case ARCON_GUIDE: assert(params.data1 != NULL && params.data1->GetDataType() == DATA_TYPE_GROUP); return new GroupALGuideList(static_cast<CARGroup*>(params.data1));
				case ARCON_WEBSERVICE: assert(params.data1 != NULL && params.data1->GetDataType() == DATA_TYPE_GROUP); return new GroupWebserviceList(static_cast<CARGroup*>(params.data1));
				}
			}
			break;
		
		case PAGE_ROLE_SCHEMA_LIST: assert(params.data1 != NULL); return new RoleSchemaList(params.data1);
		case PAGE_ROLE_FIELD_LIST: assert(params.data1 != NULL); return new RoleFieldList(params.data1);
		case PAGE_ROLE_ACTIVELINK_LIST: assert(params.data1 != NULL); return new RoleALList(params.data1);
		case PAGE_ROLE_CONTAINER_LIST:
			{
				switch(params.uint1)
				{
				case ARCON_PACK: assert(params.data1 != NULL); return new RolePackListList(params.data1);
				case ARCON_GUIDE: assert(params.data1 != NULL); return new RoleALGuideList(params.data1);
				case ARCON_WEBSERVICE: assert(params.data1 != NULL); return new RoleWebserviceList(params.data1);
				}
			}
			break;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		case PAGE_IMAGE_DATA: assert(params.obj1 != NULL && params.obj1->GetServerObjectTypeXML() == AR_STRUCT_ITEM_XML_IMAGE); return new ObjectNameImageData(static_cast<CARImage*>(params.obj1));
#endif

		case PAGE_SERVER_INFO: return new ServerInfo();
		case PAGE_GLOBALFIELS: return new GlobalFields();
		case PAGE_MESSAGES: return new Messages();

		case PAGE_ANALYZER_MAIN: return new AnalyzerMain();
		case PAGE_ANALYZER_QBE_CHECK: return new AnalyzerQBEChecker();

		case PAGE_VALIDATOR_MAIN: return new ValidatorMain();
		case PAGE_VALIDATOR_MISSING_FIELDS: return new ValidatorMissingFields();
		case PAGE_VALIDATOR_MISSING_MENUS: return new ValidatorMissingMenus();
		case PAGE_VALIDATOR_FORM_GROUPS: return new ValidatorFormGroups();
		case PAGE_VALIDATOR_FIELD_GROUPS: return new ValidatorFieldGroups();
		case PAGE_VALIDATOR_FIELD_GROUP_DETAILS: assert(params.obj1 != NULL); return new ValidatorFieldGroupDetails(params.obj1);
		case PAGE_VALIDATOR_ACTIVELINKS_GROUPS: return new ValidatorActiveLinkGroups();
		case PAGE_VALIDATOR_CONTAINER_GROUPS: return new ValidatorContainerGroups();
	}

	if (params.page >= PAGE_OVERVIEW_LETTER_A && params.page <= PAGE_OVERVIEW_LETTER_Z || 
		  params.page >= PAGE_OVERVIEW_LETTER_0 && params.page <= PAGE_OVERVIEW_LETTER_9 ||
			params.page == PAGE_OVERVIEW_OTHER)
	{
		switch (params.uint1)
		{
			case AR_STRUCT_ITEM_XML_NONE:
				{
					switch (params.uint2)
					{
					case DATA_TYPE_USER: return new ObjectNameUserLetterOverview(params.page);
					case DATA_TYPE_GROUP: return new GroupLetterOverview(params.page);
					case DATA_TYPE_ROLE: return new RoleLetterOverview(params.page);
					}
				}
				break;
			case AR_STRUCT_ITEM_XML_SCHEMA: return new SchemaLetterOverview(params.page);
			case AR_STRUCT_ITEM_XML_ACTIVE_LINK: return new ObjectNameActiveLinkLetterOverview(params.page);
			case AR_STRUCT_ITEM_XML_FILTER: return new ObjectNameFilterLetterOverview(params.page);
			case AR_STRUCT_ITEM_XML_ESCALATION: return new ObjectNameEscalationLetterOverview(params.page);
			case AR_STRUCT_ITEM_XML_CHAR_MENU: return new ObjectNameMenuLetterOverview(params.page);
			case AR_STRUCT_ITEM_XML_CONTAINER:
				{
					switch (params.uint2)
					{
					case ARCON_GUIDE: return new ObjectNameALGuideLetterOverview(params.page);
					case ARCON_APP: return new ObjectNameApplicationLetterOverview(params.page);
					case ARCON_FILTER_GUIDE: return new ObjectNameFilterGuideLetterOverview(params.page);
					case ARCON_PACK: return new ObjectNamePackingListLetterOverview(params.page);
					case ARCON_WEBSERVICE: return new ObjectNameWebserviceLetterOverview(params.page);
					}
				}
				break;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
			case AR_STRUCT_ITEM_XML_IMAGE: return new ObjectNameImageLetterOverview(params.page);
#endif
		}
	}

	assert(false); /// whoops, this shouldn't happen ... maybe a needed implementation is missing!
	return NULL;
}

////////////////////////////////////////////////////////////////////
// namespace and file end
}} // end of namespace OUTPUT::FileNamingDefault
////////////////////////////////////////////////////////////////////

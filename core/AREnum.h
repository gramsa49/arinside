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

#pragma once
#include "../arapi.h"

#define EnumDefault "Unknown";

class CAREnum
{
public:

	static string TrimType(int nType)
	{
		switch(nType)
		{
		case AR_DVAL_TRIM_NONE: return "Unknown";
		case AR_DVAL_TRIM_LINE: return "Line";
		case AR_DVAL_TRIM_SHAPE: return "Shape";
		case AR_DVAL_TRIM_TEXT: return "Multi-Row text";
		case AR_DVAL_TRIM_IMAGE: return "Image";
		default: return EnumDefault;
		}
	}

	static string ControlType(int bMaskIn)
	{
		stringstream strm;
		strm.str("Control");

		try
		{
			unsigned int bitmask[10] = { 1, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7, 1<<8, 1<<9};
			char executeText[10][30] = { "Button", "Menu", "Toolbar", "Tab Switch", "Url", "Chart", "Meter", "Horiz-Nav", "Vert-Nav", "Nav-Item"};

			for (unsigned int k= 0; k < 10; k++)
			{
				if ( (bMaskIn & bitmask[k]) != 0)
				{				
					strm.str("");
					strm << executeText[k];
				}
			}	
		}
		catch(...)
		{
			cout << "EXCEPTION in Filter GetExecuteOn" << endl; 
		}
		return strm.str();
	}

	static string Operand(int nType)
	{
		switch(nType)
		{
		case AR_ARITH_OP_ADD: return " + ";					
		case AR_ARITH_OP_SUBTRACT: return " - ";					
		case AR_ARITH_OP_MULTIPLY: return " * ";
		case AR_ARITH_OP_DIVIDE: return " / ";
		case AR_ARITH_OP_MODULO: return " % ";
		case AR_ARITH_OP_NEGATE: return " -";
		default: return EnumDefault;
		}
	}

	static string Function(int nType)
	{
		switch(nType)
		{
		case AR_FUNCTION_DATE: return "DATE";
		case AR_FUNCTION_TIME: return "TIME";
		case AR_FUNCTION_MONTH: return "MONTH";
		case AR_FUNCTION_DAY: return "DAY";
		case AR_FUNCTION_YEAR: return "YEAR";
		case AR_FUNCTION_WEEKDAY: return "WEEKDAY";
		case AR_FUNCTION_HOUR: return "HOUR";
		case AR_FUNCTION_MINUTE: return "MINUTE";
		case AR_FUNCTION_SECOND: return "SECOND";
		case AR_FUNCTION_TRUNC: return "TRUNC";
		case AR_FUNCTION_ROUND: return "ROUND";
		case AR_FUNCTION_CONVERT: return "CONVERT";
		case AR_FUNCTION_LENGTH: return "LENGTH";
		case AR_FUNCTION_UPPER: return "UPPER";
		case AR_FUNCTION_LOWER: return "LOWER";
		case AR_FUNCTION_SUBSTR: return "SUBSTR";
		case AR_FUNCTION_LEFT: return "LEFT";
		case AR_FUNCTION_RIGHT: return "RIGHT";
		case AR_FUNCTION_LTRIM: return "LTRIM";
		case AR_FUNCTION_RTRIM: return "RTRIM";
		case AR_FUNCTION_LPAD: return "LPAD";
		case AR_FUNCTION_RPAD: return "RPAD";
		case AR_FUNCTION_REPLACE: return "REPLACE";
		case AR_FUNCTION_STRSTR: return "STRSTR";
		case AR_FUNCTION_MIN: return "MIN";
		case AR_FUNCTION_MAX: return "MAX";
		case AR_FUNCTION_COLSUM: return "COLSUM";
		case AR_FUNCTION_COLCOUNT: return "COLCOUNT";
		case AR_FUNCTION_COLAVG: return "COLAVR";
		case AR_FUNCTION_COLMIN: return "COLMIN";
		case AR_FUNCTION_COLMAX: return "COLMAX";
		case AR_FUNCTION_DATEADD: return "DATEADD";
		case AR_FUNCTION_DATEDIFF: return "DATEDIFF";
		case AR_FUNCTION_DATENAME: return "DATENAME";
		case AR_FUNCTION_DATENUM: return "DATENUM";
		case AR_FUNCTION_CURRCONVERT: return "CURRCONVERT";
		case AR_FUNCTION_CURRSETDATE: return "CURRSETDATE";
		case AR_FUNCTION_CURRSETTYPE: return "CURRSETTYPE";
		case AR_FUNCTION_CURRSETVALUE: return "CURRSETTVALUE";
		case AR_FUNCTION_LENGTHC: return "LENGTH";
		case AR_FUNCTION_LEFTC: return "LEFTC";
		case AR_FUNCTION_RIGHTC: return "RIGHTC";
		case AR_FUNCTION_LPADC: return "LPADC";
		case AR_FUNCTION_RPADC: return "RPADC";
		case AR_FUNCTION_STRSTRC: return "STRSTRC";
		case AR_FUNCTION_SUBSTRC: return "SUBSTRC";
		case AR_FUNCTION_ENCRYPT: return "ENCRYPT";
		case AR_FUNCTION_DECRYPT: return "DECRYPT";
		default: return EnumDefault;
		}
	}

	static string FieldPropertiesLabel(int nType)
	{
		switch(nType)
		{
		case AR_OPROP_RESERVED: return "Reserved";   
		case AR_OPROP_VENDOR_NAME: return "Vendor Name";
		case AR_OPROP_VENDOR_PRODUCT: return "Vendor Product";
		case AR_OPROP_VENDOR_VERSION: return "Vendor Version";
		case AR_OPROP_GUID: return "Guid";
		case AR_OPROP_COPYRIGHT: return "Copyright";
		case AR_OPROP_SCC_LOCKED_BY: return "SCC Locked By";
		case AR_OPROP_SCC_VERSION: return "SCC Version";
		case AR_OPROP_SCC_TIMESTAMP: return "SCC Timestamp";
		case AR_OPROP_SCC_USER: return "SCC User";
		case AR_OPROP_SCC_LOCATION: return "SCC Location";
		case AR_OPROP_SCC_DATA_LOCKED_BY: return "SCC Locked By";
		case AR_OPROP_SCC_DATA_VERSION: return "SCC Version";
		case AR_OPROP_SCC_DATA_TIMESTAMP: return "SCC Timestamp";
		case AR_OPROP_SCC_DATA_USER: return "SCC User";
		case AR_OPROP_SCC_DATA_LOCATION: return "SCC Location";
		case AR_OPROP_WINDOW_OPEN_IF_SAMPLE_SERVER_SCHEMA: return "Sample Server schema";
		case AR_OPROP_WINDOW_OPEN_ELSE_SAMPLE_SERVER_SCHEMA: return "Sample Server Schema";
		case AR_OPROP_FORM_NAME_WEB_ALIAS: return "Web Alias";
		case AR_OPROP_VIEW_LABEL_WEB_ALIAS: return "Web Alias";
		case AR_OPROP_APP_WEB_ALIAS: return "Web Alias";
		case AR_OPROP_INTERVAL_VALUE: return "Interval Value";
		case AR_OPROP_INTEGRITY_KEY: return "Integrity Key";
		case AR_OPROP_NEXT_ID_BLOCK_SIZE: return "Next ID block size";
		case AR_OPROP_GUIDE_PARAMETERS: return "Guide Parameters";
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
		case AR_OPROP_CACHE_DISP_PROP: return "Cache Display Prop";
#endif
		case AR_DPROP_NONE: return "None";
		case AR_DPROP_TRIM_TYPE: return "Trim Type";
		case AR_DPROP_CNTL_TYPE: return "Control Type";
		case AR_DPROP_BBOX: return "BBox";
		case AR_DPROP_VISIBLE: return "Visible";
		case AR_DPROP_ENABLE: return "Enabled";
		case AR_DPROP_HELP: return "Help";
		case AR_DPROP_Z_ORDER: return "Z Order";
		case AR_DPROP_COLOR_FILL: return "Color Fill";
		case AR_DPROP_DEPTH_EFFECT: return "Depth Effect";
		case AR_DPROP_DEPTH_AMOUNT: return "Depth Amount";
		case AR_DPROP_COLOR_LINE: return "Color Line";
		case AR_DPROP_COLOR_TEXT: return "Color Text";
		case AR_DPROP_PROMPT: return "Prompt";
		case AR_DPROP_HIDE_WEBHELP: return "Hide Webhelp";
		case AR_DPROP_LABEL: return "Label";
		case AR_DPROP_LABEL_BBOX : return "Label BBox";
		case AR_DPROP_LABEL_FONT_STYLE: return "Label Font Style";
		case AR_DPROP_LABEL_FONT_SIZE: return "Label Font Size";
		case AR_DPROP_LABEL_COLOR_TEXT: return "Label Color Text";
		case AR_DPROP_LABEL_JUSTIFY: return "Label Justify";
		case AR_DPROP_LABEL_ALIGN : return "Label Align";
		case AR_DPROP_LABEL_POS_SECTOR: return "Label Pos Sector";
		case AR_DPROP_LABEL_POS_JUSTIFY: return "Label Pos Justify";
		case AR_DPROP_LABEL_POS_ALIGN: return "Label Pos Align";
		case AR_DPROP_LABEL_COLOR_FILL: return "Label Color Fill";
		case AR_DPROP_LABEL_COLOR_LINE: return "Label Color Line";
		case AR_DPROP_COORDS: return "Coords";
		case AR_DPROP_LINE_WIDTH : return "Line Width";
		case AR_DPROP_LINE_PATTERN: return "Line Pattern";
		case AR_DPROP_JOINT_STYLE: return "Joint Style";
		case AR_DPROP_ENDCAP_START: return "Endcap Start";
		case AR_DPROP_ENDCAP_END: return "Endcap End";
		case AR_DPROP_DATA_ROWS: return "Data Rows";
		case AR_DPROP_DATA_COLS: return "Data Columns";
		case AR_DPROP_DATA_SPIN: return "Data Spin";
		case AR_DPROP_DATA_MENU: return "Data Menu";
		case AR_DPROP_DATA_RADIO: return "Data Radio";
		case AR_DPROP_DATA_MENU_BBOX: return "Data Menu BBox";
		case AR_DPROP_DATA_EXPAND_BBOX: return "Data Expand BBox";
		case AR_DPROP_CHARFIELD_DISPLAY_TYPE: return "Char Field Display Type";
		case AR_DPROP_TEXT: return "Text";
		case AR_DPROP_TEXT_FONT_STYLE: return "Text Font Style";
		case AR_DPROP_TEXT_FONT_SIZE: return "Text Font Size";
		case AR_DPROP_HTML_TEXT: return "Html Text";
		case AR_DPROP_HTML_TEXT_COLOR: return "Html Text Color";
		case AR_DPROP_JUSTIFY: return "Justify";
		case AR_DPROP_ALIGN: return "Align";
		case AR_DPROP_IMAGE: return "Image";
		case AR_DPROP_PUSH_BUTTON_IMAGE: return "Push Button Image";
		case AR_DPROP_BUTTON_TEXT: return "Button Text";
		case AR_DPROP_BUTTON_2D: return "Button 2D";
		case AR_DPROP_BUTTON_IMAGE_POSITION: return "Button Image Position";
		case AR_DPROP_BUTTON_SCALE_IMAGE: return "Button Scale Image";
		case AR_DPROP_BUTTON_MAINTAIN_RATIO: return "Button Maintain Ratio";
		case AR_DPROP_MENU_TEXT: return "Menu Text";
		case AR_DPROP_MENU_POS: return "Menu Pos";
		case AR_DPROP_MENU_MODE: return "Menu Mode";
		case AR_DPROP_MENU_PARENT: return "Menu Parent";
		case AR_DPROP_MENU_HELP: return "Menu Help";
		case AR_DPROP_TOOLTIP: return "Tooltip";
		case AR_DPROP_TOOLBAR_POS: return "Toobar Position";
		case AR_DPROP_TOOLBAR_MODE: return "Toolbar Mode";
		case AR_DPROP_TOOLBAR_TEXT: return "Toolbar Text";
		case AR_DPROP_TAB_MODE: return "Tab Mode";
		case AR_DPROP_TAB_COORD: return "Tab Coords";
		case AR_DPROP_TAB_TEXT: return "Tab Text";
		case AR_DPROP_TAB_ORDER: return "Tab Order";
		case AR_DPROP_DATETIME_POPUP: return "DateTime Popup";
		case AR_DPROP_BACKGROUND_MODE: return "Background Mode";
		case AR_DPROP_TAB_NEXT: return "Tab Next";
		case AR_DPROP_DATA_BBOX: return "Data BBox";
		case AR_DPROP_VIEW_GRID_BBOX: return "View Grid BBox";
		case AR_DPROP_VUI_DEFAULT: return "Vui Default";
		case AR_DPROP_PANE_LAYOUT: return "Pane Layout";
		case AR_DPROP_DETAIL_PANE_VISIBILITY: return "Detail Pane Visibility";
		case AR_DPROP_PROMPT_PANE_VISIBILITY: return "Prompt Pane Visibility";
		case AR_DPROP_RESULT_PANE_VISIBILITY: return "Result Pane Visibility";
		case AR_DPROP_DETAIL_PANE_COLOR: return "Detail Pane Color";
		case AR_DPROP_DETAIL_PANE_IMAGE: return "Detail Pane Image";
		case AR_DPROP_IMAGE_ALIGN: return "Image Align";
		case AR_DPROP_IMAGE_JUSTIFY: return "Image Justify";
		case AR_DPROP_DISPLAY_PARENT: return "Display Parent";
		case AR_DPROP_PAGE_ORDER: return "Page Order";
		case AR_DPROP_PAGE_LABEL_DISPLAY : return "Page Label Display";
		case AR_DPROP_PAGE_ARRANGEMENT: return "Page Arrangement";
		case AR_DPROP_DEFAULT_PAGE: return "Default Page";
		case AR_DPROP_TITLE_BAR_ICON_IMAGE: return "Title Bar Icon Image";
		case AR_DPROP_DETAIL_PANE_WIDTH : return "Detail Pane Width";
		case AR_DPROP_DETAIL_PANE_HEIGHT: return "Detail Pane Height";
		case AR_DPROP_DETAIL_BANNER_VISIBILITY: return "Detail Banner Visibility";
		case AR_DPROP_PROMPT_BANNER_VISIBILITY: return "Prompt Banner Visibility";
		case AR_DPROP_RESULT_BANNER_VISIBILITY: return "result Banner Visibility";
		case AR_DPROP_ALIAS_SINGULAR: return "Alias Singular";
		case AR_DPROP_ALIAS_PLURAL: return "Alias Plural";
		case AR_DPROP_ALIAS_SHORT_SINGULAR: return "Alias Short Singular";
		case AR_DPROP_ALIAS_SHORT_PLURAL: return "Alias Short Plural";
		case AR_DPROP_ALIAS_ABBREV_SINGULAR: return "Alias Abbrev Singular";
		case AR_DPROP_ALIAS_ABBREV_PLURAL: return "Alias Abbrev  Plural";
		case AR_DPROP_NAMED_SEARCHES: return "Named Seraches";
		case AR_DPROP_MENU_ACCESS: return "Menu Access";
		case AR_DPROP_PANE_VISIBILITY_OPTION: return "Pane Visibility Option";
		case AR_DPROP_REQUEST_IDENTIFIER: return "Request Identifier";
		case AR_DPROP_QUERY_LIST_COLOR: return "Query List Color";
		case AR_DPROP_COLUMN_WIDTH: return "Column Width";
		case AR_DPROP_COLUMN_ORDER: return "Column Order";
		case AR_DPROP_SORT_SEQ: return "Sort Sequence";
		case AR_DPROP_SORT_DIR: return "Sort Direction";
		case AR_DPROP_DRILL_DOWN: return "Drilldown";
		case AR_DPROP_REFRESH: return "Refresh";
		case AR_DPROP_AUTO_REFRESH: return "Auto Refresh";
		case AR_DPROP_AUTOFIT_COLUMNS: return "Autofit Columns";
		case AR_DPROP_APPLY_DIRTY: return "Apply Dirty";
		case AR_DPROP_IMAGE_CACHE: return "Image Cache";
		case AR_DPROP_ENUM_LABELS: return "Enum Labels";
		case AR_DPROP_MANAGE_EXPAND_BOX: return "Manage Expand Box";
		case AR_DPROP_ATTACH_ADD_LABEL: return "Attachment Add Label";
		case AR_DPROP_ATTACH_DELETE_LABEL: return "Attachment Delete Label";
		case AR_DPROP_ATTACH_DISPLAY_LABEL: return "Attachmet Display Label";
		case AR_DPROP_ATTACH_SAVE_LABEL: return "Attachment Save Label";
		case AR_DPROP_ATTACH_LABEL_TITLE: return "Attachment Label Title";
		case AR_DPROP_ATTACH_FILENAME_TITLE: return "Attachment Filename Title";
		case AR_DPROP_ATTACH_FILESIZE_TITLE: return "Attachment Filesize Title";
		case AR_DPROP_HIDE_PAGE_TABS_BORDERS: return "Hide Page Tabs Order";
		case AR_DPROP_DISPLAY_AS_TEXT_ONLY: return "Display as Text Only";
		case AR_DPROP_AR_OBJECT_NAME: return "ARSystem Object Name";
		case AR_DPROP_DISPLAY_FIELD_APP: return "Display Field Application";
		case AR_DPROP_ZERO_SIZE_WHEN_HIDDEN: return "Zero Size when Hidden";
		case AR_DPROP_ACCESSIBLE_HINT: return "Accessible Hint";
		case AR_DPROP_INITIAL_CURRENCY_TYPE: return "Initial Currency Type";
		case AR_DPROP_AUTO_FIELD_COLPROP: return "Auto Field Color Property";
		case AR_DPROP_AUTO_FIELD_ROWNUM: return "Auto Field RowNum";
		case AR_DPROP_AUTO_FIELD_ROWPART: return "Auto Field RowPart";
		case AR_DPROP_AUTO_FIELD_ORDER: return "Auto Field Order";
		case AR_DPROP_AUTO_FIELD_TYPE: return "Auto Field Type";
		case AR_DPROP_AUTO_LAYOUT : return "Auto Layout";
		case AR_DPROP_AUTO_LAYOUT_VUI_NAV: return "Auto Layout Vui Nav";
		case AR_DPROP_AUTO_LAYOUT_STYLE_SHEET: return "Auto Layout Style Sheet";
		case AR_DPROP_AUTO_FIELD_NAVPROP: return "Auto Field Nav Property";
		case AR_DPROP_AUTO_FIELD_SPACER: return "Auto Field Spacer";
		case AR_DPROP_FORMACTION_FIELDS: return "Formaction Fields";
		case AR_DPROP_AUTO_SET_OVERLAP_FIELD: return "Auto Set Overlap Field";
		case AR_DPROP_AR_SERVER_NAME: return "ARSystem Server Name";
		case AR_DPROP_AUTO_FIELD_ALIGN: return "Auto Field Align";
		case AR_DPROP_FORMACTION_PAGE_PROPERTIES: return "Form Action Page Properties";
		case AR_DPROP_FORMACTION_FLDS_EXCLUDE: return "Form Action Fields Exclude";
		case AR_DPROP_AUTO_FIELD_NEW_COLUMN : return "Auto Field new Column";
		case AR_DPROP_AUTO_FIELD_NEW_SECTION: return "Auto Field new Section";
		case AR_DPROP_VUI_DEFAULT_PROCESS: return "Vui Default Process";
		case AR_DPROP_WEB_HEADER_CONTENT: return "Web Header Content";
		case AR_DPROP_WEB_FOOTER_CONTENT: return "Web Footer Content";
		case AR_DPROP_PATH_TO_BKG_IMAGE: return "Path to Background Image";
		case AR_DPROP_WEB_TOOLBAR_VISIBILITY: return "Web Toobar Visibility";
		case AR_DPROP_AR_GRAPH_PLUGIN_NAME: return "Graph Plugin Name";
		case AR_DPROP_EXPAND_COLLAPSE_TREE_LEVELS: return "Expand/Collapse Tree Levels";
		case AR_DPROP_PREFIX_NEW: return "Prefix New";
		case AR_DPROP_PREFIX_SEARCH: return "Prefix Search";
		case AR_DPROP_PREFIX_MODIFY: return "Prefix Modify";
		case AR_DPROP_PREFIX_MODIFY_ALL: return "Prefix Modify All";
		case AR_DPROP_PREFIX_DISPLAY: return "Prefix Display";
		case AR_DPROP_PREFIX_MATCHING_REQ: return "Prefix Matching Request";
		case AR_DPROP_PREFIX_NO_MATCHING_REQ: return "Prefix No Matching Request";		
		case AR_DPROP_TABLE_DISPLAY_TYPE: return "Table Display Type";
		case AR_DPROP_TABLE_SELINIT: return "Table Select Init";
		case AR_DPROP_TABLE_SELREFRESH: return "Table Select Refresh";
		case AR_DPROP_TABLE_CHUNK_SIZE: return "Table Chunk Size";
		case AR_DPROP_TABLE_CHUNK_NEXT : return "Table Chunk Next";
		case AR_DPROP_TABLE_CHUNK_PREV : return "Table Chung Prev";
		case AR_DPROP_TABLE_NOT_REFRESHED: return "Table not Refreshed";
		case AR_DPROP_TABLE_ENTRIES_RETURNED: return "Table Entries Returned";
		case AR_DPROP_TABLE_AUTOREFRESH: return "Table Auto Refresh";
		case AR_DPROP_TABLE_DRILL_COL: return "Table Drilldown Column";
		case AR_DPROP_TABLE_SELROWS_DISABLE: return "Table Selected Rows Disable";
		case AR_DPROP_TABLE_SELECT_ALL: return "Table Select All";
		case AR_DPROP_TABLE_DESELECT_ALL: return "Table Deselect All";
		case AR_DPROP_TABLE_REFRESH : return "Table Refresh";
		case AR_DPROP_TABLE_REPORT: return "Table Report";
		case AR_DPROP_TABLE_DELETE: return "Table Delete";
		case AR_DPROP_TABLE_READ : return "Table Read";
		case AR_DPROP_TABLE_UNREAD: return "Table Unread";
		case AR_DPROP_TABLE_SELECTIONCOLUMN_LABEL: return "Table Selection Column Label";
		case AR_DPROP_TABLE_COL_DISPLAY_TYPE: return "Table Column Display Type";
		case AR_DPROP_TABLE_COL_INITVAL: return "Table Column Init. Value";
		case AR_DPROP_FIXED_TABLE_HEADERS : return "Fixed Table Headers";
		case AR_DPROP_VIEWFIELD_SCROLLBARS: return "View Field Scrollbards";
		case AR_DPROP_VIEWFIELD_BORDERS: return "Viewfield Borders";
		case AR_DPROP_ENTRYPOINT_LABEL_DEFAULT_NEW : return "Entrypoint Label Default New";
		case AR_DPROP_ENTRYPOINT_LABEL_DEFAULT_SEARCH: return "Entrypoint Label Default Search";
		case AR_DPROP_TABLE_COL_WRAP_TEXT: return "Table Col Wrap Text";
		case AR_DPROP_TABLE_PREFERENCES: return "Table Preferences";
		case AR_DPROP_FIELD_CUSTOMSTYLE: return "Field Custom Style";
		case AR_DPROP_TABLE_TREE_CUSTOM_NULL_VALUE: return "Table Tree Custom Null Value";
		case AR_DPROP_NAVBAR_INITIAL_SELECTED_ITEM: return "Navbar Initial Selected Item";
		case AR_DPROP_NAVBAR_WORKFLOW_ON_SELECTED_ITEM : return "Bavbar Workflow on Selected Item";
		case AR_DPROP_NAVBAR_SELECT_ITEM_ON_CLICK: return "Navbar Select Item on Click";
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
		case AR_DPROP_BUTTON_ALT_TEXT: return "Button Alt Text";
		case AR_DPROP_TABLE_USE_LOCALE: return "Use Locale";
		case AR_DPROP_QUERY_LIST_BKG_COLOR: return "Query List Background Color";
		case AR_DPROP_AUTO_MAXIMIZE_WINDOW: return "Auto Maximize Window";
#endif
		default: 
#ifdef _DEBUG
			stringstream tmpLabel; tmpLabel.str(""); tmpLabel << "Unknown (" << nType << ")"; return tmpLabel.str();
#else
			return EnumDefault;
#endif

		}
	}


	static string FieldPropertiesValue(int nProp, int nVal)
	{
		switch(nProp)
		{

		case AR_DPROP_NAVBAR_WORKFLOW_ON_SELECTED_ITEM:
			{
				switch(nVal)
				{
				case AR_DVAL_NAVBAR_SELITEM_NOFIRE: return "Do Not Fire Workflow";
				case AR_DVAL_NAVBAR_SELITEM_FIRE: return "Fire Workflow";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_TABLE_COL_WRAP_TEXT:
			{
				switch(nVal)
				{
				case AR_DVAL_TABLE_COL_WRAP_TEXT_DISABLE: return "Disable";
				case AR_DVAL_TABLE_COL_WRAP_TEXT_ENABLE: return "Enable";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_VIEWFIELD_BORDERS:
			{
				switch(nVal)
				{
				case AR_DVAL_VIEWFIELD_BORDERS_DEFAULT: return "Default";
				case AR_DVAL_VIEWFIELD_BORDERS_NONE: return "None";
				case AR_DVAL_VIEWFIELD_BORDERS_ENABLE: return "Enable";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_VIEWFIELD_SCROLLBARS:
			{
				switch(nVal)
				{
				case AR_DVAL_VIEWFIELD_SCROLLBARS_AUTO: return "Auto";
				case AR_DVAL_VIEWFIELD_SCROLLBARS_ON: return "On";
				case AR_DVAL_VIEWFIELD_SCROLLBARS_HIDDEN: return "Hidden";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_FIXED_TABLE_HEADERS:
			{
				switch(nVal)
				{
				case AR_DVAL_FIXED_TABLE_HEADERS_DISABLE: return "Disabled";
				case AR_DVAL_FIXED_TABLE_HEADERS_ENABLE: return "Enabled";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_TABLE_COL_DISPLAY_TYPE:
			{
				switch(nVal)
				{
				case AR_DVAL_TABLE_COL_DISPLAY_NONEDITABLE: return "Not editable";
				case AR_DVAL_TABLE_COL_DISPLAY_EDITABLE: return "Editable";
				case AR_DVAL_TABLE_COL_DISPLAY_HTML: return "Display Html in Column";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_TABLE_SELROWS_DISABLE:
			{
				switch(nVal)
				{
				case AR_DVAL_TABLE_SELROWS_MULTI_SELECT: return "Multiple Selection";
				case AR_DVAL_TABLE_SELROWS_DISABLE_YES: return "Disable Selection";
				case AR_DVAL_TABLE_SELROWS_SINGLE_SELECT: return "Single Select";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_TABLE_ENTRIES_RETURNED:
			{
				switch(nVal)
				{
				case AR_DPROP_TABLE_AUTOREFRESH: return "Auto Refresh";
				case AR_DPROP_TABLE_DRILL_COL: return "Drill Down";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_TABLE_SELREFRESH:
			{
				switch(nVal)
				{
				case AR_DVAL_TABLE_SELREFRESH_RETFIRE: return " Retain Select, Fire Workflow";
				case AR_DVAL_TABLE_SELREFRESH_RETNOFIRE: return "Retain Select, No Workflow";
				case AR_DVAL_TABLE_SELREFRESH_FIRSTFIRE: return "Select 1st, Fire Workflow";
				case AR_DVAL_TABLE_SELREFRESH_FIRSTNOFIRE: return "Select 1st, No Workflow";
				case AR_DVAL_TABLE_SELREFRESH_NOSEL: return "No Selection, No Workflow";			
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_TABLE_SELINIT:
			{
				switch(nVal)
				{
				case AR_DVAL_TABLE_SELINIT_SELFIRE: return "Select 1st, Fire Workflow";
				case AR_DVAL_TABLE_SELINIT_SELNOFIRE: return "Select 1st, No Workflow";
				case AR_DVAL_TABLE_SELINIT_NOSEL: return "No Select, No Workflow";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_TABLE_DISPLAY_TYPE:
			{
				switch(nVal)
				{
				case AR_DVAL_TABLE_DISPLAY_TABLE: return "Table";
				case AR_DVAL_TABLE_DISPLAY_RESULTS_LIST: return "Result List";
				case AR_DVAL_TABLE_DISPLAY_NOTIFICATION: return "Notification";
				case AR_DVAL_TABLE_DISPLAY_SINGLE_TABLE_TREE: return "Single Table Tree";
				case AR_DVAL_TABLE_DISPLAY_MULTI_TABLE_TREE: return "Multi Table Tree";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_EXPAND_COLLAPSE_TREE_LEVELS:
			{
				switch(nVal)
				{
				case AR_DVAL_EXPAND_ALL_LEVELS: return "Expand All";
				case AR_DVAL_COLLAPSE_ALL_LEVELS: return "Collapse All";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_AUTO_FIELD_NEW_SECTION:
			{
				switch(nVal)
				{
				case AR_DVAL_AUTO_FIELD_NEW_SECTION_OFF: return "Off";
				case AR_DVAL_AUTO_FIELD_NEW_SECTION_ON: return "On";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_AUTO_FIELD_NEW_COLUMN:
			{
				switch(nVal)
				{
				case AR_DVAL_AUTO_FIELD_NEW_COLUMN_OFF: return "Off";
				case AR_DVAL_AUTO_FIELD_NEW_COLUMN_ON: return "On";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_FORMACTION_FLDS_EXCLUDE:
			{
				switch(nVal)
				{
				case AR_DVAL_FORMACTION_FLDS_EXCLUDE_OFF: return "Off";
				case AR_DVAL_FORMACTION_FLDS_EXCLUDE_ON: return "On";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_AUTO_FIELD_ALIGN:
			{
				switch(nVal)
				{
				case AR_DVAL_AUTO_FIELD_ALIGN_LEFT: return "Left";
				case AR_DVAL_AUTO_FIELD_ALIGN_RIGHT: return "Right";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_AUTO_FIELD_SPACER:
			{
				switch(nVal)
				{
				case AR_DVAL_AUTO_FIELD_SPACER_OFF: return "Off";
				case AR_DVAL_AUTO_FIELD_SPACER_ON: return "On";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_AUTO_FIELD_NAVPROP:
			{
				switch(nVal)
				{
				case AR_DVAL_AUTO_FIELD_LEVEL1: return "Level1";
				case AR_DVAL_AUTO_FIELD_LEVEL2: return "Level2";
				case AR_DVAL_AUTO_FIELD_LEVEL3: return "Level3";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_AUTO_LAYOUT_VUI_NAV:
			{
				switch(nVal)
				{
				case AR_DVAL_AUTO_LAYOUT_VUI_NAV_OFF: return "Off";
				case AR_DVAL_AUTO_LAYOUT_VUI_NAV_ON: return "On";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_AUTO_LAYOUT:
			{
				switch(nVal)
				{
				case AR_DVAL_AUTO_LAYOUT_OFF: return "Off";
				case AR_DVAL_AUTO_LAYOUT_ON: return "On";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_AUTO_FIELD_TYPE:
			{
				switch(nVal)
				{
				case AR_DVAL_AUTO_FIELD_REGULAR: return "Regular";
				case AR_DVAL_AUTO_FIELD_NAV: return "Nav";
				case AR_DVAL_AUTO_FIELD_ACTION: return "Action";
				case AR_DVAL_AUTO_FIELD_GROUPTITLE: return "Group Title";
				case AR_DVAL_AUTO_FIELD_PAGETITLE: return "Page Title";
				case AR_DVAL_AUTO_FIELD_APPTITLE: return "App Title";
				default: return EnumDefault;
				}
			}
			break;		
		case AR_DPROP_AUTOFIT_COLUMNS:
			{
				switch(nVal)
				{
				case AR_DVAL_AUTOFIT_COLUMNS_NONE: return "None";
				case AR_DVAL_AUTOFIT_COLUMNS_SET: return "Set";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_REFRESH :
			{
				switch(nVal)
				{
				case AR_DVAL_REFRESH_NONE: return "None";
				case AR_DVAL_REFRESH_TABLE_MAX: return "Refresh";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_DRILL_DOWN:
			{
				switch(nVal)
				{                                          
				case AR_DVAL_DRILL_DOWN_NONE: return "None";
				case AR_DVAL_DRILL_DOWN_ENABLE: return "Enable";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_SORT_DIR:
			{
				switch(nVal)
				{                                         
				case AR_DVAL_SORT_DIR_ASCENDING: return "Ascending";
				case AR_DVAL_SORT_DIR_DESCENDING: return "Descending";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_PANE_VISIBILITY_OPTION:
			{
				switch(nVal)
				{
				case AR_DVAL_PANE_VISIBILITY_USER_CHOICE: return "User Choice";
				case AR_DVAL_PANE_VISIBILITY_ADMIN: return "Administrator defined";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_PAGE_ARRANGEMENT:
			{
				switch(nVal)
				{
				case AR_DVAL_PAGE_SCROLL: return "Scroll";
				case AR_DVAL_PAGE_LAYER: return "Layer";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_PAGE_LABEL_DISPLAY:
			{
				switch(nVal)
				{
				case AR_DVAL_PAGE_DISPLAY_TOP: return "Top";
				case AR_DVAL_PAGE_DISPLAY_BOTTOM: return "Bottom";
				case AR_DVAL_PAGE_DISPLAY_LEFT: return "Left";
				case AR_DVAL_PAGE_DISPLAY_RIGHT: return "Right";
				case AR_DVAL_PAGE_DISPLAY_NONE: return "None";
				default: return EnumDefault;
				}

			}
			break;
		case AR_DPROP_DETAIL_PANE_VISIBILITY:
			{
				switch(nVal)
				{
				case AR_DVAL_PANE_ALWAYS_HIDDEN: return "Always Hidden";
				case AR_DVAL_PANE_HIDDEN : return "Hidden";
				case AR_DVAL_PANE_VISIBLE : return "Visible";
				case AR_DVAL_PANE_ALWAYS_VISIBLE: return "Always Visible";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_BACKGROUND_MODE:
			{
				switch(nVal)
				{
				case AR_DVAL_BKG_MODE_OPAQUE: return "Default";
				case AR_DVAL_BKG_MODE_TRANSPARENT: return "Transparent";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_DATETIME_POPUP:
			{
				switch(nVal)
				{
				case AR_DVAL_DATETIME_BOTH: return "Time and Date";
				case AR_DVAL_DATETIME_TIME: return "Time Only";
				case AR_DVAL_DATETIME_DATE: return "Date Only";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_MENU_MODE:
			{
				switch(nVal)
				{
				case AR_DVAL_CNTL_ITEM: return "Item";
				case AR_DVAL_CNTL_ON: return "On";
				case AR_DVAL_CNTL_SEPARATOR: return "Separator";
				case AR_DVAL_CNTL_CHOICE: return "Choice";
				case AR_DVAL_CNTL_DIALOG: return "Dialog";
				case AR_DVAL_CNTL_A_MENU: return "Menu";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_BUTTON_IMAGE_POSITION:
			{
				switch(nVal)
				{
				case AR_DVAL_IMAGE_CENTER: return "Center";
				case AR_DVAL_IMAGE_LEFT: return "Left";
				case AR_DVAL_IMAGE_RIGHT: return "Right";
				case AR_DVAL_IMAGE_ABOVE: return "Above";
				case AR_DVAL_IMAGE_BELOW: return "Below";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_LABEL_POS_SECTOR:
			{				
				stringstream tmp;
				tmp.str("");

				unsigned int bitmask[6] = { 1, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5  };
				char sectText[6][30] = { "None", "Center", "North", "East", "South", "West"};
				for (unsigned int k= 0; k < 6; k++)
				{
					if ( (nVal & bitmask[k]) != 0)
					{				
						tmp << sectText[k];
					}
				}	

				return tmp.str();
			}
			break;
		case AR_DPROP_CHARFIELD_DISPLAY_TYPE:
			{
				switch(nVal)
				{
				case AR_DVAL_CHARFIELD_EDIT: return "Edit";
				case AR_DVAL_CHARFIELD_DROPDOWN: return "Dropdown";
				case AR_DVAL_CHARFIELD_MASKED: return "Masked";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_DATA_RADIO:
			{
				switch(nVal)
				{
				case AR_DVAL_RADIO_DROPDOWN: return "Dropdown";
				case AR_DVAL_RADIO_RADIO: return "Radio";
				case AR_DVAL_RADIO_CHECKBOX: return "Checkbox";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_ENDCAP_END:
			{
				switch(nVal)
				{
				case AR_DVAL_ENDCAP_ROUND: return "Rounded";
				case AR_DVAL_ENDCAP_FLUSH: return "Flush";
				case AR_DVAL_ENDCAP_EXTENDED: return "Extended";
				case AR_DVAL_ENDCAP_ARROW1: return "Arrow1";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_JOINT_STYLE:
			{
				switch(nVal)
				{
				case AR_DVAL_JOINT_EXTENDED: return "Extended";
				case AR_DVAL_JOINT_SHARP: return "Sharp";
				case AR_DVAL_JOINT_ROUNDED: return "Rounded";
				case AR_DVAL_JOINT_SMOOTH: return "Smooth";
				case AR_DVAL_JOINT_MAX_SMOOTH: return "Max. Smooth";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_ALIGN:
		case AR_DPROP_LABEL_POS_ALIGN:
		case AR_DPROP_LABEL_ALIGN:
			{
				switch(nVal)
				{
				case AR_DVAL_ALIGN_DEFAULT: return "Default";
				case AR_DVAL_ALIGN_TOP: return "Top";
				case AR_DVAL_ALIGN_MIDDLE: return "Middle";
				case AR_DVAL_ALIGN_FILL: return "Fill";
				case AR_DVAL_ALIGN_BOTTOM : return "Bottom";
				case AR_DVAL_ALIGN_TILE: return "Tile";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_JUSTIFY:
		case AR_DPROP_LABEL_POS_JUSTIFY:
		case AR_DPROP_LABEL_JUSTIFY:
			{
				switch(nVal)
				{
				case AR_DVAL_JUSTIFY_DEFAULT: return "Default";
				case AR_DVAL_JUSTIFY_LEFT: return "Left"; 
				case AR_DVAL_JUSTIFY_CENTER: return "Center";
				case AR_DVAL_JUSTIFY_FILL: return "Fill";
				case AR_DVAL_JUSTIFY_RIGHT: return "Right";
				case AR_DVAL_JUSTIFY_TILE: return "Tile";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_DEPTH_EFFECT:
			{
				switch(nVal)
				{
				case AR_DVAL_DEPTH_EFFECT_FLAT: return "Flat";
				case AR_DVAL_DEPTH_EFFECT_RAISED: return "Raised";
				case AR_DVAL_DEPTH_EFFECT_SUNKEN: return "Sunken";
				case AR_DVAL_DEPTH_EFFECT_FLOATING: return "Floating";
				case AR_DVAL_DEPTH_EFFECT_ETCHED: return "Etched";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_ENABLE:
			{
				switch(nVal)
				{
				case AR_DVAL_ENABLE_DEFAULT: return "Default";
				case AR_DVAL_ENABLE_READ_ONLY: return "Read Only";
				case AR_DVAL_ENABLE_READ_WRITE: return "Read/Write";
				case AR_DVAL_ENABLE_DISABLE:return "Disabled";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_TRIM_TYPE:
			{
				return TrimType(nVal);
			}
			break;
		case AR_DPROP_MANAGE_EXPAND_BOX:
			{
				switch(nVal)
				{
				case AR_DVAL_EXPAND_BOX_DEFAULT: return "Default";
				case AR_DVAL_EXPAND_BOX_HIDE: return "Hide";
				case AR_DVAL_EXPAND_BOX_SHOW: return "Show";
				default: return EnumDefault;
				}
			}
			break;
		case AR_DPROP_CNTL_TYPE:
			{
				return ControlType(nVal);
			}
			break;
		default: return EnumDefault;
		}
	}


	static string ContainerRefType(int nType)
	{
		switch (nType)
		{
		case ARREF_NONE: return "None";
		case ARREF_ALL: return "All";
		case ARREF_SCHEMA: return "Schema";
		case ARREF_FILTER: return "Filter";
		case ARREF_ESCALATION: return "Escalation";
		case ARREF_ACTLINK: return "Active Link";
		case ARREF_CONTAINER: return "Container";
		case ARREF_CHAR_MENU: return "Char Menu";
		case ARREF_LAST_SERVER_OBJ: return "LastServerObject";			
		case ARREF_ICON: return "Icon";
		case ARREF_SMALL_ICON: return "Small Icon";
		case ARREF_MAXIMIZE_FORMS: return "Maximize Forms";
		case ARREF_APPLICATION_FORMS: return "Forms";
		case ARREF_ABOUT_BOX_IMAGE: return "About Box Image";
		case ARREF_ABOUT_BOX_FORM: return "About Box Form";
		case ARREF_NULL_STRING: return "NULL String";
		case ARREF_APPLICATION_HELP_EXT: return "Ext. Help";
		case ARREF_APPLICATION_HELP_FILE: return "Help File";
		case ARREF_APPLICATION_PRIMARY_FORM: return "Primary Form";
		case ARREF_APPLICATION_FORM_VUI: return "Vui ID";
		case ARREF_APPLICATION_DISABLE_BEGIN_TASK: return "Begin Task";
		case ARREF_APPLICATION_HELP_INDEX_EXT: return "Help Index";
		case ARREF_APPLICATION_HELP_INDEX_FILE: return "Help File Index";
		case ARREF_APPLICATION_HELP_FILE_NAME : return "Help File Name";
		case ARREF_PACKINGLIST_GUIDE: return "Guide";
		case ARREF_PACKINGLIST_APP: return "Application";
		case ARREF_PACKINGLIST_PACK: return "Packing LIst";
		case ARREF_GROUP_DATA: return "Data";
		case ARREF_DISTMAPPING_DATA: return "Data Mapping";
		case ARREF_APPLICATION_HAS_EXT_HELP: return "Use ext. Help";
		case ARREF_APPLICATION_SUPPORT_FILES: return "Support Files";
		case ARREF_PACKINGLIST_DSOPOOL: return "Distr. Pool Schema";
		case ARREF_PACKINGLIST_FILTER_GUIDE: return "Filter Guide";
		case ARREF_FLASH_BOARD_DEF: return "Flashboard";
		case ARREF_FLASH_DATA_SOURCE_DEF: return "Flashboard Datasource";
		case ARREF_FLASH_VARIABLE_DEF: return "Flashboard Variable";
		case ARREF_WS_PROPERTIES: return "Webservice Property";
		case ARREF_WS_OPERATION: return "Webservice Operation";
		case ARREF_WS_ARXML_MAPPING: return "Webservice Mapping";
		case ARREF_WS_WSDL: return "Webservice WSDL";
		case ARREF_PACKINGLIST_WEBSERVICE: return "Webservice";
		case ARREF_WS_PUBLISHING_LOC: return "Webservice Publishing Location";
		case ARREF_APPLICATION_HELP_FILE_NAME2: return "Help File";
		case ARREF_APPLICATION_HELP_EXT2: return "Help Extension";
		case ARREF_APPLICATION_HELP_FILE2: return "Help File";
		case ARREF_APPLICATION_HELP_INDEX_EXT2: return "Help Extension";
		case ARREF_APPLICATION_HELP_INDEX_FILE2: return "Help File Index";
		case ARREF_APPLICATION_HELP_FILE_NAME3: return "Help File Name";
		case ARREF_APPLICATION_HELP_EXT3: return "Help Extension";
		case ARREF_APPLICATION_HELP_FILE3: return "Help File";
		case ARREF_APPLICATION_HELP_INDEX_EXT3: return "Help Extension";
		case ARREF_APPLICATION_HELP_INDEX_FILE3: return "Help File Index";
		case ARREF_APPLICATION_HELP_FILE_NAME4 : return "Help File Name";
		case ARREF_APPLICATION_HELP_EXT4: return "Help Extension";
		case ARREF_APPLICATION_HELP_FILE4: return "Help File";
		case ARREF_APPLICATION_HELP_INDEX_EXT4: return "Help Extension";
		case ARREF_APPLICATION_HELP_INDEX_FILE4: return "Help File Index";
		case ARREF_APPLICATION_HELP_FILE_NAME5: return "Help File Name";
		case ARREF_APPLICATION_HELP_EXT5: return "Help Extension";
		case ARREF_APPLICATION_HELP_FILE5: return "Help File";
		case ARREF_APPLICATION_HELP_INDEX_EXT5: return "Help Extension";
		case ARREF_APPLICATION_HELP_INDEX_FILE5: return "Help File";
		case ARREF_APPLICATION_HELP_LABEL: return "Help Label";
		case ARREF_APPLICATION_HELP_LABEL3: return "Help Label";
		case ARREF_APPLICATION_HELP_LABEL4: return "Help Label";
		case ARREF_APPLICATION_HELP_LABEL5: return "Help Label";
		case ARREF_WS_XML_SCHEMA_LOC: return "Webservice Schema Location";
		case ARREF_ENTRYPOINT_ORDER: return "Entry Point";
		case ARREF_ENTRYPOINT_START_ACTLINK: return "Start Active Link";
		case ARREF_APP_AUTOLAYOUT_SS: return "Stylesheet";
		case ARREF_APP_FORMACTION_FIELDS: return "Form Action Fields";
		case ARREF_ENCAPSULATED_APP_DATA: return "Application Data";
		case ARREF_APP_DEFAULT_OBJ_PERMS: return "Default Permissions";
		case ARREF_APP_ADD_FORMACTION_FIELDS: return "Form Action Fields";
		case ARREF_APP_FORMACTION_RESULTS_LIST_FIXED_HEADER: return "Header Properties";
		case ARREF_APP_FORMACTION_PAGE_PROPERTIES: return "Page Properties";
		case ARREF_APP_OBJECT_VERSION: return "Version";
		case ARREF_APP_PACKING_LISTS: return "Packing List";
		case ARREF_APP_DATA_MERGE_IMP_QUAL: return "Tmp. Merge";
		case ARREF_APP_DATA_MERGE_IMP_OPTION : return "Tmp Option";
		case ARREF_LAST_RESERVED: return "Last Reserved";
		default: return EnumDefault;
		}
	}

	static string SchemaType(int nType)
	{
		switch (nType)
		{
		case AR_SCHEMA_NONE: return "None";
		case AR_SCHEMA_REGULAR: return "Regular";
		case AR_SCHEMA_JOIN: return "Join";
		case AR_SCHEMA_VIEW: return "View";
		case AR_SCHEMA_DIALOG: return "Dialog";
		case AR_SCHEMA_VENDOR: return "Vendor";
		default: return EnumDefault;
		}
	}

	static string ContainerType(int nType)
	{
		switch (nType)
		{
		case ARCON_GUIDE: return "Active Link Guide";
		case ARCON_APP: return "Application";
		case ARCON_PACK: return "Packing List";
		case ARCON_FILTER_GUIDE: return "Filter Guide";
		case ARCON_WEBSERVICE: return "Webservice";
		default: return EnumDefault;
		}
	}

	static string ContainerImage(int nType)
	{
		switch (nType)
		{
		case ARCON_GUIDE: return "al_guide.gif";
		case ARCON_APP: return "application.gif";
		case ARCON_PACK: return "packing_list.gif";
		case ARCON_FILTER_GUIDE: return "filter_guide.gif";
		case ARCON_WEBSERVICE: return "webservice.gif";
		default: return "doc.gif";
		}
	}

	static string ContainerDir(int nType)
	{
		switch (nType)
		{
		case ARCON_GUIDE: return "active_link_guide";
		case ARCON_APP: return "application";
		case ARCON_PACK: return "packing_list";
		case ARCON_FILTER_GUIDE: return "filter_guide";
		case ARCON_WEBSERVICE: return "webservice";
		default: return EnumDefault;
		}
	}

	static string GroupType(int nType)
	{
		switch (nType)
		{
		case AR_GROUP_TYPE_VIEW: return "View";
		case AR_GROUP_TYPE_CHANGE: return "Change";
		default: return "None";
		}
	}

	static string ObjectEnable(int nType)
	{
		switch (nType)
		{
		case 0: return "Disabled";
		case 1: return "Enabled";	
		default: return EnumDefault;
		}
	}

	static string MenuDDLabelFormat(int nType)
	{
		switch(nType)
		{
		case AR_CHAR_MENU_DD_DB_NAME: return "Name";
		case AR_CHAR_MENU_DD_LOCAL_NAME: return "Label";
		case AR_CHAR_MENU_DD_ID: return "ID";
		default: return EnumDefault;
		}
	}

	static string MenuDDValueFormat(int nType)
	{
		switch(nType)
		{
		case AR_CHAR_MENU_DD_FORMAT_NONE: return "None";
		case AR_CHAR_MENU_DD_FORMAT_ID: return "ID";
		case AR_CHAR_MENU_DD_FORMAT_NAME: return "Name";
		case AR_CHAR_MENU_DD_FORMAT_QUOTES: return "'Name'";
		case AR_CHAR_MENU_DD_FORMAT_DOLLARS: return "$Name$";
		case AR_CHAR_MENU_DD_FORMAT_ID_NAME: return "ID;Name";
		case AR_CHAR_MENU_DD_FORMAT_NAMEL: return "Label";
		case AR_CHAR_MENU_DD_FORMAT_QUOTESL: return "'Label'";
		case AR_CHAR_MENU_DD_FORMAT_DOLLARSL: return "$Label$";
		case AR_CHAR_MENU_DD_FORMAT_ID_L : return "ID;Label";
		case AR_CHAR_MENU_DD_FORMAT_NAME_L: return ";Name;Label";
		case AR_CHAR_MENU_DD_FORMAT_L_NAME: return ";Label;Name";
		default: return EnumDefault;
		}
	}

	static string MenuItemType(int nType)
	{
		switch (nType)
		{
		case AR_MENU_TYPE_VALUE: return "Value";
		case AR_MENU_TYPE_MENU: return "Label";
		default: return EnumDefault;
		}
	}

	static string MenuFileLocation(int nType)
	{
		switch (nType)
		{
		case AR_MENU_FILE_SERVER: return "Server";
		case AR_MENU_FILE_CLIENT: return "Client";
		default: return EnumDefault;
		}
	}

	static string MenuType(int nType)
	{
		switch (nType)
		{		
		case AR_CHAR_MENU_NONE: return "None";
		case AR_CHAR_MENU_LIST: return "Character";
		case AR_CHAR_MENU_QUERY: return "Search";
		case AR_CHAR_MENU_FILE: return "File";
		case AR_CHAR_MENU_SQL: return "SQL";
		case AR_CHAR_MENU_SS: return "SS";
		case AR_CHAR_MENU_DATA_DICTIONARY: return "Data Dictionary";
		default: return EnumDefault;		
		}
	}

	static string MenuRefresh(int nType)
	{
		switch (nType)
		{
		case AR_MENU_REFRESH_CONNECT: return "On Connect";
		case AR_MENU_REFRESH_OPEN: return "On Open";
		case AR_MENU_REFRESH_INTERVAL: return "On 15-minute Intervall";
		default: return EnumDefault;
		}
	}

	static string DataType(int nType)
	{
		switch (nType)
		{
		case AR_DATA_TYPE_NULL: return "Null";
		case AR_DATA_TYPE_KEYWORD: return "Keyword";
		case AR_DATA_TYPE_INTEGER: return "Integer"; 
		case AR_DATA_TYPE_REAL: return "Real";
		case AR_DATA_TYPE_CHAR: return "Character";
		case AR_DATA_TYPE_DIARY: return "Diary";
		case AR_DATA_TYPE_ENUM: return "Selection";
		case AR_DATA_TYPE_TIME: return "Date/Time";          
		case AR_DATA_TYPE_BITMASK: return "Bitmask";
		case AR_DATA_TYPE_BYTES: return "Bytes";
		case AR_DATA_TYPE_DECIMAL: return "Decimal";
		case AR_DATA_TYPE_ATTACH: return "Attach";
		case AR_DATA_TYPE_CURRENCY: return "Currency";
		case AR_DATA_TYPE_DATE: return "Date";
		case AR_DATA_TYPE_TIME_OF_DAY: return "Time of Day";
		case AR_DATA_TYPE_JOIN: return "Join";
		case AR_DATA_TYPE_TRIM: return "Trim";
		case AR_DATA_TYPE_CONTROL: return "Control";
		case AR_DATA_TYPE_TABLE: return "Table";
		case AR_DATA_TYPE_COLUMN: return "Column";
		case AR_DATA_TYPE_PAGE: return "Page";
		case AR_DATA_TYPE_PAGE_HOLDER: return "Page Holder";
		case AR_DATA_TYPE_ATTACH_POOL: return "Attach Pool";
		case AR_DATA_TYPE_ULONG: return "Long";
		case AR_DATA_TYPE_COORDS: return "Coords";
		case AR_DATA_TYPE_VIEW: return "View";
		case AR_DATA_TYPE_DISPLAY: return "Display";
		default: return EnumDefault;   
		}
	}

	static string FieldType(int nType)
	{
		switch (nType)
		{
		case AR_FIELD_TYPE_DATA: return "Data";       
		case AR_FIELD_TYPE_TRIM: return "Trim";      
		case AR_FIELD_TYPE_CONTROL: return "Control"; 
		case AR_FIELD_TYPE_PAGE: return "Page";
		case AR_FIELD_TYPE_PAGE_HOLDER: return "Holder";
		case AR_FIELD_TYPE_TABLE: return "Table";
		case AR_FIELD_TYPE_COLUMN: return "Column";
		case AR_FIELD_TYPE_ATTACH: return "Attach";
		case AR_FIELD_TYPE_ATTACH_POOL: return "Attach Pool";
		default: return EnumDefault;   
		}
	}

	static string VuiType(int nType)
	{
		switch (nType)
		{
		case AR_VUI_TYPE_NONE: return "None";
		case AR_VUI_TYPE_STANDARD: return "Standard";
		case AR_VUI_TYPE_WEB: return "Web";
		case AR_VUI_TYPE_WEB_ABS_POS: return "Web (absolute)";
		case AR_VUI_TYPE_WIRELESS: return "Wireless";
		case AR_VUI_TYPE_WEB_AUTOGEN: return "Web (autogen)";
		case AR_VUI_TYPE_WEB_ABS_POS_AUTOGEN: return "Web (absolute/autogen)";
		default: return EnumDefault;
		}
	}

	static string SchemaSortOrder(int nType)
	{
		switch (nType)
		{
		case AR_SORT_ASCENDING: return "Ascending";
		case AR_SORT_DESCENDING: return "Descending";
		default: return EnumDefault;
		}
	}

	static string ObjectPermission(int nType)
	{
		switch (nType)
		{
		case AR_PERMISSIONS_NONE: return "None";
		case AR_PERMISSIONS_VISIBLE: return "Visible";
		case AR_PERMISSIONS_HIDDEN: return "Hidden";
		default: return EnumDefault;
		}
	}

	static string FieldPermission(int nType)
	{
		switch (nType)
		{
		case AR_PERMISSIONS_NONE: return "None";
		case AR_PERMISSIONS_VIEW: return "View";
		case AR_PERMISSIONS_CHANGE: return "Change";
		default: return EnumDefault;
		}
	}

	static string UserGetLicType(int nType)
	{
		switch (nType)
		{
		case 0: return "Read";
		case 1: return "Fixed";
		case 2: return "Floating";
		case 3: return "Restricted";
		default: return EnumDefault;
		}
	}

	static string UserGetFTLicType(int nType)
	{
		switch (nType)
		{
		case 0: return "None";
		case 1: return "Fixed";
		case 2: return "Floating";
		default: return EnumDefault;
		}
	}


	static string UserGetDefNotify(int nType)
	{
		switch (nType)
		{
		case 0: return "None";
		case 1: return "Alert";
		case 2: return "Email";
		default: return EnumDefault;
		}
	}

	static string ActiveLinkAction(int nType)
	{
		switch (nType)
		{
		case AR_ACTIVE_LINK_ACTION_NONE: return "None";
		case AR_ACTIVE_LINK_ACTION_MACRO: return "Run Macro";
		case AR_ACTIVE_LINK_ACTION_FIELDS: return "Set Fields";
		case AR_ACTIVE_LINK_ACTION_PROCESS: return "Run Process";
		case AR_ACTIVE_LINK_ACTION_MESSAGE: return "Message";
		case AR_ACTIVE_LINK_ACTION_SET_CHAR: return "Change Field";
		case AR_ACTIVE_LINK_ACTION_DDE: return "DDE";
		case AR_ACTIVE_LINK_ACTION_FIELDP: return "Push Fields";
		case AR_ACTIVE_LINK_ACTION_SQL: return "Direct SQL";
		case AR_ACTIVE_LINK_ACTION_AUTO: return "OLE Automation";
		case AR_ACTIVE_LINK_ACTION_OPENDLG: return "Open Window";
		case AR_ACTIVE_LINK_ACTION_COMMITC: return "Commit Changes";
		case AR_ACTIVE_LINK_ACTION_CLOSEWND: return "Close Windows";
		case AR_ACTIVE_LINK_ACTION_CALLGUIDE: return "Call Guide";
		case AR_ACTIVE_LINK_ACTION_EXITGUIDE: return "Exit Guide";
		case AR_ACTIVE_LINK_ACTION_GOTOGUIDELABEL: return "Go To Guide Label";
		case AR_ACTIVE_LINK_ACTION_WAIT: return "Wait";
		case AR_ACTIVE_LINK_ACTION_GOTOACTION: return "Goto";
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
		case AR_ACTIVE_LINK_ACTION_SERVICE: return "Service";
#endif
		default: return EnumDefault;
		}
	}

	static string ActiveLinkMessageType(int nType)
	{
		switch (nType)
		{		
		case AR_RETURN_OK: return "Note";
		case AR_RETURN_WARNING: return "Warning";
		case AR_RETURN_ERROR: return "Error";
		case AR_RETURN_FATAL: return "Fatal Error";									
		case AR_RETURN_BAD_STATUS: return "Bad Status";
		case AR_RETURN_PROMPT: return "Prompt";
		case AR_RETURN_ACCESSIBLE: return "Accessible";
		default: return EnumDefault;
		}
	}

	static string Keyword(int nType)
	{
		switch (nType)
		{		
		case AR_KEYWORD_DEFAULT: return "DEFAULT";         
		case AR_KEYWORD_USER: return "USER";             
		case AR_KEYWORD_TIMESTAMP: return "TIMESTAMP";        
		case AR_KEYWORD_TIME_ONLY: return "TIME";											
		case AR_KEYWORD_DATE_ONLY: return "DATE";										             
		case AR_KEYWORD_SCHEMA: return "SCHEMA";           
		case AR_KEYWORD_SERVER: return "SERVER";           
		case AR_KEYWORD_WEEKDAY: return "WEEKDAY";         
		case AR_KEYWORD_GROUPS: return "GROUPS";           
		case AR_KEYWORD_OPERATION: return "OPERATION";											
		case AR_KEYWORD_HARDWARE: return "HARDWARE";        
		case AR_KEYWORD_OS: return "OS";              
		case AR_KEYWORD_DATABASE: return "DATABASE";        
		case AR_KEYWORD_LASTID: return "LASTID";											              
		case AR_KEYWORD_LASTCOUNT: return "LASTCOUNT";										
		case AR_KEYWORD_VERSION: return "VERSION";         
		case AR_KEYWORD_VUI: return "VUI";             
		case AR_KEYWORD_GUIDETEXT: return "GUIDETEXT";       
		case AR_KEYWORD_FIELDHELP: return "FIELDHELP";       
		case AR_KEYWORD_GUIDE: return "GUIDE";           
		case AR_KEYWORD_APPLICATION: return "APPLICATION";     
		case AR_KEYWORD_LOCALE: return "LOCALE";          
		case AR_KEYWORD_CLIENT_TYPE: return "CLIENT-TYPE";     
		case AR_KEYWORD_SCHEMA_ALIAS: return "SCHEMA-ALIAS";    
		case AR_KEYWORD_ROWSELECTED: return "ROWSELECTED";     
		case AR_KEYWORD_ROWCHANGED: return "ROWCHANGED";										
		case AR_KEYWORD_BROWSER: return "BROWSER";         
		case AR_KEYWORD_VUI_TYPE: return "VUI-TYPE";       
		case AR_KEYWORD_TCPPORT: return "TCPPORT";        
		case AR_KEYWORD_HOMEURL: return "HOMEURL";        
		case AR_KEYWORD_ROLES: return "ROLES";           
		case AR_KEYWORD_EVENTTYPE: return "EVENTTYPE";											
		case AR_KEYWORD_EVENTSRCWINID: return "EVENTSRCWINID";      
		case AR_KEYWORD_CURRENTWINID: return "CURRENTWINID";    
		case AR_KEYWORD_LASTOPENEDWINID: return "LASTOPENWINID"; 
		case AR_KEYWORD_INBULKTRANS: return "INBULKTRANSACTION";     
		case AR_KEYWORD_FIELDID: return "FIELDID";         
		case AR_KEYWORD_FIELDNAME: return "FIELDNAME";       
		case AR_KEYWORD_FIELDLABEL: return "FIELDLABEL";      
		case AR_KEYWORD_SERVERTIMESTAMP: return "SERVERTIMESTAMP"; 
		case AR_KEYWORD_GROUPIDS: return "GROUPIDS";
		case AR_KEYWORD_EVENTDATA: return "EVENTDATA";
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
		case AR_KEYWORD_ERRNO: return "ERRNO";
		case AR_KEYWORD_ERRMSG: return "ERRMSG";
		case AR_KEYWORD_ERRAPPENDMSG: return "ERRAPPENDMSG";
#endif
		default: return EnumDefault;
		}
	}

	static string OpenWindowMode(int nType)
	{
		switch(nType)
		{
		case AR_ACTIVE_LINK_ACTION_OPEN_DLG: return "Dialog";
		case AR_ACTIVE_LINK_ACTION_OPEN_SEARCH: return "Search";
		case AR_ACTIVE_LINK_ACTION_OPEN_SUBMIT: return "Submit";     
		case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_LST: return "Modify (List)";
		case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_DETAIL: return "Modify (Details)";
		case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY_SPLIT: return "Modify (Split)";
		case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY_LST: return "Display (List)";
		case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY_DETAIL: return "Display (Detail)";
		case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY_SPLIT: return "Display (Split)";
		case AR_ACTIVE_LINK_ACTION_OPEN_REPORT: return "Report";
		case AR_ACTIVE_LINK_ACTION_OPEN_MODIFY: return "Modify";
		case AR_ACTIVE_LINK_ACTION_OPEN_DSPLY: return "Display";
		default: return EnumDefault;
		}
	}

	static string FilterAction(int nType)
	{
		switch(nType)
		{	
		case AR_FILTER_ACTION_NONE: return "None";
		case AR_FILTER_ACTION_NOTIFY: return "Notify";
		case AR_FILTER_ACTION_MESSAGE: return "Message";
		case AR_FILTER_ACTION_LOG: return "Log to File";
		case AR_FILTER_ACTION_FIELDS: return "Set Fields";
		case AR_FILTER_ACTION_PROCESS: return "Run Process";
		case AR_FILTER_ACTION_FIELDP: return "Push Fields";
		case AR_FILTER_ACTION_SQL: return "Direct SQL";
		case AR_FILTER_ACTION_GOTOACTION: return "Goto";
		case AR_FILTER_ACTION_CALLGUIDE: return "Call Guide";
		case AR_FILTER_ACTION_EXITGUIDE: return "Exit Guide";
		case AR_FILTER_ACTION_GOTOGUIDELABEL: return "Go To Guide Label";
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		case AR_FILTER_ACTION_SERVICE: return "Service";
#endif
		default: return EnumDefault;
		}
	}

	static string MessageType(int nType)
	{
		switch(nType)
		{
		case AR_RETURN_OK: return "Note";
		case AR_RETURN_WARNING: return "Warning";
		case AR_RETURN_ERROR: return "Error";
		case AR_RETURN_PROMPT: return "Prompt";
		case AR_RETURN_ACCESSIBLE: return "Accessible";
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		case AR_RETURN_TOOLTIP: return "Tooltip";
#endif
		default: return EnumDefault;
		}
	}

	static string NotifyMechanism(int nType)
	{
		switch(nType)
		{
		case AR_NOTIFY_NONE: return "None";
		case AR_NOTIFY_VIA_NOTIFIER: return "Alert";
		case AR_NOTIFY_VIA_EMAIL: return "Email";
		case AR_NOTIFY_VIA_DEFAULT: return "Default";
		default: return "Other";
		}
	}

	static string NotifyFieldList(int nType)
	{
		switch(nType)
		{
		case AR_FILTER_FIELD_IDS_NONE: return "None";
		case AR_FILTER_FIELD_IDS_ALL: return "All";
		case AR_FILTER_FIELD_IDS_LIST: return "Selected";
		case AR_FILTER_FIELD_IDS_CHANGED: return "Changed";
		default: return EnumDefault;
		}
	}

	static string FieldOption(int nType)
	{
		switch(nType)
		{
		case AR_FIELD_OPTION_REQUIRED: return "Required";
		case AR_FIELD_OPTION_OPTIONAL: return "Optional";
		case AR_FIELD_OPTION_SYSTEM: return "System";
		case AR_FIELD_OPTION_DISPLAY: return "Display Only";
		default: return EnumDefault;
		}
	}

	static string FieldCreateMode(int nType)
	{
		switch(nType)
		{
		case AR_FIELD_OPEN_AT_CREATE: return "Allow Any User To Submit";
		case AR_FIELD_OPTION_OPTIONAL: return "Protected";		
		default: return EnumDefault;
		}
	}	

	static string FieldQbeMatch(int nType)
	{
		switch(nType)
		{
		case AR_QBE_MATCH_ANYWHERE: return "Anywhere";
		case AR_QBE_MATCH_LEADING: return "Leading";
		case AR_QBE_MATCH_EQUAL: return "Equal";
		default: return EnumDefault;
		}
	}

	static string FieldFTOption(int nType)
	{
		switch(nType)
		{
		case AR_FULLTEXT_OPTIONS_NONE: return "None";
		case AR_FULLTEXT_OPTIONS_INDEXED: return "Index";
		default: return EnumDefault;
		}
	}

	static string EnumStyle(int nType)
	{
		switch(nType)
		{
		case AR_ENUM_STYLE_REGULAR: return "Regular"; /* list auto-indexed starting at 0 */
		case AR_ENUM_STYLE_CUSTOM: return "Custom";   /* list indexed manually, gaps in numbers OK */
		case AR_ENUM_STYLE_QUERY: return "Query";     /* search performed to find name/number pairs*/
		default: return EnumDefault;
		}
	}

	static string AssignType(int nType)
	{
		switch(nType)
		{
		case AR_ASSIGN_TYPE_NONE: return "None";
		case AR_ASSIGN_TYPE_VALUE: return "Value";
		case AR_ASSIGN_TYPE_FIELD: return "Field";
		case AR_ASSIGN_TYPE_PROCESS: return "Process";
		case AR_ASSIGN_TYPE_ARITH: return "Calculated";
		case AR_ASSIGN_TYPE_FUNCTION: return "Function";
		case AR_ASSIGN_TYPE_DDE: return "DDE";
		case AR_ASSIGN_TYPE_SQL: return "SQL";
		case AR_ASSIGN_TYPE_FILTER_API: return "Filter Api";
		default: return EnumDefault;
		}
	}

	static string NoMatchRequest(int nType)
	{
		switch(nType)
		{
		case AR_NO_MATCH_ERROR: return "Display 'No Match' Error";
		case AR_NO_MATCH_SET_NULL: return "Set Fields to $NULL$";											
		case AR_NO_MATCH_NO_ACTION: return "Take No Action";											
		case AR_NO_MATCH_SUBMIT: return "Create a New Request";
		default: return "";											
		}
	}


	static string MultiMatchRequest(int nType)
	{
		switch(nType)
		{
		case AR_MULTI_MATCH_ERROR: return "Display 'Multiple Match' Error";
		case AR_MULTI_MATCH_SET_NULL: return "Set Fields to $NULL$";												
		case AR_MULTI_MATCH_USE_FIRST: return "Use First Matching Request";
		case AR_MULTI_MATCH_PICKLIST: return "Display a List";											
		case AR_MULTI_MATCH_MODIFY_ALL: return "Modify All Matching Requests";											
		case AR_MULTI_MATCH_NO_ACTION: return "Take No Action";												
		case AR_MULTI_MATCH_USE_LOCALE: return "Use First Matching Request Based on Locale";
		default: return "";		
		}
	}

	static string XmlStructItem(int nType)
	{
		switch(nType)
		{
		case AR_STRUCT_ITEM_XML_NONE: return "None";
		case AR_STRUCT_ITEM_XML_SCHEMA: return "Schema";
		case AR_STRUCT_ITEM_XML_FILTER: return "Filter";
		case AR_STRUCT_ITEM_XML_ACTIVE_LINK: return "Active Link";
		case AR_STRUCT_ITEM_XML_CHAR_MENU: return "Menu";
		case AR_STRUCT_ITEM_XML_ESCALATION: return "Escalation";
		case AR_STRUCT_ITEM_XML_DIST_MAP: return "DistMapping";
		case AR_STRUCT_ITEM_XML_CONTAINER: return "Container";
		case AR_STRUCT_ITEM_XML_DIST_POOL: return "DistPool";
		case AR_STRUCT_ITEM_XML_VUI: return "Vui";
		case AR_STRUCT_ITEM_XML_FIELD: return "Field";
		case AR_STRUCT_ITEM_XML_APP: return "Application";
		case AR_STRUCT_ITEM_XML_SCHEMA_DATA: return "Data";
		case AR_STRUCT_ITEM_XML_LOCK_BLOCK: return "Block";
		default: return EnumDefault;
		}
	}

	static string DDEAction(int nType)
	{
		switch(nType)
		{
		case AR_DDE_EXECUTE: return "Execute";
		case AR_DDE_POKE: return "Poke";
		case AR_DDE_REQUEST: return "Request";
		default: return EnumDefault;
		}
	}

	//ChangeField
	static string SetCharFieldAccess(int nType)
	{
		switch(nType)
		{			
		case AR_DVAL_ENABLE_READ_ONLY: return "Read Only";
		case AR_DVAL_ENABLE_READ_WRITE: return "Read/Write";
		case AR_DVAL_ENABLE_DISABLE:return "Disabled";
		default: return "Unchanged";
		}
	}

	static string SetCharFieldVisibility(int nType)
	{
		switch(nType)
		{			
		case 0: return "Hidden";
		case 1: return "Visible";
		default: return "Unchanged";
		}
	}


	static string ServerInfoApiCall(int nType)
	{
		switch (nType)
		{
		case AR_SERVER_INFO_DB_TYPE: return  "AR_SERVER_INFO_DB_TYPE";
		case AR_SERVER_INFO_SERVER_LICENSE: return  "AR_SERVER_INFO_SERVER_LICENSE";
		case AR_SERVER_INFO_FIXED_LICENSE: return  "AR_SERVER_INFO_FIXED_LICENSE";
		case AR_SERVER_INFO_VERSION: return  "AR_SERVER_INFO_VERSION";
		case AR_SERVER_INFO_ALLOW_GUESTS: return  "AR_SERVER_INFO_ALLOW_GUESTS";
		case AR_SERVER_INFO_USE_ETC_PASSWD: return  "AR_SERVER_INFO_USE_ETC_PASSWD";
		case AR_SERVER_INFO_XREF_PASSWORDS: return  "AR_SERVER_INFO_XREF_PASSWORDS";
		case AR_SERVER_INFO_DEBUG_MODE: return  "AR_SERVER_INFO_DEBUG_MODE";
		case AR_SERVER_INFO_DB_NAME: return  "AR_SERVER_INFO_DB_NAME";
		case AR_SERVER_INFO_DB_PASSWORD: return  "AR_SERVER_INFO_DB_PASSWORD";
		case AR_SERVER_INFO_HARDWARE: return  "AR_SERVER_INFO_HARDWARE";
		case AR_SERVER_INFO_OS: return  "AR_SERVER_INFO_OS";
		case AR_SERVER_INFO_SERVER_DIR: return  "AR_SERVER_INFO_SERVER_DIR";
		case AR_SERVER_INFO_DBHOME_DIR: return  "AR_SERVER_INFO_DBHOME_DIR";
		case AR_SERVER_INFO_SET_PROC_TIME: return  "AR_SERVER_INFO_SET_PROC_TIME";
		case AR_SERVER_INFO_EMAIL_FROM: return  "AR_SERVER_INFO_EMAIL_FROM";
		case AR_SERVER_INFO_SQL_LOG_FILE: return  "AR_SERVER_INFO_SQL_LOG_FILE";
		case AR_SERVER_INFO_FLOAT_LICENSE: return  "AR_SERVER_INFO_FLOAT_LICENSE";
		case AR_SERVER_INFO_FLOAT_TIMEOUT: return  "AR_SERVER_INFO_FLOAT_TIMEOUT";
		case AR_SERVER_INFO_UNQUAL_QUERIES: return  "AR_SERVER_INFO_UNQUAL_QUERIES";
		case AR_SERVER_INFO_FILTER_LOG_FILE: return  "AR_SERVER_INFO_FILTER_LOG_FILE";
		case AR_SERVER_INFO_USER_LOG_FILE: return  "AR_SERVER_INFO_USER_LOG_FILE";
		case AR_SERVER_INFO_REM_SERV_ID: return  "AR_SERVER_INFO_REM_SERV_ID";
		case AR_SERVER_INFO_MULTI_SERVER: return  "AR_SERVER_INFO_MULTI_SERVER";
		case AR_SERVER_INFO_EMBEDDED_SQL: return  "AR_SERVER_INFO_EMBEDDED_SQL";
		case AR_SERVER_INFO_MAX_SCHEMAS: return  "AR_SERVER_INFO_MAX_SCHEMAS";
		case AR_SERVER_INFO_DB_VERSION: return  "AR_SERVER_INFO_DB_VERSION";
		case AR_SERVER_INFO_MAX_ENTRIES: return  "AR_SERVER_INFO_MAX_ENTRIES";
		case AR_SERVER_INFO_MAX_F_DAEMONS: return  "AR_SERVER_INFO_MAX_F_DAEMONS";
		case AR_SERVER_INFO_MAX_L_DAEMONS: return  "AR_SERVER_INFO_MAX_L_DAEMONS";
		case AR_SERVER_INFO_ESCALATION_LOG_FILE: return  "AR_SERVER_INFO_ESCALATION_LOG_FILE";
		case AR_SERVER_INFO_ESCL_DAEMON: return  "AR_SERVER_INFO_ESCL_DAEMON";
		case AR_SERVER_INFO_SUBMITTER_MODE: return  "AR_SERVER_INFO_SUBMITTER_MODE";
		case AR_SERVER_INFO_API_LOG_FILE: return  "AR_SERVER_INFO_API_LOG_FILE";
		case AR_SERVER_INFO_FTEXT_FIXED: return  "AR_SERVER_INFO_FTEXT_FIXED";
		case AR_SERVER_INFO_FTEXT_FLOAT: return  "AR_SERVER_INFO_FTEXT_FLOAT";
		case AR_SERVER_INFO_FTEXT_TIMEOUT: return  "AR_SERVER_INFO_FTEXT_TIMEOUT";
		case AR_SERVER_INFO_RESERV1_A: return  "AR_SERVER_INFO_RESERV1_A";
		case AR_SERVER_INFO_RESERV1_B: return  "AR_SERVER_INFO_RESERV1_B";
		case AR_SERVER_INFO_RESERV1_C: return  "AR_SERVER_INFO_RESERV1_C";
		case AR_SERVER_INFO_SERVER_IDENT: return  "AR_SERVER_INFO_SERVER_IDENT";
		case AR_SERVER_INFO_DS_SVR_LICENSE: return  "AR_SERVER_INFO_DS_SVR_LICENSE";
		case AR_SERVER_INFO_DS_MAPPING: return  "AR_SERVER_INFO_DS_MAPPING";
		case AR_SERVER_INFO_DS_PENDING: return  "AR_SERVER_INFO_DS_PENDING";
		case AR_SERVER_INFO_DS_RPC_SOCKET: return  "AR_SERVER_INFO_DS_RPC_SOCKET";
		case AR_SERVER_INFO_DS_LOG_FILE: return  "AR_SERVER_INFO_DS_LOG_FILE";
		case AR_SERVER_INFO_SUPPRESS_WARN: return  "AR_SERVER_INFO_SUPPRESS_WARN";
		case AR_SERVER_INFO_HOSTNAME: return  "AR_SERVER_INFO_HOSTNAME";
		case AR_SERVER_INFO_FULL_HOSTNAME: return  "AR_SERVER_INFO_FULL_HOSTNAME";
		case AR_SERVER_INFO_SAVE_LOGIN: return  "AR_SERVER_INFO_SAVE_LOGIN";
		case AR_SERVER_INFO_U_CACHE_CHANGE: return  "AR_SERVER_INFO_U_CACHE_CHANGE";
		case AR_SERVER_INFO_G_CACHE_CHANGE: return  "AR_SERVER_INFO_G_CACHE_CHANGE";
		case AR_SERVER_INFO_STRUCT_CHANGE: return  "AR_SERVER_INFO_STRUCT_CHANGE";
		case AR_SERVER_INFO_CASE_SENSITIVE: return  "AR_SERVER_INFO_CASE_SENSITIVE";
		case AR_SERVER_INFO_SERVER_LANG: return  "AR_SERVER_INFO_SERVER_LANG";
		case AR_SERVER_INFO_ADMIN_ONLY: return  "AR_SERVER_INFO_ADMIN_ONLY";
		case AR_SERVER_INFO_CACHE_LOG_FILE: return  "AR_SERVER_INFO_CACHE_LOG_FILE";
		case AR_SERVER_INFO_FLASH_DAEMON: return  "AR_SERVER_INFO_FLASH_DAEMON";
		case AR_SERVER_INFO_THREAD_LOG_FILE: return  "AR_SERVER_INFO_THREAD_LOG_FILE";
		case AR_SERVER_INFO_ADMIN_TCP_PORT: return  "AR_SERVER_INFO_ADMIN_TCP_PORT";
		case AR_SERVER_INFO_ESCL_TCP_PORT: return  "AR_SERVER_INFO_ESCL_TCP_PORT";
		case AR_SERVER_INFO_FAST_TCP_PORT: return  "AR_SERVER_INFO_FAST_TCP_PORT";
		case AR_SERVER_INFO_LIST_TCP_PORT: return  "AR_SERVER_INFO_LIST_TCP_PORT";
		case AR_SERVER_INFO_FLASH_TCP_PORT: return  "AR_SERVER_INFO_FLASH_TCP_PORT";
		case AR_SERVER_INFO_TCD_TCP_PORT: return  "AR_SERVER_INFO_TCD_TCP_PORT";
		case AR_SERVER_INFO_DSO_DEST_PORT: return  "AR_SERVER_INFO_DSO_DEST_PORT";
		case AR_SERVER_INFO_INFORMIX_DBN: return  "AR_SERVER_INFO_INFORMIX_DBN";
		case AR_SERVER_INFO_INFORMIX_TBC: return  "AR_SERVER_INFO_INFORMIX_TBC";
		case AR_SERVER_INFO_INGRES_VNODE: return  "AR_SERVER_INFO_INGRES_VNODE";
		case AR_SERVER_INFO_ORACLE_SID: return  "AR_SERVER_INFO_ORACLE_SID";
		case AR_SERVER_INFO_ORACLE_TWO_T: return  "AR_SERVER_INFO_ORACLE_TWO_T";
		case AR_SERVER_INFO_SYBASE_CHARSET: return  "AR_SERVER_INFO_SYBASE_CHARSET";
		case AR_SERVER_INFO_SYBASE_SERV: return  "AR_SERVER_INFO_SYBASE_SERV";
		case AR_SERVER_INFO_SHARED_MEM: return  "AR_SERVER_INFO_SHARED_MEM";
		case AR_SERVER_INFO_SHARED_CACHE: return  "AR_SERVER_INFO_SHARED_CACHE";
		case AR_SERVER_INFO_CACHE_SEG_SIZE: return  "AR_SERVER_INFO_CACHE_SEG_SIZE";
		case AR_SERVER_INFO_DB_USER: return  "AR_SERVER_INFO_DB_USER";
		case AR_SERVER_INFO_NFY_TCP_PORT: return  "AR_SERVER_INFO_NFY_TCP_PORT";
		case AR_SERVER_INFO_FILT_MAX_TOTAL: return  "AR_SERVER_INFO_FILT_MAX_TOTAL";
		case AR_SERVER_INFO_FILT_MAX_STACK: return  "AR_SERVER_INFO_FILT_MAX_STACK";
		case AR_SERVER_INFO_DEFAULT_ORDER_BY: return  "AR_SERVER_INFO_DEFAULT_ORDER_BY";
		case AR_SERVER_INFO_DELAYED_CACHE: return  "AR_SERVER_INFO_DELAYED_CACHE";
		case AR_SERVER_INFO_DSO_MERGE_STYLE: return  "AR_SERVER_INFO_DSO_MERGE_STYLE";
		case AR_SERVER_INFO_EMAIL_LINE_LEN: return  "AR_SERVER_INFO_EMAIL_LINE_LEN";
		case AR_SERVER_INFO_EMAIL_SYSTEM: return  "AR_SERVER_INFO_EMAIL_SYSTEM";
		case AR_SERVER_INFO_INFORMIX_RELAY_MOD: return  "AR_SERVER_INFO_INFORMIX_RELAY_MOD";
		case AR_SERVER_INFO_PS_RPC_SOCKET: return  "AR_SERVER_INFO_PS_RPC_SOCKET";
		case AR_SERVER_INFO_REGISTER_PORTMAPPER: return  "AR_SERVER_INFO_REGISTER_PORTMAPPER";
		case AR_SERVER_INFO_SERVER_NAME: return  "AR_SERVER_INFO_SERVER_NAME";
		case AR_SERVER_INFO_DBCONF: return  "AR_SERVER_INFO_DBCONF";
		case AR_SERVER_INFO_APPL_PENDING: return  "AR_SERVER_INFO_APPL_PENDING";
		case AR_SERVER_INFO_AP_RPC_SOCKET: return  "AR_SERVER_INFO_AP_RPC_SOCKET";
		case AR_SERVER_INFO_AP_LOG_FILE: return  "AR_SERVER_INFO_AP_LOG_FILE";
		case AR_SERVER_INFO_AP_DEFN_CHECK: return  "AR_SERVER_INFO_AP_DEFN_CHECK";
		case AR_SERVER_INFO_MAX_LOG_FILE_SIZE: return  "AR_SERVER_INFO_MAX_LOG_FILE_SIZE";
		case AR_SERVER_INFO_CLUSTERED_INDEX: return  "AR_SERVER_INFO_CLUSTERED_INDEX";
		case AR_SERVER_INFO_ACTLINK_DIR: return  "AR_SERVER_INFO_ACTLINK_DIR";
		case AR_SERVER_INFO_ACTLINK_SHELL: return  "AR_SERVER_INFO_ACTLINK_SHELL";
		case AR_SERVER_INFO_USER_CACHE_UTILS: return  "AR_SERVER_INFO_USER_CACHE_UTILS";
		case AR_SERVER_INFO_EMAIL_TIMEOUT: return  "AR_SERVER_INFO_EMAIL_TIMEOUT";
		case AR_SERVER_INFO_EXPORT_VERSION: return  "AR_SERVER_INFO_EXPORT_VERSION";
		case AR_SERVER_INFO_ENCRYPT_AL_SQL: return  "AR_SERVER_INFO_ENCRYPT_AL_SQL";
		case AR_SERVER_INFO_SCC_ENABLED: return  "AR_SERVER_INFO_SCC_ENABLED";
		case AR_SERVER_INFO_SCC_PROVIDER_NAME: return  "AR_SERVER_INFO_SCC_PROVIDER_NAME";
		case AR_SERVER_INFO_SCC_TARGET_DIR: return  "AR_SEAR_SERVER_INFO_SCC_COMMENT_CHECKINRVER_INFO_SCC_TARGET_DIR";
		case AR_SERVER_INFO_SCC_COMMENT_CHECKIN: return  "AR_SERVER_INFO_SCC_COMMENT_CHECKIN";
		case AR_SERVER_INFO_SCC_COMMENT_CHECKOUT: return  "AR_SERVER_INFO_SCC_COMMENT_CHECKOUT";
		case AR_SERVER_INFO_SCC_INTEGRATION_MODE: return  "AR_SERVER_INFO_SCC_INTEGRATION_MODE";
		case AR_SERVER_INFO_EA_RPC_SOCKET: return  "AR_SERVER_INFO_EA_RPC_SOCKET";
		case AR_SERVER_INFO_EA_RPC_TIMEOUT: return  "AR_SERVER_INFO_EA_RPC_TIMEOUT";
		case AR_SERVER_INFO_USER_INFO_LISTS: return  "AR_SERVER_INFO_USER_INFO_LISTS";
		case AR_SERVER_INFO_USER_INST_TIMEOUT: return  "AR_SERVER_INFO_USER_INST_TIMEOUT";
		case AR_SERVER_INFO_DEBUG_GROUPID: return  "AR_SERVER_INFO_DEBUG_GROUPID";
		case AR_SERVER_INFO_APPLICATION_AUDIT: return  "AR_SERVER_INFO_APPLICATION_AUDIT";
		case AR_SERVER_INFO_EA_SYNC_TIMEOUT: return  "AR_SERVER_INFO_EA_SYNC_TIMEOUT";
		case AR_SERVER_INFO_SERVER_TIME: return  "AR_SERVER_INFO_SERVER_TIME";
		case AR_SERVER_INFO_SVR_SEC_CACHE: return  "AR_SERVER_INFO_SVR_SEC_CACHE";
		case AR_SERVER_INFO_LOGFILE_APPEND: return  "AR_SERVER_INFO_LOGFILE_APPEND";
		case AR_SERVER_INFO_MINIMUM_API_VER: return  "AR_SERVER_INFO_MINIMUM_API_VER";
		case AR_SERVER_INFO_MAX_AUDIT_LOG_FILE_SIZE: return  "AR_SERVER_INFO_MAX_AUDIT_LOG_FILE_SIZE";
		case AR_SERVER_INFO_CANCEL_QUERY: return  "AR_SERVER_INFO_CANCEL_QUERY";
		case AR_SERVER_INFO_MULT_ASSIGN_GROUPS: return  "AR_SERVER_INFO_MULT_ASSIGN_GROUPS";
		case AR_SERVER_INFO_ARFORK_LOG_FILE: return  "AR_SERVER_INFO_ARFORK_LOG_FILE";
		case AR_SERVER_INFO_DSO_PLACEHOLDER_MODE: return  "AR_SERVER_INFO_DSO_PLACEHOLDER_MODE";
		case AR_SERVER_INFO_DSO_POLLING_INTERVAL: return  "AR_SERVER_INFO_DSO_POLLING_INTERVAL";
		case AR_SERVER_INFO_DSO_SOURCE_SERVER: return  "AR_SERVER_INFO_DSO_SOURCE_SERVER";
		case AR_SERVER_INFO_DS_POOL: return  "AR_SERVER_INFO_DS_POOL";
		case AR_SERVER_INFO_DSO_TIMEOUT_NORMAL: return  "AR_SERVER_INFO_DSO_TIMEOUT_NORMAL";
		case AR_SERVER_INFO_ENC_PUB_KEY: return  "AR_SERVER_INFO_ENC_PUB_KEY";
		case AR_SERVER_INFO_ENC_PUB_KEY_EXP: return  "AR_SERVER_INFO_ENC_PUB_KEY_EXP";
		case AR_SERVER_INFO_ENC_DATA_KEY_EXP: return  "AR_SERVER_INFO_ENC_DATA_KEY_EXP";
		case AR_SERVER_INFO_ENC_DATA_ENCR_ALG: return  "AR_SERVER_INFO_ENC_DATA_ENCR_ALG";
		case AR_SERVER_INFO_ENC_SEC_POLICY: return  "AR_SERVER_INFO_ENC_SEC_POLICY";
		case AR_SERVER_INFO_ENC_SESS_H_ENTRIES: return  "AR_SERVER_INFO_ENC_SESS_H_ENTRIES";
		case AR_SERVER_INFO_DSO_TARGET_CONNECTION: return  "AR_SERVER_INFO_DSO_TARGET_CONNECTION";
		case AR_SERVER_INFO_PREFERENCE_PRIORITY: return  "AR_SERVER_INFO_PREFERENCE_PRIORITY";
		case AR_SERVER_INFO_ORACLE_QUERY_ON_CLOB: return  "AR_SERVER_INFO_ORACLE_QUERY_ON_CLOB";
		case AR_SERVER_INFO_MESSAGE_CAT_SCHEMA: return  "AR_SERVER_INFO_MESSAGE_CAT_SCHEMA";
		case AR_SERVER_INFO_ALERT_SCHEMA: return  "AR_SERVER_INFO_ALERT_SCHEMA";
		case AR_SERVER_INFO_LOCALIZED_SERVER: return  "AR_SERVER_INFO_LOCALIZED_SERVER";
		case AR_SERVER_INFO_SVR_EVENT_LIST: return  "AR_SERVER_INFO_SVR_EVENT_LIST";
		case AR_SERVER_INFO_DISABLE_ADMIN_OPERATIONS: return  "AR_SERVER_INFO_DISABLE_ADMIN_OPERATIONS";
		case AR_SERVER_INFO_DISABLE_ESCALATIONS: return  "AR_SERVER_INFO_DISABLE_ESCALATIONS";
		case AR_SERVER_INFO_ALERT_LOG_FILE: return  "AR_SERVER_INFO_ALERT_LOG_FILE";
		case AR_SERVER_INFO_DISABLE_ALERTS: return  "AR_SERVER_INFO_DISABLE_ALERTS";
		case AR_SERVER_INFO_CHECK_ALERT_USERS: return  "AR_SERVER_INFO_CHECK_ALERT_USERS";
		case AR_SERVER_INFO_ALERT_SEND_TIMEOUT: return  "AR_SERVER_INFO_ALERT_SEND_TIMEOUT";
		case AR_SERVER_INFO_ALERT_OUTBOUND_PORT: return  "AR_SERVER_INFO_ALERT_OUTBOUND_PORT";
		case AR_SERVER_INFO_ALERT_SOURCE_AR: return  "AR_SERVER_INFO_ALERT_SOURCE_AR";
		case AR_SERVER_INFO_ALERT_SOURCE_FB: return  "AR_SERVER_INFO_ALERT_SOURCE_FB";
		case AR_SERVER_INFO_DSO_USER_PASSWD: return  "AR_SERVER_INFO_DSO_USER_PASSWD";
		case AR_SERVER_INFO_DSO_TARGET_PASSWD: return  "AR_SERVER_INFO_DSO_TARGET_PASSWD";
		case AR_SERVER_INFO_APP_SERVICE_PASSWD: return  "AR_SERVER_INFO_APP_SERVICE_PASSWD";
		case AR_SERVER_INFO_MID_TIER_PASSWD: return  "AR_SERVER_INFO_MID_TIER_PASSWD";
		case AR_SERVER_INFO_PLUGIN_LOG_FILE: return  "AR_SERVER_INFO_PLUGIN_LOG_FILE";
		case AR_SERVER_INFO_SVR_STATS_REC_MODE: return  "AR_SERVER_INFO_SVR_STATS_REC_MODE";
		case AR_SERVER_INFO_SVR_STATS_REC_INTERVAL: return  "AR_SERVER_INFO_SVR_STATS_REC_INTERVAL";
		case AR_SERVER_INFO_DEFAULT_WEB_PATH: return  "AR_SERVER_INFO_DEFAULT_WEB_PATH";
		case AR_SERVER_INFO_FILTER_API_RPC_TIMEOUT: return  "AR_SERVER_INFO_FILTER_API_RPC_TIMEOUT";
		case AR_SERVER_INFO_DISABLED_CLIENT: return  "AR_SERVER_INFO_DISABLED_CLIENT";
		case AR_SERVER_INFO_PLUGIN_PASSWD: return  "AR_SERVER_INFO_PLUGIN_PASSWD";
		case AR_SERVER_INFO_PLUGIN_ALIAS: return  "AR_SERVER_INFO_PLUGIN_ALIAS";
		case AR_SERVER_INFO_PLUGIN_TARGET_PASSWD: return  "AR_SERVER_INFO_PLUGIN_TARGET_PASSWD";
		case AR_SERVER_INFO_REM_WKFLW_PASSWD: return  "AR_SERVER_INFO_REM_WKFLW_PASSWD";
		case AR_SERVER_INFO_REM_WKFLW_TARGET_PASSWD: return  "AR_SERVER_INFO_REM_WKFLW_TARGET_PASSWD";
		case AR_SERVER_INFO_EXPORT_SVR_OPS: return  "AR_SERVER_INFO_EXPORT_SVR_OPS";
		case AR_SERVER_INFO_INIT_FORM: return  "AR_SERVER_INFO_INIT_FORM";
		case AR_SERVER_INFO_ENC_PUB_KEY_ALG: return  "AR_SERVER_INFO_ENC_PUB_KEY_ALG";
		case AR_SERVER_INFO_IP_NAMES: return  "AR_SERVER_INFO_IP_NAMES";
		case AR_SERVER_INFO_DSO_CACHE_CHK_INTERVAL: return  "AR_SERVER_INFO_DSO_CACHE_CHK_INTERVAL";
		case AR_SERVER_INFO_DSO_MARK_PENDING_RETRY: return  "AR_SERVER_INFO_DSO_MARK_PENDING_RETRY";
		case AR_SERVER_INFO_DSO_RPCPROG_NUM: return  "AR_SERVER_INFO_DSO_RPCPROG_NUM";
		case AR_SERVER_INFO_DELAY_RECACHE_TIME: return  "AR_SERVER_INFO_DELAY_RECACHE_TIME";
		case AR_SERVER_INFO_DFLT_ALLOW_CURRENCIES: return  "AR_SERVER_INFO_DFLT_ALLOW_CURRENCIES";
		case AR_SERVER_INFO_CURRENCY_INTERVAL: return  "AR_SERVER_INFO_CURRENCY_INTERVAL";
		case AR_SERVER_INFO_ORACLE_CURSOR_SHARE: return  "AR_SERVER_INFO_ORACLE_CURSOR_SHARE";
		case AR_SERVER_INFO_DB2_DB_ALIAS: return  "AR_SERVER_INFO_DB2_DB_ALIAS";
		case AR_SERVER_INFO_DB2_SERVER: return  "AR_SERVER_INFO_DB2_SERVER";
		case AR_SERVER_INFO_DFLT_FUNC_CURRENCIES: return  "AR_SERVER_INFO_DFLT_FUNC_CURRENCIES";
		case AR_SERVER_INFO_EMAIL_IMPORT_FORM: return  "AR_SERVER_INFO_EMAIL_IMPORT_FORM";
		case AR_SERVER_INFO_EMAIL_AIX_USE_OLD_EMAIL: return  "AR_SERVER_INFO_EMAIL_AIX_USE_OLD_EMAIL";
		case AR_SERVER_INFO_TWO_DIGIT_YEAR_CUTOFF: return  "AR_SERVER_INFO_TWO_DIGIT_YEAR_CUTOFF";
		case AR_SERVER_INFO_ALLOW_BACKQUOTE_IN_PROCESS: return  "AR_SERVER_INFO_ALLOW_BACKQUOTE_IN_PROCESS";
		case AR_SERVER_INFO_DB_CONNECTION_RETRIES: return  "AR_SERVER_INFO_DB_CONNECTION_RETRIES";
		case AR_SERVER_INFO_DB_CHAR_SET: return  "AR_SERVER_INFO_DB_CHAR_SET";
		case AR_SERVER_INFO_CURR_PART_VALUE_STR: return  "AR_SERVER_INFO_CURR_PART_VALUE_STR";
		case AR_SERVER_INFO_CURR_PART_TYPE_STR: return  "AR_SERVER_INFO_CURR_PART_TYPE_STR";
		case AR_SERVER_INFO_CURR_PART_DATE_STR: return  "AR_SERVER_INFO_CURR_PART_DATE_STR";
		case AR_SERVER_INFO_HOMEPAGE_FORM: return  "AR_SERVER_INFO_HOMEPAGE_FORM";
		case AR_SERVER_INFO_DISABLE_FTS_INDEXER: return  "AR_SERVER_INFO_DISABLE_FTS_INDEXER";
		case AR_SERVER_INFO_DISABLE_ARCHIVE: return  "AR_SERVER_INFO_DISABLE_ARCHIVE";
		case AR_SERVER_INFO_SERVERGROUP_MEMBER: return  "AR_SERVER_INFO_SERVERGROUP_MEMBER";
		case AR_SERVER_INFO_SERVERGROUP_LOG_FILE: return  "AR_SERVER_INFO_SERVERGROUP_LOG_FILE";
		case AR_SERVER_INFO_FLUSH_LOG_LINES: return  "AR_SERVER_INFO_FLUSH_LOG_LINES";
		case AR_SERVER_INFO_SERVERGROUP_INTERVAL: return  "AR_SERVER_INFO_SERVERGROUP_INTERVAL";
		case AR_SERVER_INFO_JAVA_VM_OPTIONS: return  "AR_SERVER_INFO_JAVA_VM_OPTIONS";
		case AR_SERVER_INFO_PER_THREAD_LOGS: return  "AR_SERVER_INFO_PER_THREAD_LOGS";
		case AR_SERVER_INFO_CONFIG_FILE: return  "AR_SERVER_INFO_CONFIG_FILE";
		case AR_SERVER_INFO_SSTABLE_CHUNK_SIZE: return  "AR_SERVER_INFO_SSTABLE_CHUNK_SIZE";
		case AR_SERVER_INFO_SG_EMAIL_STATE: return  "AR_SERVER_INFO_SG_EMAIL_STATE";
		case AR_SERVER_INFO_SG_FLASHBOARDS_STATE: return  "AR_SERVER_INFO_SG_FLASHBOARDS_STATE";
		case AR_SERVER_INFO_SERVERGROUP_NAME: return  "AR_SERVER_INFO_SERVERGROUP_NAME";
		case AR_SERVER_INFO_SG_ADMIN_SERVER_NAME: return  "AR_SERVER_INFO_SG_ADMIN_SERVER_NAME";
		case AR_SERVER_INFO_LOCKED_WKFLW_LOG_MODE: return  "AR_SERVER_INFO_LOCKED_WKFLW_LOG_MODE";
		case AR_SERVER_INFO_ROLE_CHANGE: return  "AR_SERVER_INFO_ROLE_CHANGE";
		case AR_SERVER_INFO_SG_ADMIN_SERVER_PORT: return  "AR_SERVER_INFO_SG_ADMIN_SERVER_PORT";
		case AR_SERVER_INFO_PLUGIN_LOOPBACK_RPC: return  "AR_SERVER_INFO_PLUGIN_LOOPBACK_RPC";
		case AR_SERVER_INFO_CACHE_MODE: return  "AR_SERVER_INFO_CACHE_MODE";
		case AR_SERVER_INFO_DB_FREESPACE: return  "AR_SERVER_INFO_DB_FREESPACE";
		case AR_SERVER_INFO_GENERAL_AUTH_ERR: return  "AR_SERVER_INFO_GENERAL_AUTH_ERR";
		case AR_SERVER_INFO_AUTH_CHAINING_MODE: return  "AR_SERVER_INFO_AUTH_CHAINING_MODE";
		case AR_SERVER_INFO_RPC_NON_BLOCKING_IO: return  "AR_SERVER_INFO_RPC_NON_BLOCKING_IO";
		case AR_SERVER_INFO_SYS_LOGGING_OPTIONS: return  "AR_SERVER_INFO_SYS_LOGGING_OPTIONS";
		case AR_SERVER_INFO_EXT_AUTH_CAPABILITIES: return  "AR_SERVER_INFO_EXT_AUTH_CAPABILITIES";
		case AR_SERVER_INFO_DSO_ERROR_RETRY: return  "AR_SERVER_INFO_DSO_ERROR_RETRY";
		case AR_SERVER_INFO_PREF_SERVER_OPTION: return  "AR_SERVER_INFO_PREF_SERVER_OPTION";
		case AR_SERVER_INFO_FTINDEXER_LOG_FILE: return  "AR_SERVER_INFO_FTINDEXER_LOG_FILE";
		case AR_SERVER_INFO_EXCEPTION_OPTION: return  "AR_SERVER_INFO_EXCEPTION_OPTION";
		case AR_SERVER_INFO_ERROR_EXCEPTION_LIST: return  "AR_SERVER_INFO_ERROR_EXCEPTION_LIST";
		case AR_SERVER_INFO_DSO_MAX_QUERY_SIZE: return  "AR_SERVER_INFO_DSO_MAX_QUERY_SIZE";
		case AR_SERVER_INFO_ADMIN_OP_TRACKING: return  "AR_SERVER_INFO_ADMIN_OP_TRACKING";
		case AR_SERVER_INFO_ADMIN_OP_PROGRESS: return  "AR_SERVER_INFO_ADMIN_OP_PROGRESS";
		case AR_SERVER_INFO_PLUGIN_DEFAULT_TIMEOUT: return  "AR_SERVER_INFO_PLUGIN_DEFAULT_TIMEOUT";
		case AR_SERVER_INFO_EA_IGNORE_EXCESS_GROUPS: return  "AR_SERVER_INFO_EA_IGNORE_EXCESS_GROUPS";
		case AR_SERVER_INFO_EA_GROUP_MAPPING: return  "AR_SERVER_INFO_EA_GROUP_MAPPING";
		case AR_SERVER_INFO_PLUGIN_LOG_LEVEL: return  "AR_SERVER_INFO_PLUGIN_LOG_LEVEL";
		case AR_SERVER_INFO_FT_THRESHOLD_LOW: return  "AR_SERVER_INFO_FT_THRESHOLD_LOW";
		case AR_SERVER_INFO_FT_THRESHOLD_HIGH: return  "AR_SERVER_INFO_FT_THRESHOLD_HIGH";
		case AR_SERVER_INFO_NOTIFY_WEB_PATH: return  "AR_SERVER_INFO_NOTIFY_WEB_PATH";
		case AR_SERVER_INFO_DISABLE_NON_UNICODE_CLIENTS: return  "AR_SERVER_INFO_DISABLE_NON_UNICODE_CLIENTS";
		case AR_SERVER_INFO_FT_COLLECTION_DIR: return  "AR_SERVER_INFO_FT_COLLECTION_DIR";
		case AR_SERVER_INFO_FT_CONFIGURATION_DIR: return  "AR_SERVER_INFO_FT_CONFIGURATION_DIR";
		case AR_SERVER_INFO_FT_TEMP_DIR: return  "AR_SERVER_INFO_FT_TEMP_DIR";
		case AR_SERVER_INFO_FT_REINDEX: return  "AR_SERVER_INFO_FT_REINDEX";
		case AR_SERVER_INFO_FT_DISABLE_SEARCH: return  "AR_SERVER_INFO_FT_DISABLE_SEARCH";
		case AR_SERVER_INFO_FT_CASE_SENSITIVITY: return  "AR_SERVER_INFO_FT_CASE_SENSITIVITY";
		case AR_SERVER_INFO_FT_SEARCH_MATCH_OP: return  "AR_SERVER_INFO_FT_SEARCH_MATCH_OP";
		case AR_SERVER_INFO_FT_STOP_WORDS: return  "AR_SERVER_INFO_FT_STOP_WORDS";
		case AR_SERVER_INFO_FT_RECOVERY_INTERVAL: return  "AR_SERVER_INFO_FT_RECOVERY_INTERVAL";
		case AR_SERVER_INFO_FT_OPTIMIZE_THRESHOLD: return  "AR_SERVER_INFO_FT_OPTIMIZE_THRESHOLD";
		case AR_SERVER_INFO_MAX_PASSWORD_ATTEMPTS: return  "AR_SERVER_INFO_MAX_PASSWORD_ATTEMPTS";
		case AR_SERVER_INFO_GUESTS_RESTRICT_READ: return  "AR_SERVER_INFO_GUESTS_RESTRICT_READ";
		case AR_SERVER_INFO_ORACLE_CLOB_STORE_INROW: return  "AR_SERVER_INFO_ORACLE_CLOB_STORE_INROW";
		case AR_SERVER_INFO_NEXT_ID_BLOCK_SIZE: return  "AR_SERVER_INFO_NEXT_ID_BLOCK_SIZE";
		case AR_SERVER_INFO_NEXT_ID_COMMIT: return  "AR_SERVER_INFO_NEXT_ID_COMMIT";
		case AR_SERVER_INFO_RPC_CLIENT_XDR_LIMIT: return  "AR_SERVER_INFO_RPC_CLIENT_XDR_LIMIT";
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
		case AR_SERVER_INFO_CACHE_DISP_PROP: return  "AR_SERVER_INFO_CACHE_DISP_PROP";
		case AR_SERVER_INFO_USE_CON_NAME_IN_STATS: return  "AR_SERVER_INFO_USE_CON_NAME_IN_STATS";
		case AR_SERVER_INFO_DB_MAX_ATTACH_SIZE: return  "AR_SERVER_INFO_DB_MAX_ATTACH_SIZE";
		case AR_SERVER_INFO_DB_MAX_TEXT_SIZE: return  "AR_SERVER_INFO_DB_MAX_TEXT_SIZE";
		case AR_SERVER_INFO_GUID_PREFIX: return  "AR_SERVER_INFO_GUID_PREFIX";
		case AR_SERVER_INFO_MULTIPLE_ARSYSTEM_SERVERS: return  "AR_SERVER_INFO_MULTIPLE_ARSYSTEM_SERVERS";
		case AR_SERVER_INFO_ORACLE_BULK_FETCH_COUNT: return  "AR_SERVER_INFO_ORACLE_BULK_FETCH_COUNT";
		case AR_SERVER_INFO_MINIMUM_CMDB_API_VER: return  "AR_SERVER_INFO_MINIMUM_CMDB_API_VER";
		case AR_SERVER_INFO_PLUGIN_PORT: return  "AR_SERVER_INFO_PLUGIN_PORT";
#if AR_CURRENT_API_VERSION > 13 // Version 7.5 and higher
		case AR_SERVER_INFO_PLUGIN_LIST: return "AR_SERVER_INFO_PLUGIN_LIST";
		case AR_SERVER_INFO_PLUGIN_PATH_LIST: return "AR_SERVER_INFO_PLUGIN_PATH_LIST";
		case AR_SERVER_INFO_SHARED_LIB: return "AR_SERVER_INFO_SHARED_LIB";
		case AR_SERVER_INFO_SHARED_LIB_PATH: return "AR_SERVER_INFO_SHARED_LIB_PATH";
		case AR_SERVER_INFO_CMDB_INSTALL_DIR: return "AR_SERVER_INFO_CMDB_INSTALL_DIR";
		case AR_SERVER_INFO_RE_LOG_DIR: return "AR_SERVER_INFO_RE_LOG_DIR";
		case AR_SERVER_INFO_LOG_TO_FORM: return "AR_SERVER_INFO_LOG_TO_FORM";
		case AR_SERVER_INFO_SQL_LOG_FORM: return "AR_SERVER_INFO_SQL_LOG_FORM";
		case AR_SERVER_INFO_API_LOG_FORM: return "AR_SERVER_INFO_API_LOG_FORM";
		case AR_SERVER_INFO_ESCL_LOG_FORM: return "AR_SERVER_INFO_ESCL_LOG_FORM";
		case AR_SERVER_INFO_FILTER_LOG_FORM: return "AR_SERVER_INFO_FILTER_LOG_FORM";
		case AR_SERVER_INFO_USER_LOG_FORM: return "AR_SERVER_INFO_USER_LOG_FORM";
		case AR_SERVER_INFO_ALERT_LOG_FORM: return "AR_SERVER_INFO_ALERT_LOG_FORM";
		case AR_SERVER_INFO_SVRGRP_LOG_FORM: return "AR_SERVER_INFO_SVRGRP_LOG_FORM";
		case AR_SERVER_INFO_FTINDX_LOG_FORM: return "AR_SERVER_INFO_FTINDX_LOG_FORM";
		case AR_SERVER_INFO_THREAD_LOG_FORM: return "AR_SERVER_INFO_THREAD_LOG_FORM";
		case AR_SERVER_INFO_FIPS_SERVER_MODE: return "AR_SERVER_INFO_FIPS_SERVER_MODE";
		case AR_SERVER_INFO_FIPS_CLIENT_MODE: return "AR_SERVER_INFO_FIPS_CLIENT_MODE";
		case AR_SERVER_INFO_FIPS_STATUS: return "AR_SERVER_INFO_FIPS_STATUS";
		case AR_SERVER_INFO_ENC_LEVEL: return "AR_SERVER_INFO_ENC_LEVEL";
		case AR_SERVER_INFO_ENC_ALGORITHM: return "AR_SERVER_INFO_ENC_ALGORITHM";
		case AR_SERVER_INFO_FIPS_MODE_INDEX: return "AR_SERVER_INFO_FIPS_MODE_INDEX";
		case AR_SERVER_INFO_FIPS_DUAL_MODE_INDEX: return "AR_SERVER_INFO_FIPS_DUAL_MODE_INDEX";
		case AR_SERVER_INFO_ENC_LEVEL_INDEX: return "AR_SERVER_INFO_ENC_LEVEL_INDEX";
		case AR_SERVER_INFO_DSO_MAIN_POLL_INTERVAL: return "AR_SERVER_INFO_DSO_MAIN_POLL_INTERVAL";
		case AR_SERVER_INFO_RECORD_OBJECT_RELS: return "AR_SERVER_INFO_RECORD_OBJECT_RELS";
		case AR_SERVER_INFO_LICENSE_USAGE: return "AR_SERVER_INFO_LICENSE_USAGE";
		case AR_SERVER_INFO_COMMON_LOG_FORM: return "AR_SERVER_INFO_COMMON_LOG_FORM";
		case AR_SERVER_INFO_LOG_FORM_SELECTED: return "AR_SERVER_INFO_LOG_FORM_SELECTED";
		case AR_SERVER_INFO_MAX_CLIENT_MANAGED_TRANSACTIONS: return "AR_SERVER_INFO_MAX_CLIENT_MANAGED_TRANSACTIONS";
		case AR_SERVER_INFO_CLIENT_MANAGED_TRANSACTION_TIMEOUT: return "AR_SERVER_INFO_CLIENT_MANAGED_TRANSACTION_TIMEOUT";
		case AR_SERVER_INFO_OBJ_RESERVATION_MODE: return "AR_SERVER_INFO_OBJ_RESERVATION_MODE";
		case AR_SERVER_INFO_NEW_ENC_PUB_KEY_EXP: return "AR_SERVER_INFO_NEW_ENC_PUB_KEY_EXP";
		case AR_SERVER_INFO_NEW_ENC_DATA_KEY_EXP: return "AR_SERVER_INFO_NEW_ENC_DATA_KEY_EXP";
		case AR_SERVER_INFO_NEW_ENC_DATA_ALG: return "AR_SERVER_INFO_NEW_ENC_DATA_ALG";
		case AR_SERVER_INFO_NEW_ENC_SEC_POLICY: return "AR_SERVER_INFO_NEW_ENC_SEC_POLICY";
		case AR_SERVER_INFO_NEW_FIPS_SERVER_MODE: return "AR_SERVER_INFO_NEW_FIPS_SERVER_MODE";
		case AR_SERVER_INFO_NEW_ENC_LEVEL: return "AR_SERVER_INFO_NEW_ENC_LEVEL";
		case AR_SERVER_INFO_NEW_ENC_ALGORITHM: return "AR_SERVER_INFO_NEW_ENC_ALGORITHM";
		case AR_SERVER_INFO_NEW_FIPS_MODE_INDEX: return "AR_SERVER_INFO_NEW_FIPS_MODE_INDEX";
		case AR_SERVER_INFO_NEW_ENC_LEVEL_INDEX: return "AR_SERVER_INFO_NEW_ENC_LEVEL_INDEX";
		case AR_SERVER_INFO_NEW_ENC_PUB_KEY: return "AR_SERVER_INFO_NEW_ENC_PUB_KEY";
		case AR_SERVER_INFO_CUR_ENC_PUB_KEY: return "AR_SERVER_INFO_CUR_ENC_PUB_KEY";
		case AR_SERVER_INFO_NEW_ENC_PUB_KEY_INDEX: return "AR_SERVER_INFO_NEW_ENC_PUB_KEY_INDEX";
		case AR_SERVER_INFO_CURRENT_ENC_SEC_POLICY: return "AR_SERVER_INFO_CURRENT_ENC_SEC_POLICY";
		case AR_SERVER_INFO_ENC_LIBRARY_LEVEL: return "AR_SERVER_INFO_ENC_LIBRARY_LEVEL";
		case AR_SERVER_INFO_NEW_FIPS_ALG: return "AR_SERVER_INFO_NEW_FIPS_ALG";
		case AR_SERVER_INFO_FIPS_ALG: return "AR_SERVER_INFO_FIPS_ALG";
		case AR_SERVER_INFO_FIPS_PUB_KEY: return "AR_SERVER_INFO_FIPS_PUB_KEY";
		case AR_SERVER_INFO_WFD_QUEUES: return "AR_SERVER_INFO_WFD_QUEUES";
		case AR_SERVER_INFO_VERCNTL_OBJ_MOD_LOG_MODE: return "AR_SERVER_INFO_VERCNTL_OBJ_MOD_LOG_MODE";
		case AR_SERVER_INFO_MAX_RECURSION_LEVEL: return "AR_SERVER_INFO_MAX_RECURSION_LEVEL";
		case AR_SERVER_INFO_FT_SERVER_NAME: return "AR_SERVER_INFO_FT_SERVER_NAME";
		case AR_SERVER_INFO_FT_SERVER_PORT: return "AR_SERVER_INFO_FT_SERVER_PORT";
		case AR_SERVER_INFO_DISABLE_AUDIT_ONLY_CHANGED_FIELDS: return "AR_SERVER_INFO_DISABLE_AUDIT_ONLY_CHANGED_FIELDS";
		case AR_SERVER_INFO_VERCNTL_OBJ_MOD_LOG_SAVE_DEF: return "AR_SERVER_INFO_VERCNTL_OBJ_MOD_LOG_SAVE_DEF";
		case AR_SERVER_INFO_SG_AIE_STATE: return "AR_SERVER_INFO_SG_AIE_STATE";
		case AR_SERVER_INFO_MAX_VENDOR_TEMP_TABLES: return "AR_SERVER_INFO_MAX_VENDOR_TEMP_TABLES";
		case AR_SERVER_INFO_DSO_LOG_LEVEL: return "AR_SERVER_INFO_DSO_LOG_LEVEL";
		case AR_SERVER_INFO_DS_PENDING_ERR: return "AR_SERVER_INFO_DS_PENDING_ERR";
		case AR_SERVER_INFO_REGISTRY_LOCATION: return "AR_SERVER_INFO_REGISTRY_LOCATION";
		case AR_SERVER_INFO_REGISTRY_USER: return "AR_SERVER_INFO_REGISTRY_USER";
		case AR_SERVER_INFO_REGISTRY_PASSWORD: return "AR_SERVER_INFO_REGISTRY_PASSWORD";
		case AR_SERVER_INFO_DSO_LOG_ERR_FORM: return "AR_SERVER_INFO_DSO_LOG_ERR_FORM";
		case AR_SERVER_INFO_ARSIGNALD_LOG_FILE: return "AR_SERVER_INFO_ARSIGNALD_LOG_FILE";
		case AR_SERVER_INFO_FIRE_ESCALATIONS: return "AR_SERVER_INFO_FIRE_ESCALATIONS";
		case AR_SERVER_INFO_PRELOAD_NUM_THREADS: return "AR_SERVER_INFO_PRELOAD_NUM_THREADS";
		case AR_SERVER_INFO_PRELOAD_NUM_SCHEMA_SEGS: return "AR_SERVER_INFO_PRELOAD_NUM_SCHEMA_SEGS";
		case AR_SERVER_INFO_PRELOAD_THREAD_INIT_ONLY: return "AR_SERVER_INFO_PRELOAD_THREAD_INIT_ONLY";
#endif
#endif
		default: return EnumDefault;
		}
	}

	static string AuditStyle(int nType)
	{
		switch (nType)
		{
		case AR_AUDIT_NONE: return "None";
		case AR_AUDIT_COPY: return "Form";
		case AR_AUDIT_LOG: return "Log";
		default: return EnumDefault;
		}
	}

#if AR_CURRENT_API_VERSION > 13 // Version 7.5 and higher
	static string AuditChangedFields(int nType)
	{
		switch (nType)
		{
		case AR_AUDIT_ONLY_CHNG_FLDS_DEFAULT: return "Default";
		case AR_AUDIT_ONLY_CHNG_FLDS_YES: return "Yes";
		case AR_AUDIT_ONLY_CHNG_FLDS_NO: return "No";
		default: return EnumDefault;
		}
	}
#endif
};

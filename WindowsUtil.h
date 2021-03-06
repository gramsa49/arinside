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
#include "AppConfig.h"

class CWindowsUtil
{
public:
	CWindowsUtil(AppConfig &appConfig);
	~CWindowsUtil(void);

	void Load();
	bool CreateAppDirectory();
	int CreateSubDirectory(string name);
	static std::string GetRealPathName(const std::string &path);

private:
	void LoadFromResource(unsigned int res, string fileName, string path);
	AppConfig appConfig;
};

#ifndef WIN32
extern "C" {
	extern char _binary_res_active_link_gif_start[];	extern unsigned int _binary_res_active_link_gif_size[];
	extern char _binary_res_al_guide_gif_start[]; extern unsigned int _binary_res_al_guide_gif_size[];
	extern char _binary_res_application_gif_start[]; extern unsigned int _binary_res_application_gif_size[];
	extern char _binary_res_ars_edit_gif_start[]; extern unsigned int _binary_res_ars_edit_gif_size[];
	extern char _binary_res_chapter_gif_start[]; extern unsigned int _binary_res_chapter_gif_size[];
	extern char _binary_res_disabled_gif_start[]; extern unsigned int _binary_res_disabled_gif_size[];
	extern char _binary_res_doc_gif_start[]; extern unsigned int _binary_res_doc_gif_size[];
	extern char _binary_res_down_gif_start[]; extern unsigned int _binary_res_down_gif_size[];
	extern char _binary_res_down2_gif_start[]; extern unsigned int _binary_res_down2_gif_size[];
	extern char _binary_res_down2_on_gif_start[]; extern unsigned int _binary_res_down2_on_gif_size[];
	extern char _binary_res_edit_gif_start[]; extern unsigned int _binary_res_edit_gif_size[];
	extern char _binary_res_empty_list_gif_start[]; extern unsigned int _binary_res_empty_list_gif_size[];
	extern char _binary_res_escalation_gif_start[]; extern unsigned int _binary_res_escalation_gif_size[];
	extern char _binary_res_filter_gif_start[]; extern unsigned int _binary_res_filter_gif_size[];
	extern char _binary_res_filter_guide_gif_start[]; extern unsigned int _binary_res_filter_guide_gif_size[];
	extern char _binary_res_folder_gif_start[]; extern unsigned int _binary_res_folder_gif_size[];
	extern char _binary_res_group_gif_start[]; extern unsigned int _binary_res_group_gif_size[];
	extern char _binary_res_hidden_gif_start[]; extern unsigned int _binary_res_hidden_gif_size[];
	extern char _binary_res_image_gif_start[]; extern unsigned int _binary_res_image_gif_size[];
	extern char _binary_res_info_gif_start[]; extern unsigned int _binary_res_info_gif_size[];
	extern char _binary_res_menu_gif_start[]; extern unsigned int _binary_res_menu_gif_size[];
	extern char _binary_res_next_gif_start[]; extern unsigned int _binary_res_next_gif_size[];
	extern char _binary_res_packing_list_gif_start[]; extern unsigned int _binary_res_packing_list_gif_size[];
	extern char _binary_res_prev_gif_start[]; extern unsigned int _binary_res_prev_gif_size[];
	extern char _binary_res_schema_gif_start[]; extern unsigned int _binary_res_schema_gif_size[];
	extern char _binary_res_script_js_start[]; extern unsigned int _binary_res_script_js_size[];
	extern char _binary_res_search_gif_start[]; extern unsigned int _binary_res_search_gif_size[];
	extern char _binary_res_server_gif_start[]; extern unsigned int _binary_res_server_gif_size[];
	extern char _binary_res_sort_asc_gif_start[]; extern unsigned int _binary_res_sort_asc_gif_size[];
	extern char _binary_res_sort_desc_gif_start[]; extern unsigned int _binary_res_sort_desc_gif_size[];
	extern char _binary_res_filtermenu_js_start[]; extern unsigned int _binary_res_filtermenu_js_size[];
	extern char _binary_res_prototype_js_start[]; extern unsigned int _binary_res_prototype_js_size[];
	extern char _binary_res_sortscript_js_start[]; extern unsigned int _binary_res_sortscript_js_size[];
	extern char _binary_res_style_css_start[]; extern unsigned int _binary_res_style_css_size[];
	extern char _binary_res_tabscript_js_start[]; extern unsigned int _binary_res_tabscript_js_size[];
	extern char _binary_res_up_gif_start[]; extern unsigned int _binary_res_up_gif_size[];
	extern char _binary_res_user_gif_start[]; extern unsigned int _binary_res_user_gif_size[];
	extern char _binary_res_visible_gif_start[]; extern unsigned int _binary_res_visible_gif_size[];
	extern char _binary_res_webservice_gif_start[]; extern unsigned int _binary_res_webservice_gif_size[];
};
#endif
//Copyright (C) 2012 John Luthgers | jls17
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
#include "ScanMenus.h"
#include "../ARInside.h"

CScanMenus::CScanMenus(CARCharMenu& menu)
: mnu(menu)
{
}

CScanMenus::~CScanMenus(void)
{
}

void CScanMenus::Start()
{
	cout << "Checking menu references";

	CARInside* pInside = CARInside::GetInstance();
	unsigned int menuCount = pInside->menuList.GetCount();
	for (unsigned int menuIndex = 0; menuIndex < menuCount ; ++menuIndex)
	{
		CARCharMenu menu(menuIndex);
		if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(menu))
			continue;

		int overlayType = menu.GetOverlayType();
		if (overlayType == AR_OVERLAY_OBJECT || overlayType == AR_CUSTOM_OBJECT)
		{
			pInside->menuList.AddOverlayOrCustom(menu.GetInsideId());
		}

		CScanMenus scanMenu(menu);
		scanMenu.Scan();
	}

	cout << endl;
}

void CScanMenus::Scan()
{
}

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

#include "stdafx.h"
#include "ARServerObject.h"
#include "../output/WebUtil.h"
#include "../ARInside.h"

using namespace OUTPUT;

int CARServerObject::GetOverlayType() const
{
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
	if (CARInside::GetInstance()->appConfig.bOverlaySupport)
	{
		ARValueStruct* overlayProp = CARProplistHelper::Find(GetPropList(), AR_SMOPROP_OVERLAY_PROPERTY);
		if (overlayProp != NULL && overlayProp->dataType == AR_DATA_TYPE_INTEGER)
		{
			return overlayProp->u.intVal;
		}
	}
#endif
	return 0; // AR_ORIGINAL_OBJECT
}

int CARServerObject::GetOverlayGroup() const
{
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
	if (CARInside::GetInstance()->appConfig.bOverlaySupport)
	{
		ARValueStruct* overlayProp = CARProplistHelper::Find(GetPropList(), AR_SMOPROP_OVERLAY_GROUP);
		if (overlayProp != NULL && overlayProp->dataType == AR_DATA_TYPE_INTEGER)
		{
			return overlayProp->u.intVal;
		}
	}
#endif
	return -1; // No Overlay Group / Base
}

// NOTE: those functions couldn't be used for sub-objects like fields or views.
string CARServerObject::GetOverlayBaseName() const
{
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
	return GetName() + (CARInside::GetInstance()->overlayMode == 1 ? AR_RESERV_OVERLAY_STRING : "");
#else
	return "";
#endif
}

string CARServerObject::GetOverlayName() const
{
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
	return GetName() + (CARInside::GetInstance()->overlayMode == 0 ? AR_RESERV_OVERLAY_STRING : "");
#else
	return "";
#endif
}

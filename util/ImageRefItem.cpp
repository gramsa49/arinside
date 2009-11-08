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
#include "ImageRefItem.h"

CImageRefItem::CImageRefItem(int imageIndex, const string &description, /*int arsStructItemType, int schemaInsideId, int fieldId*/ CARServerObject *fromItem)
: fromItem(fromItem)
{
	this->imageIndex = imageIndex;
	this->description = description;
	//this->arsStructItemType = arsStructItemType;
	//this->schemaInsideId = schemaInsideId;
	//this->fromFieldId = fieldId;

}

CImageRefItem::~CImageRefItem(void)
{
}

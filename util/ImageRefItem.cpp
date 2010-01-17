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

// storing a pointer of CARServerObject isnt working anymore, because such objects are created and destroyed dynamically at runtime.
// implement a virtual "Clone" method in CARServerObject and in each derived class that copies a object via new and returns a ptr.
// or find another solution...

CImageRefItem::CImageRefItem(const CImageRefItem& from)
{
	this->imageIndex = from.imageIndex;
	this->description = from.description;

	if (from.fromItem != NULL)
		this->fromItem = from.fromItem->Clone();
	else
		this->fromItem = NULL;
}

CImageRefItem::CImageRefItem(int imageIndex, const string &description, /*int arsStructItemType, int schemaInsideId, int fieldId*/ CARServerObject *fromItem)
{
	this->imageIndex = imageIndex;
	this->description = description;

	if (fromItem != NULL)
		this->fromItem = fromItem->Clone();
	else
		this->fromItem = NULL;
}

CImageRefItem::~CImageRefItem(void)
{
	if (this->fromItem != NULL && this->fromItem->IsClonable())
		delete fromItem;
}

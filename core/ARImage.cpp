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
#include "../ARApi.h"
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
#include "ARImage.h"
#include "../ARInside.h"

bool CARImage::IsClonable() const
{
	return true;
}

CARServerObject* CARImage::Clone() const
{
	return new CARImage(*this);
}

string CARImage::GetName()
{
	return CARInside::GetInstance()->imageList.ImageGetName(GetInsideId());
}

string CARImage::GetName() const
{
	return CARInside::GetInstance()->imageList.ImageGetName(GetInsideId());
}

string CARImage::GetNameFirstChar()
{
	return CUtil::String2Comp(std::string(1, CARInside::GetInstance()->imageList.ImageGetName(GetInsideId())[0]));
}

bool CARImage::NameStandardFirstChar()
{
	return CARObject::NameStandardFirstChar(GetNameFirstChar());
}

const char* CARImage::GetHelpText() const
{ 
	return CARInside::GetInstance()->imageList.ImageGetHelptext(GetInsideId()); 
}

ARTimestamp CARImage::GetTimestamp()
{
	return CARInside::GetInstance()->imageList.ImageGetTimestamp(GetInsideId()); 
}

const ARAccessNameType& CARImage::GetOwner() const
{
	return CARInside::GetInstance()->imageList.ImageGetOwner(GetInsideId()); 
}

const ARAccessNameType& CARImage::GetLastChanged() const
{
	return CARInside::GetInstance()->imageList.ImageGetModifiedBy(GetInsideId()); 
}

const char* CARImage::GetChangeDiary() const
{
	return CARInside::GetInstance()->imageList.ImageGetChangeDiary(GetInsideId()); 
}

char* CARImage::GetType() const
{
	return CARInside::GetInstance()->imageList.ImageGetType(GetInsideId());
}

char* CARImage::GetDescription() const
{
	return CARInside::GetInstance()->imageList.ImageGetDescription(GetInsideId());
}

const ARImageDataStruct& CARImage::GetData() const
{
	return CARInside::GetInstance()->imageList.ImageGetData(GetInsideId());
}

string CARImage::GetURL(int rootLevel, bool showImage)
{
	// TODO: add support for the showImage param
	throw CARInside::GetInstance()->imageList.ImageGetURL(GetInsideId(), rootLevel);
}
#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
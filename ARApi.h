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
#include <ar.h>
#include <arfree.h>
#include <arstruct.h>
#include <arextern.h>

// this is just a little workaround for a bug in ar.h version 7.5
#ifndef AR_FILTER_ERRHANDLER_ENABLE
#ifdef AR_ERRHANDLER_ENABLE
#define AR_FILTER_ERRHANDLER_ENABLE AR_ERRHANDLER_ENABLE
#else
#define AR_FILTER_ERRHANDLER_ENABLE 1
#endif
#endif

// to support our own xml object types used in lists
#define AR_STRUCT_ITEM_USER    500
#define AR_STRUCT_ITEM_GROUP   501
#define AR_STRUCT_ITEM_ROLE    502

#define AR_STRUCT_ITEM_XML_USER       (AR_STRUCT_XML_OFFSET | AR_STRUCT_ITEM_USER)
#define AR_STRUCT_ITEM_XML_GROUP      (AR_STRUCT_XML_OFFSET | AR_STRUCT_ITEM_GROUP)
#define AR_STRUCT_ITEM_XML_ROLE       (AR_STRUCT_XML_OFFSET | AR_STRUCT_ITEM_ROLE)

// api version definitions (see AR_CURRENT_API_VERSION in ar.h)
#define AR_API_VERSION_450 7
#define AR_API_VERSION_500 8			// TODO: check this again !!
#define AR_API_VERSION_510 9
#define AR_API_VERSION_600 10
#define AR_API_VERSION_630 11
#define AR_API_VERSION_700 12
#define AR_API_VERSION_710 13
#define AR_API_VERSION_750 14
#define AR_API_VERSION_763 17

// see AR_EXPORT_VERSION in arstruct.h for definition
#define AR_XML_VERSION_450 5
#define AR_XML_VERSION_500 6
#define AR_XML_VERSION_510 7
#define AR_XML_VERSION_600 8
#define AR_XML_VERSION_630 8
#define AR_XML_VERSION_700 9
#define AR_XML_VERSION_710 10
#define AR_XML_VERSION_750 11

// template for clearing memory of AR structs
template<class T>
void ARZeroMemory(T* obj) { memset(obj, 0, sizeof(T)); }

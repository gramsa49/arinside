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

#define AR_API_VERSION_450 7
#define AR_API_VERSION_500 8			// TODO: check this again !!
#define AR_API_VERSION_510 9			// TODO: check this again !!
#define AR_API_VERSION_600 10
#define AR_API_VERSION_630 11
#define AR_API_VERSION_700 12
#define AR_API_VERSION_710 13
#define AR_API_VERSION_750 14


//Copyright (C) 2014 John Luthgers | jls17
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
//    along with ARInside. If not, see <http://www.gnu.org/licenses/>.

#include "ARInsideMain.h"
#include "CrashHandler.h"

#ifdef ARINSIDE_BREAKPAD_SUPPORT

// we need to include platform specific header file, because
// each platform has it's own solution and implementation

#ifdef WIN32
#include <client/windows/handler/exception_handler.h>
#elif __linux__
#include <client/linux/handler/exception_handler.h>
#else
#error CrashHandler not supported/tested on this platform! Please disabled breakpad-support in CMakeList.txt
#endif

using namespace google_breakpad;

#ifdef WIN32
std::wstring GetWinTempPath()
{
	// get length of temp path
	DWORD nPathSize = GetTempPath(0, NULL);
	if (nPathSize == 0) 
	{
		cerr << "error " << GetLastError() << ": failed to get size of temp path! Crash dump support disabled!";
		return L"";
	}

	// get the temp path
	std::vector<TCHAR> buffer(nPathSize);
	LPTSTR path = &buffer[0];
	DWORD result = GetTempPath(nPathSize, path);
	if (result == 0 || result > nPathSize)
	{
		cerr << "error " << GetLastError() << ": failed to get temp path! Crash dump support disabled!";
		return L"";
	}
	
	return std::wstring(path);
}
#endif

/*
###############################################################################
      Implementation of the callback function
*/
#ifdef WIN32
bool BreakpadMinidumpCallback(
				const wchar_t* dump_path,
				const wchar_t* minidump_id,
				void* context, EXCEPTION_POINTERS* exinfo,
				MDRawAssertionInfo* assertion,
				bool succeeded)
#elif __linux__
bool BreakpadMinidumpCallback(
				const MinidumpDescriptor &descriptor, 
				void *context, 
				bool succeeded)
#endif
{
	cout << "Application crash! Mini dump written to: ";
	cout.flush();
#ifdef WIN32
  wcout << dump_path << minidump_id << L".dmp";
#else
  cout << descriptor.path();
#endif
  cout << endl;
	return true;
}

/*
###############################################################################
      CrashHandler implementation
*/

CrashHandler::CrashHandler()
{
	handler = NULL;
#ifdef WIN32
	wstring path = GetWinTempPath();
	if (path.empty()) return;
	handler = new ExceptionHandler(path, NULL, BreakpadMinidumpCallback, NULL, ExceptionHandler::HANDLER_ALL);
#elif __linux__
	MinidumpDescriptor descriptor("/tmp");
	handler = new ExceptionHandler(descriptor, NULL, BreakpadMinidumpCallback, NULL, true, -1);
#endif
}

CrashHandler::~CrashHandler()
{
	delete handler;
}

#endif // ARINSIDE_BREAKPAD_SUPPORT

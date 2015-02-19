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
//    along with ARInside. If not, see <http://www.gnu.org/licenses/>.

#include "ARInsideMain.h"
#include "Main.h"
#include "CrashHandler.h"

int main(int argc, char* argv[])
{
	void *pCrashHandler =
#ifdef ARINSIDE_BREAKPAD_SUPPORT
		new CrashHandler();
#else
		NULL;
#endif
	CMain theMain;
	theMain.SetCrashHandler(pCrashHandler);
	return theMain.Run(argc, argv);

}

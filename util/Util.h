
/****************************************************************************** 
 * 
 *  file:  Util.h
 * 
 *  Copyright (c) 2007, Stefan Nerlich | stefan.nerlich@hotmail.com 
 *  All rights reverved.
 * 
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *  
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.  
 *  
 *****************************************************************************/

#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <time.h>
#include <list>
#include <vector>
#include <algorithm>

#include <ar.h>

using namespace std;

class CUtil
{
public:
	CUtil(void);
public:
	~CUtil(void);	
	static string TimeToString(ARTimestamp ts);
	static string TimeToTextString(ARTimestamp ts);
	static string ClearSpaces(string strIn);
	static string String2Comp(string strIn);
	static string StrReplace(const std::string &search, const std::string &replace, std::string searchin);
	//static int SplitString(const string& input, const string& delimiter, vector<string>& results, bool includeEmpties);	

	static int SplitString(const std::string& input_record, std::vector<std::string>& colv);
	static void CleanUpStream(stringstream &strm, unsigned int nLength);
};

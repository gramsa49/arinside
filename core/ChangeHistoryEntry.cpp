
/****************************************************************************** 
 * 
 *  file:  ChangeHistoryEntry.cpp
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

#include "StdAfx.h"
#include "ChangeHistoryEntry.h"

CChangeHistoryEntry::CChangeHistoryEntry(void)
{
	this->ts = 0;
	this->user = "";
	this->entry = "";
}

CChangeHistoryEntry::CChangeHistoryEntry(ARTimestamp ts, string user, string entry)
{
	this->ts = ts;
	this->user = user;
	this->entry = entry;
}

CChangeHistoryEntry::~CChangeHistoryEntry(void)
{
}

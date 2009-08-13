
/****************************************************************************** 
 * 
 *  file:  AppException.h
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
#include <exception>
#include <string>

class AppException : public std::exception
{
	public:
	
		/**
		 * Constructor.
		 * \param text - The text of the exception.
		 * \param id - The text identifying the argument source.
		 * \param td - Text describing the type of ArgException it is.
		 * of the exception.
		 */
		AppException( const std::string& text = "undefined exception", 
					  const std::string& id = "undefined",
					  const std::string& td = "Generic AppException")
			: std::exception(), 
			  _errorText(text), 
			  _argId( id ), 
			  _typeDescription(td)
		{ 
		} 
		
		virtual ~AppException() throw() 
		{ 
		}

		string error() const 
		{ 
			return ( _errorText ); 
		}

		std::string argId() const  
		{ 
			if ( _argId == "undefined" )
				return " ";
			else
				return ( "Argument: " + _argId ); 
		}

		const char* what() const throw() 
		{
			static std::string ex; 
			ex = _argId + " -- " + _errorText;
			return ex.c_str();
		}
		
		std::string typeDescription() const
		{
			return _typeDescription; 
		}


	private:
		std::string _errorText;
		std::string _argId;
		std::string _typeDescription;
};

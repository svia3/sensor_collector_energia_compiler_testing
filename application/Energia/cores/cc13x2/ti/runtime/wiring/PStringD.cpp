/*
  PString.cpp - Lightweight printable string class
  Copyright (c) 2009-2012 Mikal Hart.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 * Put 'format' into its own file to avoid always bringing in System_vsnprintf()
 */
#include "PString.h"
#include <stdarg.h>
#include <stdio.h>
//#include <xdc/runtime/System.h>

int PString::format(const char *str, ...) 
{ 
    va_list argptr;  
    va_start(argptr, str); 
    int ret = vsnprintf(_cur, _size - (_cur - _buf), str, argptr);
    if (_size) {
        while (*_cur) {
            ++_cur;
        }
    }
    return ret;
}


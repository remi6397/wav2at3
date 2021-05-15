/* Copyright (c) 1993-2021 the Wine project authors (see the file AUTHORS
 * .wine for a complete list)
 * 
 * Wine is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * A copy of the GNU Lesser General Public License is included in the
 * Wine distribution in the file COPYING.LIB. If you did not receive this
 * copy, write to the Free Software Foundation, Inc., 51 Franklin St,
 * Fifth Floor, Boston, MA 02110-1301, USA. */
#pragma once

#include <windows.h> 

LPSTR* CommandLineToArgvA(LPSTR lpCmdLine, INT *pNumArgs);

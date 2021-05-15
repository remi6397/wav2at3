/* wav2at3
 * Copyright (C) 2021  Jeremiasz Nelz <jeremiasz ~at~ nelz ~dot~ pl>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */
#include "findat3driver.h"

static BOOL WINAPI findAt3Driver_cb(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport) {
	ACMDRIVERDETAILS details;
	details.cbStruct = sizeof(ACMDRIVERDETAILS);
	acmDriverDetails(hadid, &details, 0);

	if (strcmp(details.szShortName, "ATRAC3") == 0) {
		*(LPHACMDRIVERID)dwInstance = hadid;
		return FALSE;
	}

	return TRUE;
}

void findAt3Driver(LPHACMDRIVERID lpHadid) {
	acmDriverEnum(findAt3Driver_cb, (DWORD_PTR)lpHadid, 0);
}

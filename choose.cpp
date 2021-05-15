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
#include "choose.h"

#include <iostream>

struct chooseFormat_data {
	const char *formatName;
	HACMDRIVER had;
	LPACMFORMATDETAILS pafd;
	LPWAVEFORMATEX pwfx;
	BOOL matched;
};

static BOOL WINAPI chooseFormat_cb(HACMDRIVERID hadid, LPACMFORMATDETAILS pafd, DWORD_PTR dwInstance, DWORD fdwSupport) {
	chooseFormat_data *cd = (chooseFormat_data*)dwInstance;

	if (strcmp(pafd->szFormat, cd->formatName) == 0) {
		*(cd->pwfx) = *(pafd->pwfx);
		cd->matched = TRUE;
		return FALSE;
	}

	return TRUE;
}

BOOL chooseFormat(HACMDRIVER driver, LPWAVEFORMATEX pwfx, DWORD dwFormatTag, const char *formatName) {
	ACMFORMATDETAILS afd;
	afd.cbStruct = sizeof(ACMFORMATDETAILS);
	
	afd.fdwSupport = 0;
	afd.dwFormatTag = dwFormatTag;
	WAVEFORMATEX wfx;
	afd.pwfx = &wfx;

	chooseFormat_data cd;
	cd.formatName = formatName;
	cd.had = driver;
	cd.pafd = &afd;
	cd.pwfx = pwfx;
	cd.matched = FALSE;

	acmFormatEnum(driver, &afd, chooseFormat_cb, (DWORD_PTR)&cd, 0);

	return cd.matched;
}

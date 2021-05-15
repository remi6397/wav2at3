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
#include <unistd.h>

#include <windows.h>
#include <mmsystem.h>
#include <msacmdrv.h>
#include <msacm.h>

#include <iostream>

#include "util.h"
#include "choose.h"
#include "findat3driver.h"

BOOL WINAPI printFormatList_cb(HACMDRIVERID hadid, LPACMFORMATDETAILS pafd, DWORD_PTR dwInstance, DWORD fdwSupport) {
	std::cerr << "- " << pafd->szFormat << std::endl;

	return TRUE;
}

void printFormatList(HACMDRIVERID hadid, DWORD dwFormatTag) {
	ACMDRIVERDETAILS details;
	details.cbStruct = sizeof(ACMDRIVERDETAILS);
	acmDriverDetails(hadid, &details, 0);
	std::cerr << "Formats for " << details.szShortName << " (" << details.szLongName << "):" << std::endl;

	HACMDRIVER driver;
	acmDriverOpen(&driver, hadid, 0);

	ACMFORMATDETAILS afd;
	afd.cbStruct = sizeof(ACMFORMATDETAILS);
	
	afd.fdwSupport = 0;
	afd.dwFormatTag = dwFormatTag;
	WAVEFORMATEX wfx;
	afd.pwfx = &wfx;
	acmMetrics((HACMOBJ)driver, ACM_METRIC_MAX_SIZE_FORMAT, &afd.cbwfx);
	acmFormatEnum(driver, &afd, printFormatList_cb, 0, 0);
	acmDriverClose(driver, 0);
}

bool matchWfx(HACMDRIVERID hadid, const char *formatName, DWORD dwFormatTag, LPWAVEFORMATEX pwfx) {
	HACMDRIVER driver;
	if (acmDriverOpen(&driver, hadid, 0)) {
		std::cerr << "acmDriverOpen() failed" << std::endl;
		return false;
	}

	if (!chooseFormat(driver, pwfx, dwFormatTag, formatName)) {
		std::cerr << "Format \"" << formatName << "\" not available" << std::endl;
		return false;
	}

	acmDriverClose(driver, 0);

	return true;
}

int main() {
	int argc;
	auto argv = CommandLineToArgvA(GetCommandLine(), &argc);
	int c;
	bool listThenExit = false;
	std::string input;
	std::string output;
	std::string format;

	while ((c = getopt(argc, argv, "li:o:f:")) != -1) {
		switch (c) {
		case 'l':
			listThenExit = true;
			break;
		case 'i':
			input = std::string(optarg);
			break;
		case 'o':
			output = std::string(optarg);
			break;
		case 'f':
			format = std::string(optarg);
			break;
		case '?':
			return 1;
		default:
			abort();
		}
	}
	HACMDRIVERID at3hadid = nullptr;
	findAt3Driver(&at3hadid);
	if (!at3hadid) {
		std::cerr << "ATRAC3 codec not installed" << std::endl;
		return 1;
	}
	if (listThenExit) {
		printFormatList(at3hadid, WAVE_FORMAT_SONY_SCX);
		return 0;
	}
	if (input.empty()) {
		std::cerr << "Input not specified (use `-i')" << std::endl;
		return 1;
	}
	if (output.empty()) {
		std::cerr << "Output not specified (use `-o')" << std::endl;
		return 1;
	}
	if (format.empty()) {
		std::cerr << "Format not specified (use `-f')" << std::endl;
		return 1;
	}

	FILE *sfp = fopen(input.c_str(), "rb");
	if (!sfp) {
		std::cerr << "Cannot open input file" << std::endl;
		return 0;
	}

	unsigned char *bFmt = nullptr, *bData = nullptr;
	unsigned int sizeFmt = 0, sizeData = 0;

	char cb[12];
	fread(cb, 1, 12, sfp);
	if (memcmp(cb, "RIFF", 4) || memcmp(&cb[8], "WAVE", 4)) {
		std::cerr << "Not a WAV" << std::endl;
		return 1;
	}

	while (!feof(sfp)) {
		char magic[4];
		DWORD32 blockSize;
		fread(magic, 1, 4, sfp);
		if (feof(sfp)) break;
		fread(&blockSize, 4, 1, sfp);
		if (feof(sfp)) break;

		if (memcmp(magic, "fmt ", 4) == 0 && !bFmt) {
			bFmt = new unsigned char[blockSize];
			sizeFmt = blockSize;
			fread(bFmt, 1, sizeFmt, sfp);
		}
		else if (memcmp(magic, "data", 4) == 0 && !bData) {
			bData = new unsigned char[blockSize];
			sizeData = blockSize;
			fread(bData, 1, sizeData, sfp);
		}
		else {
			fseek(sfp, blockSize, SEEK_CUR);
		}
	}

	if (sizeFmt != 0x10) {
		std::cerr << "Unknown fmt size" << std::endl;
		return 1;
	}

	PCMWAVEFORMAT *pStoredFmt = (PCMWAVEFORMAT*)bFmt;

	char dwfxBuf[0x20];
	memset(dwfxBuf, 0, 0x20);
	WAVEFORMATEX swfx, *dwfx;
	memset(&swfx, 0, sizeof(WAVEFORMATEX));
	dwfx = (PWAVEFORMATEX)dwfxBuf;
	if (!matchWfx(at3hadid, format.c_str(), WAVE_FORMAT_SONY_SCX, dwfx))
		return 1;

	swfx.wFormatTag = pStoredFmt->wf.wFormatTag;
	swfx.nChannels = pStoredFmt->wf.nChannels;
	swfx.nSamplesPerSec = pStoredFmt->wf.nSamplesPerSec;
	swfx.nAvgBytesPerSec = pStoredFmt->wf.nAvgBytesPerSec;
	swfx.nBlockAlign = pStoredFmt->wf.nBlockAlign;
	swfx.wBitsPerSample = pStoredFmt->wBitsPerSample;
	swfx.cbSize = 0;

	memcpy(&dwfxBuf[0x12], "\x01\x00\x00\x10\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00", 0xE);

	HACMDRIVER at3had;
	if (acmDriverOpen(&at3had, at3hadid, 0)) {
		std::cerr << "Driver could not be opened" << std::endl;
		return 1;
	}

	HACMSTREAM stream;
	if(acmStreamOpen(&stream, at3had, &swfx, dwfx, nullptr, 0, 0, ACM_STREAMOPENF_NONREALTIME)) {
		std::cerr << "Initialization failed" << std::endl;
		return 1;
	}

	DWORD dlen;
	if (acmStreamSize(stream, sizeData, &dlen, ACM_STREAMSIZEF_SOURCE)) {
		acmStreamClose(stream, 0);
		std::cerr << "Could not infer stream size" << std::endl;
		return 1;
	}
	printf("streamSize recommends %08x\n", dlen);
	dlen += 0x400;
	unsigned char *dbuf = new unsigned char[dlen];

	ACMSTREAMHEADER header;
	memset(&header, 0, sizeof(header));
	header.cbStruct = sizeof(header);
	header.fdwStatus = 0;
	header.pbSrc = bData;
	header.cbSrcLength = sizeData;
	header.cbSrcLengthUsed = 0;
	header.pbDst = dbuf;
	header.cbDstLength = dlen;
	header.cbDstLengthUsed = 0;

	if (acmStreamPrepareHeader(stream, &header, 0)) {
		acmStreamClose(stream, 0);
		std::cerr << "Could not prepare header" << std::endl;
		return 1;
	}
	if (acmStreamConvert(stream, &header, ACM_STREAMCONVERTF_BLOCKALIGN)) {
		acmStreamUnprepareHeader(stream, &header, 0);
		acmStreamClose(stream, 0);
		std::cerr << "Conversion failed" << std::endl;
		return 1;
	}
	acmStreamUnprepareHeader(stream, &header, 0);
	acmStreamClose(stream, 0);

	printf("converted? %08x\n", header.cbDstLengthUsed);

	FILE *dfp = fopen(output.c_str(), "wb");
	if (!dfp) {
		std::cerr << "cannot open output file" << std::endl;
		return 1;
	}

	DWORD32 i = 0xC + 8 + 0x20 + 8 + header.cbDstLengthUsed;
	fwrite("RIFF", 1, 4, dfp);
	fwrite(&i, 4, 1, dfp);
	fwrite("WAVE", 1, 4, dfp);

	fwrite("fmt ", 1, 4, dfp);
	i = 0x20;
	fwrite(&i, 4, 1, dfp);
	fwrite(dwfxBuf, 1, 0x20, dfp);

	fwrite("data", 1, 4, dfp);
	i = header.cbDstLengthUsed;
	fwrite(&i, 4, 1, dfp);
	fwrite(dbuf, 1, header.cbDstLengthUsed, dfp);

	fclose(dfp);

	return 0;
}

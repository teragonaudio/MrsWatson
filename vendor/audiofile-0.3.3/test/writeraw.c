/*
	Audio File Library

	Copyright (C) 1998-1999, Michael Pruett <michael@68k.org>
	Copyright (C) 2002, Silicon Graphics, Inc.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be
	useful, but WITHOUT ANY WARRANTY; without even the implied
	warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
	PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public
	License along with this program; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
	MA 02111-1307, USA.
*/

/*
	writeraw.c

	This program tests the validity of the AIFF file reading and writing
	code.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TEST_FILE "/tmp/test.raw"

void cleanup (void)
{
#ifndef DEBUG
	unlink(TEST_FILE);
#endif
}

void ensure (int condition, const char *message)
{
	if (!condition)
	{
		printf("%s.\n", message);
		cleanup();
		exit(EXIT_FAILURE);
	}
}

int main (int argc, char **argv)
{
	AFfilehandle	file;
	AFfilesetup	setup;
	uint16_t	samples[] = {11, 51, 101, 501, 1001, 5001, 10001, 50001};
	int		i;
	int		sampleFormat, sampleWidth;
	int		framesRead, framesWritten;
	int		nativeByteOrder;

#ifdef WORDS_BIGENDIAN
	nativeByteOrder = AF_BYTEORDER_BIGENDIAN;
#else
	nativeByteOrder = AF_BYTEORDER_LITTLEENDIAN;
#endif

	setup = afNewFileSetup();
	afInitFileFormat(setup, AF_FILE_RAWDATA);
	afInitChannels(setup, AF_DEFAULT_TRACK, 1);
	afInitSampleFormat(setup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);

	file = afOpenFile(TEST_FILE, "w", setup);
	ensure(file != AF_NULL_FILEHANDLE, "unable to open file for writing");

	framesWritten = afWriteFrames(file, AF_DEFAULT_TRACK, samples, 8);
	ensure(framesWritten == 8,
		"number of frames written does not match number of frames requested");

	ensure(afCloseFile(file) == 0, "error closing file");

	file = afOpenFile(TEST_FILE, "r", setup);
	ensure(file != AF_NULL_FILEHANDLE, "unable to open file for reading");
	afFreeFileSetup(setup);

	ensure(afGetFileFormat(file, NULL) == AF_FILE_RAWDATA,
		"test file not created as raw audio data file");

	afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);
	ensure(sampleFormat == AF_SAMPFMT_TWOSCOMP,
		"test file not two's complement");
	ensure(sampleWidth == 16,
		"test file sample format is not 16-bit");

	ensure(afGetChannels(file, AF_DEFAULT_TRACK) == 1,
		"test file doesn't have exactly one channel");

	ensure(afGetByteOrder(file, AF_DEFAULT_TRACK) == nativeByteOrder,
		"test file not in native byte order");

	for (i=0; i<8; i++)
	{
		uint16_t	temporary;

		framesRead = afReadFrames(file, AF_DEFAULT_TRACK, &temporary, 1);
		ensure(framesRead == 1,
			"number of frames read does not match number of frames requested");

		ensure(temporary == samples[i],
			"data written to file doesn't match data read from file");
	}

	ensure(afCloseFile(file) == 0, "error closing file");

	cleanup();

	printf("writeraw test passed.\n");

	exit(EXIT_SUCCESS);
}

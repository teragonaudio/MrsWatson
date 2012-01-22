/*
	Audio File Library

	Copyright (C) 2001, Silicon Graphics, Inc.

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
	pipe.c

	This program provides a simple test of Audio File Library
	operation on non-seekable file handles.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <audiofile.h>

#define SAMPLE_COUNT 12
#define FRAME_COUNT 6

void ensure (int condition, const char *message)
{
	if (!condition)
	{
		printf("%s.\n", message);
		exit(EXIT_FAILURE);
	}
}

int main (int argc, char **argv)
{
	AFfilesetup	setup;
	AFfilehandle	file;
	int16_t		samples[SAMPLE_COUNT] = {-1,3,9,2,-5,4,8,-3,6,21,11,-2};
	int		output = 0;

	setup = afNewFileSetup();

	afInitFileFormat(setup, AF_FILE_RAWDATA);
	afInitSampleFormat(setup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);
	afInitChannels(setup, AF_DEFAULT_TRACK, 2);
	afInitRate(setup, AF_DEFAULT_TRACK, 44100);
#ifdef WORDS_BIGENDIAN
	afInitByteOrder(setup, AF_DEFAULT_TRACK, AF_BYTEORDER_BIGENDIAN);
#else
	afInitByteOrder(setup, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);
#endif

	if (argc > 1 && !strcmp(argv[1], "out"))
		output = 1;

	if (output)
	{
		AFframecount	framesWritten;

		file = afOpenFD(1, "w", setup);

		afFreeFileSetup(setup);

		framesWritten = afWriteFrames(file, AF_DEFAULT_TRACK, samples,
			FRAME_COUNT);

		ensure(framesWritten == FRAME_COUNT,
			"incorrect number of frames written");

		fprintf(stderr, "pipe write passed\n");
	}
	else
	{
		AFframecount	framesRead;
		int16_t		samplesRead[SAMPLE_COUNT];

		file = afOpenFD(0, "r", setup);

		afFreeFileSetup(setup);

		framesRead = afReadFrames(file, AF_DEFAULT_TRACK, samplesRead,
			FRAME_COUNT);

		ensure(framesRead == FRAME_COUNT,
			"incorrect number of frames read");

		ensure(memcmp(samplesRead, samples,
			SAMPLE_COUNT * sizeof (int16_t)) == 0,
			"samples read do not match samples written");

		fprintf(stderr, "pipe read passed\n");
	}

	afCloseFile(file);

	exit(EXIT_SUCCESS);
}

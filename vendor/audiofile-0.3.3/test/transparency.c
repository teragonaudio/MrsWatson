/*
	Audio File Library

	Copyright 1998-2000, Michael Pruett <michael@68k.org>

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
	transparency.c

	This program does a small sanity check on file writing
	operations.

	If this program fails, something in the Audio File Library is broken.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

void usage (void)
{
	fprintf(stderr, "usage: transparency file1 file2 [format]\n");
	fprintf(stderr, "where format is one of aiff, aifc, next, or wave\n");
	fprintf(stderr, "(format defaults to aiff.)\n");
	exit(0);
}

int main (int argc, char **argv)
{
	AFfilehandle	outfile;
	AFfilesetup	outfilesetup;
	unsigned short	buffer[] = {0x1122, 0x3344, 0x5566, 0x7788,
				    0x99aa, 0xbbcc, 0xddee, 0xff00};
	int		format = AF_FILE_AIFF;
	AFframecount	result;

	if (argc < 3)
		usage();

	if (argc > 3)
	{
		if (!strcmp(argv[3], "aiff"))
			format = AF_FILE_AIFF;
		else if (!strcmp(argv[3], "aifc"))
			format = AF_FILE_AIFFC;
		else if (!strcmp(argv[3], "next"))
			format = AF_FILE_NEXTSND;
		else if (!strcmp(argv[3], "wave"))
			format = AF_FILE_WAVE;
		else
		{
			fprintf(stderr, "%s: invalid format.\n", argv[3]);
			usage();
		}
	}

	outfilesetup = afNewFileSetup();
	afInitFileFormat(outfilesetup, format);

	outfile = afOpenFile(argv[1], "w", outfilesetup);
	if (outfile == AF_NULL_FILEHANDLE)
	{
		fprintf(stderr, "could not open file %s for writing\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	result = afWriteFrames(outfile, AF_DEFAULT_TRACK, buffer, 2);
	if (result != 2)
	{
		fprintf(stderr, "afWriteFrames did not return expected result\n");
		fprintf(stderr, "got %ld, expected 2\n", result);
		exit(EXIT_FAILURE);
	}
	afWriteFrames(outfile, AF_DEFAULT_TRACK, buffer + 4, 2);
	if (result != 2)
	{
		fprintf(stderr, "afWriteFrames did not return expected result\n");
		fprintf(stderr, "got %ld, expected 2\n", result);
		exit(EXIT_FAILURE);
	}

	afCloseFile(outfile);

	outfile = afOpenFile(argv[2], "w", outfilesetup);
	if (outfile == AF_NULL_FILEHANDLE)
	{
		fprintf(stderr, "could not open file %s for writing\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	result = afWriteFrames(outfile, AF_DEFAULT_TRACK, buffer, 4);
	if (result != 4)
	{
		fprintf(stderr, "afWriteFrames did not return expected result\n");
		fprintf(stderr, "got %ld, expected 4\n", result);
		exit(EXIT_FAILURE);
	}

	afCloseFile(outfile);

	afFreeFileSetup(outfilesetup);

	return 0;
}

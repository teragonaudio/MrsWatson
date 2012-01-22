/*
	Audio File Library
	Copyright (C) 1998, 2011, Michael Pruett <michael@68k.org>
	Copyright (C) 2000, Silicon Graphics, Inc.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the
	Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA  02111-1307  USA.
*/

/*
	WAVE.h

	This file contains structures and constants related to the RIFF
	WAVE sound file format.
*/

#ifndef WAVE_H
#define WAVE_H

#include "FileHandle.h"
#include <stdint.h>

#define _AF_WAVE_NUM_INSTPARAMS 7
extern const InstParamInfo _af_wave_inst_params[_AF_WAVE_NUM_INSTPARAMS];
#define _AF_WAVE_NUM_COMPTYPES 2
extern const int _af_wave_compression_types[_AF_WAVE_NUM_COMPTYPES];

struct UUID;

class WAVEFile : public _AFfilehandle
{
public:
	static bool recognize(File *fh);
	static AFfilesetup completeSetup(AFfilesetup);

	WAVEFile();

	status readInit(AFfilesetup);
	status writeInit(AFfilesetup);

	status update();

	bool isInstrumentParameterValid(AUpvlist, int);

private:
	AFfileoffset	factOffset;	/* start of fact (frame count) chunk */
	AFfileoffset	miscellaneousStartOffset;
	AFfileoffset	totalMiscellaneousSize;
	AFfileoffset	markOffset;
	AFfileoffset	dataSizeOffset;

	/*
		The following information is specified in the format
		chunk and is for use with compressed data formats.
	*/
	uint32_t	blockAlign, samplesPerBlock;

	/*
		The index into the coefficient array is of type
		uint8_t, so we can safely limit msadpcmCoefficients to
		be 256 coefficient pairs.
	*/
	int16_t		msadpcmCoefficients[256][2];

	status parseFrameCount(const Tag &type, uint32_t size);
	status parseFormat(const Tag &type, uint32_t size);
	status parseData(const Tag &type, uint32_t size);
	status parsePlayList(const Tag &type, uint32_t size);
	status parseCues(const Tag &type, uint32_t size);
	status parseADTLSubChunk(const Tag &type, uint32_t size);
	status parseINFOSubChunk(const Tag &type, uint32_t size);
	status parseList(const Tag &type, uint32_t size);
	status parseInstrument(const Tag &type, uint32_t size);

	status writeFormat();
	status writeFrameCount();
	status writeMiscellaneous();
	status writeCues();
	status writeData();

	bool readUUID(UUID *g);
	bool writeUUID(const UUID *g);
};

#endif

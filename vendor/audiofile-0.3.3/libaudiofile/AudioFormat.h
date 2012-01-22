/*
	Audio File Library
	Copyright (C) 2010, Michael Pruett <michael@68k.org>

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

#ifndef AUDIOFORMAT_H
#define AUDIOFORMAT_H

#include <sys/types.h>
#include <string>
#include "aupvlist.h"

struct PCMInfo
{
	double slope, intercept, minClip, maxClip;
};

struct AudioFormat
{
	double sampleRate;		/* sampling rate in Hz */
	int sampleFormat;		/* AF_SAMPFMT_... */
	int sampleWidth;		/* sample width in bits */
	int byteOrder;		/* AF_BYTEORDER_... */

	PCMInfo pcm;		/* parameters of PCM data */

	int channelCount;		/* number of channels */

	int compressionType;	/* AF_COMPRESSION_... */
	AUpvlist compressionParams;	/* NULL if no compression */

	bool packed : 1;

	size_t bytesPerSample(bool stretch3to4) const;
	size_t bytesPerFrame(bool stretch3to4) const;
	size_t bytesPerSample() const;
	size_t bytesPerFrame() const;
	bool isInteger() const;
	bool isSigned() const;
	bool isUnsigned() const;
	bool isFloat() const;
	bool isCompressed() const;
	bool isUncompressed() const;
	bool isPacked() const { return packed; }
	std::string description() const;
};

#endif

/*
	Audio File Library
	Copyright (C) 2011, Michael Pruett <michael@68k.org>

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

#include "config.h"
#include "CAF.h"

#include "File.h"
#include "Setup.h"
#include "Tag.h"
#include "Track.h"
#include "Track.h"
#include "byteorder.h"
#include "util.h"
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>

const int _af_caf_compression_types[_AF_CAF_NUM_COMPTYPES] =
{
	AF_COMPRESSION_G711_ULAW,
	AF_COMPRESSION_G711_ALAW
};

enum
{
	kCAFLinearPCMFormatFlagIsFloat = (1L << 0),
	kCAFLinearPCMFormatFlagIsLittleEndian = (1L << 1)
};

static _AFfilesetup caf_default_filesetup =
{
	_AF_VALID_FILESETUP,	// valid
	AF_FILE_CAF,			// fileFormat
	true,					// trackSet
	true,					// instrumentSet
	true,					// miscellaneousSet
	1,						// trackCount
	NULL,					// tracks
	1,						// instrumentCount
	NULL,					// instruments
	0,						// miscellaneousCount
	NULL					// miscellaneous
};

CAFFile::CAFFile() :
	m_dataOffset(-1)
{
	setFormatByteOrder(AF_BYTEORDER_BIGENDIAN);
}

CAFFile::~CAFFile()
{
}

bool CAFFile::recognize(File *file)
{
	file->seek(0, File::SeekFromBeginning);
	uint8_t buffer[8];
	if (file->read(buffer, 8) != 8 || memcmp(buffer, "caff", 4) != 0)
		return false;
	const uint8_t versionAndFlags[4] = { 0, 1, 0, 0 };
	if (memcmp(buffer + 4, versionAndFlags, 4) != 0)
		return false;
	return true;
}

status CAFFile::readInit(AFfilesetup setup)
{
	fh->seek(8, File::SeekFromBeginning);

	if (!allocateTrack())
		return AF_FAIL;

	off_t currentOffset = fh->tell();
	off_t fileLength = fh->length();

	while (currentOffset < fileLength)
	{
		Tag chunkType;
		int64_t chunkLength;
		if (!readTag(&chunkType) ||
			!readS64(&chunkLength))
			return AF_FAIL;

		currentOffset += 12;

		if (chunkType == "data" && chunkLength == -1)
			chunkLength = fileLength - currentOffset;
		else if (chunkLength < 0)
			_af_error(AF_BAD_HEADER,
				"invalid chunk length %lld for chunk type %s\n",
				chunkLength, chunkType.name().c_str());

		if (chunkType == "desc")
		{
			if (parseDescription(chunkType, chunkLength) == AF_FAIL)
				return AF_FAIL;
		}
		else if (chunkType == "data")
		{
			if (parseData(chunkType, chunkLength) == AF_FAIL)
				return AF_FAIL;
		}

		currentOffset = fh->seek(currentOffset + chunkLength,
			File::SeekFromBeginning);
	}

	return AF_SUCCEED;
}

status CAFFile::writeInit(AFfilesetup setup)
{
	if (_af_filesetup_make_handle(setup, this) == AF_FAIL)
		return AF_FAIL;

	Tag caff("caff");
	if (!writeTag(&caff)) return AF_FAIL;
	const uint8_t versionAndFlags[4] = { 0, 1, 0, 0 };
	if (fh->write(versionAndFlags, 4) != 4) return AF_FAIL;

	if (writeDescription() == AF_FAIL)
		return AF_FAIL;
	if (writeData(false) == AF_FAIL)
		return AF_FAIL;

	return AF_SUCCEED;
}

AFfilesetup CAFFile::completeSetup(AFfilesetup setup)
{
	if (setup->trackSet && setup->trackCount != 1)
	{
		_af_error(AF_BAD_NUMTRACKS, "CAF file must have 1 track");
		return AF_NULL_FILESETUP;
	}

	TrackSetup *track = &setup->tracks[0];

	if (track->sampleFormatSet)
	{
		if (track->f.isUnsigned())
		{
			_af_error(AF_BAD_FILEFMT, "CAF format does not support unsigned data");
			return AF_NULL_FILESETUP;
		}
	}
	else
		_af_set_sample_format(&track->f, AF_SAMPFMT_TWOSCOMP,
			track->f.sampleWidth);

	if (track->f.isSigned() && (track->f.sampleWidth < 1 || track->f.sampleWidth > 32))
	{
		_af_error(AF_BAD_WIDTH,
			"invalid sample width %d for CAF file (must be 1-32)",
			track->f.sampleWidth);
		return AF_NULL_FILESETUP;
	}

	if (!track->byteOrderSet)
		track->f.byteOrder = _AF_BYTEORDER_NATIVE;

	if (track->f.compressionType != AF_COMPRESSION_NONE &&
		track->f.compressionType != AF_COMPRESSION_G711_ULAW &&
		track->f.compressionType != AF_COMPRESSION_G711_ALAW)
	{
		_af_error(AF_BAD_COMPTYPE,
			"compression format %d not supported in CAF file",
			track->f.compressionType);
		return AF_NULL_FILESETUP;
	}

	return _af_filesetup_copy(setup, &caf_default_filesetup, true);
}

status CAFFile::update()
{
	if (writeData(true) == AF_FAIL)
		return AF_FAIL;
	return AF_SUCCEED;
}

status CAFFile::parseDescription(const Tag &, int64_t)
{
	double sampleRate;
	Tag formatID;
	uint32_t formatFlags;
	uint32_t channelsPerFrame;
	uint32_t bitsPerChannel;
	if (!readDouble(&sampleRate) ||
		!readTag(&formatID) ||
		!readU32(&formatFlags) ||
		!readU32(&m_bytesPerPacket) ||
		!readU32(&m_framesPerPacket) ||
		!readU32(&channelsPerFrame) ||
		!readU32(&bitsPerChannel))
		return AF_FAIL;

	Track *track = getTrack();
	track->f.channelCount = channelsPerFrame;
	track->f.sampleWidth = bitsPerChannel;
	track->f.sampleRate = sampleRate;
	if (formatID == "lpcm")
	{
		track->f.compressionType = AF_COMPRESSION_NONE;
		if (formatFlags & kCAFLinearPCMFormatFlagIsFloat)
		{
			if (bitsPerChannel != 32 && bitsPerChannel != 64)
			{
				_af_error(AF_BAD_WIDTH, "Invalid bits per sample %d for floating-point audio data", bitsPerChannel);
				return AF_FAIL;
			}
			track->f.sampleFormat = bitsPerChannel == 32 ? AF_SAMPFMT_FLOAT :
				AF_SAMPFMT_DOUBLE;
		}
		else
		{
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
		}
		track->f.byteOrder = (formatFlags & kCAFLinearPCMFormatFlagIsLittleEndian) ?
			AF_BYTEORDER_LITTLEENDIAN : AF_BYTEORDER_BIGENDIAN;

		_af_set_sample_format(&track->f, track->f.sampleFormat, track->f.sampleWidth);
		return AF_SUCCEED;
	}
	else if (formatID == "ulaw")
	{
		track->f.compressionType = AF_COMPRESSION_G711_ULAW;
		track->f.byteOrder = _AF_BYTEORDER_NATIVE;
		_af_set_sample_format(&track->f, AF_SAMPFMT_TWOSCOMP, 16);
		return AF_SUCCEED;
	}
	else if (formatID == "alaw")
	{
		track->f.compressionType = AF_COMPRESSION_G711_ALAW;
		track->f.byteOrder = _AF_BYTEORDER_NATIVE;
		_af_set_sample_format(&track->f, AF_SAMPFMT_TWOSCOMP, 16);
		return AF_SUCCEED;
	}
	else
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED, "Compression type %s not supported",
			formatID.name().c_str());
		return AF_FAIL;
	}
}

status CAFFile::parseData(const Tag &tag, int64_t length)
{
	uint32_t editCount;
	if (!readU32(&editCount))
		return AF_FAIL;

	Track *track = getTrack();
	if (length == -1)
		track->data_size = fh->length() - fh->tell();
	else
		track->data_size = length - 4;
	track->fpos_first_frame = fh->tell();
	int bytesPerFrame = track->f.bytesPerFrame(false);
	if (track->f.compressionType == AF_COMPRESSION_G711_ULAW ||
		track->f.compressionType == AF_COMPRESSION_G711_ALAW)
	{
		bytesPerFrame = track->f.channelCount;
	}
	track->totalfframes = track->data_size / bytesPerFrame;
	return AF_SUCCEED;
}

status CAFFile::writeDescription()
{
	Track *track = getTrack();

	Tag desc("desc");
	int64_t chunkLength = 32;
	double sampleRate = track->f.sampleRate;
	Tag formatID("lpcm");
	uint32_t formatFlags = 0;
	if (track->f.byteOrder == AF_BYTEORDER_LITTLEENDIAN)
		formatFlags |= kCAFLinearPCMFormatFlagIsLittleEndian;
	if (track->f.isFloat())
		formatFlags |= kCAFLinearPCMFormatFlagIsFloat;
	uint32_t bytesPerPacket = track->f.bytesPerFrame(false);
	uint32_t framesPerPacket = 1;
	uint32_t channelsPerFrame = track->f.channelCount;
	uint32_t bitsPerChannel = track->f.sampleWidth;

	if (track->f.compressionType == AF_COMPRESSION_G711_ULAW)
	{
		formatID = "ulaw";
		formatFlags = 0;
		bytesPerPacket = channelsPerFrame;
		bitsPerChannel = 8;
	}
	else if (track->f.compressionType == AF_COMPRESSION_G711_ALAW)
	{
		formatID = "alaw";
		formatFlags = 0;
		bytesPerPacket = channelsPerFrame;
		bitsPerChannel = 8;
	}

	if (!writeTag(&desc) ||
		!writeS64(&chunkLength) ||
		!writeDouble(&sampleRate) ||
		!writeTag(&formatID) ||
		!writeU32(&formatFlags) ||
		!writeU32(&bytesPerPacket) ||
		!writeU32(&framesPerPacket) ||
		!writeU32(&channelsPerFrame) ||
		!writeU32(&bitsPerChannel))
		return AF_FAIL;
	return AF_SUCCEED;
}

status CAFFile::writeData(bool update)
{
	Track *track = getTrack();

	if (m_dataOffset == -1)
		m_dataOffset = fh->tell();
	else
		fh->seek(m_dataOffset, File::SeekFromBeginning);

	Tag data("data");
	int64_t dataLength = -1;
	uint32_t editCount = 0;
	if (update)
		dataLength = track->data_size + 4;

	if (!writeTag(&data) ||
		!writeS64(&dataLength) ||
		!writeU32(&editCount))
		return AF_FAIL;
	if (track->fpos_first_frame == 0)
		track->fpos_first_frame = fh->tell();
	return AF_SUCCEED;
}

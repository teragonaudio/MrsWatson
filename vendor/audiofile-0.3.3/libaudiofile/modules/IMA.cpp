/*
	Audio File Library
	Copyright (C) 2001, Silicon Graphics, Inc.
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

/*
	IMA.cpp

	This module implements IMA ADPCM compression for the Audio File
	Library.
*/

#include "config.h"
#include "IMA.h"

#include <errno.h>
#include <string.h>
#include <assert.h>

#include <audiofile.h>

#include "FileModule.h"
#include "Track.h"
#include "adpcm.h"
#include "afinternal.h"
#include "byteorder.h"
#include "compression.h"
#include "units.h"
#include "util.h"

#define CHNK(X)

typedef struct
{
} ima_adpcm_data;

class IMA : public FileModule
{
public:
	static Module *createDecompress(Track *track, File *fh, bool canSeek,
		bool headerless, AFframecount *chunkFrames);
	virtual const char *name() const { return "ima_adpcm_decompress"; }
	virtual void describe();
	virtual void runPull();
	virtual void reset1();
	virtual void reset2();

private:
	int m_blockAlign, m_framesPerBlock;
	AFframecount m_framesToIgnore;

	IMA(Track *, File *fh, bool canSeek);
	int decodeBlock(const uint8_t *encoded, int16_t *decoded);
};

IMA::IMA(Track *track, File *fh, bool canSeek) :
	FileModule(Decompress, track, fh, canSeek),
	m_blockAlign(-1),
	m_framesPerBlock(-1),
	m_framesToIgnore(-1)
{
}

int IMA::decodeBlock (const uint8_t *encoded, int16_t *decoded)
{
	int channelCount = m_track->f.channelCount;
	adpcm_state state[channelCount];

	for (int c=0; c<channelCount; c++)
	{
		state[c].valprev = (encoded[1]<<8) | encoded[0];
		if (encoded[1] & 0x80)
			state[c].valprev -= 0x10000;

		state[c].index = encoded[2];

		*decoded++ = state[c].valprev;

		encoded += 4;
	}

	_af_adpcm_decoder(encoded, decoded, m_framesPerBlock - 1, channelCount, state);

	return m_framesPerBlock * channelCount * sizeof (int16_t);
}

bool _af_ima_adpcm_format_ok (AudioFormat *f)
{
	if (f->channelCount != 1)
	{
		_af_error(AF_BAD_COMPRESSION,
			"IMA ADPCM compression requires 1 channel");
		return false;
	}

	if (f->sampleFormat != AF_SAMPFMT_TWOSCOMP || f->sampleWidth != 16)
	{
		_af_error(AF_BAD_COMPRESSION,
			"IMA ADPCM compression requires 16-bit signed integer format");
		f->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		f->sampleWidth = 16;
		/* non-fatal */
	}

	if (f->byteOrder != _AF_BYTEORDER_NATIVE)
	{
		_af_error(AF_BAD_COMPRESSION,
			"IMA ADPCM compression requires native byte order");
		f->byteOrder = _AF_BYTEORDER_NATIVE;
		/* non-fatal */
	}

	return true;
}

void IMA::describe()
{
	m_outChunk->f.byteOrder = _AF_BYTEORDER_NATIVE;
	m_outChunk->f.compressionType = AF_COMPRESSION_NONE;
	m_outChunk->f.compressionParams = AU_NULL_PVLIST;
}

Module *IMA::createDecompress(Track *track, File *fh, bool canSeek,
	bool headerless, AFframecount *chunkFrames)
{
	assert(fh->tell() == track->fpos_first_frame);

	IMA *ima = new IMA(track, fh, canSeek);

	AUpvlist pv = (AUpvlist) track->f.compressionParams;

	long l;
	if (_af_pv_getlong(pv, _AF_FRAMES_PER_BLOCK, &l))
		ima->m_framesPerBlock = l;
	else
		_af_error(AF_BAD_CODEC_CONFIG, "samples per block not set");

	if (_af_pv_getlong(pv, _AF_BLOCK_SIZE, &l))
		ima->m_blockAlign = l;
	else
		_af_error(AF_BAD_CODEC_CONFIG, "block size not set");

	*chunkFrames = ima->m_framesPerBlock;

	return ima;
}

void IMA::runPull()
{
	AFframecount framesToRead = m_outChunk->frameCount;
	AFframecount framesRead = 0;

	assert(m_outChunk->frameCount % m_framesPerBlock == 0);
	int blockCount = m_outChunk->frameCount / m_framesPerBlock;

	/* Read the compressed frames. */
	ssize_t bytesRead = read(m_inChunk->buffer, m_blockAlign * blockCount);
	ssize_t blocksRead = bytesRead >= 0 ? bytesRead / m_blockAlign : 0;

	/* This condition would indicate that the file is bad. */
	if (blocksRead < 0)
	{
		if (m_track->filemodhappy)
		{
			_af_error(AF_BAD_READ, "file missing data");
			m_track->filemodhappy = false;
		}
	}

	if (blocksRead < blockCount)
		blockCount = blocksRead;

	/* Decompress into module->outc. */
	for (int i=0; i<blockCount; i++)
	{
		decodeBlock(static_cast<const uint8_t *>(m_inChunk->buffer) + i * m_blockAlign,
			static_cast<int16_t *>(m_outChunk->buffer) + i * m_framesPerBlock * m_track->f.channelCount);

		framesRead += m_framesPerBlock;
	}

	m_track->nextfframe += framesRead;

	assert(tell() == m_track->fpos_next_frame);

	/*
		If we got EOF from read, then we return the actual amount read.

		Complain only if there should have been more frames in the file.
	*/

	if (m_track->totalfframes != -1 && framesRead != framesToRead)
	{
		/* Report error if we haven't already */
		if (m_track->filemodhappy)
		{
			_af_error(AF_BAD_READ,
				"file missing data -- read %d frames, should be %d",
				m_track->nextfframe,
				m_track->totalfframes);
			m_track->filemodhappy = false;
		}
	}

	m_outChunk->frameCount = framesRead;
}

void IMA::reset1()
{
	AFframecount nextTrackFrame = m_track->nextfframe;
	m_track->nextfframe = (nextTrackFrame / m_framesPerBlock) *
		m_framesPerBlock;

	m_framesToIgnore = nextTrackFrame - m_track->nextfframe;
	/* postroll = frames2ignore */
}

void IMA::reset2()
{
	m_track->fpos_next_frame = m_track->fpos_first_frame +
		m_blockAlign * (m_track->nextfframe / m_framesPerBlock);
	m_track->frames2ignore += m_framesToIgnore;

	assert(m_track->nextfframe % m_framesPerBlock == 0);
}

Module *_af_ima_adpcm_init_decompress(Track *track, File *fh,
	bool canSeek, bool headerless, AFframecount *chunkFrames)
{
	return IMA::createDecompress(track, fh, canSeek, headerless, chunkFrames);
}

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
	This module implements Microsoft ADPCM compression.
*/

#include "config.h"
#include "MSADPCM.h"

#include <errno.h>
#include <string.h>
#include <assert.h>

#include "File.h"
#include "FileModule.h"
#include "Track.h"
#include "afinternal.h"
#include "audiofile.h"
#include "byteorder.h"
#include "compression.h"
#include "units.h"
#include "util.h"

struct ms_adpcm_state
{
	uint8_t		predictor;
	uint16_t	delta;
	int16_t		sample1, sample2;
};

class MSADPCM : public FileModule
{
public:
	static Module *createDecompress(Track *, File *, bool canSeek,
		bool headerless, AFframecount *chunkFrames);
	virtual const char *name() const { return "ms_adpcm_decompress"; }
	virtual void describe();
	virtual void runPull();
	virtual void reset1();
	virtual void reset2();

private:
	/*
		We set framesToIgnore during a reset1 and add it to
		framesToIgnore during a reset2.
	*/
	AFframecount m_framesToIgnore;

	int m_blockAlign, m_framesPerBlock;

	/* m_coefficients is an array of m_numCoefficients ADPCM coefficient pairs. */
	int	m_numCoefficients;
	int16_t	m_coefficients[256][2];

	MSADPCM(Track *track, File *fh, bool canSeek);
	int decodeBlock(const uint8_t *encoded, int16_t *decoded);
};

/*
	Compute a linear PCM value from the given differential coded
	value.
*/
static int16_t ms_adpcm_decode_sample (ms_adpcm_state *state,
	uint8_t code, const int16_t *coefficient)
{
	const int32_t MAX_INT16 = 32767, MIN_INT16 = -32768;
	const int32_t adaptive[] =
	{
		230, 230, 230, 230, 307, 409, 512, 614,
		768, 614, 512, 409, 307, 230, 230, 230
	};
	int32_t	linearSample, delta;

	linearSample = ((state->sample1 * coefficient[0]) +
		(state->sample2 * coefficient[1])) / 256;

	if (code & 0x08)
		linearSample += state->delta * (code-0x10);
	else
		linearSample += state->delta * code;

	/* Clamp linearSample to a signed 16-bit value. */
	if (linearSample < MIN_INT16)
		linearSample = MIN_INT16;
	else if (linearSample > MAX_INT16)
		linearSample = MAX_INT16;

	delta = ((int32_t) state->delta * adaptive[code])/256;
	if (delta < 16)
	{
		delta = 16;
	}

	state->delta = delta;
	state->sample2 = state->sample1;
	state->sample1 = linearSample;

	/*
		Because of earlier range checking, new_sample will be
		in the range of an int16_t.
	*/
	return (int16_t) linearSample;
}

/* Decode one block of MS ADPCM data. */
int MSADPCM::decodeBlock (const uint8_t *encoded, int16_t *decoded)
{
	int16_t *coefficient[2];
	ms_adpcm_state decoderState[2] = {{0}};
	ms_adpcm_state *state[2];

	int channelCount = m_track->f.channelCount;

	/* Calculate the number of bytes needed for decoded data. */
	int outputLength = m_framesPerBlock * sizeof (int16_t) * channelCount;

	state[0] = &decoderState[0];
	if (channelCount == 2)
		state[1] = &decoderState[1];
	else
		state[1] = &decoderState[0];

	/* Initialize predictor. */
	for (int i=0; i<channelCount; i++)
	{
		state[i]->predictor = *encoded++;
		assert(state[i]->predictor < m_numCoefficients);
	}

	/* Initialize delta. */
	for (int i=0; i<channelCount; i++)
	{
		state[i]->delta = (encoded[1]<<8) | encoded[0];
		encoded += sizeof (uint16_t);
	}

	/* Initialize first two samples. */
	for (int i=0; i<channelCount; i++)
	{
		state[i]->sample1 = (encoded[1]<<8) | encoded[0];
		encoded += sizeof (uint16_t);
	}

	for (int i=0; i<channelCount; i++)
	{
		state[i]->sample2 = (encoded[1]<<8) | encoded[0];
		encoded += sizeof (uint16_t);
	}

	coefficient[0] = m_coefficients[state[0]->predictor];
	coefficient[1] = m_coefficients[state[1]->predictor];

	for (int i=0; i<channelCount; i++)
		*decoded++ = state[i]->sample2;

	for (int i=0; i<channelCount; i++)
		*decoded++ = state[i]->sample1;

	/*
		The first two samples have already been 'decoded' in
		the block header.
	*/
	int samplesRemaining = (m_framesPerBlock - 2) * m_track->f.channelCount;

	while (samplesRemaining > 0)
	{
		uint8_t code;
		int16_t newSample;

		code = *encoded >> 4;
		newSample = ms_adpcm_decode_sample(state[0], code, coefficient[0]);
		*decoded++ = newSample;

		code = *encoded & 0x0f;
		newSample = ms_adpcm_decode_sample(state[1], code, coefficient[1]);
		*decoded++ = newSample;

		encoded++;
		samplesRemaining -= 2;
	}

	return outputLength;
}

void MSADPCM::describe()
{
	m_outChunk->f.byteOrder = _AF_BYTEORDER_NATIVE;
	m_outChunk->f.compressionType = AF_COMPRESSION_NONE;
	m_outChunk->f.compressionParams = AU_NULL_PVLIST;
}

MSADPCM::MSADPCM(Track *track, File *fh, bool canSeek) :
	FileModule(Decompress, track, fh, canSeek)
{
}

Module *MSADPCM::createDecompress(Track *track, File *fh,
	bool canSeek, bool headerless, AFframecount *chunkFrames)
{
	assert(fh->tell() == track->fpos_first_frame);

	MSADPCM *msadpcm = new MSADPCM(track, fh, canSeek);

	AUpvlist pv = (AUpvlist) track->f.compressionParams;
	long l;
	if (_af_pv_getlong(pv, _AF_MS_ADPCM_NUM_COEFFICIENTS, &l))
		msadpcm->m_numCoefficients = l;
	else
		_af_error(AF_BAD_CODEC_CONFIG, "number of coefficients not set");

	void *v;
	if (_af_pv_getptr(pv, _AF_MS_ADPCM_COEFFICIENTS, &v))
		memcpy(msadpcm->m_coefficients, v, sizeof (int16_t) * 2 * msadpcm->m_numCoefficients);
	else
		_af_error(AF_BAD_CODEC_CONFIG, "coefficient array not set");

	if (_af_pv_getlong(pv, _AF_FRAMES_PER_BLOCK, &l))
		msadpcm->m_framesPerBlock = l;
	else
		_af_error(AF_BAD_CODEC_CONFIG, "samples per block not set");

	if (_af_pv_getlong(pv, _AF_BLOCK_SIZE, &l))
		msadpcm->m_blockAlign = l;
	else
		_af_error(AF_BAD_CODEC_CONFIG, "block size not set");

	*chunkFrames = msadpcm->m_framesPerBlock;

	return msadpcm;
}

void MSADPCM::runPull()
{
	AFframecount framesToRead = m_outChunk->frameCount;
	AFframecount framesRead = 0;

	assert(m_outChunk->frameCount % m_framesPerBlock == 0);
	int blockCount = m_outChunk->frameCount / m_framesPerBlock;

	/* Read the compressed frames. */
	ssize_t bytesRead = read(m_inChunk->buffer, m_blockAlign * blockCount);
	size_t blocksRead = bytesRead >= 0 ? bytesRead / m_blockAlign : 0;

	/* Decompress into m_outChunk. */
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

void MSADPCM::reset1()
{
	AFframecount nextTrackFrame = m_track->nextfframe;
	m_track->nextfframe = (nextTrackFrame / m_framesPerBlock) *
		m_framesPerBlock;

	m_framesToIgnore = nextTrackFrame - m_track->nextfframe;
	/* postroll = frames2ignore */
}

void MSADPCM::reset2()
{
	m_track->fpos_next_frame = m_track->fpos_first_frame +
		m_blockAlign * (m_track->nextfframe / m_framesPerBlock);
	m_track->frames2ignore += m_framesToIgnore;

	assert(m_track->nextfframe % m_framesPerBlock == 0);
}

bool _af_ms_adpcm_format_ok (AudioFormat *f)
{
	if (f->channelCount != 1 && f->channelCount != 2)
	{
		_af_error(AF_BAD_COMPRESSION,
			"MS ADPCM compression requires 1 or 2 channels");
		return false;
	}

	if (f->sampleFormat != AF_SAMPFMT_TWOSCOMP || f->sampleWidth != 16)
	{
		_af_error(AF_BAD_COMPRESSION,
			"MS ADPCM compression requires 16-bit signed integer format");
		f->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		f->sampleWidth = 16;
		/* non-fatal */
	}

	if (f->byteOrder != AF_BYTEORDER_BIGENDIAN)
	{
		_af_error(AF_BAD_COMPRESSION,
			"MS ADPCM compression requires big endian format");
		f->byteOrder = AF_BYTEORDER_BIGENDIAN;
		/* non-fatal */
	}

	return true;
}

Module *_af_ms_adpcm_init_decompress (Track *track, File *fh,
	bool canSeek, bool headerless, AFframecount *chunkFrames)
{
	return MSADPCM::createDecompress(track, fh, canSeek, headerless, chunkFrames);
}

/*
	Audio File Library
	Copyright (C) 2000-2001, Silicon Graphics, Inc.
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

#include "config.h"
#include "G711.h"

#include <errno.h>
#include <string.h>
#include <assert.h>

#include "FileModule.h"
#include "Track.h"
#include "afinternal.h"
#include "audiofile.h"
#include "byteorder.h"
#include "compression.h"
#include "units.h"
#include "util.h"

#include "../g711.h"

#define CHNK(X)

static void ulaw2linear_buf (const uint8_t *ulaw, int16_t *linear, int nsamples)
{
	for (int i=0; i < nsamples; i++)
		linear[i] = _af_ulaw2linear(ulaw[i]);
}

static void linear2ulaw_buf (const int16_t *linear, uint8_t *ulaw, int nsamples)
{
	for (int i=0; i < nsamples; i++)
		ulaw[i] = _af_linear2ulaw(linear[i]);
}

static void alaw2linear_buf (const uint8_t *alaw, int16_t *linear, int nsamples)
{
	for (int i=0; i < nsamples; i++)
		linear[i] = _af_alaw2linear(alaw[i]);
}

static void linear2alaw_buf (const int16_t *linear, uint8_t *alaw, int nsamples)
{
	for (int i=0; i < nsamples; i++)
		alaw[i] = _af_linear2alaw(linear[i]);
}

bool _af_g711_format_ok (AudioFormat *f)
{
	if (f->sampleFormat != AF_SAMPFMT_TWOSCOMP || f->sampleWidth != 16)
	{
		_af_error(AF_BAD_COMPRESSION,
		       "G711 compression requires 16-bit signed integer format");
		f->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		f->sampleWidth = 16;
		/* non-fatal */
	}

	if (f->byteOrder != AF_BYTEORDER_BIGENDIAN)
	{
		_af_error(AF_BAD_COMPRESSION,
		       "G711 compression requires big endian format");
		f->byteOrder = AF_BYTEORDER_BIGENDIAN;
		/* non-fatal */
	}

	return true;
}

class G711 : public FileModule
{
public:
	static Module *createCompress(Track *trk, File *fh, bool canSeek,
		bool headerless, AFframecount *chunkframes);
	static Module *createDecompress(Track *trk, File *fh, bool canSeek,
		bool headerless, AFframecount *chunkframes);

	virtual const char *name() const
	{
		return mode() == Compress ? "g711compress" : "g711decompress";
	}
	virtual void describe();
	virtual void runPull();
	virtual void reset2();
	virtual void runPush();
	virtual void sync1();
	virtual void sync2();

private:
	G711(Mode mode, Track *track, File *fh, bool canSeek);

	AFfileoffset m_savedPositionNextFrame;
	AFframecount m_savedNextFrame;
};

G711::G711(Mode mode, Track *track, File *fh, bool canSeek) :
	FileModule(mode, track, fh, canSeek),
	m_savedPositionNextFrame(-1),
	m_savedNextFrame(-1)
{
	if (mode == Decompress)
		track->f.compressionParams = AU_NULL_PVLIST;
}

Module *G711::createCompress(Track *track, File *fh,
	bool canSeek, bool headerless, AFframecount *chunkframes)
{
	return new G711(Compress, track, fh, canSeek);
}

void G711::runPush()
{
	AFframecount framesToWrite = m_inChunk->frameCount;
	AFframecount samplesToWrite = m_inChunk->frameCount * m_inChunk->f.channelCount;
	int framesize = m_inChunk->f.channelCount;

	assert(m_track->f.compressionType == AF_COMPRESSION_G711_ULAW ||
		m_track->f.compressionType == AF_COMPRESSION_G711_ALAW);

	/* Compress frames into i->outc. */

	if (m_track->f.compressionType == AF_COMPRESSION_G711_ULAW)
		linear2ulaw_buf(static_cast<const int16_t *>(m_inChunk->buffer),
			static_cast<uint8_t *>(m_outChunk->buffer), samplesToWrite);
	else
		linear2alaw_buf(static_cast<const int16_t *>(m_inChunk->buffer),
			static_cast<uint8_t *>(m_outChunk->buffer), samplesToWrite);

	/* Write the compressed data. */

	ssize_t bytesWritten = write(m_outChunk->buffer, framesize * framesToWrite);
	AFframecount framesWritten = bytesWritten >= 0 ? bytesWritten / framesize : 0;

	CHNK(printf("writing %d frames to g711 file\n", framesToWrite));

	if (framesWritten != framesToWrite)
	{
		/* report error if we haven't already */
		if (m_track->filemodhappy)
		{
			/* i/o error */
			if (framesWritten < 0)
				_af_error(AF_BAD_WRITE,
					"unable to write data (%s) -- "
					"wrote %d out of %d frames",
					strerror(errno),
					m_track->nextfframe + framesWritten,
					m_track->nextfframe + framesToWrite);

			/* usual disk full error */
			else
				_af_error(AF_BAD_WRITE,
					"unable to write data (disk full) -- "
					"wrote %d out of %d frames",
					m_track->nextfframe + framesWritten,
					m_track->nextfframe + framesToWrite);

			m_track->filemodhappy = false;
		}
	}

	m_track->nextfframe += framesWritten;
	m_track->totalfframes = m_track->nextfframe;

	assert(!canSeek() || (tell() == m_track->fpos_next_frame));
}

void G711::sync1()
{
	m_savedPositionNextFrame = m_track->fpos_next_frame;
	m_savedNextFrame = m_track->nextfframe;
}

void G711::sync2()
{
	/* sanity check. */
	assert(!canSeek() || (tell() == m_track->fpos_next_frame));

	/* We can afford to do an lseek just in case because sync2 is rare. */
	m_track->fpos_after_data = tell();

	m_track->fpos_next_frame = m_savedPositionNextFrame;
	m_track->nextfframe = m_savedNextFrame;
}

void G711::describe()
{
	if (mode() == Compress)
	{
		m_outChunk->f.compressionType = m_track->f.compressionType;
	}
	else
	{
		m_outChunk->f.byteOrder = _AF_BYTEORDER_NATIVE;
		m_outChunk->f.compressionType = AF_COMPRESSION_NONE;
	}
}

Module *G711::createDecompress(Track *track, File *fh,
	bool canSeek, bool headerless, AFframecount *chunkframes)
{
	return new G711(Decompress, track, fh, canSeek);
}

void G711::runPull()
{
	AFframecount framesToRead = m_outChunk->frameCount;
	AFframecount samplesToRead = m_outChunk->frameCount * m_outChunk->f.channelCount;
	int framesize = m_outChunk->f.channelCount;

	/* Read the compressed frames. */

	ssize_t bytesRead = read(m_inChunk->buffer, framesize * framesToRead);
	AFframecount framesRead = bytesRead >= 0 ? bytesRead / framesize : 0;

	/* Decompress into i->outc. */

	if (m_track->f.compressionType == AF_COMPRESSION_G711_ULAW)
		ulaw2linear_buf(static_cast<const uint8_t *>(m_inChunk->buffer),
			static_cast<int16_t *>(m_outChunk->buffer), samplesToRead);
	else
		alaw2linear_buf(static_cast<const uint8_t *>(m_inChunk->buffer),
			static_cast<int16_t *>(m_outChunk->buffer), samplesToRead);

	CHNK(printf("reading %d frames from g711 file (got %d)\n",
		framesToRead, framesRead));

	m_track->nextfframe += framesRead;
	assert(!canSeek() || (tell() == m_track->fpos_next_frame));

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

void G711::reset2()
{
	int framesize = m_inChunk->f.channelCount;

	m_track->fpos_next_frame = m_track->fpos_first_frame +
		framesize * m_track->nextfframe;

	m_track->frames2ignore = 0;
}

Module *_AFg711initcompress(Track *track, File *fh, bool canSeek,
	bool headerless, AFframecount *chunkFrames)
{
	return G711::createCompress(track, fh, canSeek, headerless, chunkFrames);
}

Module *_AFg711initdecompress(Track *track, File *fh, bool canSeek,
	bool headerless, AFframecount *chunkFrames)
{
	return G711::createDecompress(track, fh, canSeek, headerless, chunkFrames);
}

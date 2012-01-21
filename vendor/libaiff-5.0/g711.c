/* 	$Id: g711.c,v 1.1 2007/09/19 11:49:46 toad32767 Exp $ */
/* 	Id: ulaw.c,v 1.10 2007/01/07 14:47:32 toad32767 Exp    */
/* 	Id: alaw.c,v 1.6 2007/01/07 14:47:32 toad32767 Exp     */

/*-
 * Copyright (c) 2006, 2007 Marco Trillo
 *
 * Permission is hereby granted, free of charge, to any
 * person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the
 * Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice
 * shall be included in all copies or substantial portions of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#define LIBAIFF 1
#include <stdlib.h>
#include <libaiff/libaiff.h>
#include "private.h"

/*
 * G.711 mu-Law (u-Law)
 *
 * G.711 mu-Law is a 8-bit floating point representation
 * of 14-bit PCM samples, with a 4-bit mantissa and a 3-bit
 * exponent.
 *
 * This gives 5-bit (4 + hidden bit) mantissa precision for all samples,
 * so low samples (with zeros in the most-significant bits) get
 * more precision with this encoding than with normal 8-bit LPCM.
 *
 * For more information on G.711 u-Law:
 *   http://shannon.cm.nctu.edu.tw/comtheory/chap3.pdf
 */
static          int16_t
ulawdec(uint8_t x)
{
	int             sgn, exp, mant;
	int             y;

	x = ~x;			/* bits are sent reversed */
	sgn = x & 0x80;
	x &= 0x7F;
	mant = (int) ((x & 0xF) << 1) | 0x21;	/* mantissa plus hidden bits */
	exp = (int) ((x & 0x70) >> 4);	/* exponent */
	mant <<= exp;

	/*
	 * Subtract 33 from the 'raised output'
	 * to get the normal output (14-bit),
	 * and then convert the 14-bit output
	 * to 16-bit
	 */
	y = (sgn ? (-mant + 33) : (mant - 33));
	return (int16_t) (y << 2);
}


/*
 * G.711 A-Law (A-Law)
 *
 * G.711 A-Law is a 8-bit floating point representation
 * of 13-bit PCM samples, with a 4-bit mantissa and a 3-bit
 * exponent.
 *
 * This gives 5-bit (4 + hidden bit) mantissa precision for all samples,
 * so low samples (with zeros in the most-significant bits) get
 * more precision with this encoding than with normal 8-bit LPCM.
 *
 * For more information on G.711 A-Law:
 *   http://shannon.cm.nctu.edu.tw/comtheory/chap3.pdf
 *   http://en.wikipedia.org/wiki/G.711
 */
static          int16_t
alawdec(uint8_t x)
{
	int             sgn, exp, mant;
	int             y;

	/*
	 * The bits at even positions are inversed.
	 * In addition, the sign bit is inversed too.
	 */
	x = ((~x & 0xD5) | (x & 0x2A));
	sgn = x & 0x80;
	x &= 0x7F;
	mant = (int) ((x & 0xF) << 1) | 0x21;
	exp = (int) ((x & 0x70) >> 4);

	/*
	 * Denormalized numbers? then remove hidden bit
	 */
	if (exp == 0)
		mant &= ~0x20;
	else
		mant <<= (exp - 1);

	y = (sgn ? -mant : mant);
	return (int16_t) (y << 3);	/* convert 13-bit to 16-bit */
}

static int
g711_ulaw_create(AIFF_Ref r)
{
	int             i;
	int16_t        *p = malloc(256 * sizeof(int16_t));
	if (p != NULL) {
		for (i = 0; i < 256; ++i)
			p[i] = ulawdec(i);
		r->pdata = p;
		return 1;
	} else
		return -1;
}

static int
g711_alaw_create(AIFF_Ref r)
{
	int             i;
	int16_t        *p = malloc(256 * sizeof(int16_t));
	if (p != NULL) {
		for (i = 0; i < 256; ++i)
			p[i] = alawdec(i);
		r->pdata = p;
		return 1;
	} else
		return -1;
}

static void
g711_delete(AIFF_Ref r)
{
	free(r->pdata);
}

static          size_t
g711_read_lpcm(AIFF_Ref r, void *buffer, size_t len)
{
	size_t          n, i, rem, bytesToRead, bytesRead;
	uint8_t        *bytes;
	int16_t        *samples, *table = r->pdata;

	/* length must be even (16-bit samples) */
	if (len & 1)
		--len;

	n = len >> 1;
	rem = (size_t) (r->soundLen) - (size_t) (r->pos);
	bytesToRead = MIN(n, rem);
	if (bytesToRead == 0)
		return 0;

	if (r->buffer2 == NULL || r->buflen2 < bytesToRead) {
		if (r->buffer2 != NULL)
			free(r->buffer2);
		r->buffer2 = malloc(bytesToRead);
		if (r->buffer2 == NULL) {
			r->buflen2 = 0;
			return 0;
		}
		r->buflen2 = bytesToRead;
	}
	bytesRead = fread(r->buffer2, 1, bytesToRead, r->fd);
	if (bytesRead > 0) {
		r->pos += bytesRead;
	} else {
		return 0;
	}

	bytes = (uint8_t *) (r->buffer2);
	samples = (int16_t *) buffer;
	for (i = 0; i < bytesRead; ++i) {
		samples[i] = table[bytes[i]];
	}

	return (bytesRead << 1);/* bytesRead * 2 */
}

static int
g711_seek(AIFF_Ref r, uint64_t pos)
{
	long            of;
	uint32_t        b;

	b = (uint32_t) pos * r->nChannels;
	if (b >= r->soundLen)
		return 0;
	of = (long) b;

	if (fseek(r->fd, of, SEEK_CUR) < 0) {
		return -1;
	}
	r->pos = b;
	return 1;
}

static int
g711_read_float32(AIFF_Ref r, float *buffer, int nFrames)
{
	size_t          n = (size_t) nFrames, i, rem, bytesToRead, bytesRead;
	uint8_t        *bytes;
	int16_t        *table = r->pdata;

	rem = (size_t) (r->soundLen) - (size_t) (r->pos);
	bytesToRead = MIN(n, rem);
	if (bytesToRead == 0)
		return 0;

	if (r->buffer2 == NULL || r->buflen2 < bytesToRead) {
		if (r->buffer2 != NULL)
			free(r->buffer2);
		r->buffer2 = malloc(bytesToRead);
		if (r->buffer2 == NULL) {
			r->buflen2 = 0;
			return 0;
		}
		r->buflen2 = bytesToRead;
	}
	bytesRead = fread(r->buffer2, 1, bytesToRead, r->fd);
	if (bytesRead > 0) {
		r->pos += bytesRead;
	} else {
		return 0;
	}

	bytes = (uint8_t *) (r->buffer2);
	for (i = 0; i < bytesRead; ++i) {
		buffer[i] = (float) (table[bytes[i]]) / 32768.0;
	}

	return bytesRead;	/* bytesRead = framesRead (segmentSize = 1) */
}


struct decoder  ulaw = {
	AUDIO_FORMAT_ULAW,
	g711_ulaw_create,
	g711_read_lpcm,
	g711_read_float32,
	g711_seek,
	g711_delete
};

struct decoder  alaw = {
	AUDIO_FORMAT_ALAW,
	g711_alaw_create,
	g711_read_lpcm,
	g711_read_float32,
	g711_seek,
	g711_delete
};



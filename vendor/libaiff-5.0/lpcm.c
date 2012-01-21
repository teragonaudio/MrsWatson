/*	$Id: lpcm.c,v 1.20 2007/09/19 13:22:10 toad32767 Exp $ */

/*-
 * Copyright (c) 2005, 2006, 2007 Marco Trillo
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
#include <libaiff/libaiff.h>
#include <libaiff/endian.h>
#include "private.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

void 
lpcm_swap_samples(int segmentSize, int flags, void *from, void *to, int nsamples)
{
	register int n = nsamples;
	register int i;
	/* 8 bit */
	uint8_t *fubytes = (uint8_t *) from;
	uint8_t *ubytes = (uint8_t *) to;
	/* 16 bit */
	int16_t *fwords = (int16_t *) from;
	int16_t *words = (int16_t *) to;
	/* 32 bit */
	int32_t *fdwords = (int32_t *) from;
	int32_t *dwords = (int32_t *) to;
	/* 24 bit */
	uint8_t x, y, z;

	/*
	 * Do we really need to do something?
	 */
	if (from == to && !(flags & LPCM_NEED_SWAP))
		return;

	switch (segmentSize) {
	case 2:
		if (flags & LPCM_NEED_SWAP) {
			for (i = 0; i < n; ++i)
				words[i] = ARRANGE_ENDIAN_16(fwords[i]);
		} else {
			memmove(words, fwords, n << 1 /* n * 2 */);
		}
		break;
	case 3:
		if (flags & LPCM_NEED_SWAP) {
			n *= 3;
			for (i = 0; i < n; i += 3) {
				x = fubytes[i];
				y = fubytes[i + 1];
				z = fubytes[i + 2];

				ubytes[i] = z;
				ubytes[i + 1] = y;
				ubytes[i + 2] = x;
			}
			n /= 3;
		} else {
			memmove(ubytes, fubytes, n * 3);
		}
		break;
	case 4:
		if (flags & LPCM_NEED_SWAP) {
			for (i = 0; i < n; ++i)
				dwords[i] = ARRANGE_ENDIAN_32(fdwords[i]);
		} else {
			memmove(dwords, fdwords, n << 2 /* n * 4 */);
		}
		break;
	}

	return;
}

static size_t 
lpcm_read_lpcm(AIFF_Ref r, void *buffer, size_t len)
{
	int n;
	uint32_t clen;
	size_t slen;
	size_t bytes_in;
	size_t bytesToRead;

	n = (int) len;
	len -= n % r->segmentSize;
	n /= r->segmentSize;

	slen = (size_t) (r->soundLen) - (size_t) (r->pos);
	bytesToRead = MIN(len, slen);

	if (bytesToRead == 0)
		return 0;

	bytes_in = fread(buffer, 1, bytesToRead, r->fd);
	if (bytes_in > 0)
		clen = (uint32_t) bytes_in;
	else
		clen = 0;
	r->pos += clen;

	lpcm_swap_samples(r->segmentSize, r->flags, buffer, buffer, n);

	return bytes_in;
}

static int 
lpcm_seek(AIFF_Ref r, uint64_t pos)
{
	long of;
	uint32_t b;

	b = (uint32_t) pos * r->nChannels * r->segmentSize;
	if (b >= r->soundLen)
		return 0;
	of = (long) b;

	if (fseek(r->fd, of, SEEK_CUR) < 0) {
		return -1;
	}
	r->pos = b;
	return 1;
}

/*
 * Dequantize LPCM (buffer) to floating point PCM (samples)
 */
void
lpcm_dequant(int segmentSize, void *buffer, float *outSamples, int nSamples)
{
	switch (segmentSize) {
		case 4:
		  {
			  int32_t *integers = (int32_t *) buffer;
			  
			  while (nSamples-- > 0)
				{
				  outSamples[nSamples] = (float) integers[nSamples] / 2147483648.0;
				}
			  break;
		  }
		case 3:
		  {
			  uint8_t *b = (uint8_t *) buffer;
			  int32_t integer;
			  int sgn;
			  
			  while (nSamples-- > 0)
				{
#ifdef WORDS_BIGENDIAN
				  sgn = b[0] & 0x80;
				  
				  integer = ((int32_t) b[0] << 16) + 
					  ((int32_t) b[1] << 8) + 
					  (int32_t) b[2];
#else
				  sgn = b[2] & 0x80;
				  
				  integer = ((int32_t) b[2] << 16) + 
					  ((int32_t) b[1] << 8) + 
					  (int32_t) b[0];
#endif /* WORDS_BIGENDIAN */
				  
				  if (sgn)
					{
					  /*
					   * sign propagation
					   * (host CPU must use two's complement ordering)
					   */
					  integer = (int32_t) (0xFF000000 | (uint32_t) integer);
					}
				  
				  outSamples[nSamples] = (float) integer / 8388608.0;
				  b += 3;
				}
			  break;
		  }
		case 2:
		  {
			  int16_t *integers = (int16_t *) buffer;
			  
			  while (nSamples-- > 0)
				{
				  outSamples[nSamples] = (float) integers[nSamples] / 32768.0;
				}
			  break;
		  }
		case 1:
		  {
			  int8_t *integers = (int8_t *) buffer;
			  
			  while (nSamples-- > 0)
				{
				  outSamples[nSamples] = (float) integers[nSamples] / 128.0;
				}
			  break;
		  }
	}
}
			  
static int
lpcm_read_float32(AIFF_Ref r, float *buffer, int nSamples)
{
	size_t len, slen, bytesToRead, bytes_in;
	uint32_t clen;
	int nSamplesRead;
	
	len = (size_t) nSamples * r->segmentSize;
	slen = (size_t) (r->soundLen) - (size_t) (r->pos);
	bytesToRead = MIN(len, slen);
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
	
	bytes_in = fread(r->buffer2, 1, bytesToRead, r->fd);
	if (bytes_in > 0)
		clen = (uint32_t) bytes_in;
	else
		clen = 0;
	r->pos += clen;
	nSamplesRead = (int) clen / (r->segmentSize);
	
	lpcm_swap_samples(r->segmentSize, r->flags, r->buffer2, r->buffer2, nSamplesRead);
	lpcm_dequant(r->segmentSize, r->buffer2, buffer, nSamplesRead);
	
	return nSamplesRead;
}


struct decoder lpcm = {
	AUDIO_FORMAT_LPCM,
	NULL,
	lpcm_read_lpcm,
	lpcm_read_float32,
	lpcm_seek,
	NULL
};


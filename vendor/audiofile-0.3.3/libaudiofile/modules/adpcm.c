/***********************************************************
Copyright 1992 by Stichting Mathematisch Centrum, Amsterdam, The
Netherlands.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Stichting Mathematisch
Centrum or CWI not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior permission.

STICHTING MATHEMATISCH CENTRUM DISCLAIMS ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL STICHTING MATHEMATISCH CENTRUM BE LIABLE
FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

******************************************************************/

/*
** Intel/DVI ADPCM coder/decoder.
**
** The algorithm for this coder was taken from the IMA Compatability Project
** proceedings, Vol 2, Number 2; May 1992.
**
** Version 1.2, 18-Dec-92.
**
** Change log:
** - Fixed a stupid bug, where the delta was computed as
**   stepsize*code/4 in stead of stepsize*(code+0.5)/4.
** - There was an off-by-one error causing it to pick
**   an incorrect delta once in a blue moon.
** - The NODIVMUL define has been removed. Computations are now always done
**   using shifts, adds and subtracts. It turned out that, because the standard
**   is defined using shift/add/subtract, you needed bits of fixup code
**   (because the div/mul simulation using shift/add/sub made some rounding
**   errors that real div/mul don't make) and all together the resultant code
**   ran slower than just using the shifts all the time.
** - Changed some of the variable names to be more meaningful.
*/

#include "config.h"

#include <stdint.h>
#include <assert.h>

#include "adpcm.h"

/* Intel ADPCM step variation table */
static const int indexTable[16] =
{
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8,
};

static const int stepsizeTable[89] =
{
	7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
	19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
	50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
	130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
	337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
	876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
	2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
	5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
	15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

static inline int clamp (int x, int low, int high)
{
	if (x < low) return low;
	if (x > high) return high;
	return x;
}

void _af_adpcm_coder (const int16_t *indata, uint8_t *outdata, int frameCount,
	int channelCount, struct adpcm_state *state)
{
    const int16_t *inp;	/* Input buffer pointer */
    uint8_t *outp;		/* Output buffer pointer */
    int val;			/* Current input sample value */
    int sign;			/* Current adpcm sign bit */
    int delta;			/* Current adpcm output value */
    int diff;			/* Difference between val and valprev */
    int step;			/* Stepsize */
    int valpred;		/* Predicted output value */
    int vpdiff;			/* Current change to valpred */
    int index;			/* Current step change index */
    int outputbuffer = 0;	/* place to keep previous 4-bit value */
    int bufferstep;		/* toggle between outputbuffer/output */

    outp = outdata;
    inp = indata;

    valpred = state->valprev;
    index = state->index;
    step = stepsizeTable[index];
    
    bufferstep = 1;

    for (; frameCount > 0 ; frameCount--) {
	val = *inp++;

	/* Step 1 - compute difference with previous value */
	diff = val - valpred;
	sign = (diff < 0) ? 8 : 0;
	if ( sign ) diff = (-diff);

	/* Step 2 - Divide and clamp */
	/* Note:
	** This code *approximately* computes:
	**    delta = diff*4/step;
	**    vpdiff = (delta+0.5)*step/4;
	** but in shift step bits are dropped. The net result of this is
	** that even if you have fast mul/div hardware you cannot put it to
	** good use since the fixup would be too expensive.
	*/
	delta = 0;
	vpdiff = (step >> 3);
	
	if ( diff >= step ) {
	    delta = 4;
	    diff -= step;
	    vpdiff += step;
	}
	step >>= 1;
	if ( diff >= step  ) {
	    delta |= 2;
	    diff -= step;
	    vpdiff += step;
	}
	step >>= 1;
	if ( diff >= step ) {
	    delta |= 1;
	    vpdiff += step;
	}

	/* Step 3 - Update previous value */
	if ( sign )
	  valpred -= vpdiff;
	else
	  valpred += vpdiff;

	/* Step 4 - Clamp previous value to 16 bits */
	if ( valpred > 32767 )
	  valpred = 32767;
	else if ( valpred < -32768 )
	  valpred = -32768;

	/* Step 5 - Assemble value, update index and step values */
	delta |= sign;
	
	index += indexTable[delta];
	if ( index < 0 ) index = 0;
	if ( index > 88 ) index = 88;
	step = stepsizeTable[index];

	/* Step 6 - Output value */
	if ( bufferstep ) {
	    outputbuffer = delta & 0x0f;
	} else {
	    *outp++ = ((delta << 4) & 0xf0) | outputbuffer;
	}
	bufferstep = !bufferstep;
    }

    /* Output last step, if needed */
    if ( !bufferstep )
      *outp++ = outputbuffer;
    
    state->valprev = valpred;
    state->index = index;
}

void _af_adpcm_decoder (const uint8_t *indata, int16_t *outdata, int frameCount,
	int channelCount, struct adpcm_state *state)
{
	const uint8_t *inp = indata;		/* input buffer pointer */
	int16_t *outp = outdata;		/* output buffer pointer */
	int step[channelCount];			/* step size */
	int valpred[channelCount];		/* predicted value */
	int index[channelCount];		/* current step change index */

	for (int c=0; c<channelCount; c++)
	{
		valpred[c] = state[c].valprev;
		index[c] = state[c].index;
		step[c] = stepsizeTable[index[c]];
	}

	assert((frameCount % 8) == 0);

	for (; frameCount > 0; frameCount -= 8)
	{
		for (int c=0; c<channelCount; c++)
		{
			int bufferstep = 0;
			uint8_t inputbuffer = 0; /* storage for next 4-bit encoded value */
			for (int s=0; s<8; s++)
			{
				int delta;
				/* Step 1 - get the delta value */
				if (bufferstep)
				{
					delta = (inputbuffer >> 4) & 0xf;
				}
				else
				{
					inputbuffer = *inp++;
					delta = inputbuffer & 0xf;
				}

				/* Step 2 - Find new index value (for later) */
				index[c] += indexTable[delta];
				index[c] = clamp(index[c], 0, 88);

				/* Step 3 - Separate sign and magnitude */
				int sign = delta & 8;
				delta = delta & 7;

				/* Step 4 - Compute difference and new predicted value */
				/*
				** Computes 'vpdiff = (delta+0.5)*step/4', but see comment
				** in adpcm_coder.
				*/
				int vpdiff = step[c] >> 3;
				if (delta & 4) vpdiff += step[c];
				if (delta & 2) vpdiff += step[c]>>1;
				if (delta & 1) vpdiff += step[c]>>2;

				if (sign)
					valpred[c] -= vpdiff;
				else
					valpred[c] += vpdiff;

				/* Step 5 - Clamp output value */
				valpred[c] = clamp(valpred[c], -32768, 32767);

				/* Step 6 - Update step value */
				step[c] = stepsizeTable[index[c]];

				/* Step 7 - Output value */
				outp[s*channelCount + c] = valpred[c];

				bufferstep = !bufferstep;
			}
		}
		outp += channelCount * 8;
	}

	for (int c=0; c<channelCount; c++)
	{
		state[c].valprev = valpred[c];
		state[c].index = index[c];
	}
}

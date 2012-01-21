/*	$Id: libaiff.h,v 1.28 2007/09/19 11:57:58 toad32767 Exp $ */

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

#ifndef LIBAIFF_H_INCL
#define LIBAIFF_H_INCL 1

#include <stdio.h>
#include <stdlib.h>
#include <libaiff/config.h>
#include <libaiff/endian.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#define LIBAIFF_API_VERSION	499


/* == Typedefs == */
typedef uint32_t IFFType ;
typedef uint8_t iext ;
typedef uint16_t MarkerId ;

/* Struct for AIFF I/O functions */
struct s_AIFF_Ref {
	FILE* fd;
	int flags;
	int stat; /* status */
	int segmentSize;
	int bitsPerSample;
	int nMarkers;
	int nChannels;
	double samplingRate;
	uint64_t nSamples;
	int markerPos;
	uint64_t len;
	uint64_t soundLen;
	uint64_t pos;
	uint64_t sampleBytes;
	uint64_t commonOffSet;
	uint64_t soundOffSet;
	uint64_t markerOffSet;
	IFFType format;
	IFFType audioFormat;
	void* decoder;
	void* pdata;
	void* buffer;
	size_t buflen;
	void* buffer2;
	size_t buflen2;
	int tics;
} ;

typedef struct s_AIFF_Ref* AIFF_Ref;
#ifdef LIBAIFF
static const size_t kAIFFRefSize = sizeof(struct s_AIFF_Ref);
#endif /* LIBAIFF */


/* 
 * == Interchange File Format (IFF) attributes ==
 */
#define AIFF_NAME		0x4e414d45
#define AIFF_AUTH		0x41555448
#define AIFF_COPY		0x28632920
#define AIFF_ANNO		0x414e4e4f

#if !defined(LIBAIFF) && !defined(LIBAIFF_NOCOMPAT)
/*
 * Backwards compatibility with LibAiff 1 and 2
 */
#ifdef WORDS_BIGENDIAN
static char NameID[4] = {'N', 'A', 'M', 'E'};
static char AuthID[4] = {'A', 'U', 'T', 'H'};
static char CopyID[4] = {'(', 'c', ')', ' '};
static char AnnoID[4] = {'A', 'N', 'N', 'O'};
#else
static char NameID[4] = {'E', 'M', 'A', 'N'};
static char AuthID[4] = {'H', 'T', 'U', 'A'};
static char CopyID[4] = {' ', ')', 'c', '('};
static char AnnoID[4] = {'O', 'N', 'N', 'A'};
#endif /* WORDS_BIGENDIAN */
#endif /* !LIBAIFF && !LIBAIFF_NOCOMPAT */

/*
 * Flags.
 * Flags which start with the F_ prefix are
 * user-specified and are documented.
 * Flags from (1<<20) to (1<<29) are reserved
 * to module private flags.
 */
#define F_RDONLY	(1<<0)
#define F_WRONLY	(1<<1)
#define LPCM_BIG_ENDIAN	(1<<2)
#define LPCM_LTE_ENDIAN	(1<<3)
#ifdef WORDS_BIGENDIAN
#define LPCM_SYS_ENDIAN	LPCM_BIG_ENDIAN
#define LPCM_NEED_SWAP	LPCM_LTE_ENDIAN
#else
#define LPCM_SYS_ENDIAN	LPCM_LTE_ENDIAN
#define LPCM_NEED_SWAP	LPCM_BIG_ENDIAN
#endif /* WORDS_BIGENDIAN */
#define F_AIFC		(1<<4)
#define F_OPTIMIZE	(F_AIFC | LPCM_SYS_ENDIAN)
#define F_NOTSEEKABLE	(1<<5)

/* Play modes */
#define kModeNoLooping			0
#define kModeForwardLooping		1
#define kModeForwardBackwardLooping	2

struct s_Loop
{
	int16_t playMode ;
	uint64_t beginLoop ;
	uint64_t endLoop ;
} ;

struct s_Instrument
{
	int8_t baseNote ;
	int8_t detune ;
	int8_t lowNote ;
	int8_t highNote ;
	int8_t lowVelocity ;
	int8_t highVelocity ;
	int16_t gain ;
	struct s_Loop sustainLoop ;
	struct s_Loop releaseLoop ;
} ;
typedef struct s_Instrument Instrument ;

/* == Function prototypes == */
AIFF_Ref AIFF_OpenFile(const char *, int) ;
int AIFF_CloseFile(AIFF_Ref) ;
char* AIFF_GetAttribute(AIFF_Ref,IFFType) ;
int AIFF_GetInstrumentData(AIFF_Ref,Instrument*) ;
size_t AIFF_ReadSamples(AIFF_Ref,void*,size_t) ;
int AIFF_ReadSamplesFloat(AIFF_Ref r, float *buffer, int n) ;
int AIFF_Seek(AIFF_Ref,uint64_t) ;
int AIFF_ReadSamples32Bit(AIFF_Ref,int32_t*,int) ;
int AIFF_ReadMarker(AIFF_Ref,int*,uint64_t*,char**) ;
int AIFF_GetAudioFormat(AIFF_Ref,uint64_t*,int*,double*,int*,int*) ;
int AIFF_SetAttribute(AIFF_Ref,IFFType,char*) ;
int AIFF_CloneAttributes(AIFF_Ref w, AIFF_Ref r, int cloneMarkers) ;
int AIFF_SetAudioFormat(AIFF_Ref,int,double,int ) ;
int AIFF_StartWritingSamples(AIFF_Ref) ;
int AIFF_WriteSamples(AIFF_Ref,void*,size_t) ;
int AIFF_WriteSamples32Bit(AIFF_Ref,int32_t*,int) ;
int AIFF_EndWritingSamples(AIFF_Ref) ;
int AIFF_StartWritingMarkers(AIFF_Ref) ;
int AIFF_WriteMarker(AIFF_Ref,uint64_t,char*) ;
int AIFF_EndWritingMarkers(AIFF_Ref) ;

#if !defined(LIBAIFF) && !defined(LIBAIFF_NOCOMPAT)
#define AIFF_ReadRef		AIFF_Ref
#define AIFF_WriteRef		AIFF_Ref
#define AIFF_Open(f)		AIFF_OpenFile((f), F_RDONLY)
#define AIFF_WriteOpen(f)	AIFF_OpenFile((f), F_WRONLY)
#define AIFF_Close(f)		(void) AIFF_CloseFile(f)
#define AIFF_WriteClose(f)	AIFF_CloseFile(f)
#define AIFF_SetSoundFormat(r,a,s,b) AIFF_SetAudioFormat((r),(a),(double)(s),(b))
#endif /* !LIBAIFF && !LIBAIFF_NOCOMPAT */

#ifndef LIBAIFF

#ifdef HAVE_BZERO
#undef HAVE_BZERO
#endif
#ifdef HAVE_ABORT
#undef HAVE_ABORT
#endif
#ifdef HAVE_INTTYPES_H
#undef HAVE_INTTYPES_H
#endif
#ifdef HAVE_MEMORY_H
#undef HAVE_MEMORY_H
#endif
#ifdef HAVE_MEMSET
#undef HAVE_MEMSET
#endif
#ifdef HAVE_STDINT_H
#undef HAVE_STDINT_H
#endif
#ifdef HAVE_STDLIB_H
#undef HAVE_STDLIB_H
#endif
#ifdef HAVE_STRINGS_H
#undef HAVE_STRINGS_H
#endif
#ifdef HAVE_STRING_H
#undef HAVE_STRING_H
#endif
#ifdef HAVE_SYS_STAT_H
#undef HAVE_SYS_STAT_H
#endif
#ifdef HAVE_SYS_TYPES_H
#undef HAVE_SYS_TYPES_H
#endif
#ifdef HAVE_UNISTD_H
#undef HAVE_UNISTD_H
#endif

#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#ifdef STDC_HEADERS
#undef STDC_HEADERS
#endif
#ifdef WORDS_BIGENDIAN
#undef WORDS_BIGENDIAN
#endif

#endif /* !LIBAIFF */


#endif /* LIBAIFF_H_INCL */



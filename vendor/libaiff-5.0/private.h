/* 	$Id: private.h,v 1.22 2007/09/19 12:00:59 toad32767 Exp $ */

#if !defined(HAVE_MEMSET) && defined(HAVE_BZERO)
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#define memset(a, b, c) bzero((a), (c))

#endif

#ifdef MIN
#undef MIN
#endif
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#ifdef ASSERT
#undef ASSERT
#endif

void AIFFAssertionFailed (const char*, int);

#define ASSERT(x) if(!(x)) AIFFAssertionFailed(__FILE__, __LINE__)

/* == Supported formats == */

/* File formats */
#define AIFF_TYPE_IFF  ARRANGE_BE32(0x464F524D)
#define AIFF_TYPE_AIFF ARRANGE_BE32(0x41494646)
#define AIFF_TYPE_AIFC ARRANGE_BE32(0x41494643)

/* Audio encoding formats */
#define AUDIO_FORMAT_LPCM  ARRANGE_BE32(0x4E4F4E45)
#define AUDIO_FORMAT_lpcm  ARRANGE_BE32(0x6C70636D)
#define AUDIO_FORMAT_twos  ARRANGE_BE32(0x74776F73)
#define AUDIO_FORMAT_sowt  ARRANGE_LE32(0x74776F73)
#define AUDIO_FORMAT_ULAW  ARRANGE_BE32(0x554C4157)
#define AUDIO_FORMAT_ulaw  ARRANGE_BE32(0x756C6177)
#define AUDIO_FORMAT_ALAW  ARRANGE_BE32(0x414C4157)
#define AUDIO_FORMAT_alaw  ARRANGE_BE32(0x616C6177)
#define AUDIO_FORMAT_FL32  ARRANGE_BE32(0x464c3332)
#define AUDIO_FORMAT_fl32  ARRANGE_BE32(0x666c3332)
#define AUDIO_FORMAT_UNKNOWN 0xFFFFFFFF

/* OSTypes */
#define AIFF_FORM 0x464f524d
#define AIFF_AIFF 0x41494646
#define AIFF_AIFC 0x41494643
#define AIFF_FVER 0x46564552
#define AIFF_COMM 0x434f4d4d
#define AIFF_SSND 0x53534e44
#define AIFF_MARK 0x4d41524b
#define AIFF_INST 0x494e5354
#define AIFF_COMT 0x434f4d54

/* Standards & specifications */
#define AIFC_STD_DRAFT_082691	2726318400U

struct s_IFFHeader
{
	IFFType hid ;
	uint32_t len ;
	IFFType fid ;
} ;
typedef struct s_IFFHeader IFFHeader ;

struct s_IFFChunk
{
	IFFType id ;
	uint32_t len ;
} ;
typedef struct s_IFFChunk IFFChunk ;

/*
 * WARNING: this struct is not alignment-safe!
 */
struct s_AIFFCommon
{
	uint16_t numChannels ;
	uint32_t numSampleFrames ;
	uint16_t sampleSize ;
	iext sampleRate ; /* Motorola 80-bit extended */
} ;
typedef struct s_AIFFCommon CommonChunk ;

struct s_AIFFSound
{
	uint32_t offset ;
	uint32_t blockSize ;
} ;
typedef struct s_AIFFSound SoundChunk ;

struct s_Marker
{
	MarkerId id ;
	uint32_t position ;
	uint8_t markerNameLen ;
	char markerName ;
} ;
typedef struct s_Marker Marker ;

struct s_AIFFMarker
{
	uint16_t numMarkers ;
	char markers ;
} ;
typedef struct s_AIFFMarker MarkerChunk ;

struct s_AIFFLoop
{
	int16_t playMode ;
	MarkerId beginLoop ;
	MarkerId endLoop ;
	uint16_t garbage ; /* not read (size=6 bytes) */
} ;
typedef struct s_AIFFLoop AIFFLoop ;


struct s_Comment
{
	uint32_t timeStamp ;
	MarkerId marker ;
	uint16_t count ;
	char text ;
} ;
typedef struct s_Comment Comment ;

struct s_AIFFComment
{
	uint16_t numComments ;
	char comments ;
} ;
typedef struct s_AIFFComment CommentChunk ;

struct decoder {
	IFFType fmt;
	int (*construct) (AIFF_Ref);
	size_t (*read_lpcm)(AIFF_Ref, void *, size_t);
	int (*read_float32)(AIFF_Ref, float *, int);
	int (*seek)(AIFF_Ref, uint64_t);
	void (*delete) (AIFF_Ref);
};

/* iff.c */
int find_iff_chunk(IFFType, AIFF_Ref, uint32_t *) ;
char* get_iff_attribute(AIFF_Ref r,IFFType attrib) ;
int set_iff_attribute(AIFF_Ref w,IFFType attrib,char* str) ;
int clone_iff_attributes(AIFF_Ref w, AIFF_Ref r) ;

/* aifx.c */
int init_aifx(AIFF_Ref) ;
int read_aifx_marker(AIFF_Ref r,int* id,uint64_t* position,char** name) ;
int get_aifx_instrument(AIFF_Ref r,Instrument* inpi) ;
int do_aifx_prepare(AIFF_Ref r) ;
char * get_aifx_enc_name(IFFType) ;

/* lpcm.c */
void lpcm_swap_samples(int,int,void*,void*,int);
void lpcm_dequant(int segmentSize, void *buffer, float *outFrames, int nFrames);
extern struct decoder lpcm;

/* g711.c */
extern struct decoder ulaw;
extern struct decoder alaw;

/* float32.c */
extern struct decoder float32;

/* extended.c */
void ieee754_write_extended (double, uint8_t*);
double ieee754_read_extended (uint8_t*);

/* pascal.c */
int PASCALInGetLength (FILE *);
char * PASCALInRead (FILE *, int *);
int PASCALOutGetLength (char *);
int PASCALOutWrite (FILE *, char *);



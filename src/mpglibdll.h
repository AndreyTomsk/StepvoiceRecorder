/*
 * mpglibdll.h - Interface file for mpglib.dll
 * See "example.c" how to use it.
 *
 * NOTE: The memory where mpstr is pointing to will be
 * handled by the mpglib as a complex struct which
 * needs about 20-30KB. For applications that use
 * the mpglib.dll it should be enough to allocate
 * the memory - mpglib knows what to do with it.
 * For details see mpglib/mpg123.h and mpglib/mpglib.h
 *
 * WARNING: If decodeMP3 returns MP3_ERR you should
 * instantly reinitialize the mpglib. Otherwise it could
 * crash. Originally the mpglib exits if a heavy error occurs.
 * This is disabled in the mpglib.dll (#define BE_QUIET)
 * to force playback of corrupted MP3 files.
 */

struct mpstr { char c[40000]; };

#define MP3_ERR       -1
#define MP3_OK         0
#define MP3_NEED_MORE  1

typedef BOOL (*INITMP3)   (struct mpstr *mp);
typedef void (*EXITMP3)   (struct mpstr *mp);
typedef int  (*DECODEMP3) (struct mpstr *mp, char *inmemory, int inmemsize,
                           char *outmemory,  int outmemsize, int *done);

#define TEXT_INITMP3   "_InitMP3"
#define TEXT_EXITMP3   "_ExitMP3"
#define TEXT_DECODEMP3 "_decodeMP3"

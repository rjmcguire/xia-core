#ifndef __XFTP_H__
#define __XFTP_H__

#undef XIA_MAXBUF
#define XIA_MAXBUF 500

#define MB(__mb) (KB(__mb) * 1024)
#define KB(__kb) ((__kb) * 1024)
#define CHUNKSIZE MB(4)


#endif

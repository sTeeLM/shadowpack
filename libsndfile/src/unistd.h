#ifndef _LIBSNDFILE_UNISTD_H_
#define _LIBSNDFILE_UNISTD_H_
#include <process.h>
#include <io.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>



#ifdef _WIN64
typedef __int64 ssize_t;
typedef __int64 pid_t;
#else
typedef int ssize_t;
typedef int pid_t;
#endif

#ifndef R_OK
#define R_OK 2
#endif 

#ifndef W_OK
#define W_OK 4
#endif 

#ifndef X_OK
#define X_OK 6
#endif 

#endif
#ifndef _NETPBM_UNISTD_H_
#define _NETPBM_UNISTD_H_
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
typedef int     ssize_t;
typedef int pid_t;
#endif

#endif
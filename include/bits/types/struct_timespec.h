#ifndef __timespec_defined
#define __timespec_defined 1

#include <bits/types.h>

#if __WORDSIZE == 32
__extension__ typedef long long int __syscall_slong_t;
#else
typedef long int __syscall_slong_t;
#endif

/* POSIX.1b structure for a time value.  This is like a `struct timeval' but
   has nanoseconds instead of microseconds.  */
struct timespec
{
  __time_t tv_sec;		/* Seconds.  */
  __syscall_slong_t tv_nsec;	/* Nanoseconds.  */
};

#endif

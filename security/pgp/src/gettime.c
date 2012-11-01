/*
 *	Gettimeofday.  Simulate as much as possible.  Only accurate
 *	to nearest second.  tzp is ignored.  Derived from an old
 *	emacs implementation.
 */

#include <sys/types.h>
#include <sys/time.h>

gettimeofday (tp, tzp)
     struct timeval *tp;
     struct timezone *tzp;
{
  extern long time ();

  tp->tv_sec = time ((long *)0);
  tp->tv_usec = 0;
}

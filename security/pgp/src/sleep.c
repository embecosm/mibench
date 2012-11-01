/*************
* sleep.c -- provide unix style sleep function
*
*************/
#include <time.h>

int sleep(unsigned secs){
	long	start;
	long	check;
	long	finish;

	time(&start);
	finish = start + (long) secs;
#ifdef DEBUG
	printf ("sleep for %d secs, stop sleeping at %ld\n", secs, finish);
	time(&check);
	printf ("it is now %ld\n", check );
#endif
	for (;;) {
		time(&check);
		if (check > finish)
			break;
	}
	return (0);
}

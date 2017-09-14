#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

// average gettimeofday call ~25 us
// (for reference, printf was ~4000ms
struct timeval tv;
struct timezone *tz;
FILE * fout;

void KSVprint_time (char * str, long node)
{
	gettimeofday (&tv, tz);
	fprintf(fout, "%s %d: %d  %d\n", str, node,
			tv.tv_sec, tv.tv_usec);
}

void KSVinit_time ()
{
	if (!(fout = fopen ("KSV.annote", "a"))) {	
		exit (-1);
	}	
}

void KSVend_time ()
{
	gettimeofday (&tv, tz);
	fprintf(fout, "%s %d: %d  %d\n", "main end", 0,
			tv.tv_sec, tv.tv_usec);
}
// header
//void KSVprint_time (char * str, long node);
//void KSVinit_time ();

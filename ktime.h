// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  ktime.h

  Header file for ktime.cc.  Auxilary file to define TimeUnit.
TimeUnit stores the results of gettimeofday and gives it a
good interface.
************************************************************/

#ifndef KTIME_H_
#define KTIME_H_

#include <map>
#include <fstream>

// TimeUnit stores the results of gettimeofday and gives it a
// good interface.
class TimeUnit {
	long sec_;   // seconds
	long usec_;  // microseconds
public:
	// Constructors
	TimeUnit ();
	TimeUnit (long s, long us);

	// update
	void set_time (long s, long us) { sec_ = s; usec_ = us; }

	// query
	long sec() { return sec_; }
	const long sec() const { return sec_; }
	long usec() { return usec_; }
	const long usec() const { return usec_; }

	// overloaded operators
	TimeUnit operator+ (TimeUnit tu);
	TimeUnit operator- (TimeUnit tu);
	bool operator< (TimeUnit tu);
};

// overloaded output operator
std::ostream & operator<<(std::ostream & os, const TimeUnit & tu_a);

#endif



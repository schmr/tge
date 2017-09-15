// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  ktime.cc

  This is a support file which defines the class TimeUnit.
TimeUnit is used to store time info from gettimeofday calls.
************************************************************/

#include "ktime.h"

// Constructors  (units are seconds and microseconds)
TimeUnit::TimeUnit () :
	sec_(-1), usec_(-1)
{
}

TimeUnit::TimeUnit (long s, long us) :
	sec_(s), usec_(us)
{
}

// Overload addition
TimeUnit TimeUnit::operator+ (TimeUnit tu)
{
	TimeUnit tmp;

	tmp.sec_ = tu.sec() + sec_;
	tmp.usec_ = tu.usec() + usec_;
	
	return tmp;
}

// Overload subtraction
TimeUnit TimeUnit::operator- (TimeUnit tu)
{
	TimeUnit tmp;

	tmp.sec_ = -tu.sec() + sec_;
	tmp.usec_ = -tu.usec() + usec_;

	if (tmp.usec_ < 0) {
		tmp.sec_--;
		tmp.usec_ += 1000000L;
	}
	return tmp;
}

// Overload a comparator...the other ones aren't done
bool TimeUnit::operator< (TimeUnit tu)
{
	if (tu.sec_ > sec_) {
		return true;
	} else if ((tu.sec_ == sec_) && (tu.usec_ > usec_)) {
		return true;
	}
	return false;
}

// overload output operator
std::ostream & operator<<(std::ostream & os, const TimeUnit & tu_a)
{
	os << tu_a.sec() + tu_a.usec()/1000000.0;
	return os;
}


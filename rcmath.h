// Copyright 2000 by Robert Dick.
// All rights reserved.

#ifndef R_CMATH_H_
#define R_CMATH_H_

/* Ugly tricks and hacks needed to use part of ISOC99 cmath header on
different platforms.

FIXME: This should go away when ISOC99 is supported correctly.
FIXME: This mess can go away when the compiler supports numeric_limits<>.
FIXME: When numeric limits used, turn off ISOC99 extensions. */

/*###########################################################################*/
#include <cmath>

// Don't use this.
const double R_HUGE_VAL_ = HUGE_VAL;
#undef HUGE_VAL

// Use this.
const double HUGE_VAL = R_HUGE_VAL_;

#ifdef SunOS
extern "C" {
#	include <ieeefp.h>
}
#endif

inline bool isfinite(double x) { return finite(x); }

/*###########################################################################*/
#endif

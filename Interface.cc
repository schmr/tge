// Copyright 2000 by Robert Dick.
// All rights reserved.

#include "Interface.h"

/*===========================================================================*/
ostream &
operator<<(ostream & os, comp_type c) {
	switch (c) {
	case LESS:
		os << "less";
		break;
	case GREATER:
		os << "greater";
		break;
	case EQ:
		os << "equal";
		break;
	default:
		Rabort();
	}

	return os;
}


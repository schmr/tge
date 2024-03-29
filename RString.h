// Copyright 2000 by Robert Dick.
// All rights reserved.

#ifndef R_STRING_H_
#define R_STRING_H_

/*###########################################################################*/
#include "RFunctional.h"

#include <string>
#include <strstream>

template <typename T> class RVector;

/*===========================================================================*/
// Support routines for strings.

const RVector<string>
	tokenize(const string &s, const char * delim = " \t\n");

const string first_token(const string & s, const char * delim = " \t\n");
void pop_token(string & s, const char * delim = " \t\n");

template <typename T>
	struct insen_less : rbinary_function<const T &, const T &, bool>
	{ bool operator()(const T & a, const T & b) const; };

template <typename T>
	struct insen_equal_to : rbinary_function<const T &, const T &, bool>
	{ bool operator()(const T & a, const T & b) const; };

template <>
	struct insen_less<char>
	{ bool operator()(const char & a, const char & b) const; };

template <>
	struct insen_equal_to<char>
	{ bool operator()(const char & a, const char & b) const; };

template <>
	struct insen_less<string>
	{ bool operator()(const string & a, const string & b) const; };

template <>
	struct insen_equal_to<string>
	{ bool operator()(const string & a, const string & b) const; };

/*===========================================================================*/
// Automatically converts strings to many other types.

class Conv {
public:
	Conv(const string & str);

	operator bool();

	operator signed char();
	operator unsigned char();

	operator short();
	operator unsigned short();

	operator int();
	operator unsigned();

	operator long();
	operator unsigned long();

	operator float();
	operator double();

private:
		string str_;
};

template <typename T> string to_string(const T & data);

/*===========================================================================*/
void RString_test();

/*###########################################################################*/
#include "RString.cct"
#endif

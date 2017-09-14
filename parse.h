// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  parse.h

  Header file used to interface scan.l, gram.y and parse.cc
************************************************************/


#ifndef PARSE_H_
#define PARSE_H_
#include <string>

// global variables used by scan.l, gram.y and parse.cc
extern int yytypedef_flag;
extern int column;

// interface functions
void yytypedef_table_add(string str);
char yytypedef_table_lookup(string str);
void yypopulate_tree (string str, long n);
void yyadd_whitespace (string str);

#endif


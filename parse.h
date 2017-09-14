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
void yytypedef_table_add(std::string str);
char yytypedef_table_lookup(std::string str);
void yypopulate_tree (std::string str, long n);
void yyadd_whitespace (std::string str);

#endif


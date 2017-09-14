// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  parse.cc

  This file provides interface between the lex/yacc code and
the rest of the program.  'lex.yy.cc' tokenizes the input C
and passes the values to 'gram.tab.cc'.  'gram.tab.cc' parses
these tokens and calls yypopulate_tree found in this file.

Functions of note:
------------------
do_parse: starts the yacc parser

yytypedef_table_add: used by lex code to add a user-defined datatype

yytypedef_table_lookup: used by lex code to determine if a 
user-defined word is a type name or just an identifier

yypopulate_tree:  an interface function to the tree member of
the database class and is used to construct the abstract syntax 
tree (AST).

yyadd_whitespace: stores whitespace with tokens so source code
can be reprinted almost exactly
************************************************************/

#include "parse.h"
#include <iostream>
#include <stdio.h>
#include "database.h"

int yyparse();
extern FILE *yyin;

int column = 0;
int yytypedef_flag = 0;
string yywhitespace ("");

// starts yacc parser
void do_parse(string s)
{
	yyin = fopen (s.c_str(), "r");
	Rassert(yyin);
	yyparse();
}

// adds str to the list of known data type names
void yytypedef_table_add(string str)
{
	string tmpstr = str;
//	cerr << "type: " << tmpstr << endl;
	DB::write_db().type_map_insert(tmpstr, 0);
}

// determines if str is a data type name or just an ordinary word
char yytypedef_table_lookup(string str)
{
	string s = str;
	if ((DB::write_db().type_map_lookup(s)) == -1) {
		return 0;
	} else {
		return 1;
	}
}

// adds str to AST and tells the tree that it has n children
void yypopulate_tree (string str, long n)
{
	string tmpstr = str;
//cout << n << ":   " << tmpstr << endl;

	if (n == 0) {
		DB::write_db().add_tree_node(tmpstr, n, yywhitespace);
		yywhitespace = "";
	} else {
		DB::write_db().add_tree_node(tmpstr, n, "");
	}
}

// used to make reprinting the source code look better
void yyadd_whitespace (string tmpstr)
{
	string tmp_str = tmpstr;
	yywhitespace += tmp_str;
}


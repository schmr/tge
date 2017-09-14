// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  ArgPack.cc

  This file contains a global datastructure 'def_ap_' which
contains the global variable information.  This is used to
hold parameters passed to the program at run time.  This way
they are all held in one location.
************************************************************/

#include "ArgPack.h"

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <strstream>

#include "RVector.h"
#include "RString.h"

/*###########################################################################*/
auto_ptr<ArgPack> ArgPack::def_ap_;
/*===========================================================================*/
void ArgPack::init(int argc, char * const argv [], ostream & output) {
	def_ap_.reset(new ArgPack(argc, argv, output));
}

/*===========================================================================*/
ArgPack::ArgPack(int argc, char *const argv [], ostream & output) :
	file_name_(),
	is_(),
	is_line_(1),
	rand_seed (1),
	echo_on_ (false),
	display_state_end_ (true),
	strict_dependence_ (false),
	profile_ (true),
	interpret_ (true),
	pointer_size_ (-1),
	display_arc_variables_(false),
	loosest_dependence_(false),
	display_actions_(true),
	display_edge_labels_(true),
	calculate_edge_lengths_(true),
	create_task_graph_(false)
{
	opterr = 0;

	// List of possible flag identifiers
	string usage = string("usage: ") + argv[0] + 
			" [-AeEghHILlp:PsStv] [file1]\n";

	while (1) {
		// List of possible flag identifiers
		long ch = getopt(argc, argv, "AeEghHILlp:PsStv");
    
		long parsed = -1;

		if (ch == -1)
			break;

		RVector<string> vec;

		// Parses flags
		switch (ch) {
			case 'A':
				display_actions_ = false;
				break;
			case 'e':
				echo_on_ = true;
				break;
			case 'E':
				calculate_edge_lengths_ = false;
				display_edge_labels_ = false;
				break;
			case 'g':
				create_task_graph_ = true;
				break;
	      case 'h':
				help(output);
				throw init_error("");
				break;
			case 'H':
				help(output);
				throw init_error("");
				break;
			case 'I':
				interpret_ = false;
				break;
			case 'L':
				display_edge_labels_ = false;
				break;
			case 'l':
				loosest_dependence_ = true;
				break;
			case 'p':
				pointer_size_ = Conv(optarg);
				break;
			case 'P':
				profile_ = false;
				break;
			case 's':
				strict_dependence_ = true;
				break;
			case 'S':
				display_state_end_ = false;
				break;
			case 't':
				extern int yydebug;
				yydebug = 1;
				break;
			case 'v':
				display_arc_variables_ = true;
				break;
			case '?':
				help(output);
				throw init_error(usage);
		}

		if (! parsed || parsed > 2) {
			output << "Parsing error during ``" << ch << "'' flag.\n";
			throw init_error("Parsing error");
		}
	}

	// Last argument should be the source file name
	argc -= optind;
	argv += optind;
	if (argc > 1) {
		throw init_error(usage);
	} else if (argc == 1) {
		file_name_ = argv[0];
	}

	if (file_name_ == "") {
		throw init_error(usage);
	} else {
		is_.open(argv[0]);
		if (! is_) {
			output << "Input file ``" << file_name_ 
					<< "'' could not be opened.\n";
			throw init_error("");
		}
	}
}

/*===========================================================================*/
void ArgPack::help(ostream &os) const {
	os <<
	  "A:  Don't display actions inside of nodes for vcg file.\n" <<
	  "e:  Echo tokens while they are read.\n" <<
	  "E:  Don't calculate or display edge lengths for vcg file.\n" <<
	  "g:  Generate a vcg file using node weights and edge values.\n" <<
	  "h:  Display help info.\n" <<
	  "H:  Display help info.\n" <<
	  "I:  Only parse input.  Don't analyze.\n" <<
	  "L:  Don't display edge labels in vcg file\n" <<
	  "l:  Use loosest dependence. (Use w/ K&R style C.)\n" <<
	  "p:  Set pointer data size.\n" <<
	  "P:  Don't profile\n" <<
	  "s:  Use stricter dependence.\n" <<
	  "S:  Don't generate new nodes for statement_end keyword.\n" <<
	  "t:  Turn on Bison debugging for parsing.\n" <<
	  "v:  Print variable names for arcs in '.tg' file.\n";
}

/*===========================================================================*/
void ArgPack::parse_error(ostream & os, const char message[]) const {
	os << message << " on line " << is_line_ << "\n";
	throw init_error("");
}

/*===========================================================================*/
const RVector<string>
ArgPack::get_line() {
	string str;

	string::size_type text_pos;
	do {
		getline(is_, str);
		if (! is_) return RVector<string>();
		is_line_++;

		text_pos = str.find_first_not_of(" \t");
	} while (text_pos >= str.size() || str[text_pos] == '#');

	return tokenize(str);
}

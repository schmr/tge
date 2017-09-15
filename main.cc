// Copyright 2002 by Keith Vallerio.
// All rights reserved.

#include "ArgPack.h"
#include <iostream>
#include <fstream>
#include "database.h"

void do_parse(std::string s);
/*###########################################################################*/
int main(int argc, char *argv[]) {
	try {
		// read input arguments and store settings in one class
		ArgPack::init(argc, argv, std::cout);
		std::ofstream os_;

		// generate a database to contain all processing information
		DB single_db;

		// get input filename and run lex and yacc (parse) it
		std::string str = ArgPack::write_ap().get_fname();
		do_parse(str);

		// if interpret flag set, analyze program structure
		if (ArgPack::ap().interpret()) {
			DB::write_db().interpret();

			// if profile flag set, profile, recompile and generate TG
			if (ArgPack::ap().profile()) {
				DB::write_db().profile();
				DB::write_db().print_task_graphs();
				// also generate TG in "standard" format
				DB::write_db().print_standard_task_graphs();
			}

			// print both vcg files
			DB::write_db().print_tgraph_vcg();
			DB::db().print_tree_vcg();
		}

	// for error conditions
	} catch (ArgPack::init_error & ie) {
		std::cout << ie.what();
	}
}














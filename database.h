// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  database.h

  The database is the main class of the program.  It contains
the AST, functions, variables, call graph, and task graphs.
This class provides interface between its various components.
Maps are used to provide quick lookups of user-defined data 
types, variables and functions.
************************************************************/

#ifndef DATABASE_H_
#define DATABASE_H_

#include "RVector.h"
#include "RString.h"
#include <iostream>
#include <fstream>
#include <map>
#include "function.h"
#include "tree.h"
#include "node.h"
#include "fgraph.h"

// DB contains everything
class DB {
	// contains all functions and variables in original source
	rstd::RVector<Function> function_;
	rstd::RVector<Var> variable_;

	// pointer to the only DB (there can be only one)
	static DB * def_db_;

	// the AST
	rstd::RVector<TreeNode> tree_node_;
	rstd::RVector<long> tree_start_;

	// Maps for quick lookups of vectors in DB
	std::map<std::string, long> type_map_;
	std::map<std::string, long> func_map_;
	std::map<std::string, long> var_map_;

	// for parsing tree
	bool fn_redeclaration_;

	// Task (dependence) graphs for each function
	TGraph tg_;
	rstd::RVector<TGraph> fg_;

	// function call graph
	FGraph call_graph_;

	// records profiling information
	rstd::RVector<rstd::RVector<std::string> > annote_data_;
	
	rstd::RVector<std::string> func_def_name_list_;
	std::set<std::string> undefined_structs_;
public:
	// Constructor
   DB ();

	// main analysis functions
	void interpret();
	void profile();

	// annotate original C to indicate which tasks are which
	void print_task_graph_c ();

	// profiling functions
	void print_annotated_c ();
	void prof_recompile();
	void prof_rerun();
	void prof_read_data();
	void prof_get_arc_lengths();
	long determine_node_function_out_size (long f, long n);

	// profiling cleanup functions
	void set_all_fc_task_times ();
	void set_se_task_time (long f, long tree_node, TimeUnit t);
	void set_lc_task_time (long f, long tree_node, TimeUnit t);

	// generate vcg task graphs
	void print_task_graphs();
	void print_standard_task_graphs();

	// intepretation functions
	void do_dependence();
	void build_call_graph();
	TGraph fn_dep (long id);
	rstd::RVector<long> add_alias_data (long fn, std::pair<long,long> a_node);
	rstd::RVector<long> do_func_call (long id, long fc_num);
	long get_assign_lhs_var (long fn, std::pair<long,long> a_node);
	void update_lhs_var_aliases (long id, rstd::RVector<long> lhs_var);

	// Gets the default Dbase
	static DB & write_db() { return *def_db_; }
	static const DB & db() { return *def_db_; }

	// construction
	Function function(long i) { return function_[i]; }
	const Function function(long i) const { return function_[i]; }
	Var variable(long i) { return variable_[i]; }
	const Var variable(long i) const { return variable_[i]; }

	long add_function (std::string n, std::string t, rstd::RVector<std::string> s);
	long add_func_call (long f, std::string c, long r, long node);
	void add_variable (long f, std::string n, std::string t, rstd::RVector<std::string> s,
							long size);
	void add_parameter (long f, std::string n, std::string t, rstd::RVector<std::string> s);
	void add_func_call_arg (long f, long c, long a, ArgType atype);

	// query
	long size_function () { return function_.size(); }
	const long size_function () const { return function_.size(); }
	long size_variable () { return variable_.size(); }
	const long size_variable () const { return variable_.size(); }

	long func_map (std::string s);
	long var_map (std::string s);
	void set_function_defined (long f) { function_[f].set_defined(); }
	long get_literal_id (std::string s);

	bool type_map_insert(std::string s, long length);
	long type_map_lookup(std::string s);

	////////////////////////////////////////
	// AST related functions
	////////////////////////////////////////
	// main AST analysis function
	void interpret_tree();

	// AST analysis helper functions
	long get_ancestor (long n, std::string s);
	long has_child (long n, std::string s);
	std::string node_child_name (long n, long c);
	std::string node_parent_name (long n);
	std::string get_first_var_child (long n, long curr_function);
	long get_next_literal (long tn);

	// AST construction
	void add_tree_node(std::string str, int num_subtrees, std::string space);

	// output functions
	void print_terminals () const;
	void print_fgraph_vcg ();
	void print_tgraph_vcg ();
	void print_tree_vcg () const;

	std::string create_var_name (std::string s, long f);
};

// utility functions
void vector_merge (rstd::RVector<std::string> a, rstd::RVector<std::string> b,rstd::RVector<std::string> &out);
std::ostream & operator<<(std::ostream & os, const DB & db_a);
#endif










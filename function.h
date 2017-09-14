// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  funciton.h

  A main file which defines Var, FuncCall and Function classes.
These classes are used to determine and store semantic information
about the variables, function calls and function definitions
inside the original C code.  FuncCall is its own class because
function calls have a lot of info which must be stored.
************************************************************/


#ifndef FUNCTION_H_
#define FUNCTION_H_

#include "RVector.h"
#include "RString.h"
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <utility>
#include "node.h"
#include <algorithm>

// Arguments to functions can be variables, constants or functions.
enum ArgType { VAR, FUNC, CONST };

///////////////////////////////////////////////////
// Actions:
///////////////////////////////////////////////////
// "Actions" are used to divide the original source code into
// logical units to simply analysis.  There are many different 
// types of "Actions".  The types are enumerated below: 
//    variable use and declarations:  self-explanitory
//    assignments and assignment ends: indicate boundaries of assigns 
//    ends of statements: to delinate between statements
//    function start/end: to handle nested function calls
//    loop/conditional starts/ends: to get nesting info correct
//		statement numbers: for ordering...not needed anymore
//    short assignments: increments/decrements
//    bracket open/close: not used anymore
enum ActionType { VAR_USE, VAR_DECL, ASSIGN, ASSIGN_END, 
			STATE_END, FN_START, FN_END, LOOP_START, LOOP_END, 
			SELECT_START, SELECT_END, STATE_NUM, SHORT_ASSIGN_VAR,
			BRAC_OPEN, BRAC_CLOSE };

// Var holds information about a variable in the original source
class Var : public Prints<Var> {
	// 'name_' has variable's name + "__#" to indicate which function
	//     that variable is found in
	// 'type_' is variables data type
	// 'total_size_' number of bytes variable takes (pointers are special)
	// 'num_elements_' number of elements if array, 1 otherwise
	// 'stype' attributes such as pointer, const, static, etc.
	string name_;
	string type_;
	long total_size_;
	long num_elements_;
	RVector<string> stype_;
	bool is_pointer_;
	bool is_static_;
public:
	// Constructors
	Var (string n, string t, RVector<string> s, long num_elements);

	// Construction
	void add_type (string s) { type_ = s; }
	void determine_total_size (long type_size);

	// Interface
	string name() { return name_; }
	const string name() const { return name_; }
	string type() { return type_; }
	const string type() const { return type_; }
	long total_size() { return total_size_; }
	const long total_size() const { return total_size_; }
	long num_elements() { return num_elements_; }
	const long num_elements() const { return num_elements_; }
	RVector<string> stype() { return stype_; }
	const RVector<string> stype() const { return stype_; }
	bool is_pointer() { return is_pointer_; }
	const bool is_pointer() const { return is_pointer_; }
	bool is_static() { return is_static_; }
	const bool is_static() const { return is_static_; }
};

// FuncCall stores information reguarding function calls.  'name' is
// the name of the function called.  'parms_' indicates the index
// and type of a parameter passed at call time.  'root_node_' is 
// used to determine position of the function call within the AST.
class FuncCall : public Prints<Var> {
	string name_;
	RVector<pair<long,ArgType> > parms_;
	long commas_;
	long root_node_;
	long fn_name_node_;
public:
	// Constructor
	FuncCall(string n, long r, long nnode);

	// Construction
	void add_name(string s) { name_ = s; }
	void add_parm(pair<long,ArgType> p);
	void add_comma();
	void balance_commas();

	// Interface
	string name() { return name_; }
	const string name() const { return name_; }
	RVector<pair<long,ArgType> > parms() { return parms_; }
	const RVector<pair<long,ArgType> > parms() const { return parms_; }
	long root_node() { return root_node_; }
	const long root_node() const { return root_node_; }
	long fn_name_node() { return fn_name_node_; }
	const long fn_name_node() const { return fn_name_node_; }
};

// Function contains all the known information about the function
// it contains.  'v2v_alias_' is used to store which variables
// are aliased to other variables within the function.
// 'vars_touched_' is used to record which variables the function
// could potentially use/modify when called.
class Function : public Prints<Function> {
	string name_;   // function name
	string type_;   // return value
	RVector<string> stype_;  // return value modifiers (i.e., pointer)

	// indexes (in global variable list) of paramters and variables
	RVector<long> parameter_;
	RVector<long> variable_;

	RVector<string> var_use_;

	// list of actions
	RVector<string> action_;
	RVector<ActionType> action_type_;
	RVector<long> action_tree_node_;

	RVector<FuncCall> func_call_;
	RVector<pair <long, long> > assign_node_;
	bool defined_;		// if func is defined, or just a header
	bool self_dependent_;  // if it has dependence between calls to itself

	// to handle interprocedural dependence analysis
	set<long> vars_touched_;
	RVector<set<long> > v2v_alias_;
	bool elipses_;
public:
	// Constructors
	Function (string n, string t, RVector<string> s);

	// Analysis routines
	RVector<string> func_call_names ();
	void add_aliases (long target, RVector<long> vars, bool clear_old_alias);
	void init_alias_vectors (long v_size);
	bool var_in_v_alias (long v, long v_set_num);
	void remove_from_v_alias (long v, long v_set_num);
	void append_to_v_alias (long target_num, long source_num);
	set<long> func_call_cleanup_aliases (RVector<pair<long,ArgType> > p,
														RVector<long> parms);
	RVector<long> v_alias_vec(long v);
	set<long> parameter_set();

	// Construction routines
	void add_name(string s) { name_ = s; }
	void add_type(string s) { type_ = s; }
	void add_stype (RVector<string> s) { stype_ = s; }
	void add_parameter(long v);
	void add_variable(long v);
	void add_elipses();
	void add_var_use (string v) { var_use_.push_back(v); }
	long add_func_call(string n, long r, long nnode);
	void add_func_call_arg (long c, long arg, ArgType arg_type);
	void add_action (string n, ActionType t, long tn);
	void add_assign_node (pair<long,long> p) { assign_node_.push_back(p); }
	void set_defined () { Rassert(!defined_); defined_ = true; }
	void set_vars_touched (set<long> s) { vars_touched_ = s; }
	void set_v2v_alias (RVector<set<long> > s) { v2v_alias_ = s; }
	void set_v2v_alias_vec (long v, set<long> s) { v2v_alias_[v] = s; }

	void add_func_call_comma (long fc_num);
	void balance_func_call_commas (long fc_num);

	// query methods
	string name() { return name_; }
	const string name() const { return name_; }
	string type() { return type_; }
	const string type() const { return type_; }
	RVector<string> stype() { return stype_; }
	const RVector<string> stype() const { return stype_; }
	RVector<long> parameter() { return parameter_; }
	const RVector<long> parameter() const { return parameter_; }
	RVector<long> variable() { return variable_; }
	const RVector<long> variable() const { return variable_; }
	RVector<string> var_use() { return var_use_; }
	const RVector<string> var_use() const { return var_use_; }
	RVector<string> action() { return action_; }
	const RVector<string> action() const { return action_; }
	RVector<ActionType> action_type() { return action_type_; }
	const RVector<ActionType> action_type() const { return action_type_; }
	RVector<long> action_tree_node () { return action_tree_node_; }
	const RVector<long> action_tree_node () const { return action_tree_node_; }

	RVector<FuncCall> func_call() { return func_call_; }
	const RVector<FuncCall> func_call() const { return func_call_; }
	RVector<pair <long,long> > assign_node() { return assign_node_; }
	const RVector<pair <long,long> > assign_node() const 
			{ return assign_node_; }

	bool defined() { return defined_; }
	const bool defined() const { return defined_; }
	set<long> vars_touched() { return vars_touched_; }
	const set<long> vars_touched() const { return vars_touched_; }


	RVector<set<long> > v2v_alias() { return v2v_alias_; }
	const RVector<set<long> > v2v_alias() const { return v2v_alias_; }
	set<long> v2v_alias_var (long v) { return v2v_alias_[v]; }
	const set<long> v2v_alias_var (long v) const { return v2v_alias_[v]; }

	bool elipses() { return elipses_; }
	const bool elipses() const { return elipses_; }
	
};

// printout routines
ostream & operator<<(ostream & os, const Var & var_a);
ostream & operator<<(ostream & os, const FuncCall & fc_a);
ostream & operator<<(ostream & os, const Function & fn_a);

ostream &operator <<(ostream &output, set<long> &sa);

#endif


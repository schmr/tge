// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  node.h

  Header file for node.cc.  Defines Node, Edge and TGraph classes.
A TGraph is a task graph (dependence graph if it has no weights) for
a particular function.  A TGraph has Nodes and Edges.  The
connectivity methods are inherited from a foundation graph template.
************************************************************/

#ifndef NODE_H_
#define NODE_H_

#include "ArgPack.h"
#include "RVector.h"
#include "RString.h"
#include "Graph.h"
#include "ktime.h"
#include <set>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>

// Nodes correspond to one or multiple "actions" in the original
// C code.  An "action" consists of elements such as loops,
// statements, function calls, and variable uses.
class Node {
	string name_;
	RVector<string> action_;   // List of "actions" in node
	long start_;	// Treenode val of first statement
	long end_;		// Treenode val of last statement
	set<long> var_use_;	// indecies of variables used in node
	set<long> var_mod_;	// indecies of variables updated in node
	RVector<long> func_;
	map<long, long> func_map_;
	bool is_assign_node_;
	bool is_statement_node_;
	bool is_loop_or_cond_node_;
	TimeUnit time_unit_;    // Execution time for code in node
public:
	// Constructor
	Node (string n);

	// Update methods
	void add_action (string s, long tn);
	void add_var_use (long v);
	void add_var_mod (long v);
	void add_var_use (RVector<long> v);
	void add_var_mod (RVector<long> v);
	void add_func (long v);
	void set_time_unit (long s, long us);

	// Query methods
	bool no_actions () { return action_.empty(); }
	long func_map (long v);	

	string name() { return name_; }
	const string name() const { return name_; }
	RVector<string> action() { return action_; }
	const RVector<string> action() const { return action_; }
	long start () { return start_; }
	const long start () const { return start_; }
	set<long> var_use() { return var_use_; }
	const set<long> var_use() const { return var_use_; }
	set<long> var_mod() { return var_mod_; }
	const set<long> var_mod() const { return var_mod_; }
	RVector<long> func() { return func_; }
	const RVector<long> func() const { return func_; }
	bool is_assign_node() { return is_assign_node_; }
	const bool is_assign_node() const { return is_assign_node_; }
	bool is_statement_node() { return is_statement_node_; }
	const bool is_statement_node() const { return is_statement_node_; }
	bool is_loop_or_cond_node() { return is_loop_or_cond_node_; }
	const bool is_loop_or_cond_node() const { return is_loop_or_cond_node_; }
	TimeUnit time_unit() { return time_unit_; }
	const TimeUnit time_unit() const { return time_unit_; }
};

// Edges are used to indicate dependencies between Nodes.  'val_'
// indicates the size (in bytes).  'vars_' indicates which variables
// cause the dependency.
class Edge {
	long val_;
	set<long> vars_;
public:
	// Constructor
	Edge (long v);

	// Update
	void set_val (long v) { val_ = v; }
	void add_var (long s) { vars_.insert(s); }
	void set_vars (set<long> s) { vars_ = s; }

	// Query
	long val() { return val_; }
	const long val() const { return val_; }
	set<long> vars() { return vars_; }
	const set<long> vars() const { return vars_; }
};

// TGraph inherits from Graph template.  It contains Nodes and
// Edges.  The connectivity methods come from the Graph template.
// Info defined here is specific to task (dependence) graphs.
// (A dependence graph is a task graph without node or edge weights.)
class TGraph : public Graph<Node, Edge> {
	long val_;
	string name_;    // name of the function this graph is for
	TimeUnit time_unit_;   // execution time of whole function
	RVector<long> tree_node_starts_;

	// 3 lists to sort out nodes by type slightly
	RVector<long> func_call_nodes_;  // nodes with function calls
	RVector<long> statement_nodes_;  // nodes with statement ends
	RVector<long> loop_or_cond_nodes_;  // nodes with loop/conds 
public:
	// Constructor
	TGraph(string n);

	// interface methods
	void print_vcg_inside (ofstream &out_file);
	long get_new_node (long id);
	void make_depend_arcs ();
	void compact_graph();
	void find_tree_node_starts ();
	void make_node_lists();
	long find_fc_node(long tn);
	long find_se_node(long tn);
	long find_lc_node(long tn);
	set<long> determine_vars_touched();
	void set_time_unit (TimeUnit t);

	// query
	string name() { return name_; }
	const string name() const { return name_; }
	TimeUnit time_unit() { return time_unit_; }
	const TimeUnit time_unit() const { return time_unit_; }
	RVector<long> tree_node_starts() { return tree_node_starts_; }
	const RVector<long> tree_node_starts() const { return tree_node_starts_; }
	RVector<long> func_call_nodes() { return func_call_nodes_; }
	const RVector<long> func_call_nodes() const { return func_call_nodes_; }
	RVector<long> statement_nodes() { return statement_nodes_; }
	const RVector<long> statement_nodes() const { return statement_nodes_; }
	RVector<long> loop_or_cond_nodes() { return loop_or_cond_nodes_; }
	const RVector<long> loop_or_cond_nodes() const{return loop_or_cond_nodes_;}

	// Routines for merging
	bool check_if_merge_forms_cycle (vertex_index a, vertex_index b);
	int merge_tasks(vertex_index a, vertex_index b);
	void copy_vertex_data (vertex_index from, vertex_index to);
};

#endif


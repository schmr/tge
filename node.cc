// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  node.cc

  This file handles the Task Graph classes.  A task graph consists
of nodes and edges.  Nodes contain information from the input source
code.  This info is in the form of actions and variables.  Nodes
record variable use/modification, func calls, assignments, 
loops/conditionals and statement ends.  The edges are used to
indicate dependencies between the nodes.
************************************************************/

#include "node.h"
ostream &operator <<(ostream &output, set<long> &sa);

// Constructor
Node::Node (string n) :
	name_(n), action_(), start_(-1), end_(-1), var_use_(),
	var_mod_(), func_(), func_map_(), is_assign_node_(false),
	is_statement_node_(false), is_loop_or_cond_node_(false),
	time_unit_()
{
}

// Interface method for func_map.  Func map is used to list the
// functions called in a node.
long Node::func_map (long v)
{
	if (func_map_.find(v) != func_map_.end()) {
		return func_map_[v];
	} else {
		return -1;
	}
}

// Adds an action word to a node (actions come from database.cc)
void Node::add_action (string s, long tn)
{
	// 'start_' records the lowest tree node value in the tg node
	if (start_ == -1) {
		start_ = tn;
	} else {
		Rassert(tn > start_);
	}
	action_.push_back(s);

	// For special 's', record node is a special type.
	// This is used for later processing.
	if ((s == "loop_start") || (s == "select_start")) {
		is_loop_or_cond_node_ = true;
	} else if (!is_loop_or_cond_node_) {
		if (s == "statement_end") {
			is_statement_node_ = true;
		}
		if (s == "assignment") {
			is_assign_node_ = true;
		}
	}
}

// Adds a variable use to a node.
void Node::add_var_use (long v)
{
	var_use_.insert(v);
}

// Adds a variable use and modification to a node.
void Node::add_var_mod (long v)
{
	var_use_.insert(v);
	var_mod_.insert(v);
}

// Adds multiple variable uses
void Node::add_var_use (RVector<long> v)
{
	MAP (x, v.size()) {
		var_use_.insert(v[x]);
	}
}

// Adds multiple variable uses and modifications
void Node::add_var_mod (RVector<long> v)
{
	MAP (x, v.size()) {
		var_use_.insert(v[x]);
		var_mod_.insert(v[x]);
	}
}

// Add a function call to the node.  Func map is used to list the
// functions called in a node.
void Node::add_func (long v)
{
	if (func_map_.find(v) == func_map_.end()) {
		func_map_[v] = func_.size();
		func_.push_back(v);
	}
}

// Store how long node took (based on profiling info)...worst-case
void Node::set_time_unit (long s, long us)
{
	TimeUnit t;
	t.set_time(s, us);
	TimeUnit old_t = time_unit_;

	// keep worst-case time
	if (old_t < t) {
		time_unit_ = t;
	}
}

// Constuctor
Edge::Edge (long v) :
	val_(v), vars_()
{
}

// Constuctor
TGraph::TGraph (string n) :
	val_(-1), name_(n), time_unit_(), tree_node_starts_(), func_call_nodes_(), 
	statement_nodes_(), loop_or_cond_nodes_()
{
}

// Generate a new node in TG
// Only adds a node if the last one isn't empty
long TGraph::get_new_node (long id)
{
	long tmp = size_vertex();
	tmp--;

	if (tmp < 0) {
		add_vertex (Node (to_string(id) + "__" + to_string(++tmp)));
	} else {
		RVector<string> svec = (*this)[tmp].action();

		// Code to only adds a node if the last one isn't empty
		if (svec.size() != 0) {
			add_vertex (Node (to_string(id) + "__" + to_string(++tmp)));
		}
	}

	return tmp;
}

// Draws dependence edges based on variable uses and modifications
void TGraph::make_depend_arcs ()
{
	// For each node in TG
	MAP (x, size_vertex()) {
		// Get it's variable use set
		set<long> xset = (*this)[x].var_use();
		set<long> tmpset, intersect_set;

		// Look at previous nodes and find most recent modification
		// of a variable in 'x' use set.
		for (long y = x - 1; y >= 0; y--) {
			// If the nodes aren't already linked, draw an arc and
			// remove the common variables from 'xset'
			if (!nodes_linked(x, y)) {
				tmpset = (*this)[y].var_mod();
				intersect_set.clear();

				// Find the intersection between 'x' and 'y'
				set_intersection (xset.begin(), xset.end(),
						tmpset.begin(), tmpset.end(),
						inserter(intersect_set, intersect_set.begin()));

				// If there is an interesection, create the edge
				if (!(intersect_set.empty())) {
					long e = size_edge();
					add_edge (y, x, Edge(-10000));
					(*this)(e).set_vars(intersect_set);
				}

				// Update 'xset' based on new edge
				set<long> new_xset;
				set_difference(xset.begin(), xset.end(), 
									intersect_set.begin(), intersect_set.end(),
									inserter(new_xset, new_xset.begin()));
				xset = new_xset;
				tmpset.clear();
				intersect_set.clear();
			}
			if (!(xset.size())) {
				break;
			}
		}
	}
}

// Removes some unnecessary nodes which don't have significant
// computation.  Mostly just removes statement_end nodes.
void TGraph::compact_graph()
{
	long statement_end = -1;
	long assign = -1;
	long loop_cond = -1;
	RVector<long> a, b;		// merge pair (a, b)

	// If the last node is empty, remove it.
	long vsize = size_vertex();
	if (((*this)[vsize - 1].action()).empty()) {
		erase_vertex(vsize - 1);
	}

	// Merge statement_end with assign nodes first because
	// they are out of order
	MAP (x, size_vertex()) {
		if ((*this)[x].is_statement_node()) {
			statement_end = x;
		} else if ((*this)[x].is_assign_node()) {
			assign = x;
			statement_end = -1;
		} else if ((*this)[x].is_loop_or_cond_node()) {
			statement_end = -1;
			loop_cond = x;
		}

		if ((statement_end != -1) && (assign != -1)) {
			a.push_back(assign);
			b.push_back(statement_end);
			statement_end = -1;
			assign = -1;
		}
		if (loop_cond != -1) {
			Rassert ((statement_end == -1) && (assign == -1));
			loop_cond = -1;
		}
	}
	// Merge nodes here...
	for (long i = a.size()-1; i >= 0; i--) {
		if (nodes_linked (a[i], b[i])) {
			merge_tasks (a[i], b[i]);
		} else {
			cout << "HEY: " << a << " " << b << endl;
		}
	}

	////////////////////////////////////////////////
	// Clear lists and prepare to remove other nodes
	////////////////////////////////////////////////
	a.clear();
	b.clear();
	MAP (x, size_vertex()) {
		if (((*this)[x].is_statement_node()) && 
			(!((*this)[x].is_assign_node()))) {
			long size = ((*this)[x].action()).size();
			if (size == 1) {
				if (x > 0) {
					a.push_back(x-1);
					b.push_back(x);
				}
			}
		}
	}

	for (long i = a.size()-1; i >= 0; i--) {
		if (nodes_linked(a[i], b[i])) {
// DON'T MERGE, just erase...merging can make cycles.
//			merge_tasks (a[i], b[i]);
			copy_vertex_data(b[i],a[i]);
			erase_vertex(b[i]);
		} else {
			erase_vertex(b[i]);
		}
	}

	// Separates nodes into 3 types:
	//       func_call, statement_end, and loop/conditional
	make_node_lists();
}

// Finds the lowest tree node value for each tg node
void TGraph::find_tree_node_starts ()
{
	long last_tns = -1;
	MAP (x, size_vertex()) {
		long tns = (*this)[x].start();
		Rassert(tns > last_tns);
		tree_node_starts_.push_back(tns);
		last_tns = tns;
	}
}

// Separates nodes into 3 types: (non-excusive)
//       func_call, statement_end, and loop/conditional
void TGraph::make_node_lists()
{
	MAP (x, size_vertex()) {
		if ((*this)[x].is_loop_or_cond_node()) {
			loop_or_cond_nodes_.push_back(x);
		}
		if ((*this)[x].is_statement_node()) {
			statement_nodes_.push_back(x);
		}
		if (((*this)[x].func()).size()) {
			func_call_nodes_.push_back(x);
		}
	}
}

//--------------------------------------------------
// Find XX node:
// These 3 functions find the task graph node of type XX which
// would contain the tree node 'tn' passed to the function.
//--------------------------------------------------
long TGraph::find_fc_node(long tn)
{
	Rassert (func_call_nodes_.size());
	long ret_val = -1;
	MAP (x, func_call_nodes_.size()) {
		if (tn <= (*this)[func_call_nodes_[x]].start()) {
			ret_val = func_call_nodes_[x - 1];
			break;
		}
	}
	long last_node = func_call_nodes_.size() - 1;
	if (tn > (*this)[func_call_nodes_[last_node]].start()) {
		ret_val = func_call_nodes_[last_node];
	}
	Rassert(ret_val > -1);
	return ret_val;
}

long TGraph::find_se_node(long tn)
{
	long ret_val = -1;
	MAP (x, statement_nodes_.size()) {
		if (tn <= (*this)[statement_nodes_[x]].start()) {
			if (x != 0) {
				ret_val = statement_nodes_[x - 1];
				break;
			} else {
				ret_val = statement_nodes_[x];
				break;
			}
		}
	}
	long last_node = statement_nodes_.size() - 1;
	if (tn > (*this)[statement_nodes_[last_node]].start()) {
		ret_val = statement_nodes_[last_node];
	}
	Rassert(ret_val > -1);
	return ret_val;
}

long TGraph::find_lc_node(long tn)
{
	long ret_val = -1;
	MAP (x, loop_or_cond_nodes_.size()) {
		if (tn < (*this)[loop_or_cond_nodes_[x]].start()) {
			ret_val = loop_or_cond_nodes_[x - 1];
			break;
		}
	}
	long last_node = loop_or_cond_nodes_.size() - 1;
	if (tn >= (*this)[loop_or_cond_nodes_[last_node]].start()) {
		ret_val = loop_or_cond_nodes_[last_node];
	}
	Rassert(ret_val > -1);
	return ret_val;
}

// Determines all vars used by the task graph for this function
set<long> TGraph::determine_vars_touched()
{
   set<long> s;
	MAP (x, size_vertex()) {
		// 'vs' is the list of vars touched by a given tg node
      set<long> vs = (*this)[x].var_use();
		// At the end, the union is all the vars touched by the whole tg
      set_union(s.begin(), s.end(), vs.begin(), vs.end(),
            inserter(s, s.begin()));
	}
	return s;
}

// Records the time for the whole tg.  (Based on profiling)
void TGraph::set_time_unit (TimeUnit t)
{
	TimeUnit old_t = time_unit_;

	// keep worst-case time
	if (old_t < t) {
		time_unit_ = t;
	}
}

// Prints the graph structure information for the vcg file for
// the tg for this function.
void TGraph::print_vcg_inside (ofstream &out_file)
{
	MAP (x, size_vertex()) {
		RVector<string> act = (*this)[x].action();

		out_file << "  node: { title: \"" 
			<< (*this)[x].name() << "\" label: \"";

		out_file << "NODE " << x << ": ";
		if (ArgPack::ap().create_task_graph()) {
			out_file << "(" << (*this)[x].time_unit() << ")";
		} else if (ArgPack::ap().display_actions()) {
			out_file << "\\n";
			MAP (y, act.size()) {
				out_file << act[y] << "\\n";
			}
		}
		out_file << "\" } \n";
	}

	MAP (x, size_edge()) {
		out_file << "  edge: { thickness: 2 sourcename:\""
			<< (*this)[edge(x)->from()].name() << "\" targetname: \""
			<< (*this)[edge(x)->to()].name() << "\" label: \"("
			<< (*this)(x).val() << ")\" }\n";
	}
}

// Checks if merging 'a' and 'b' would create a cycle
bool TGraph::check_if_merge_forms_cycle (vertex_index a, vertex_index b)
{
	RVector<int> visited;
	RVector<vertex_index> seen;
	vertex_index v = a;

	MAP (x, size_vertex())
		visited.push_back(0);

	visited[v] = 1;
	MAP (x, vertex(v)->size_out()) {
		seen.push_back(edge(vertex(v)->out(x))->to());
	}
	while (seen.size()) {
		v = seen.back();
		seen.pop_back();
		visited[v] = 1;
		MAP (x, vertex(v)->size_out()) {
		if (edge(vertex(v)->out(x))->to() == b)
			return true;
		if (!visited[edge(vertex(v)->out(x))->to()])
			seen.push_back(edge(vertex(v)->out(x))->to());
		}
	}
	return false;
}

// Merges tasks 'a' and 'b'.  Redraws edges appropriately.
// Nodes must be adjacent and 'a' must point to 'b'
int TGraph::merge_tasks(vertex_index a, vertex_index b)
{
	// Should never try to merge nodes which would create a cycle
	Rassert (check_if_merge_forms_cycle(a,b) == false);

	// Check that nodes are adjacent and 'a' points to 'b'
	bool correct_order=false;
	MAP (x, vertex(a)->size_out()) {
		if (edge(vertex(a)->out(x))->to() == b)
			correct_order=true;
	}
	if (!correct_order) {
		cout <<"Can't merge " << a <<" "<< b 
				<< " Must be (a->b) and adjacent\n";
		exit (-1);
	}

	// Copy outgoing edges from 'b' and give them to 'a'
	bool copy_this_arc;
	MAP (x, vertex(b)->size_out()) {
		edge_index tempe = vertex(b)->out(x);
		copy_this_arc=true;
		MAP (y, vertex(a)->size_out()) {
			if (edge(vertex(a)->out(y))->to() == edge(tempe)->to()) {
				// Error condition if we already had an identical arc
				Rassert(copy_this_arc == true);
				copy_this_arc=false;
			}
		}
		if (copy_this_arc)
			add_edge(a, edge(tempe)->to(), Edge(-10000));
	}

	// Copy incoming edges to 'b' and give them to 'a'
	MAP (x, vertex(b)->size_in()) {
		edge_index tempe = vertex(b)->in(x);
		copy_this_arc=true;
		MAP (y, vertex(a)->size_in()) {
			if (edge(vertex(a)->in(y))->from() == edge(tempe)->from()) {
				// Error condition if we already had an identical arc
				Rassert(copy_this_arc == true);
				copy_this_arc=false;
			}
		}
		if (copy_this_arc && (edge(tempe)->from() != a))
			add_edge(edge(tempe)->from(), a, Edge(-10000));
	}

	// Copying over data before removing vertex 'b'
	copy_vertex_data(b,a);
	erase_vertex(b);

	return 0;
}

// Copy information from one node to the the other.
// (Usually used before erasing the 'from' node in the calling funciton.)
void TGraph::copy_vertex_data (vertex_index from, vertex_index to)
{
	RVector<string> act = (*this)[from].action();
	long start_val = (*this)[from].start();
	set<long> vu = (*this)[from].var_use();
	set<long> vm = (*this)[from].var_mod();

	MAP (x, act.size()) {	
		(*this)[to].add_action(act[x], start_val);
	}
	set<long>::iterator i;
	for (i = vu.begin(); i != vu.end(); i++) {
		long t = *i;
		(*this)[to].add_var_use(t);
	}
	for (i = vm.begin(); i != vm.end(); i++) {
		long t = *i;
		(*this)[to].add_var_mod(t);
	}
}







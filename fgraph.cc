// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  fgraph.cc

  Small file to support the call graph.
************************************************************/

#include "fgraph.h"
#include <algorithm>

// Constructor
FNode::FNode (string n, long i) :
	name_(n), index_(i)
{
}

// Constructor
FEdge::FEdge (long v) :
	val_(v)
{
}

//-----------------------------------------------------------

// Constructor
FGraph::FGraph () :
	val_(-1), subtree_()
{
}

// Determine if node 'n' is in subtree of 't'
bool FGraph::in_subtree(long t, long n)
{
	if (subtree_[t].find(n) != subtree_[t].end()) {
		return true;
	} else {
		return false;
	}
}

// Determine which nodes are in the subtrees of other nodes
void FGraph::setup_subtrees (long main_node)
{
	RVector<long> top = top_sort(main_node);
	
	MAP (x, size_vertex()) {
		subtree_.push_back(set<long>());
	}

	for (long i = top.size() - 1; i >= 0; i--) {
		long parent = top[i];
		for (long j = i + 1; j < top.size(); j++) {
			long child = top[j];
			if (nodes_linked (parent, child)) {
				subtree_[parent].insert(child);
				set_union(subtree_[child].begin(), subtree_[child].end(),
							subtree_[parent].begin(), subtree_[parent].end(),
							inserter(subtree_[parent], subtree_[parent].begin()));
			}
		}
	}
}

// Print graph in a vcg compatible format
void FGraph::print_vcg_inside (ofstream &out_file)
{
	MAP (x, size_vertex()) {
		out_file << "  node: { title: \"" 
			<< (*this)[x].name() << "\" label: \""
			<< (*this)[x].name() << "\" } \n";
	}

	MAP (x, size_edge()) {
		out_file << "  edge: { thickness: 2 sourcename:\""
			<< (*this)[edge(x)->from()].name() << "\" targetname: \""
			<< (*this)[edge(x)->to()].name() << "\" label: \"("
			<< (*this)(x).val() << ")\" }\n";
	}
}





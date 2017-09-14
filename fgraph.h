// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  fgraph.h

  Header file for FGraph which stores the call graph.
************************************************************/

#ifndef FGRAPH_H_
#define FGRAPH_H_

#include "RVector.h"
#include "RString.h"
#include "Graph.h"
#include <set>
#include <iostream>
#include <fstream>

class FNode {
	string name_;
	long index_;
public:
	FNode (string n, long i);

	string name() { return name_; }
	const string name() const { return name_; }
	long index() { return index_; }
	const long index() const { return index_; }
	
};

class FEdge {
	long val_;
public:
	FEdge (long v);

	long val() { return val_; }
	const long val() const { return val_; }
};


class FGraph : public Graph<FNode, FEdge> {
	long val_;
	RVector<set<long> > subtree_;
public:
	FGraph();

	bool in_subtree(long t, long n);
	void setup_subtrees (long main_node);
	void print_vcg_inside (ofstream &out_file);
};

#endif


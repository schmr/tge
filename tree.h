// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  tree.h

  Header file for TreeNode class.  TreeNodes store data for the
abstract syntax tree (AST).  The tree structure itself is maintained
by inheriting methods from the Graph foundation class.
************************************************************/

#ifndef TREE_H_
#define TREE_H_

#include "RVector.h"
#include "RString.h"
#include <iostream>
#include <fstream>
#include "ArgPack.h"

// TreeNodes contain data for AST.  Each item in the AST has it's
// own TreeNode.  Each terminal node in the AST is a token from the
// original C code.  Non-terminal nodes are derived from parsing
// the tokens.
class TreeNode {
	// 'name_' is the most important member of the class
	// 'whitespace_' is just for formatting output
	// 'subtree_size_' equals the number of nodes in all subtrees of
	//      this node
	std::string name_;
	std::string whitespace_;
	long subtree_size_;
	long parent_;
	rstd::RVector<long> children_;
public:
	// Constructors
	TreeNode();
	TreeNode(std::string str, std::string space);

	// methods for updating object
	void add_child (long c) { children_.push_back(c); }
	void add_parent (long p) { 
		Rassert(parent_ == -1); parent_ = p; 
	}

	// query methods
	long subtree_size () { return subtree_size_; }
	const long subtree_size () const { return subtree_size_; }
	std::string name() { return name_; }
	const std::string name() const { return name_; }
	std::string whitespace() { return whitespace_; }
	const std::string whitespace() const { return whitespace_; }
	long parent() { return parent_; }
	const long parent() const { return parent_; }

	rstd::RVector<long> children() { return children_; }
	long child(long x) { return children_[x]; }
	void set_subtree_size (long size) { subtree_size_ = size; }
};
#endif


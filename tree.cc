// Copyright 2002 by Keith Vallerio.
// All rights reserved.

#include "tree.h"
#include "database.h"

// utility function: stores values in 'a' or 'b' in 'out'
void vector_merge (RVector<string> a, RVector<string> b,RVector<string> &out)
{
	out.clear();
	MAP (x, a.size()) {
		out.push_back(a[x]);
	}
	MAP (x, b.size()) {
		out.push_back(b[x]);
	}
}

// Constructors
TreeNode::TreeNode () :
	name_(), whitespace_(), subtree_size_(0), parent_(-1), children_()
{
}

TreeNode::TreeNode (string str, string space) :
	name_(str), whitespace_(space), subtree_size_(0), parent_(-1), children_()
{
}

// Returns next tree node which is actually in the source code
// (This is equivalent to saying 'get next leaf node')
long DB::get_next_literal (long tn)
{
	for (long i = tn + 1; i < tree_node_.size(); i++) {
		if (tree_node_[i].subtree_size() == 1)
			return i;
	}
	return -1;
}

// Returns first ancestor of node 'n' with name 's'
long DB::get_ancestor (long n, string s)
{
	long i = n;
	while (tree_node_[i].parent() != -1) {
		if (tree_node_[i].name() == s)
			return i;
		i = tree_node_[i].parent();
	}
	return -1;
}

// Returns first child of 'n' which is a variable
string DB::get_first_var_child (long n, long curr_function)
{
	long i = n - tree_node_[n].subtree_size() + 1;
	string ret_val;

	while (i < n) {
		string str = create_var_name (tree_node_[i].name(), curr_function);
		if ((var_map(str)) != -1) {
			ret_val = str;
			i = n;
		}
		i++;
	}
	return ret_val;
}

// generates a variable name with appropriate extension
// __0 for globals, __# for local variables where # is the function number
string DB::create_var_name (string s, long f)
{
	string tmp_str = s + "__" + to_string(0);

	// first check if 's' is a global variable
	if (var_map_.find(tmp_str) != var_map_.end()) {
		return tmp_str;
	} else {
		return (s + "__" + to_string(f));
	}
}

// Return child index if node 'n' has a child with name 's'
long DB::has_child (long n, string s)
{
	long i = n - tree_node_[n].subtree_size() + 1;

	while (i < n) {
		if (tree_node_[i].name() == s) {
			return i;
		}
		i++;
	}
	return -1;
}

// Get name of child 'c' of node 'n' (starts at 0)
string DB::node_child_name (long n, long c)
{
	Rassert (c < (tree_node_[n].children()).size());
	long t = tree_node_[n].child(c);
	return (tree_node_[t].name());
}

// get the name the parent of node 'n' 
string DB::node_parent_name (long n)
{
	if (tree_node_[n].parent() != -1) 
		return tree_node_[tree_node_[n].parent()].name();
	else
		return (string(""));
}

// adds a node to the abstract syntax tree (AST)
void DB::add_tree_node (string str, int num_subtrees, string space)
{
	RVector<long> node_list;
	tree_node_.push_back(TreeNode(str, space));

	// These two lines find space for the new node
	long last_node = tree_node_.size() - 1;
	long size = 1;

//cout << "\ntoken:(" << last_node << ")  " << str << endl;

	// These lines determine the immediate children of the new node
	// and the size of the whole subtree.  To accomplish this, get
	// the first child and add children and subtrees as long as there
	// are more subtrees to be added.  This also sets the parent
	// value for the children.
	long c = last_node - 1;
	MAP (x, num_subtrees) {
		node_list.push_back(c);
		tree_node_[c].add_parent(last_node);

		size += tree_node_[c].subtree_size();
		c -= tree_node_[c].subtree_size();
	}

	// add the children to the new node
	for (int i = node_list.size() - 1; i >= 0; i--) {
		tree_node_[last_node].add_child(node_list[i]);
	}

	tree_node_[last_node].set_subtree_size(size);
}

// Analyze the tree.  (Does a significant amount of processing.)
void DB::interpret_tree()
{
	// These 2 vars record when we start/end analyzing a function
	long curr_function = 0;
	long curr_function_end = 0;

	// These 3 vectors record when we start/end analyzing a function call
	RVector<long> func_call;
	RVector<long> func_call_end;
	RVector<string> func_call_name;

	//These strings store info on variable names and type info
	string type, str;
	RVector<string> stype_full, stype_one, stype_all;

	// Add a special function for global definitions
	func_def_name_list_.push_back("global_defs");

	//**********************************************************
	// FOR EACH NODE IN THE TREE...
	//-----------------------------
	// Pass through tree once.  Take actions based on current node (x)
	// and current state (variables listed above)
	//**********************************************************
	MAP (x, tree_node_.size()) {
//cout << tree_node_[x].name() << endl;

		/////////////////////////////
		// Cleanup section goes first
		//---------------------------
		// Determine if end of func def or func call is reached
		// and adjust variables accordingly.
		///////////////////////////// 
		// End of func def -> set curr_function to zero
		if (x > curr_function_end) {
			curr_function = 0;
		}
		// If currently analyzing a function
		if (func_call.size()) {
			// If we reached the end of the most recent func call
			if (x > func_call_end[func_call_end.size() - 1]) {
				// Add function call to curr_functions func_call list.
				// Make sure it has the correct number of commas.
				// And pop func_call stacks.
				function_[curr_function].add_action(
						func_call_name[func_call.size()-1], FN_END, x-1);
				if (tree_node_[func_call_end[func_call.size()-1]].subtree_size() 
								!= 5) {
					function_[curr_function].balance_func_call_commas(
										func_call[func_call.size()-1]);
				}
				func_call.pop_back();
				func_call_end.pop_back();
				func_call_name.pop_back();
			}
		}

		/////////////////////////////
		// Analysis
		//---------
		// Analyze current node in this section
		///////////////////////////// 

		// creates a variable name...checks to see if it's a global first
		str = create_var_name (tree_node_[x].name(), curr_function);

		// inside a struct declaration throw out variables and data types
		if (get_ancestor(x, "struct_declaration_list") != -1) {

		// undefined structs
		} else if (tree_node_[x].name() == "struct") {
			long next = get_next_literal(x);
			if (tree_node_[get_next_literal(next)].name() == ";") {			
//				cout << tree_node_[next].name() << endl;			
				undefined_structs_.insert("struct " + tree_node_[next].name());
			}

		// Handle identifiers (funcs, params, vars, user-defined data types
		} else if (node_parent_name(x) == "identifier") {
			long a1,a2,a3;
			if ((a1 = get_ancestor (x, "function")) != -1) {
				// function def/decl parameters
				if ((a2 = get_ancestor (x, "parameter_list")) != -1) {
					Rassert(curr_function);
					vector_merge(stype_one, stype_all, stype_full);
					add_parameter(curr_function, str, type, stype_full);

				// function definition
				} else if ((a3 = get_ancestor (x, "function_definition")) != -1) {
					Rassert(!curr_function);
					vector_merge(stype_one, stype_all, stype_full);
					curr_function = 
								add_function(tree_node_[x].name(),type,stype_full);
					curr_function_end = a3;
					set_function_defined (curr_function);
					func_def_name_list_.push_back(tree_node_[x].name());

				// function declaration
				} else {
					Rassert(!curr_function);
					vector_merge(stype_one, stype_all, stype_full);
					curr_function = 
								add_function(tree_node_[x].name(),type, stype_full);
					curr_function_end = get_ancestor (x, "declaration");
				}

			// don't count user-defined data types as variables!!!
			} else if ((type_map_lookup(tree_node_[x].name())) != -1) {
//				cout << tree_node_[x].name() << " DATA TYPE...not a variable\n";

			// variable declaration
			} else if ((a1 = get_ancestor (x, "declaration")) != -1) { 
				long arr_size = 1;

				// if it's an array, try to determine size
				if ((a2 = get_ancestor (x, "array_decl")) != -1) {
					long tmp_x = x;
					bool done = false;
//					cout << "Array:  " << str << "  ";
					while (!done) {
						long tn = get_next_literal(tmp_x);
						Rassert(tn != -1);
						if (tree_node_[tn].name() == "[") {
							tn = get_next_literal(tn);
							long tmp_val = Conv(tree_node_[tn].name());
							arr_size *= tmp_val;
//							cout << tree_node_[tn].name() << " ";
							tn = get_next_literal(tn);

// If calculation embedded in [], then just give up on guessing array size
							if (tree_node_[tn].name() != "]") {
								done = true;
								arr_size = ArgPack::ap().pointer_size();
							}
							tmp_x = tn;
						} else {
							done = true;
						}
					}
//					cout << " is " << arr_size << endl;
				}
				if ((type_map_lookup(str)) == -1) {
					vector_merge(stype_one, stype_all, stype_full);
					add_variable (curr_function, str, type, stype_full, arr_size);
					function_[curr_function].add_action(str, VAR_DECL, x);
				}
			}

		// variable usage (including inside of a function call)
		} else if ((var_map(str)) != -1) {
			if (func_call.size()) {
				long tmp = func_call[func_call.size() - 1];
				Rassert(var_map(str) != -1);
				add_func_call_arg(curr_function, tmp, var_map(str), VAR);
			}
			function_[curr_function].add_var_use (str);
			function_[curr_function].add_action (str, VAR_USE, x);

		// function call
		} else if ((func_map(tree_node_[x].name())) != -1) {
			// fn_call_root is the root node of the func call subtree
			long fn_call_root = get_ancestor (x, "function_call");
			if (fn_call_root != -1) {
				if (func_call.size()) {
					long fc_num = func_call[func_call.size() - 1];
					long fnum = func_map(tree_node_[x].name());
					Rassert(fnum != -1);
					add_func_call_arg(curr_function, fc_num, fnum, FUNC);
				}
				string fn_call_name = tree_node_[x].name();
				function_[curr_function].add_action(fn_call_name, FN_START, x);
				func_call.push_back(
							add_func_call(curr_function,fn_call_name,fn_call_root,x));
				func_call_end.push_back(fn_call_root);
				func_call_name.push_back(fn_call_name);
			}

		// storage class specifiers (adjectives for data types)
		} else if ((tree_node_[x].name() == "extern") || 
						(tree_node_[x].name() == "static") ||
						(tree_node_[x].name() == "const") || 
						(tree_node_[x].name()=="volatile")) {
			stype_all.push_back(tree_node_[x].name());

		// have to handle pointers differently since they are per variable
		} else if (tree_node_[x].name() == "pointer") {
			stype_one.push_back(tree_node_[x].name());

		// data type
		} else if (node_parent_name(x) == "type") {
			if ((tree_node_[x].name() != "signed") && 
						(tree_node_[x].name() != "unsigned")) {
				if (tree_node_[x].name() != "struct_or_union") {
					type = tree_node_[x].name();
				} else {
					long tmp = tree_node_[x].child(0);
					if (node_child_name(tmp, 1) != "{") {
						type = "struct " + node_child_name(tmp, 1);
					} else {
						type = "struct unknown";
					}
				}
			}

		// ends a list of declarations (clear out stuff like extern, const,etc)
		} else if ((tree_node_[x].name() == "declaration") || 
						(tree_node_[x].name() == "(")) {
			stype_one.clear();
			stype_all.clear();

		// ends declaration of a single variable so clear * in front of variable
		} else if (tree_node_[x].name() == "declarator") {
			stype_one.clear();

		// end of an assignment
		} else if (tree_node_[x].name() == "assignment_expr") {
			if (curr_function) {
				function_[curr_function].add_action("assign_end", ASSIGN_END, x);
			}
			pair <long, long> p (x - tree_node_[x].subtree_size(), x);
			function_[curr_function].add_assign_node (p);

		// end of a statement
		} else if (tree_node_[x].name() == "statement") {
			if (curr_function) {
				function_[curr_function].add_action("statement_end",STATE_END,x);
				function_[curr_function].add_action("#"+to_string(x),STATE_NUM,x);
			}

		// ++/-- bindings
		} else if (tree_node_[x].name() == "short_assign_op") {
			Rassert(curr_function);
			string op_target = get_first_var_child(x, curr_function);
			function_[curr_function].add_action(op_target, SHORT_ASSIGN_VAR, x);

		// separates sources and targets of assignments
		} else if ((tree_node_[x].name() == "assignment_operator") ||
					(tree_node_[x].name() == ">>=") ||
					(tree_node_[x].name() == "<<=") ||
					(tree_node_[x].name() == "+=") ||
					(tree_node_[x].name() == "-=") ||
					(tree_node_[x].name() == "*=") ||
					(tree_node_[x].name() == "/=") ||
					(tree_node_[x].name() == "%=") ||
					(tree_node_[x].name() == "&=") ||
					(tree_node_[x].name() == "^=") ||
					(tree_node_[x].name() == "|=")) {
			if (curr_function) {
				function_[curr_function].add_action("assignment", ASSIGN, x);
			}

		// get commas which separate args in func_call
		} else if (tree_node_[x].name() == ",") {
			if (func_call.size()) {
				function_[curr_function].add_func_call_comma(
							func_call[func_call.size()-1]);
			}

		// get elipses: tell func it can have variable args
		} else if (tree_node_[x].name() == "...") {
			Rassert(curr_function);
			function_[curr_function].add_elipses();

		// to evaluate bracket contents in correct order
		} else if (tree_node_[x].name() == "[") {
			if (curr_function) {
				function_[curr_function].add_action ("[", BRAC_OPEN, x);
			}

		// to evaluate bracket contents in correct order
		} else if (tree_node_[x].name() == "]") {
			if (curr_function) {
				function_[curr_function].add_action ("]", BRAC_CLOSE, x);
			}

		// beginning of conditional statements
		} else if ((tree_node_[x].name() == "if") || 
					(tree_node_[x].name() == "switch")) {
			Rassert(curr_function);
			function_[curr_function].add_action("select_start", SELECT_START, x);

		// end of conditional statement
		} else if (tree_node_[x].name() == "selection_statement") {
			Rassert(curr_function);
			function_[curr_function].add_action("select_end", SELECT_END, x);

		// beginning of loop
		} else if ((tree_node_[x].name() == "while") || 
					(tree_node_[x].name() =="do") ||
					(tree_node_[x].name() == "for")) {
			Rassert(curr_function);
			function_[curr_function].add_action("loop_start", LOOP_START, x);

		// end of loop
		} else if (tree_node_[x].name() == "iteration_statement") {
			Rassert(curr_function);
			function_[curr_function].add_action("loop_end", LOOP_END, x);
		}
	}
}

//------------------------------------------------------------
// Annotated C code for profiling
// ------------------------------
// Generates timing information and sizeof calls to get data type
// sizes.  Requires linking profiled executable with KSVtime.c
//------------------------------------------------------------
void DB::print_annotated_c ()
{
	// for printing new_cfile
	long fdnl_num = 0;
	bool in_global_def = true;
	string newc_str;
	long stack = 0;
	long prev_str_x = -1;
	bool sflag = false;

	// Open output file for annotated c code output
	ofstream new_cfile;
	string str = ArgPack::ap().get_fname() + "_new.c";
	new_cfile.open(str.c_str());
	Rassert(new_cfile);

	new_cfile << "extern void KSVprint_time (char *str, long node);\n";
	new_cfile << "extern void KSVprint_size (void * s);\n";
	new_cfile << "extern void KSVinit_time ();\n";
	new_cfile << "extern void KSVend_time ();\n";

	MAP (x, tree_node_.size()) {
		if ((tree_node_[x].name()=="if") || (tree_node_[x].name()=="switch")) {
			newc_str = "select_start";
			sflag = true;
		} else if ((tree_node_[x].name() == "while") || 
					(tree_node_[x].name() =="do") ||
					(tree_node_[x].name() == "for")) {
			newc_str = "loop_start";
			sflag = true;

		} else if ((tree_node_[x].name() == "{") || 
					(tree_node_[x].name() == "declaration_list")) {
			Rassert(x>1);
			long p = tree_node_[x].parent();
			if (((tree_node_[x-1].name() == "declarator") &&
						(tree_node_[x-2].name() == "function")) || 
						(tree_node_[p].name() == "function_body")) {
				fdnl_num++;
				in_global_def = false;
				newc_str = "fn_start";
				new_cfile << "\n{\n";
				if (func_def_name_list_[fdnl_num] == "main") {
					new_cfile << "FILE * KSVfout;\n";
					new_cfile << "KSVfout = fopen(\"KSV.annote\",\"w\");\n";

					if (ArgPack::ap().calculate_edge_lengths()) {
						MAP (y, variable_.size()) {
							string tmp_s = variable_[y].type();
							if (!tmp_s.empty()) {
								if (undefined_structs_.find(tmp_s) ==
										undefined_structs_.end()) {
									new_cfile << "fprintf(KSVfout, \"" << tmp_s.c_str()
												<< " %d\\n\", sizeof(" << tmp_s.c_str();
									new_cfile << "));\n" << endl;
								}
							}
						}
					}
					new_cfile << "fprintf (KSVfout,\"KSV type_size complete"
								<< "\\n\");\n" << endl;
					new_cfile << "fclose(KSVfout);\n";
					new_cfile << "\nKSVinit_time();\n";
					new_cfile << "atexit(KSVend_time);\n";
				}
			}
		} else if (tree_node_[x].name() == "function_body") {
			newc_str = "fn_done";
		} else if (tree_node_[x].name() == "statement") {
			if (prev_str_x != (x - 1)) {
				newc_str = "statement_end";
			}
		} else if (tree_node_[x].name() == "iteration_statement") {
			newc_str = "loop_end";
			stack--;
		} else if (tree_node_[x].name() == "selection_statement") {
			newc_str = "select_end";
			stack--;
		} 

		string full_str;
		if (!in_global_def) {
			full_str = func_def_name_list_[fdnl_num] + " " + newc_str;
		}

		if ((!stack) && newc_str.size()) {
			new_cfile << "\nKSVprint_time(\"" << full_str << "\", "<<x<< ");\n";
		}
		if (sflag) {
			stack++;
			sflag = false;
		}
		if (tree_node_[x].subtree_size() == 1) {
			new_cfile << tree_node_[x].whitespace() << tree_node_[x].name();
		}
		if (newc_str == "fn_done") {
			new_cfile << "\n}\n";
			in_global_def = true;
		}
		if (newc_str.size()) {
			prev_str_x = x;
			newc_str = "";
		}
	}
	new_cfile.close();
}

//------------------------------------------------------------
// Annotated c code for creating task graph
//-----------------------------------------
// Writes annotation comments to divide C program into tasks.
// Each comment is of the form 'KSV func_name number'.  
// 'KSV func_name' is used to start a function, and 'KSV end'
// ends a function's annotation.
//------------------------------------------------------------
void DB::print_task_graph_c ()
{
	// Open output file for annotated c code output
	ofstream new_cfile;
	string str = ArgPack::ap().get_fname() + "_tg.c";
	new_cfile.open(str.c_str());
	Rassert(new_cfile);

	long fdnl_num = 0;
	bool in_global_def = true;
	string newc_str;
	RVector<long> starts;
	long starts_index = 0;

	MAP (x, tree_node_.size()) {
		if (tree_node_[x].name() == "{") {
			Rassert(x>1);
			if (tree_node_[x-1].name() == "declarator") {
				if (tree_node_[x-2].name() == "function") {
					fdnl_num++;
					in_global_def = false;
					new_cfile << "\n/*KSV " << func_def_name_list_[fdnl_num] 
								<< "*/";

					long fn = func_map(func_def_name_list_[fdnl_num]);
					starts = fg_[fn].tree_node_starts();
					starts_index = 0;
				}
			}
		} else if (tree_node_[x].name() == "function_body") {
			new_cfile <<  "\n/*KSV end*/\n";
			in_global_def = true;
			starts.clear();
			starts_index = 0;
		}

		if (starts_index < starts.size()) {
			if (x >= starts[starts_index]) {
				new_cfile << "\n/*KSV " << func_def_name_list_[fdnl_num]
							<< " " << starts_index++ << "*/";
			}
		} 

		string full_str;

		// Prints out original code w/ original whitespace (minus comments)
		if (tree_node_[x].subtree_size() == 1) {
			new_cfile << tree_node_[x].whitespace() << tree_node_[x].name();
		}
	}
	new_cfile.close();
}

// Prints original code to stdout
void DB::print_terminals () const
{
	MAP (x, tree_node_.size()) {
		if (tree_node_[x].subtree_size() == 1)
			cout << tree_node_[x].name() << endl;
	}
}

// prints the abstract syntax tree (AST) in vcg format so it can be seen
void DB::print_tree_vcg () const
{
	ofstream out;
	string file_name = ArgPack::ap().get_fname() + ".vcg";
	out.open(file_name.c_str());

	if (!out) {
		cout << "Cannot open output file";
		Rabort();
	}

	out << "graph: { label: \"" << file_name << "\"\n";

	MAP (x, tree_node_.size()) {
		out << "  node: { title: \""
			<< x << "\" label: \"";

		string s = tree_node_[x].name();
		if (s[0] == '"') {
			out << "\\\"";
		} else if (s[0] == '\\') {
			out << "\\\\";
		} else {
			out << s << "(" << x << ")";
		}
		out <<  "\" } \n";
	}
	MAP (x, tree_node_.size()) {
		if (tree_node_[x].parent() != -1) {
			out << "  edge: { thickness: 2 sourcename:\""
				<< tree_node_[x].parent() << "\" targetname: \""
				<< x << "\" }\n";
		}
	}

	out << "}\n";
}

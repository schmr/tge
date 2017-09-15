// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  database.cc

  This is the main analysis file.  It handles the database class
which contains most of the other classes and directs their
interface.  It works in tandem with 'dbio.cc'.

NOTE: LHS -> left hand side
      RHS -> right hand side
************************************************************/

#include "database.h"
#include "ArgPack.h"
#include "RVector.h"
#include <algorithm>

#define PARAMETER_DATA_SIZE 0
#define DATA_TYPE_SIZE_UNKNOWN -1

/*###########################################################################*/
DB * DB::def_db_ = 0;
/*===========================================================================*/

DB::DB () :
	function_(), variable_(),
	tree_node_(), tree_start_(),  type_map_(), func_map_(), 
	var_map_(), fn_redeclaration_(false), tg_("task_graph"), fg_(), 
	call_graph_(),	annote_data_(), func_def_name_list_(), undefined_structs_()
{
	Rassert(!def_db_);
	def_db_ = this;
	rstd::RVector<std::string> s;

	add_function("global_defs", "void", s);
	add_variable(0, "global_alias_var", "void", s, 1);

	type_map_insert("void", 0);
	type_map_insert("char", 0);
	type_map_insert("short", 0);
	type_map_insert("int", 0);
	type_map_insert("long", 0);
	type_map_insert("register", 0);
	type_map_insert("float", 0);
	type_map_insert("double", 0);
	undefined_structs_.insert("struct unknown");
	undefined_structs_.insert("register");
}

/*===========================================================================*/
// Two major sections of the program: interpret and profile
/*===========================================================================*/
// Build dependence graph for each function in input code.  First
// analyze the abstract syntax tree, build the call graph, determine
// dependences.  End by outputting 2 new C files.
void DB::interpret()
{
	interpret_tree ();
	build_call_graph();
	do_dependence ();
	print_annotated_c();
	print_task_graph_c();
}

// Profile by recompiling annotated C generated by 'interpret'.
// Rerun the new executable.  Read it's output and determine
// task graph edge and node weights based on these values.
void DB::profile()
{
	prof_recompile();
	prof_rerun();
	prof_read_data();
	prof_get_arc_lengths();
}

/*===========================================================================*/
// Support functions for dependence graph generation from AST 
/*===========================================================================*/
// Adds a new function to the list of functions in DB.  If that
// function already exists, it just returns that functions id
// number.  The redeclaration flag is used to prevent duplication
// of the parameter list.
long DB::add_function (std::string n, std::string t, rstd::RVector<std::string> s)
{
	if (func_map_.find(n) == func_map_.end()) {
		fn_redeclaration_ = false;
		function_.push_back(Function(n, t, s));
		func_map_[n] = function_.size() - 1;
		return (function_.size() - 1);
	} else {
		fn_redeclaration_ = true;
		return func_map_[n];
	}
}

// Adds a function call to the current function (f).
// 'c' is the callee name, 'r' is the root of the callee subtree in the
// AST, and 'node' is the node number in the AST
long DB::add_func_call (long f, std::string c, long r, long node)
{
	return function_[f].add_func_call(c, r, node);
}

// Adds a variable to the current function 'f'.
// 'n' is the name of the variable, 't' is the type, 's' is a vector
// of modifying words like 'extern' and 'pointer'.  'size' is equal
// to one for scalars, or indicates array size.
void DB::add_variable (long f, std::string n, std::string t, rstd::RVector<std::string> s,
								long size)
{
	// Check if we've seen this variable name before in this scope.
	if (var_map_.find(n) != var_map_.end()) {
		// We don't support nested scopes for variables.
		// Halt unless using VERY loose dependence (allows reading K&R C)
		if (!ArgPack::ap().loosest_dependence()) {
			Rabort();
		} else {
			long var = var_map(n);
			variable_[var] = Var(n, t, s, size);
		}
	// Add the new variable to the variable list and current function
	} else {
		function_[f].add_variable(variable_.size());
		variable_.push_back(Var(n, t, s, size));
		var_map_[n] = variable_.size() - 1;
	}
}

// Add a parameter declaration for the current function 'f'.
// 'n', 't', and 's' have the same meaning as they did in add_variable.
void DB::add_parameter (long f, std::string n, std::string t, rstd::RVector<std::string> s)
{
	if (!fn_redeclaration_) {
		if (var_map_.find(n) == var_map_.end()) {
			function_[f].add_variable(variable_.size());
			function_[f].add_parameter(variable_.size());
			variable_.push_back(Var(n, t, s, PARAMETER_DATA_SIZE));
			var_map_[n] = variable_.size() - 1;
		} else {
			std::cerr << "Parm name (" << n << ") exists already! Probably part of";
			std::cerr << " standard library." <<  std::endl;
		}
	}
	// We don't support nested scopes, but needed to 
	// remove assert to understand code with function pointers
//	Rassert (var_map_.find(n) == var_map_.end());
}

// Add an argument to a function call 'c'.
// 'f' is the caller function, 'a' is the arg, 'atype' is the type.
void DB::add_func_call_arg (long f, long c, long a, ArgType atype)
{
	function_[f].add_func_call_arg(c, a, atype);
}

// Lookup map for std::string -> function index value.
long DB::func_map (std::string s)
{
	if (func_map_.find(s) != func_map_.end()) {
		return func_map_[s];
	} else {
		return -1;
	}
}

// Lookup map for std::string -> variable index value.
long DB::var_map (std::string s)
{
	if (var_map_.find(s) != var_map_.end()) {
		return var_map_[s]; 
	} else {
		return -1;
	}
}

// Lookup map to set data type size
bool DB::type_map_insert(std::string s, long length)
{
	if (type_map_.find(s) != type_map_.end()) {
		return false;
	} else {
		type_map_[s] = length;
		return true;
	}
}

// Lookup map to get data type size
long DB::type_map_lookup(std::string s)
{
	if (type_map_.find(s) != type_map_.end()) {
		return type_map_[s];
	} else {
		return DATA_TYPE_SIZE_UNKNOWN;
	}
}

// Same as var_map(s)
long DB::get_literal_id (std::string s)
{
	long a = var_map(s);
	return a;
}

//**************************************************
// Analysis routines
//------------------
// These functions determine the dependence relationships for the 
// dependence graphs and task graphs for each function.
//**************************************************

// Generates a static-time call graph to indicate which functions
// can be called from which other functions.  Currently, it insists
// that the function calls cannot be cyclic (or recursive).
// NOTE:  Call graph must contain 'main' currently.
void DB::build_call_graph()
{
	// Create one vertex in the call graph for each function
	MAP (x, function_.size()) {
		call_graph_.add_vertex (FNode (function_[x].name(), x));
	}

	// For each function, draw an outgoing edge to each function it calls
	MAP (x, function_.size()) {
		rstd::RVector<std::string> call_list = function_[x].func_call_names();

		MAP (y, call_list.size()) {
			long called_fn = func_map (call_list[y]);
			Rassert(called_fn != -1);
			if (!call_graph_.nodes_linked (x, called_fn)) {
				call_graph_.add_edge (x, called_fn, FEdge(1));
			}
		}
	}

	// Make sure graph isn't cyclic and that it contains a 'main'.
	Rassert (!call_graph_.cyclic());
	Rassert (func_map ("main") != -1);
	call_graph_.setup_subtrees(func_map ("main"));

	// Output call graph in vcg format.
	print_fgraph_vcg();
}

// Determine dependencies for each function.  Topologically sort 
// call graph.  Start at callees and analyze 'main' last.
void DB::do_dependence ()
{
	Rassert (func_map ("main") != -1);
	rstd::RVector<long> fn_list = call_graph_.top_sort (func_map ("main"));
	fn_list.push_back(0);

	// Generate a TG for each function.
	MAP (x, function_.size()) {
		fg_.push_back(TGraph (function_[x].name()));
	}

	// Perform dependence analysis in reverse topological order.
	for (long i = fn_list.size() - 1; i >= 0; i--) {
		fg_[fn_list[i]] = fn_dep (fn_list[i]);
	}
}

/////////////////////////////////////////////////////////////////
// One of the main dependence analysis functions.  Walk through the
// actions list and add nodes to the dependence graph accordingly.
TGraph DB::fn_dep (long id)
{
	TGraph fg(function_[id].name());
	// block depth indicates level of nesting for loops and conditionals
	long block_depth = 0;
	// stores nesting information for functions
	rstd::RVector<long> func_node_stack, brac_stack;
	// n is the index value of the current node being modified
	long n = fg.get_new_node(id);

	// actions and types are keywords (the type is used for the switch)
	rstd::RVector<std::string> action = function_[id].action();
	rstd::RVector<ActionType> action_type = function_[id].action_type();
	rstd::RVector<long> action_tree_node = function_[id].action_tree_node();

	rstd::RVector<std::pair<long,long> > assign = function_[id].assign_node();
	rstd::RVector<long> vm;
	long fc_num = 0, tmp;
	long last_state_end = 0;
	long assign_target = -1;
	long assign_num = 0, assign_ctr = 0;
	std::set<long> local_vars;

	function_[id].init_alias_vectors(variable_.size());

	// CLUDGE fix (sorry about that)
	// KSV: Add and extra action_type to take care of out of bounds case
	action_type.push_back(STATE_NUM);

	/////////////////////////////////////
	// For each action in the function...
	/////////////////////////////////////
	MAP (x, action.size()) {
		// get information about the action:  type, name, AST node location
		ActionType atype = action_type[x];
		std::string s = action[x];
		long tn = action_tree_node[x];
		bool draw_assign_arcs = false;
		long fc_func;		// function callee function index

		/*
		 * Switch handles actions: (modifies current node appropriately)
		 * ------------------------------------------------------------
		 *   Var use: add to node's var use list
		 *   Var mod: add to node's var use and modify lists
		 *   short assign: add to node's var use and modify lists
		 *   assign: set assign target for later
		 *   assign end: draw assignment arcs to assign target
		 *   state end: end of a statement gets its own block (why?)
		 *   loop/cond start: increase block depth, wait for end
		 *   loop/cond end: decrease block depth, assign all possible
		 *       aliases to every variable on the LHS of an assignment
		 *       inside the node
       *   brack open/close: do nothing anymore
		 *   func start: put func call on fc stack, get variables
		 *       modified by func and store in 'vm', if func not
		 *       defined then set a global var '0' as touched
		 *   func end: draw edges to current node from all edges
		 *       between current node and the node with the func start,
		 *			pop fc stack, start new node
		 */
		switch (atype) {
			case VAR_USE:
				fg[n].add_action(s,tn);
				fg[n].add_var_use(function_[id].v_alias_vec(var_map(s)));
				break;
			case VAR_DECL:
				local_vars.insert(var_map(s));
				fg[n].add_action(s,tn);
				fg[n].add_var_mod(function_[id].v_alias_vec(var_map(s)));
				if ((!block_depth) && (action_type[x+1] != VAR_DECL)) {
//KSV
					if (action_type[x+1] != BRAC_OPEN) {
						n = fg.get_new_node(id);
					}
				}
				break;
			case SHORT_ASSIGN_VAR:
				fg[n].add_action(s,tn);
				fg[n].add_var_mod(function_[id].v_alias_vec(var_map(s)));
				break;
			case ASSIGN:
//KSV2				if (!block_depth) n = fg.get_new_node(id);
				fg[n].add_action(s,tn);
				assign_target = n;
//KSV2				if (!block_depth) n = fg.get_new_node(id);
				assign_ctr++;
				break;
			case ASSIGN_END:
//KSV				fg[n].add_action(s,tn);
				draw_assign_arcs = true;
				if (!block_depth) n = fg.get_new_node(id);
				break;
			case STATE_END:
				// gets it's own node
				if (!block_depth) n = fg.get_new_node(id);
				for (long i = last_state_end + 1; i < n; i++) {
					// These edges will be removed and are of size 0
// KSV:  Adds false dependence...but is needed for some reason
					fg.add_edge(i, n, Edge(0));
				}
				last_state_end = n;
				fg[n].add_action(s,tn);
				if (!block_depth) n = fg.get_new_node(id);
				break;
			case LOOP_START:
			case SELECT_START:
				if (!block_depth) n = fg.get_new_node(id);
				block_depth++;
				fg[n].add_action(s,tn);
				break;
			case LOOP_END:
			case SELECT_END:
				block_depth--;
				fg[n].add_action(s,tn);
				if (!block_depth) {
					rstd::RVector<long> lhs_var;
					while (assign_ctr) {
						vm = add_alias_data(id, assign[assign_num]);
						fg[n].add_var_mod(vm);
						lhs_var.push_back(get_assign_lhs_var(id,assign[assign_num]));
						assign_num++;
						assign_ctr--;
					}
					update_lhs_var_aliases(id, lhs_var);
					n = fg.get_new_node(id);
				}
				break;
			case STATE_NUM:
				break;
			case BRAC_OPEN:
break;//KSV
				fg[n].add_action(s,tn);
				brac_stack.push_back(n);
				break;
			case BRAC_CLOSE:
break;//KSV
				fg[n].add_action(s,tn);
				tmp = brac_stack[brac_stack.size() - 1];
				if (tmp != n) {
					for (long i = tmp+1; i <= n; i++) {
						fg.add_edge (i, tmp, Edge(-10000));
					}
				}
				brac_stack.pop_back();
				if (!block_depth) n = fg.get_new_node(id);
				break;
			case FN_START:
//				std::cout << function_[id].func_call()[fc_num];
				if (!block_depth) n = fg.get_new_node(id);
				fg[n].add_action(s,tn);
				func_node_stack.push_back(n);
				vm = do_func_call(id, fc_num);
				fc_func = func_map(((function_[id].func_call())[fc_num]).name());
				fg[n].add_func(fc_func);
				if (!function_[fc_func].defined()) {
					std::set<long> vt;
					vt.insert(0);
					function_[fc_func].set_vars_touched(vt);
					fg[n].add_var_mod(0);
					vm.push_back(0);		
				}
				fg[n].add_var_mod(vm);
				fc_num++;
				break;
			case FN_END:
				fg[n].add_action(s,tn);
				tmp = func_node_stack[func_node_stack.size() - 1];
				if (tmp != n) {
					for (long i = tmp+1; i <= n; i++) {
						fg.add_edge (i, tmp, Edge(-10000));
					}
				}
				func_node_stack.pop_back();
				if (!block_depth) n = fg.get_new_node(id);
				break;
		}

		// Post processing for assignments:
		// If an assign end is reached and we aren't inside a
		// loop/cond, draw edges to represent assign data dependencies.
		if (draw_assign_arcs) {
			if (!block_depth) {
				Rassert(assign_target != -1);
				for (long i = assign_target + 1; i < n; i++) {
					fg.add_edge(i, assign_target, Edge(-10000));
				}
				vm = add_alias_data(id, assign[assign_num++]);
				fg[assign_target].add_var_mod(vm);
				assign_ctr--;
//				assign_ctr = 0;
			}
			if (!assign_ctr) {
				assign_target = -1;
			}
		}
	}
	///////////////////////////////////
	// function dependence cleanup
	///////////////////////////////////
	// finish drawing edges in dependence graph
	fg.make_depend_arcs();
	// remove/merge nodes to simplify graph
	fg.compact_graph();

	// Determine which variables would be used/modified during
	// a call to this function.  Currently, assumes that any variable
	// used would be modified as well.
	fg.find_tree_node_starts();
	std::set<long> vars_touched = fg.determine_vars_touched();
	std::set<long> new_touched;
//	std::cout << function_[id].name() << ":  " << vars_touched << std::endl;
	set_difference(vars_touched.begin(), vars_touched.end(),
						local_vars.begin(), local_vars.end(),
						inserter(new_touched, new_touched.begin()));
	vars_touched = new_touched;

// KSV remove this set difference for stricter analysis
	if (!(ArgPack::ap().strict_dependence())) {
		new_touched.clear();
		std::set<long> parm_set = function_[id].parameter_set();
		set_difference(vars_touched.begin(), vars_touched.end(),
						parm_set.begin(), parm_set.end(),
						inserter(new_touched, new_touched.begin()));
		vars_touched = new_touched;
	}

//	std::cout << function_[id].name() << ":  " << vars_touched << std::endl;
//	if (function_[id].name() == "print_array") {
//		std::cout << "STUFF: " << vars_touched << std::endl;
//	}

	// Finally, record variables which are used by this function
	function_[id].set_vars_touched(vars_touched);
	return fg;
}

// Do analysis for a function call by 'id' to whichever function is
// the target of func call number 'fc_num'.  Note: 'fc_num' is NOT
// the number of the callee, rather it is the X'th function call
// invoked by function 'id'!
rstd::RVector<long> DB::do_func_call (long id, long fc_num)
{
	// Get callee function index and store in 'func'
	FuncCall fc = function_[id].func_call()[fc_num];
	long func = func_map(fc.name());

	// 2 types of parms: static (in function header)
	//                   call_time (in current function invocation)
	rstd::RVector<long> static_parms = function_[func].parameter();
	rstd::RVector<std::pair<long,ArgType> > call_time_parms = fc.parms();

	rstd::RVector<long> ret_val;
	std::set<long> arg_fc_vars;

	// DEBUG STUFF: for when the 2 parms sizes are not equal
//std::cout << "caller: " << function_[id].name() << "  ";
//std::cout << fc.name() << "  " << call_time_parms.size() << "  " 
//		<< static_parms.size() << std::endl;
	if (call_time_parms.size() != static_parms.size()) {
//		if (!(ArgPack::ap().loosest_dependence())) {
			Rassert(!(function_[func].defined()));
//		}
//		Rassert(function_[func].elipses());
	}

	// Analyze function call parameters:
	//    Parameters could be function calls, constants or variables
	MAP(x, call_time_parms.size()) {
		if (call_time_parms[x].second == FUNC) {
			// Handle nested function calls
			// they should return which vars they touch
			// Add those variables to ret_val
			std::set<long> tset = function_[call_time_parms[x].first].vars_touched();
			set_union(arg_fc_vars.begin(), arg_fc_vars.end(),
							tset.begin(), tset.end(),
							inserter(arg_fc_vars, arg_fc_vars.begin()));

		} else if (call_time_parms[x].second == CONST) {
			// do nothing for consts

		} else if (variable_[call_time_parms[x].first].is_pointer()) {
			// update aliases based on parameters which are variables
			if (x < static_parms.size()) {
				rstd::RVector<long> p_alias=function_[func].v_alias_vec(static_parms[x]);
				function_[id].add_aliases (call_time_parms[x].first,p_alias,false);
			}
		}
	}

	///////////////////////////////////////
	// Prepare return value (vars touched)
	///////////////////////////////////////
	// 'parm_set' is indicies of function's static parameters
	std::set<long> parm_set = function_[func].parameter_set();
   // Returns value encompassing: variables aliased to parms,
   //    and the arguments themselves
	std::set<long> tmp = function_[id].func_call_cleanup_aliases (
													call_time_parms, static_parms);
	// variables touched inside the function
	std::set<long> v_touched = function_[func].vars_touched();
	std::set<long> ret_set;

	// combine the sets to get the set of vars touched
	set_union(arg_fc_vars.begin(), arg_fc_vars.end(),
					tmp.begin(), tmp.end(), inserter(tmp, tmp.begin()));
	set_union(v_touched.begin(), v_touched.end(),
					tmp.begin(), tmp.end(), inserter(tmp, tmp.begin()));

// KSV remove this set difference for stricter analysis
	if (!(ArgPack::ap().strict_dependence())) {
		set_difference(tmp.begin(), tmp.end(),	parm_set.begin(), parm_set.end(),
						inserter(ret_set, ret_set.begin()));
	} else {
		ret_set = tmp;
	}

	// convert set to vector
	std::set<long>::iterator i;
	for (i = ret_set.begin(); i != ret_set.end(); i++){
		long t = *i;
		ret_val.push_back(t);
	}

	return ret_val;
}

// Determine alias effects for a given assignment.
//       'fn' is the function, 
//       'a_node' gives the boundaries of the assignment
// Note: This function should handle multiple assignments in a
//       single line appropriately.
rstd::RVector<long> DB::add_alias_data (long fn, std::pair<long,long> a_node)
{
	long start = a_node.first;  // first AST node of assignment
	long end = a_node.second;   // last AST node of assignment
	long target = -1, assign_op = -1;
	bool clear_old_alias = false;  // not used
	rstd::RVector<long> v;
	std::string str;

	//-----------------------------------------------------
	// Walk through assignment from first AST node to end.
	// First variable is the target (the first if condition).
	// Next wait to grab an assignment operator. (condition 2 & 3)
	// Finally, all vars on RHS of assign operator are sources (cond 4)
	//-----------------------------------------------------
	for (long i = start; i < end; i++) {
		str = create_var_name (tree_node_[i].name(), fn);
		if (target == -1) {
			// Only grabs first var on left of '=' sign
			// this allows it to only get the 'a' in 'a[i]' statements
			// NOTE: If variable is not in map 'var_map' returns '-1'
			target = get_literal_id (str);
		} else if (tree_node_[i].name() == "assignment_operator") {
			assign_op = i;
// Perhaps one could clear old aliases, but you would have to do 
// control flow checking first so it isn't performed currently...
//KSV				clear_old_alias = true;
		} else if ((tree_node_[i].name() == ">>=") ||
					(tree_node_[i].name() == "<<=") ||
					(tree_node_[i].name() == "+=") ||
					(tree_node_[i].name() == "-=") ||
					(tree_node_[i].name() == "*=") ||
					(tree_node_[i].name() == "/=") ||
					(tree_node_[i].name() == "%=") ||
					(tree_node_[i].name() == "&=") ||
					(tree_node_[i].name() == "^=") ||
					(tree_node_[i].name() == "|=")) {
			assign_op = i;
		} else if (assign_op > -1) {
			long a = get_literal_id(str);
			if (a != -1) {
				v.push_back(a);
			}
		}
	}

	// If it was an assignment, it must have had a target.
	Rassert(target != -1);

	// If the target was a pointer add to it's alias set
	if (variable_[target].is_pointer()) {
		function_[fn].add_aliases (target, v, clear_old_alias);
	}

	// Return the alias vector of the target variable
	return (function_[fn].v_alias_vec(target));
}

// Gets the target of the assignment specified by 'a_node'.
// (The target would be the left most variable.)
long DB::get_assign_lhs_var (long fn, std::pair<long,long> a_node)
{
	long start = a_node.first, end = a_node.second;
	long target = -1;
	for (long i = start; i < end; i++) {
		std::string str = create_var_name (tree_node_[i].name(), fn);
		if (target == -1) {
			// Only grabs first var on left of '=' sign
			// this allows it to only get the 'a' in 'a[i]' statements
			// NOTE: If variable is not in map 'var_map' returns '-1'
			target = get_literal_id (str);
		}
		if (target != -1) {	
			return target;
		}
	}
	Rabort();
	return -1;
}

// Give each var in 'lhs_var' the aliases of all the others
void DB::update_lhs_var_aliases (long id, rstd::RVector<long> lhs_var)
{
	std::set<long> s;
	MAP (x, lhs_var.size()) {
		std::set<long> vs = function_[id].v2v_alias_var(lhs_var[x]);
		set_union(s.begin(), s.end(), vs.begin(), vs.end(),
				inserter(s, s.begin()));
	}
	MAP (x, lhs_var.size()) {
		function_[id].set_v2v_alias_vec(lhs_var[x], s);
	}
}

/*===========================================================================*/
// Output routines
/*===========================================================================*/
// Generates the vcg file to view the call graph
void DB::print_fgraph_vcg ()
{
	std::ofstream out;
	std::string file_name = "call_graph.vcg";
	out.open(file_name.c_str());

	if (!out) {
		std::cout << "Cannot open output file";
		Rabort();
	}

	out << "graph: { label: \"" << file_name << "\"\n";
	out << "display_edge_labels: no\n";

	call_graph_.print_vcg_inside(out);
	

	out << "}\n";
}

// Generates the vcg file to view the task graphs for each function
void DB::print_tgraph_vcg ()
{
	std::ofstream out;

	// create subdirectory to put vcg files in
	std::string dir_name = "tmp_vcg";
	std::string sysstr = "mkdir " + dir_name;
	system(sysstr.c_str());

	// For each function which contains nodes, create a vcg file
	MAP (x, fg_.size()) {
		if (fg_[x].size_vertex()) {
			std::string s = "tmp_vcg/" + fg_[x].name();
			out.open(s.c_str());
			Rassert(out);

			out << "graph: { label: \"" << s << "\"\n";
			if (ArgPack::ap().display_edge_labels()) {
				out << "display_edge_labels: yes\n";
			} else {
				out << "display_edge_labels: no\n";
			}
			fg_[x].print_vcg_inside(out);
			out << "}\n";
			out.close();
		}
	}
}

// output some database info...(Not used)
std::ostream &
operator<<(std::ostream & os, const DB & db_a) {
	os << "Functions:\n";
	MAP (x, db_a.size_function()) {
		os << db_a.function(x) << std::endl;
	}
	os << "Variables:\n";
	MAP (x, db_a.size_variable()) {
		os << db_a.variable(x) << std::endl;
	}
	return os;
}

// output method for a set of longs...
std::ostream &operator <<(std::ostream &output, std::set<long> &sa)
{
	std::set<long>::iterator i;
	for (i = sa.begin(); i != sa.end(); i++) {
		output << *i << " ";
	}
	return output;
}

// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  dbio.cc

  Twin file to database.cc.  This file contains DB methods which
are needed for profiling, recompiling and generating a
task graph.
************************************************************/

#include "database.h"
#include "ArgPack.h"
#include <fstream>
#include <map>
#include <set>
#include <algorithm>
#include "ktime.h"

// Class used to temporarily store some annotation information 
// from the start of a loop/cond or function call.  This info
// is then recalled at the end of the loop/cond or func call.
class CallStackItem {
	long tree_node_;
	long source_fn_;
	TimeUnit tu_;
public:
	CallStackItem(long tn, long sf, TimeUnit t);
	long tree_node() { return tree_node_; }
	long source_fn() { return source_fn_; }
	TimeUnit tu() { return tu_; }
};

// Constructor
CallStackItem::CallStackItem (long tn, long sf, TimeUnit t) :
	tree_node_(tn), source_fn_(sf), tu_(t)
{
}

//------------------------------------------------------------
// Profile-related methods
//------------------------------------------------------------

// Recompiles source code.  Uses "kmake.perl".  This should be
// a user-modified script to call a make file or run gcc.  Either
// way, the user should make sure that KSVtime.c is linked in with
// the new executable.
void DB::prof_recompile()
{
	string fname = ArgPack::ap().get_fname();
	string str = "kmake.perl " + fname;
	cout << str << endl;
	system (str.c_str());

// Don't use since some compilations can cause this to fail.
// It would be nice to assert that the compilation succeeded, but
// it must be done some other way...???
//	Rassert (system (str.c_str()) == 0);
}

// Reruns executable.  Removes old data so that it cannot be used
// to give bad results.  Uses "krunner.perl".  This should be a 
// user-modified script to run the executable with appropriate
// input.  This should be a trivial script, but is done this way
// to increase tool flexibility.
void DB::prof_rerun()
{
	system ("rm KSV.out");
	cout << "***Removing KSV.out***\n";
	system ("krunner.perl");

// Don't use since some programs can cause this to fail.
// It would be nice to assert that this succeeded, but
// it must be done some other way...???
//	Rassert (system ("a.out in1 in2") == 0);
}

// Read in data from rerunning new executable.  Use the new info
// to generate edge and node weights for task graph.
void DB::prof_read_data()
{
	ifstream annote_stream;
	RVector<string> vec;
	RVector<long> time_s;
	RVector<long> time_us;
	string s;

	annote_stream.open("KSV.annote");
	Rassert (annote_stream);

	// First read data type sizes.  "KSV" is the flag to indicate
	// this is complete and the next section is starting.
	do {
		getline(annote_stream, s);
		vec = tokenize (s);
		Rassert(!vec.empty());
		if (!(vec[0] == "KSV")) {
			type_map_[vec[0]] = Conv(vec[1]);
		}	
	} while (!(vec[0] == "KSV"));

	// Read data profile times.  Each line is read in as a
	// tokenized string (with 5 components).
	do {
		getline(annote_stream, s);
		vec = tokenize (s);
		if (!vec.empty()) {
			annote_data_.push_back(vec);
		}
	} while (! vec.empty());

	/////////////////////////////////////////////////
	// Use the profile data to get timing info
	// -----------------------------------
	// Each line has 5 parts:
	//    current_function
	//    event (func start/end, loop/cond start/end, statement_end
	//    AST tree node
	//    second   -----\  These are the values from gettimeofday
	//    microsecond --/
	/////////////////////////////////////////////////
	// setup
	TimeUnit t1, t2;	
	t1.set_time(0,0);
	t2.set_time(0,0);
	Rassert(annote_data_[0][1] == "fn_start");

	// Analyze each line from the profile.
	RVector<CallStackItem> fn_stack;
	RVector<CallStackItem> loop_select_stack;
	MAP (x, annote_data_.size()) {
		// 'fn' is the function which the annotation is about
		long fn = func_map(annote_data_[x][0]);
		// 'fname' is the name of that function
		string fname = function_[fn].name();

		// Check function stack.  The item on the top of the stack is
		// the innermost function called.  If 'fn' isn't in that
		// function's call graph, then pop the stack.  Repeat until
		// the 'fn' is inside the current function's call graph.
		// (Another reason to repeat poping is that 'fn' is a
		// reinvocation of the current function.  After popping
		// a function, record how long that function took.
		while (fn_stack.size()) {
			// 'f' is the index of the current function
			// 'new_f' is the same index as 'fn'
			long fn_index = fn_stack.size() - 1;
			long f = fn_stack[fn_index].source_fn();
			long new_f = func_map(annote_data_[x][0]);
			if (call_graph_.in_subtree(f, new_f)) {
				break;
			} else if (new_f == f) {
				if (annote_data_[x][1] != "fn_start") {
					break;
				}
			}

			// If neither condition met, pop this item
			t1 = fn_stack[fn_index].tu();
			t2.set_time(Conv(annote_data_[x][3]), Conv(annote_data_[x][4]));
			fg_[f].set_time_unit(t2-t1);
//			cout << "FTIME: " << function_[f].name() << " " << t2-t1 << endl;
			fn_stack.pop_back();
		}

		// Check loop/cond stack.  The item on the top of the stack is
		// the innermost loop/cond.  If 'new_f' isn't in the current
		// function's ('f') call graph, then pop the stack.  Repeat until
		// the 'f_new' is inside the current function's call graph.
		// After popping a loop/cond, record how long it took.
		while (loop_select_stack.size()) {
			long loop_select_index = loop_select_stack.size() - 1;
			long f = loop_select_stack[loop_select_index].source_fn();
			long new_f = func_map(annote_data_[x][0]);
			if (call_graph_.in_subtree(f, new_f)) {
				break;
			}
			// If neither condition met, pop this item
			t1 = loop_select_stack[loop_select_index].tu();
			t2.set_time(Conv(annote_data_[x][3]), Conv(annote_data_[x][4]));
			set_lc_task_time (
						f, loop_select_stack[loop_select_index].tree_node(), t2-t1);
//			cout << "LC: " << loop_select_stack[loop_select_index].tree_node()
//					<< " " << t2-t1 << endl;
			loop_select_stack.pop_back();
		}

		// record how long a give line took
		if (annote_data_[x][1] == "statement_end") {
			t1.set_time(Conv(annote_data_[x-1][3]), Conv(annote_data_[x-1][4]));
			t2.set_time(Conv(annote_data_[x][3]), Conv(annote_data_[x][4]));
			set_se_task_time (fn, Conv(annote_data_[x][2]), t2-t1);
//			cout << "SE: " << annote_data_[x][2] << " " << t2-t1 << endl;

		} else if (annote_data_[x][1] == "fn_start") {
			long tree_node = Conv(annote_data_[x][2]);
			long f = func_map(annote_data_[x][0]);
			Rassert(f != -1);
			t1.set_time(Conv(annote_data_[x][3]), Conv(annote_data_[x][4]));
			fn_stack.push_back(CallStackItem(tree_node, f, t1));

		// Record preliminary info to later determine length of loop/cond
		} else if ((annote_data_[x][1] == "loop_start") || 
					(annote_data_[x][1] == "select_start")) {
			long tree_node = Conv(annote_data_[x][2]);
			long f = func_map(annote_data_[x][0]);
			Rassert(f != -1);
			t1.set_time(Conv(annote_data_[x][3]), Conv(annote_data_[x][4]));
			loop_select_stack.push_back(CallStackItem(tree_node, f, t1));
		}
	}

	set_all_fc_task_times ();
}

void DB::set_all_fc_task_times ()
{
	Rassert(fg_.size() == function_.size());
	MAP (x, function_.size()) {
		RVector<long> func_node = fg_[x].func_call_nodes();
		MAP (y, func_node.size()) {
			TimeUnit tu;
			tu.set_time(0,0);
			RVector<long> call_list = fg_[x][func_node[y]].func();

			MAP (z, call_list.size()) {
				long fc_num = call_list[z];
				TimeUnit t2 = fg_[fc_num].time_unit();
				tu = tu + t2;
			}
			fg_[x][func_node[y]].set_time_unit(tu.sec(), tu.usec());
		}
	}
}

// set statement end node times
void DB::set_se_task_time (long f, long tree_node, TimeUnit t)
{
	long n = fg_[f].find_se_node(tree_node);
	Rassert(n != -1);
	TimeUnit old_t = fg_[f][n].time_unit();

	// keep worst-case time
	if (old_t < t) {
		fg_[f][n].set_time_unit(t.sec(), t.usec());
	}
}

// set loop/cond node times
void DB::set_lc_task_time (long f, long tree_node, TimeUnit t)
{
	long n = fg_[f].find_lc_node(tree_node);
	Rassert(n != -1);
	TimeUnit old_t = fg_[f][n].time_unit();

	// keep worst-case time
	if (old_t < t) {
		fg_[f][n].set_time_unit(t.sec(), t.usec());
	}
}

// Determine node edge sizes based on profile info.
void DB::prof_get_arc_lengths()
{
	//--------------------------------------------------
	// Determine sizes for each variable!!!
	//--------------------------------------------------
	MAP (x, variable_.size()) {
		string type_name = variable_[x].type();
		long type_size;
//cout << variable_[x].name() << " " << variable_[x].type();

		// unknown variables' types are set equal to pointer size
		if (type_map_.find(type_name) == type_map_.end()) {
			type_size = ArgPack::ap().pointer_size();
		} else {
			type_size = type_map_[type_name];
		}
		variable_[x].determine_total_size(type_size);
//		cout << " " << variable_[x].total_size() << endl;
	}

//--------------------------------------------------
// DEBUG PRINTOUT
//--------------------------------------------------
//MAP (x, function_.size()) {
//set bob = function_[x].vars_touched();
//cout << function_[x].name() << ": " << bob  << endl;
//}

	//--------------------------------------------------
	// Determine arc length based on variables' sizes 
	// and func return values
	//--------------------------------------------------
	long base_out_size = 0;
	MAP (x, fg_.size()) {
		MAP (y, fg_[x].size_vertex()) {
			// Get base size if node has a function return value.
			base_out_size = determine_node_function_out_size(x, y);
//			cout << function_[x].name() << " node:" << y << " ";

			// For each edge, determine it's edge size based on the
			// sum of the common variables' sizes.
			MAP (z, fg_[x].vertex(y)->size_out()) {
				long arc_num = fg_[x].vertex(y)->out(z);
				set<long> i_set = fg_[x](fg_[x].vertex(y)->out(z)).vars();

				long arc_out_size = base_out_size;
				set<long>::iterator i;
				for (i = i_set.begin(); i != i_set.end(); i++) {
					long var_index = *i;
					long tmp_size = variable_[var_index].total_size();
					if ((tmp_size >= 0) && (arc_out_size != -1)) {
						arc_out_size += tmp_size;
					} else {
						arc_out_size = -1;
					}
//cout << variable_[var_index].name() << " ";
				}
//cout << endl;
				i_set.clear();
				fg_[x](arc_num).set_val(arc_out_size);

			}
		}
	}
}

// If node isn't a loop/cond, it might contain a return value
// to be passed to other nodes.  If so, determine that size.
long DB::determine_node_function_out_size (long f, long n)
{
	//--------------------------------------------------
	// loops and cond nodes don't return values...
	//--------------------------------------------------
	if (fg_[f][n].is_loop_or_cond_node()) {
		return 0;
	}

	//--------------------------------------------------
	// Determine func call return size (if there is one)
	//--------------------------------------------------
	long sz = 0;
	RVector<long> func = fg_[f][n].func();

//	cout << function_[f].name() << " node: " << n << ":  ";

	MAP (x, func.size()) {
//		cout << function_[func[x]].name() << " ";
		string type = function_[func[x]].type();
		long tmp_size = type_map_lookup(type);

		// If data type size is known, add that to the size.
		if ((tmp_size >= 0) && (sz != -1)) {
			sz += type_map_lookup(type);
		} else {
//KSV WARNING, this might be best as set to pointer size
			sz = -1;
		}
//		cout << sz;
	}
//	cout << endl;

	return sz;
}

// Print task graph.
void DB::print_task_graphs() 
{
	ofstream out;
	string filename = ArgPack::ap().get_fname() + ".tg";
	out.open(filename.c_str());

	out << "#  " << filename << " Task Graphs:" << endl;
	out << "#  (Automatically generated file)" << endl << endl;

	MAP (x, fg_.size()) {
		out << "#  " << fg_[x].name() << endl;
		if (fg_[x].name() == "main") {
			out << "@Task Graph " << x << "  {" <<endl;
		} else {
			out << "@SubTask Graph " << x << "  {" <<endl;
		}
//		out << "SE: " << fg_[x].statement_nodes() << endl;
//		out << "LC: " << fg_[x].loop_or_cond_nodes() << endl;
//		out << "FC: " << fg_[x].func_call_nodes() << endl;

		// Print nodes
		MAP (y, fg_[x].size_vertex()) {
			out << "   TASK t" << x << "_" << y;
			out << "   SIZE " << fg_[x][y].time_unit() 
//				<< "    " << fg_[x][y].start()
				<< endl;
			
		}
		out << endl << endl;
		// Print edges
		MAP (y, fg_[x].size_edge()) {
			out << "   ARC a" << x << "_" << y;
			out << "\t FROM t" << x << "_" << fg_[x].edge(y)->from();
			out << "\t TO t" << x << "_" << fg_[x].edge(y)->to();
			out << "\t SIZE " << fg_[x](y).val();

			if (ArgPack::ap().display_arc_variables()) {
				set<long> v = fg_[x](y).vars();
				set<long>::iterator i;
				for (i = v.begin(); i != v.end(); i++) {
					long tmp = *i;
					out << "\t " << variable_[tmp].name();
				}
			}
			out << endl;
		}


		out << "}\n" << endl;
	}
	out.close();
}

// Print task graph in "Standard" format.  This format is 
// compatible with MOCSYN by Robert Dick.
void DB::print_standard_task_graphs()
{
	ofstream out;
	string filename = ArgPack::ap().get_fname() + ".std_tg";
	out.open(filename.c_str());

	out << "#  Task graph for --  " << filename << ":" << endl;
	out << "#  (Automatically generated file)" << endl << endl;
	out << "@HYPERPERIOD 1" << endl << endl;

	string main_string = "main";
	RVector<long> fn_list = call_graph_.top_sort (func_map_[main_string]);
	RVector<TimeUnit> task_type;
	RVector<long> arc_type;

	//-----------------------------------------
	// Communication quantities for arc lengths
	//-----------------------------------------
	MAP (x, fn_list.size()) {
		long curr_func = fn_list[x];
		MAP (y, fg_[curr_func].size_edge()) {
			arc_type.push_back (fg_[curr_func](y).val());
			if (arc_type[arc_type.size() - 1] < 0) {
				cerr << "Edge with negative value.  "
					<< "Probably caused by pointer."  << endl;
				Rabort();
			}
		}
	}
	out << "@COMMUN_QUANT 0 {" << endl;
	MAP (x, arc_type.size()) {
		out << x << "\t" << arc_type[x] << endl;
	}
	out << "}\n\n";

	//---------------------------
	// Calculate task graphs here
	//---------------------------
	arc_type.clear();
	MAP (x, fn_list.size()) {
		bool is_main = false;
		long curr_func = fn_list[x];
		if (fg_[curr_func].name() == "main") {
			is_main = true;
		}

		out << "#  " << fg_[curr_func].name() << endl;
		if (is_main) {
			out << "@TASK_GRAPH " << x << "  {" << endl;
			out << "  PERIOD 1" << endl;
		} else {
			out << "# @SUBTASK_GRAPH " << x << "  {" << endl;
			out << "#  APERIODIC " << endl;
		}

		// Print nodes
		MAP (y, fg_[curr_func].size_vertex()) {
			if (!is_main) {
				out << "# ";
			}
			out << "   TASK t" << x << "_" << y;
			out << "   TYPE " << task_type.size() << endl;
			task_type.push_back (fg_[curr_func][y].time_unit());
		}
		out << endl << endl;

		// Print edges
		MAP (y, fg_[curr_func].size_edge()) {
			if (!is_main) {
				out << "# ";
			}
			out << "   ARC a" << x << "_" << y;
			out << "\t FROM t" << x << "_" << fg_[curr_func].edge(y)->from();
			out << "\t TO t" << x << "_" << fg_[curr_func].edge(y)->to();
			out << "\t TYPE " << arc_type.size();

			arc_type.push_back (fg_[curr_func](y).val());
			if (arc_type[arc_type.size() - 1] < 0) {
				cerr << "Edge with negative value.  "
					<< "Probably caused by pointer."  << endl;
				Rabort();
			}

			out << endl;
		}

		// Print deadlines
		out << endl;
		long dl_cnt = 0;
		MAP (y, fg_[curr_func].size_vertex()) {
			if (fg_[curr_func].vertex(y)->size_out() == 0) {
				if (!is_main) {
					out << "# ";
				}
				out << "   HARD_DEADLINE d" << x << "_" << dl_cnt
					<< " ON t" << x << "_" << y << " AT 1";
				out << endl;

			}
		}

		out << endl;
		if (!is_main) {
			out << "# ";
		}
		out << "}\n" << endl;
	}

	// Print out processor info
	out << "@CORE 0 {\n";
	out << "# price buffered max_freq width    height   density "
		<< " preempt_power commun_en_bit io_en_bit idle_power" << endl;
	out << "  1 \t 1 \t 6.67e+08 6.48e-03 6.48e-03 .275 "
		<< " 0 \t 0 \t 0 \t 1.6" << endl;
	out << "#--------------------------------------"
		<< "----------------------------------------" << endl;
	out << "# type version valid task_time preempt_time code_bits task_power"
		<< endl;

	TimeUnit t_zero;
	t_zero.set_time(0,1);
	MAP (x, task_type.size()) {
		// output task size...last 2 numbers should be improved...
		out << x << " \t 0 \t 1 \t";
		if (task_type[x] < t_zero) {
			out << t_zero;
		} else {
			out << task_type[x];
		}
		out << "\t\t 150E-6 \t\t "
			<< "1 \t 16" << endl;
	
	}

	out << "}\n\n";

	out.close();
}


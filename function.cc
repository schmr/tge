// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  function.cc

  Provides support to database.cc.  This is one of the major files
in the program.  Defines Var, FuncCall and Function classes.
These classes store much of the actual info about the input
source code.
************************************************************/

#include "function.h"
#include "ArgPack.h"

//************************************************************
// Var
//************************************************************

// Constructor
Var::Var (std::string n, std::string t, rstd::RVector<std::string> s, long elements) :
	name_(n), type_(t), total_size_(-1), num_elements_(elements), stype_(s), 
	is_pointer_(false), is_static_(false)
{
	Rassert(elements >= 0);
	MAP (x, s.size()) {
		if (s[x] == "pointer") {
			is_pointer_ = true;			
		}
		if (s[x] == "static") {
			is_static_ = true;			
		}
	}
}

// Determines the size of the variable (type size * number of elements)
void Var::determine_total_size (long type_size)
{
	// If it's a pointer, use pre-defined value for pointers
	if (is_pointer_) {
		total_size_ = ArgPack::ap().pointer_size();
	} else {
		total_size_ = num_elements_ * type_size;
	}
}

//************************************************************
// FuncCall
//************************************************************

// Constructor
FuncCall::FuncCall (std::string n, long r, long nnode) :
	name_(n), parms_(), commas_(0), root_node_(r), fn_name_node_(nnode)
{
}

// Add a dynamic parameter to the func call
void FuncCall::add_parm(std::pair<long,ArgType> p)
{
//cout << "Comma: " << commas_ << "  " << root_node_ << std::endl;
//	Rassert(parms_.size() < (commas_ + 1));

	if (parms_.size() < (commas_ + 1)) {
		while (parms_.size() < commas_) {
			parms_.push_back(std::pair<long,ArgType> (-1, CONST));
		}
		parms_.push_back(p);
	} else {
		long b = parms_.size()-2;
		std::cerr << "Note: Argument " << b << " is not added to parm list"
			<< " for " << name() << "[" << root_node_ << "]"
			<< "because there is already one variable in the slot.\n";
	}
}

// Add a comma to keep parameters in correct order 
void FuncCall::add_comma()
{
	commas_++;
//cout << "nc: " << commas_ << "  " << root_node_ << std::endl;
}

// Help keep parameters in order
void FuncCall::balance_commas()
{
//cout << " Bal  " << root_node_ << std::endl;
	while (parms_.size() <= commas_) {
		parms_.push_back(std::pair<long,ArgType> (-1, CONST));
	}
	commas_ = 0;
}

//************************************************************
// Function
//************************************************************

Function::Function (std::string n, std::string t, rstd::RVector<std::string> s) :
	name_(n), type_(t), stype_(s), parameter_(), variable_(), var_use_(), 
	action_(), action_type_(), action_tree_node_(),
	func_call_(), assign_node_(),
	defined_(false), self_dependent_(true), vars_touched_(),
	v2v_alias_(), elipses_(false)
{
}

//--------------------------------------------------
// Analysis routines
//--------------------------------------------------
// List of names of all functions called by this function
rstd::RVector<std::string> Function::func_call_names ()
{
	rstd::RVector<std::string> call_list;
	MAP (x, func_call_.size()) {
		call_list.push_back(func_call_[x].name());
	}
	return call_list;
}

// Add all variables in 'vars' to 'target' alias set
void Function::add_aliases (long target, rstd::RVector<long> vars, 
				bool clear_old_alias)
{
	long pointer = target;
	if (clear_old_alias) {
		Rassert(0);
	}
	MAP (x, vars.size()) {
		v2v_alias_[pointer].insert(vars[x]);
		set_union(v2v_alias_[pointer].begin(), v2v_alias_[pointer].end(),
					v2v_alias_[vars[x]].begin(), v2v_alias_[vars[x]].end(),
					inserter(v2v_alias_[pointer], v2v_alias_[pointer].begin()));
	}
//	cout << pointer << " new v2v_set " << v2v_alias_[pointer] << std::endl;
}

// Return the indicies of the variables 'v' can alias to.
rstd::RVector<long> Function::v_alias_vec (long v)
{
   rstd::RVector<long> ret_val;
   ret_val.push_back(v);
   std::set<long>::iterator i;

   std::set<long> s = v2v_alias_[v];
   for (i = s.begin(); i != s.end(); i++) {
      long t = *i;
      ret_val.push_back(t);      
   }
   return ret_val;
}

// Return the indicies of the parameters of the function
std::set<long> Function::parameter_set()
{
	std::set<long> ret_val;
	MAP (x, parameter_.size()) {
		ret_val.insert(parameter_[x]);
	}
   return ret_val;
}

// Set the variable-to-variable alias vector size.
void Function::init_alias_vectors (long v_size)
{
	v2v_alias_.resize(v_size);
}

// Check if 'v' is in 'v_set_num' variable alias set
bool Function::var_in_v_alias (long v, long v_set_num)
{
	if ((v2v_alias_[v_set_num].find(v)) != (v2v_alias_[v_set_num].end())) {
		return true;
	} else {
		return false;
	}
}

// Remove 'v' from 'v_set_num' variable alias set
void Function::remove_from_v_alias (long v, long v_set_num)
{
	std::set<long>::iterator i = v2v_alias_[v_set_num].find(v);
	Rassert(i != v2v_alias_[v_set_num].end());
	v2v_alias_[v_set_num].erase(i);
}

// Add all of 'source_num' aliases to the set for 'target_num'
void Function::append_to_v_alias (long target_num, long source_num)
{
	set_union(v2v_alias_[target_num].begin(), v2v_alias_[target_num].end(), 
				v2v_alias_[source_num].begin(), v2v_alias_[source_num].end(),
				inserter(v2v_alias_[target_num], v2v_alias_[target_num].begin()));
}

// Return variables aliased to parameters and the arguments themselves
std::set<long> 
Function::func_call_cleanup_aliases (rstd::RVector<std::pair<long,ArgType> > p, 
												rstd::RVector<long> parms)
{
	// parms: part of header definition
	// p: dynamic parm binding

	// Add to dynamic arg alias set based on func_call's effects
	MAP(x, p.size()) {
		if (p[x].second == VAR) {
			// p is a variable arg to func called
			MAP (y, parms.size()) {
				if (p[y].second == VAR) {
					if ((x < parms.size()) && (y < parms.size())) {
						// x in y: remove x from y(dynamic) and add x(dynamic)
						if (var_in_v_alias(parms[x], parms[y])) {
							append_to_v_alias (p[y].first, p[x].first);
							remove_from_v_alias (parms[x], p[y].first);
						}
					}
				}
			}
		}
	}

	// Prepare a return value encompassing: variables aliased to parms,
	//    the arguments themselves, 
	std::set<long> s;
	MAP (x, p.size()) {
		if (p[x].second == 0) {
			s.insert(p[x].first);
			set_union(s.begin(), s.end(), 
					v2v_alias_[p[x].first].begin(), v2v_alias_[p[x].first].end(),
					inserter(s, s.begin()));
		}
	}
	return s;
}

//--------------------------------------------------
// "Function" creation routines
//--------------------------------------------------
// Add a function call to the current function
long Function::add_func_call(std::string n, long r, long nnode)
{
	func_call_.push_back(FuncCall(n, r, nnode));
	return (func_call_.size() - 1);
}

// Add a parameter to the current function
void Function::add_parameter (long v)
{
	parameter_.push_back(v);
}

// Add a variable to the current function
// (This might not really be useful)
void Function::add_variable(long v)
{ 
	variable_.push_back(v);
}

// Add elipses as a parameter to indicate variable parameters
void Function::add_elipses()
{
	elipses_ = true;
}

// Add an argument to a funciton call
void Function::add_func_call_arg (long c, long arg, ArgType arg_type)
{
	func_call_[c].add_parm(std::pair<long,ArgType> (arg,arg_type));
}

// Add an action (actions defined in tree.cc file)
void Function::add_action (std::string n, ActionType t, long tn)
{
	action_.push_back(n);
	action_type_.push_back(t);
	action_tree_node_.push_back(tn);
}

// To keep arguments in order
void Function::add_func_call_comma (long fc_num)
{
	func_call_[fc_num].add_comma();
}

// To keep arguments in order
void Function::balance_func_call_commas (long fc_num)
{
	func_call_[fc_num].balance_commas();
}

//**************************************************
// Printout routines
//**************************************************

std::ostream &
operator<<(std::ostream & os, const Var & var_a) {
	os << "    Var:  " << var_a.name();
	os << "  Type: " << var_a.type();
	os << "  SType: " << var_a.stype() << std::endl;

	return os;
}

std::ostream &
operator<<(std::ostream & os, const rstd::RVector<std::pair<long,ArgType> > &vp_a) {
	MAP (x, vp_a.size()) {
		if (vp_a[x].second == VAR) {
			os << vp_a[x].first << " ";
		} else if (vp_a[x].second == FUNC) {
			os << "(" << vp_a[x].first << ") ";
		} else 
			os << "(CONST) ";
	}
	return os;
}

std::ostream &
operator<<(std::ostream & os, const FuncCall & fn_a) {
	os << "    Func_call:  " << fn_a.name();
	os << "  parms: " << fn_a.parms() << std::endl;

	return os;
}

std::ostream &
operator<<(std::ostream & os, const Function & fn_a) {
	os << fn_a.defined() << " Function:  " << fn_a.name() << std::endl;
	os << "  Type: " << fn_a.type() << "  " << fn_a.stype() << std::endl;
	os << "  Parameters: " << std::endl << fn_a.parameter() << std::endl;
	os << "  Variables: " << fn_a.variable() << std::endl;
	os << "  Function calls: " << std::endl << fn_a.func_call() << std::endl;
	os << "  Var use: " << fn_a.var_use() << std::endl;
	os << "  Actions: " << fn_a.action() << std::endl;
	return os;
}




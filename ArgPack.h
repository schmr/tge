// Copyright 2002 by Keith Vallerio.
// All rights reserved.

/************************************************************
  ArgPack.h

  Header file for ArgPack which contains the global variable 
information.  There can be only one ArgPack.  It is used to
hold parameters passed to the program at run time.  This way
they are all held in one location.
************************************************************/

#ifndef ARG_PACK_H_
#define ARG_PACK_H_

/*###########################################################################*/
#include <string>
#include <memory>
#include <fstream>
#include <iosfwd>
#include <exception>
#include <iostream>

template <typename> class RVector;

/*###########################################################################*/
// Handles arguments to the program.
class ArgPack {
public:
	class init_error : public std::exception {
	public:
		init_error(const std::string & err) throw() : what_(err) {}
		virtual ~init_error() throw() {}
		virtual const char * what() const throw() { return what_.c_str(); }

	private:
			std::string what_;
	};

	class parse_error : public std::exception {
	public:
		parse_error(const std::string & file, long line, const std::string & err = "") ;
		virtual ~parse_error() throw() {}
		virtual const char * what() const throw() { return what_.c_str(); }

	private:
			std::string what_;
	};

// Gets the default ArgPack.
	static const ArgPack & ap() { return *def_ap_; }
	static ArgPack & write_ap() { return *def_ap_; }

	static void init(int argc, char * const argv [], std::ostream & output);

// Throws init_error if it can't be constructed.
	ArgPack(int argc, char * const argv [], std::ostream & output);
	void help(std::ostream &os) const;

	void parse_error(std::ostream & os = std::cout,
		const char * message = "Parse error") const;

	const RVector<std::string> get_line();
	int get_rand_seed () { return rand_seed; }

	// query
	std::string get_fname () { return file_name_; }
	const std::string get_fname () const { return file_name_; }
	bool echo_on () { return echo_on_; }
	const bool echo_on () const { return echo_on_; }
	bool display_state_end () { return display_state_end_; }
	const bool display_state_end () const { return display_state_end_; }
	bool strict_dependence () { return strict_dependence_; }
	const bool strict_dependence () const { return strict_dependence_; }
	bool profile () { return profile_; }
	const bool profile () const { return profile_; }
	bool interpret () { return interpret_; }
	const bool interpret () const { return interpret_; }
	long pointer_size () { return pointer_size_; }
	const long pointer_size () const { return pointer_size_; }
	bool display_arc_variables () { return display_arc_variables_; }
	const bool display_arc_variables () const { return display_arc_variables_; }
	bool loosest_dependence () { return loosest_dependence_; }
	const bool loosest_dependence () const { return loosest_dependence_; }
	bool display_actions () { return display_actions_; }
	const bool display_actions () const { return display_actions_; }
	bool display_edge_labels () { return display_edge_labels_; }
	const bool display_edge_labels () const { return display_edge_labels_; }
	bool calculate_edge_lengths () { return calculate_edge_lengths_; }
	const bool calculate_edge_lengths() const {return calculate_edge_lengths_;}
	bool create_task_graph () { return create_task_graph_; }
	const bool create_task_graph() const {return create_task_graph_;}

private:
		// The ArgPack
		static std::auto_ptr<ArgPack> def_ap_;

		std::string file_name_;  // original source file name
		std::ifstream is_;   // input stream
		long is_line_;
		int rand_seed;
		bool echo_on_;  // prints original C while parsing
		bool display_state_end_;  // should be always true
		bool strict_dependence_;
		bool profile_;
		bool interpret_;
		// number of bytes pointers should be replaced with (default -1)
		long pointer_size_;
		bool display_arc_variables_;
		bool loosest_dependence_;  // turn off for K&R C
		bool display_actions_;   // display actions in function vcg file
		bool display_edge_labels_;  // display edge weights in vcg file
		bool calculate_edge_lengths_;

		// create a vcg file labeled as a task graph instead of normal method
		bool create_task_graph_;
};

/*###########################################################################*/
#endif




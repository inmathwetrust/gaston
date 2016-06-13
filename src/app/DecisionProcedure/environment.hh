/*****************************************************************************
 *  gaston - We pay homage to Gaston, an Africa-born brown fur seal who
 *    escaped the Prague Zoo during the floods in 2002 and made a heroic
 *    journey for freedom of over 300km all the way to Dresden. There he
 *    was caught and subsequently died due to exhaustion and infection.
 *    Rest In Piece, brave soldier.
 *
 *  Copyright (c) 2015  Tomas Fiedor <ifiedortom@fit.vutbr.cz>
 *      Notable mentions: Ondrej Lengal <ondra.lengal@gmail.com>
 *
 *  Description:
 *		Global header file for Gaston tool, containing options for
 *		enabling/disabling debug messages, optimizations, measuring
 *		and some globally used enumerations and using directives.
 *****************************************************************************/

#ifndef __DWINA_ENV__H__
#define __DWINA_ENV__H__

#include <boost/dynamic_bitset.hpp>
#include <boost/functional/hash.hpp>
#include <exception>
#include <iostream>
#include <memory>
#include <list>
#include <typeinfo>
#include <vata/bdd_bu_tree_aut.hh>
#include <vata/parsing/timbuk_parser.hh>
#include <vata/serialization/timbuk_serializer.hh>
#include "../Frontend/dfa.h"
#include "utils/cached_binary_op.hh"
#include "mtbdd/ondriks_mtbdd.hh"

/*****************************
 * FORWARD CLASS DECLARATION *
 *****************************/
class SymbolicAutomaton;
class ZeroSymbol;
class Term;
class TermEnumerator;
class ASTForm;
template<class A, class B, class C, class D, void (*E)(A const&),void (*F)(B&)>
class BinaryCache;
template<class A>
class PairCompare;
template<class A>
class PrePairCompare;

struct ResultHashType;
struct SubsumptionHashType;
struct PreHashType;
struct DagHashType;
template<class Key>
class DagCompare;

/***********************
 * GLOBAL ENUMERATIONS *
 ***********************/
enum Decision {SATISFIABLE, UNSATISFIABLE, VALID, INVALID, UNKNOWN};
enum AutType {SYMBOLIC_BASE, ROOT_PROJECTION, BASE, COMPLEMENT,
	BINARY, TERNARY, NARY,
	INTERSECTION, TERNARY_INTERSECTION, NARY_INTERSECTION,
	UNION, TERNARY_UNION, NARY_UNION,
	IMPLICATION, TERNARY_IMPLICATION, NARY_IMPLICATION,
	BIIMPLICATION, TERNARY_BIIMPLICATION, NARY_BIIMPLICATION,
	PROJECTION};
enum StatesSetType {E_INITIAL, E_FINAL};
enum TermType {TERM_PRODUCT, TERM_TERNARY_PRODUCT, TERM_NARY_PRODUCT, TERM, TERM_EMPTY, TERM_BASE, TERM_FIXPOINT,
	TERM_LIST, TERM_CONTINUATION};
enum FixpointTermSem {E_FIXTERM_FIXPOINT, E_FIXTERM_PRE};
enum ComparisonType {E_BY_SAME_PTR, E_BY_DIFFERENT_TYPE, E_BY_STRUCTURE};
enum UnfoldedInType {E_IN_SUBSUMPTION, E_IN_ISECT_NONEMPTY, E_IN_COMPARISON, E_IN_NOWHERE};
enum SubsumptionResult {E_FALSE, E_TRUE, E_PARTIALLY, E_TRUE_BUT_WORKLIST};
enum ExampleType {SATISFYING, UNSATISFYING};
enum WorklistSearchType {E_BFS, E_DFS, E_UNGROUND_ROOT};

enum class ProductType {E_INTERSECTION, E_UNION, E_IMPLICATION, E_BIIMPLICATION};
static const char* ProductTypeColours[] = {"1;32", "1;33", "1;36", "1;37"};
static const char* ProductTypeAutomataSymbols[] = {"\u2229", "\u222A", "\u2192", "\u2194"};
static const char* ProductTypeTermSymbols[] = {"\u2293", "\u2294", "\u21FE", "\u21FF"};

inline const char* ProductTypeToColour(ProductType type) {
	return ProductTypeColours[static_cast<int>(type)];
}

inline const char* ProductTypeToAutomatonSymbol(ProductType type) {
	return ProductTypeAutomataSymbols[static_cast<int>(type)];
}

inline const char* ProductTypeToTermSymbol(ProductType type) {
	return ProductTypeTermSymbols[static_cast<int>(type)];
}

namespace Gaston {
/***************************
 * GLOBAL USING DIRECTIVES *
 ***************************/
	using Automaton 			 = VATA::BDDBottomUpTreeAut;
	using Formula_ptr            = ASTForm*;
	using Term_ptr				 = Term*;
	using Term_raw 				 = Term*;
	using ResultType			 = std::pair<Term_ptr, bool>;

	using StateType				 = size_t;
	using StateTuple 			 = std::vector<StateType>;
	using StateToStateTranslator = VATA::AutBase::StateToStateTranslWeak;
	using StateToStateMap        = std::unordered_map<StateType, StateType>;

	using SymbolicAutomaton_ptr	 = std::shared_ptr<SymbolicAutomaton>;
	using SymbolicAutomaton_raw	 = SymbolicAutomaton*;

	using Symbol				 = ZeroSymbol;
	using Symbol_ptr			 = Symbol*;
	using SymbolList			 = std::list<Symbol>;

	using BitMask				 = boost::dynamic_bitset<>;
	using VarType				 = size_t;
	using VarList                = VATA::Util::OrdVector<StateType>;
	using VarValue			     = char;
	using TrackType				 = Automaton::SymbolType;

	void dumpResultKey(std::pair<Term_ptr, Symbol_ptr> const& s);
	void dumpResultData(std::pair<Term_ptr, bool>& s);
	void dumpTermKey(Term_ptr const& s);
	void dumpSubsumptionKey(std::pair<Term_ptr, Term_ptr> const& s);
	void dumpSubsumptionData(SubsumptionResult& s);
	void dumpPreKey(std::pair<size_t, Symbol_ptr> const& s);
	void dumpPreData(Term_ptr& s);
	void dumpDagKey(Formula_ptr const& s);
	void dumpDagData(SymbolicAutomaton*& s);
	void dumpEnumKey(TermEnumerator* const& s);

	using TermHash 				 = boost::hash<Term_raw>;
	using TermCompare			 = std::equal_to<Term_raw>;
	using TermCache				 = BinaryCache<Term_raw, SubsumptionResult, TermHash, TermCompare, dumpTermKey, dumpSubsumptionData>;
	using ResultKey				 = std::pair<Term_raw, Symbol_ptr>;
	using ResultCache            = BinaryCache<ResultKey, ResultType, ResultHashType, PairCompare<ResultKey>, dumpResultKey, dumpResultData>;
	using SubsumptionKey		 = std::pair<Term_raw, Term_raw>;
	using SubsumptionCache       = BinaryCache<SubsumptionKey, SubsumptionResult, SubsumptionHashType, PairCompare<SubsumptionKey>, dumpSubsumptionKey, dumpSubsumptionData>;
	using DagKey				 = Formula_ptr;
	using DagData				 = SymbolicAutomaton*;
	using DagNodeCache			 = BinaryCache<DagKey, DagData, DagHashType, DagCompare<DagKey>, dumpDagKey, dumpDagData>;
	using EnumKey				 = TermEnumerator*;
	using EnumSubsumesCache		 = BinaryCache<EnumKey, SubsumptionResult, boost::hash<EnumKey>, std::equal_to<EnumKey>, dumpEnumKey, dumpSubsumptionData>;

	using WorkListTerm           = Term;
	using WorkListTerm_raw       = Term*;
	using WorkListTerm_ptr       = Term_ptr;
	using WorkListSet            = std::vector<std::shared_ptr<WorkListTerm>>;

	using BaseAutomatonType      = DFA;
	using BaseAutomatonStateSet  = VATA::Util::OrdVector<StateType>;
	using BaseAutomatonMTBDD	 = VATA::MTBDDPkg::OndriksMTBDD<BaseAutomatonStateSet>;

	using PreKey				 = std::pair<StateType, Symbol_ptr>;
	using PreCache				 = BinaryCache<PreKey, Term_ptr, PreHashType, PrePairCompare<PreKey>, dumpPreKey, dumpPreData>;
}

/*************************
 * ADDITIONAL STRUCTURES *
 *************************/

class NotImplementedException : public std::exception {
public:
	virtual const char* what() const throw () {
		return "Functionality not implemented yet";
	}
};

class MonaFailureException : public std::exception {
public:
	virtual const char* what() const throw() {
		return "Mona Failed on BDDs\n";
	}
};

class GastonSignalException : public std::exception {
private:
	int _raisedSignal;
public:
	GastonSignalException(int signal) : _raisedSignal(signal) {}

	virtual const char* what() const throw() {
		return (std::string("Decision procedure terminated with signal ") + std::to_string(_raisedSignal)).c_str();
	}
};

class GastonOutOfMemory : public std::exception {
public:
	virtual const char* what() const throw() {
		return "Gaston is Out of Memory. Oaw oaw.";
	}
};

/****************
 * DEBUG MACROS *
 ****************/
#define G_DEBUG_FORMULA_AFTER_PHASE(str) std::cout << "\n\n[*] Formula after '" << str << "' phase:\n"
#define G_NOT_IMPLEMENTED_YET(str) assert(false && "TODO: '" str "' Not Supported yet")

/* >>> Printing Options <<< *
 ****************************/
#define PRINT_PRETTY					true
#define PRINT_DOT_BLACK_AND_WHITE		true

/**
 * >>> Inlining Options <<< *
 ****************************/
#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#define NEVER_INLINE __attribute__((__noinline__))

/* >>> Debugging Options <<< *
 *****************************/
#define DEBUG_DAG_REMAPPING				false
#define DEBUG_ROOT_AUTOMATON		    false
#define DEBUG_AUTOMATA_ADDRESSES		false
#define DEBUG_EXAMPLE_PATHS				false
#define DEBUG_BASE_AUTOMATA 		    false
#define DEBUG_RESTRICTION_AUTOMATA		false
#define DEBUG_MONA_DFA					false
#define DEBUG_MONA_CODE_FORMULA			false
#define DEBUG_RESTRICTIONS				false
#define DEBUG_FIXPOINT 				    false
#define DEBUG_FIXPOINT_WORKLIST		    true
#define DEBUG_FIXPOINT_SYMBOLS		    false
#define DEBUG_FIXPOINT_SYMBOLS_INIT     false
#define DEBUG_INITIAL_APPROX 		    false
#define DEBUG_INTERSECT_NON_EMPTY 	    false
#define DEBUG_TERM_UNIQUENESS			false
#define DEBUG_TERM_CREATION				false
#define DEBUG_TERM_SUBSUMPTION 			false
#define DEBUG_TERM_SUBSUMED_BY 			false
#define DEBUG_TERM_CACHE_COMPARISON		false
#define DEBUG_SYMBOL_CREATION			true
#define DEBUG_CACHE_MEMBERS			    false
#define DEBUG_CACHE_BUCKETS				false
#define DEBUG_CACHE_MEMBERS_HASH		true
#define DEBUG_CONTINUATIONS 			false
#define DEBUG_NO_WORKSHOPS				false
#define DEBUG_PRE					    false
#define DEBUG_GENERATE_DOT_AUTOMATON	true
#define DEBUG_COMPUTE_FULL_FIXPOINT 	false
#define DEBUG_COMPARE_WORKLISTS		    true
#define DEBUG_VARMAP					false
#define DEBUG_MAX_SEARCH_PATH			0
#define DEBUG_M2L_AS_GROUND				false
#define DEBUG_WORKSHOPS					true	// Fixme: This should not be DEBUG, but measure
#define DEBUG_DONT_HASH_FIXPOINTS		false
#define DEBUG_DONT_CATCH_SIGSEGV		true
#define DEBUG_RESTRICTION_DRIVEN_FIX	false

#define ALT_SKIP_EMPTY_UNIVERSE			true // < Skip empty example
#define ALT_ALWAYS_DETERMINISTIC	    true
#define ALT_EXPLICIT_RESTRICTIONS		true // < Restrictions will be specific subautomata.

/*
 * >>> Automata stats options
 *****************************/
#define PRINT_STATS_PROJECTION			true
#define PRINT_STATS_QF_PROJECTION		false
#define PRINT_STATS_PRODUCT			    false
#define PRINT_STATS_TERNARY_PRODUCT		false
#define PRINT_STATS_NARY_PRODUCT		false
#define PRINT_STATS_NEGATION			false
#define PRINT_STATS_BASE				false
#define PRINT_STATS					    true
#define PRINT_IN_TIMBUK					true
#define PRINT_DOT_LIMIT					20

/* >>> Dumping Options <<< *
 ***************************/
#define DUMP_NO_SYMBOL_TABLE			true
#define DUMP_INTERMEDIATE_AUTOMATA		true
#define DUMP_EXAMPLES					true

/* >>> Measuring Options <<< *
 *****************************/
#define MEASURE_ATOMS					true	// < Measure the number of atomic formulae
#define MEASURE_BASE_SIZE				true	// < Measure the maximal and average size of the bases
#define MEASURE_STATE_SPACE 			true	// < Measures how many instances of terms were created
#define MEASURE_CACHE_HITS 				true	// < Prints the statistics for each cache on each node
#define MEASURE_CACHE_BUCKETS			false   // < Prints the statistics for cache buckets
#define MEASURE_CONTINUATION_CREATION	true	// < Measures how many continuations are created
#define MEASURE_CONTINUATION_EVALUATION	true	// < Measures how many continuations are actually unfolded
#define MEASURE_RESULT_HITS				true    // < Measure how many times the result hits in cache
#define MEASURE_SYMBOLS					true	// < Measure how many symbols are created
#define MEASURE_PROJECTION				true	// < Measures several things about projection (how many steps, how big, etc.)
#define MEASURE_POSTPONED				true	// < Measures how many terms are postponed and how many are processed
#define MEASURE_ALL						false   // < Measure everything, not really useful
#define MEASURE_COMPARISONS				false	// < Measure how many times we sucessfully compared and how
#define MEASURE_SUBSUMEDBY_HITS			true	// < Measure how many times subsumedBy cache worked
#define MEASURE_AUTOMATA_METRICS		true	// < Measure stuff like number of nodes and stuff in resulting automaton
#define MEASURE_SUBAUTOMATA_TIMING		false   // < Every SA will have a timer that will time how much time the isect does

/* >>> Anti-Prenexing Options <<< *
 **********************************/
#define ANTIPRENEXING_FULL              true
#define ANTIPRENEXING_DISTRIBUTIVE		false

/*
 * >>> Unique Terms options *
 ****************************/
#define UNIQUE_BASE						true
#define UNIQUE_PRODUCTS					true
#define UNIQUE_LISTS					true
#define UNIQUE_FIXPOINTS				true
#define UNIQUE_CONTINUATIONS			true

/* >>> Other Options <<< *
 *************************/
#define AUT_ALWAYS_DETERMINISTIC		false
#define AUT_ALWAYS_CONSTRAINT_FO		true
#define AUT_CONSTRUCT_BY_MONA			true

#define MONA_FAIR_MODE					false   // < No Continuations, No Early termination fo fixpoints, No QF automata
#define MIGHTY_GASTON					false   // < Collectively switch all good optimizations to achieve best performance

/* >>> Optimizations <<< *
 *************************/
#define OPT_USE_DAG							false   // < Instead of using the symbolic automata, will use the DAGified SA, there is probably issue with remapped cache
#define OPT_SHUFFLE_FORMULA					true    // < Will run ShuffleVisitor before creation of automaton, which should ease the procedure as well
#define OPT_DONT_CACHE_CONT					true	// < Do not cache terms containing continuations
#define OPT_DONT_CACHE_UNFULL_FIXPOINTS 	false	// < Do not cache fixpoints that were not fully computed
#define OPT_EQ_THROUGH_POINTERS				true    // < Test equality through pointers, not by structure
#define OPT_GENERATE_UNIQUE_TERMS			true	// < Use Workshops to generate unique pointers
// ^- NOTE! From v1.0 onwards, disable this will introduce not only leaks, but will fuck everything up!
#define OPT_USE_CUSTOM_PTR_HASH				false	// < Will use the custom implementation of hash function instead of boost::hash
#define OPT_TERM_HASH_BY_APPROX				true	// < Include stateSpaceApprox into hash (i.e. better distribution of cache)
#define OPT_SYMBOL_HASH_BY_APPROX			true    // < Will hash symbol by pointers
#define OPT_ANTIPRENEXING					true	// < Transform formula to anti-prenex form (i.e. all of the quantifiers are deepest on leaves)
#define OPT_DRAW_NEGATION_IN_BASE 			true    // < Negation is handled on formula level and not on computation level on base automata
#define OPT_CREATE_QF_AUTOMATON 			true    // < Transform quantifier-free automaton to formula
#define OPT_REDUCE_AUT_EVERYTIME			false	// (-) < Call reduce everytime VATA automaton is created (i.e. as intermediate result)
#define OPT_REDUCE_AUT_LAST					true	// < Call reduce after the final VATA automaton is created
#define OPT_EARLY_EVALUATION 				false   // < Evaluates early interesection of product
#define OPT_EARLY_PARTIAL_SUB				true    // < Postpone the partially subsumed terms
#define OPT_CONT_ONLY_WHILE_UNSAT			true    // < Generate continuation only if there wasn't found (un)satisfying (counter)example yet
#define OPT_CONT_ONLY_FOR_NONRESTRICTED		true	// < Generate continuations only for pairs that do not contain restrictions
#define OPT_PRUNE_EMPTY						true	// < Prune terms by empty set
#define OPT_REDUCE_FIXPOINT_EVERYTIME		false	// (-) < Prune the fixpoint everytime any iterator is invalidated
#define OPT_REDUCE_PREFIXPOINT				true	// < Prune the fixpoint when returning pre (i.e. fixpoint - symbol)
#define OPT_FIND_POSTPONED_CANDIDATE		true	// < Chose better candidate from list of postponed subsumption testing pairs
#define OPT_REDUCE_FULL_FIXPOINT			true	// < Prune the fixpoint by subsumption
#define OPT_CACHE_RESULTS 					true    // < Cache results of intersectnonempty(term, symbol)
#define OPT_CACHE_SUBSUMES					true    // < Cache the results of subsumption testing between terms
#define OPT_CACHE_SUBSUMED_BY				true	// < Cache the results of term subsumption by fixpoints
#define OPT_SMARTER_MONA_CONVERSION			false	// (-) < Use faster conversion from MONA to VATA (courtesy of PJ)
#define OPT_SMARTER_FLATTENING          	true
#define OPT_CREATE_TAGGED_AUTOMATA			false	// < Use tags to create a specific subformula to automaton
#define OPT_EXTRACT_MORE_AUTOMATA			true	// < Calls detagger to heuristically convert some subformulae to automata
#define OPT_UNIQUE_TRIMMED_SYMBOLS			true    // < Will guarantee that there will not be a collisions between symbols after trimming
#define OPT_UNIQUE_REMAPPED_SYMBOLS			true	// < Will guarantee that there will not be a collisions between symbols after remapping
#define OPT_FIXPOINT_BFS_SEARCH	        	false   // (-) < Will add new things to the back of the worklist in fixpoint
#define OPT_USE_DENSE_HASHMAP				false	// (-) < Will use the google::dense_hash_map as cache
#define OPT_NO_SATURATION_FOR_M2L			true    // < Will not saturate the final states for M2L(str) logic
#define OPT_SHORTTEST_FIXPOINT_SUB			false   // (-) < Will check the generators instead of of whole fixpoints
#define OPT_UNIQUE_FIXPOINTS_BY_SUB			false   // < Fixpoints will not be unique by equality but by subsumption (Fixme: This is most likely incorrect)
#define OPT_UNFOLD_FIX_DURING_SUB			false   // (0) < During the fixpoint testing if there are things in fixpoint, unfold maybe?
#define OPT_PARTIALLY_LIMITED_SUBSUMPTION	-1		// < Will limited the subsumption testing to certain depth (-1 = unlimited)
#define OPT_WORKLIST_DRIVEN_BY_RESTRICTIONS true    // < Worklist will be initialized according to the restrictions
#define OPT_THROW_CLASSIC_FIRST_ORDER_REP	true	// < Will interpret first orders on fixpoints as having only one one
#define OPT_SHUFFLE_HASHES					true    // < Shuffles the bits in the hashes
#define OPT_ENUMERATED_SUBSUMPTION_TESTING  false   // < Partially enumerates the products
#define OPT_USE_TERNARY_AUTOMATA			true    // < Will use ternary automata if possible
#define OPT_USE_NARY_AUTOMATA				false   // < Will use nary automata if possible
#define OPT_PRUNE_WORKLIST					true	// < Will remove stuff from worklist during the pruning
#define OPT_BI_AND_IMPLICATION_SUPPORT		true	// < Will not restrict the syntax, by removing => and <=>

/* >>> Static Assertions <<< *
 *****************************/
static_assert(!(MONA_FAIR_MODE == true && MIGHTY_GASTON == true), "Gaston cannot be might and fair at the same time!");
static_assert(sizeof(size_t) == 8, "Shuffling of hashes require 64bit architecture");
#endif

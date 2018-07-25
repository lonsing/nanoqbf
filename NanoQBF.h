//
// Created by vedad on 16/06/18.
//

#ifndef NANOQBF_NANOQBF_H
#define NANOQBF_NANOQBF_H

#include <unordered_map>
#include <unordered_set>
#include "SatSolver.h"
#include "types/Assignment.h"
#include "types/Formula.h"
#include "hashes.h"
#include "Options.h"
#include "auxutils.h"

typedef std::unordered_set<Assignment*, std::hash<Assignment*>, CompAssignment> AssignmentSet;
typedef std::unordered_map<Assignment*, Var, std::hash<Assignment*>, CompAssignment> AssignmentVarMap;

/// The main QBF solver class
class NanoQBF
{
public:
  /// NanoQBF Constructor
  /** Constructs a QBF solver object for the given formula and options.
   * @param formula Input QBF Formula
   * @param options Options for fine tuning the solver
   */
  NanoQBF(const Formula* formula, const Options* options);
  
  /// NanoQBF Destructor
  ~NanoQBF();
  
  /// Main QBF solving method
  /** Decides the given formula and returns the appropriate result.
   * @return 10 : #formula_ is TRUE \n
   *         20 : #formula_ is FALSE \n
   *         otherwise undefined
   */
  int solve();
private:
  const Formula* formula_; ///< QBF Formula which should be solved
  const Options* options_; ///< Options fine tuning the behavior of the solver
  
  SatSolver solver_a_; ///< SatSolver used for solving the universal expansion
  SatSolver solver_b_; ///< SatSolver used for solving the existential expansion
  
  unsigned iteration_; ///< Iteration counter used for logging purposes
  
  AssignmentSet subformula_solutions_a_; ///< Set of assignments used for expansions in #solver_b_
  AssignmentSet subformula_solutions_b_; ///< Set of assignments used for expansions in #solver_a_
  
  AssignmentVarMap vars_a_; ///< Map from assignments in #solver_b_ to variables in #solver_a_
  AssignmentVarMap vars_b_; ///< Map from assignments in #solver_a_ to variables in #solver_b_
  
  std::vector<std::vector<Var>> subformula_vars_a_; ///< Variables used in each sub-formula of #solver_a_
  std::vector<std::vector<Var>> subformula_vars_b_; ///< Variables used in each sub-formula of #solver_b_
  
  std::vector<Assignment*> subformula_exps_a_; ///< Vector of assignments that expanded a sub-formula in #solver_a_
  std::vector<Assignment*> subformula_exps_b_; ///< Vector of assignments that expanded a sub-formula in #solver_b_
  
  /// Initialises the SAT formula in #solver_a_
  /** Initialises a sat solver with the propositional part of #formula_ and takes the solution for initialising
   * #subformula_solutions_b_ and #solver_a_. This is done in a lazy way until the number of sub-formulas specified in
   * #options_ is reached. It can also happen that the truth value of #formula_ is decided prematurely, and the
   * appropriate value is returned. Phase based initialisation is also used, as it seems to improve the proof search
   * strategy.
   * @return 10 : #formula_ is TRUE \n
   *         20 : #formula_ is FALSE \n
   *         otherwise undefined
   */
  int initA();
  
  /// Prunes parts of formula from #solver_a_
  /** The data in #solver_a_, #vars_a_, #subformula_vars_a_, and #subformula_solutions_b_ is cleared. It is then
   * reconstructed from #solver_b_ at the next call to completeA().
   * @see pruneCheckA()
   */
  inline void pruneA();
  
  /// Prunes parts of formula from #solver_b_
  /** The data in #solver_b_, #vars_b_, #subformula_vars_b_, and #subformula_solutions_a_ is cleared. It is then
   * reconstructed from #solver_a_ at the next call to completeB().
   * @see pruneCheckB()
   */
  inline void pruneB();
  
  /// Checks whether the prune strategy specified in #options_ is met, and calls pruneA()
  inline void pruneCheckA();
  
  /// Checks whether the prune strategy specified in #options_ is met, and calls pruneB()
  inline void pruneCheckB();
  
  /// Completes the formula in #solver_a_ with the solution to #solver_b_
  /** Goes through every sub-formula in #solver_b_ and extracts an assignment, and checks whether that assignment was
   * found before. If not, it is used for extending the formula in #solver_a_ with a call to extendA(Assignment*).
   * @see extendA(Assignment*)
   */
  void completeA();
  
  /// Completes the formula in #solver_b_ with the solution to #solver_a_
  /** Goes through every sub-formula in #solver_a_ and extracts an assignment, and checks whether that assignment was
   * found before. If not, it is used for extending the formula in #solver_b_ with a call to extendB(Assignment*).
   * @see extendB(Assignment*)
   */
  void completeB();
  
  std::vector<Lit> complete_values; ///< Temporary value vector used in completeA() and completeB()
  
  /// Extends the formula in #solver_a_ by the given \a assignment
  /** Goes through the increasing sub-assignments of \a assignment according to the quantifiers, and checks whether
   * there already exist variables in #solver_a_ representing those sub-assignments. If yes, the variables are reused,
   * otherwise, new variables are introduced and stored in #vars_a_. All variables that will appear in this new
   * sub-formula of #solver_a_ are stored in #subformula_vars_a_. After that, all clauses in #formula_ are iterated,
   * it is checked whether they are satisfied by \a assignment. If not, the variables in the clause are replaced and
   * the new clause gets pushed into #solver_a_.
   * @param assignment An assignment to universal variables in #formula_
   */
  void extendA(Assignment* assignment);
  
  /// Extends the formula in #solver_b_ by the given \a assignment
  /** Goes through the increasing sub-assignments of \a assignment according to the quantifiers, and checks whether
   * there already exist variables in #solver_b_ representing those sub-assignments. If yes, the variables are reused,
   * otherwise, new variables are introduced and stored in #vars_b_. All variables that will appear in this new
   * sub-formula of #solver_b_ are stored in #subformula_vars_b_. After that, all clauses in #formula_ are iterated,
   * it is checked whether they are satisfied by \a assignment. If not, the variables in the clause are replaced and
   * the tseitin transformation of the new cube gets pushed into #solver_b_.
   * @param assignment An assignment to existential variables in #formula_
   */
  void extendB(Assignment* assignment);
  
  std::vector<Lit> extend_clause; ///< Temporary clause used in extendA(Assignment*) and extendB(Assignment*)
};

////// INLINE FUNCTION DEFINITIONS //////
void NanoQBF::pruneA()
{
  printf("c Pruning formula A\n");
  for(const auto & av : vars_a_)
    Assignment::destroy_assignment(av.first);
  vars_a_.clear();
  subformula_vars_a_.clear();
  subformula_solutions_b_.clear();
  subformula_exps_a_.clear();
  solver_a_.reset();
  const Quant* q = formula_->getQuant(0);
  if(q->type == QuantType::EXISTS)
    solver_a_.reserveVars(q->size);
}

void NanoQBF::pruneB()
{
  printf("c Pruning formula B\n");
  for(const auto & av : vars_b_)
    Assignment::destroy_assignment(av.first);
  vars_b_.clear();
  subformula_vars_b_.clear();
  subformula_solutions_a_.clear();
  subformula_exps_b_.clear();
  solver_b_.reset();
  const Quant* q = formula_->getQuant(0);
  if(q->type == QuantType::FORALL)
    solver_b_.reserveVars(q->size);
}

void NanoQBF::pruneCheckA()
{
  bool prune_periodic = (options_->pruning_a == Options::PruningMode::PERIODIC &&
                         iteration_ % options_->pparams_a.period == 0);
  bool prune_dynamic = (options_->pruning_a == Options::PruningMode::DYNAMIC &&
                        read_mem_usage() > options_->memory_limit * 0.8);
  
  if(prune_periodic || prune_dynamic) pruneA();
}

void NanoQBF::pruneCheckB()
{
  bool prune_periodic = (options_->pruning_b == Options::PruningMode::PERIODIC &&
                         iteration_ % options_->pparams_b.period == 0);
  bool prune_dynamic = (options_->pruning_b == Options::PruningMode::DYNAMIC &&
                        read_mem_usage() > options_->memory_limit * 0.8);
  
  if(prune_periodic || prune_dynamic) pruneB();
}

#endif // NANOQBF_NANOQBF_H

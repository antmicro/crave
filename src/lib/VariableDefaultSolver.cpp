#include "../crave/backend/VariableDefaultSolver.hpp"

#include <algorithm>
#include <functional>
#include <memory>
#include <set>

#include "../crave/utils/Logging.hpp"

namespace crave {

extern std::function<unsigned(unsigned)> random_unsigned;

bool VariableDefaultSolver::bypass_constraint_analysis = false;

unsigned VariableDefaultSolver::complexity_limit_for_bdd = 400;

VariableDefaultSolver::VariableDefaultSolver(const VariableContainer& vcon, const ConstraintPartition& cp)
    : VariableSolver(vcon, cp) {
  LOG(INFO) << "Create solver for partition " << constr_pttn_;

  for (ConstraintPtr c : constr_pttn_) {
    if (c->isCover()) continue;  // default solver ignores cover constraints
    if (c->isSoft()) {
      solver_->makeSoftAssertion(*c->expr());
    } else {
      solver_->makeAssertion(*c->expr());
    }
  }

  if (bypass_constraint_analysis) return;

  analyseConstraints();

  std::set<int> vars_with_dist;
  for (VariableContainer::ReadRefPair& pair : var_ctn_.dist_references) {
    assert(var_ctn_.dist_ref_to_var_map.find(pair.first) != var_ctn_.dist_ref_to_var_map.end());
    vars_with_dist.insert(var_ctn_.dist_ref_to_var_map.at(pair.first));
  }

  if (FactorySolver<CUDD>::isDefined()) {
    LOG(INFO) << "Create BDD solvers for constraints involving single variable";
    std::map<int, ConstraintList> const& svc_map = constr_pttn_.singleVariableConstraintMap();
    for (VariableContainer::WriteRefPair& pair : var_ctn_.write_references) {
      int id = pair.first;
      if (svc_map.find(id) == svc_map.end()) continue;
      if (vars_with_dist.find(id) != vars_with_dist.end()) {
        LOG(INFO) << "  Skip var #" << id << " due to existing distribution constraints";
        continue;
      }
      SolverPtr bdd_solver(FactoryMetaSMT::getNewInstance(CUDD));
      bdd_solvers_[id] = bdd_solver;
      for (ConstraintPtr c : svc_map.at(id)) {
        if (c->complexity() > 0 && c->complexity() < complexity_limit_for_bdd) bdd_solver->makeAssertion(*c->expr());
      }
      vars_with_dist.insert(id);
      LOG(INFO) << "  BDD solver for var #" << id << " created";
    }
  }

  if (2 * vars_with_dist.size() > var_ctn_.write_references.size()) return;  // not necessary to randomize more

  for (VariableContainer::WriteRefPair& pair : var_ctn_.write_references) {
    if (vars_with_dist.find(pair.first) == vars_with_dist.end()) random_write_refs_.push_back(pair);
  }
}

void VariableDefaultSolver::analyseConstraints() {
  analyseHards();
  if (contradictions_.empty()) {
    analyseSofts();
    LOG(INFO) << "Partition is solvable with " << inactive_softs_.size() << " soft constraint(s) deactivated:";

    for (std::string& s : inactive_softs_) {
      LOG(INFO) << " " << s;
    }
  } else {
    LOG(INFO) << "Partition has unsatisfiable hard constraints:";
    unsigned cnt = 0;

    for (std::vector<std::string>& vs : contradictions_) {
      LOG(INFO) << "  set #" << ++cnt;

      for (std::string& s : vs) {
        LOG(INFO) << "   " << s;
      }
    }
  }
}

bool VariableDefaultSolver::solve() {
  LOG(INFO) << "Solve constraints in partition " << constr_pttn_;
  if (!contradictions_.empty()) {
    LOG(INFO) << "Failed because partition has been analyzed to be unsolvable";
    return false;
  }
  for (VariableContainer::WriteRefPair& pair : var_ctn_.write_references) {
    int id = pair.first;
    if (bdd_solvers_.find(id) == bdd_solvers_.end()) continue;
    SolverPtr bdd_solver = bdd_solvers_[id];
    CHECK(bdd_solver->solve());  // otherwise, contradiction must have been found!
    std::string str;
    bdd_solver->read(*var_ctn_.variables[id], str);
    NodePtr value(new Constant(pair.second->to_constant(str)));
    NodePtr var = var_ctn_.variables[id];
    NodePtr eq(new EqualOpr(var, value));
    solver_->makeSuggestion(*eq);
  }

  for (VariableContainer::ReadRefPair& pair : var_ctn_.read_references) {
    solver_->makeAssumption(*pair.second->expr());
  }

  for (VariableContainer::ReadRefPair& pair : var_ctn_.dist_references) {
    solver_->makeSuggestion(*pair.second->expr());
  }

  if (!random_write_refs_.empty()) {
    std::random_shuffle(random_write_refs_.begin(), random_write_refs_.end(), crave::random_unsigned);
    for (unsigned i = 0; i < (random_write_refs_.size() + 1) / 2; i++) {
      NodePtr value(new Constant(
          random_write_refs_[i].second->value_as_constant()));  // reuse the random value generated by next()
      NodePtr var = var_ctn_.variables[random_write_refs_[i].first];
      NodePtr eq(new EqualOpr(var, value));
      solver_->makeSuggestion(*eq);
    }
  }

  if (solver_->solve()) {
    for (VariableContainer::WriteRefPair& pair : var_ctn_.write_references) {
      solver_->read(*var_ctn_.variables[pair.first], *pair.second);
    }
    LOG(INFO) << "Done solving partition " << constr_pttn_;
    return true;
  }
  LOG(INFO) << "Failed due to conflict with read references";
  return false;
}

void VariableDefaultSolver::analyseHards() {
  std::unique_ptr<metaSMTVisitor> solver(FactoryMetaSMT::getNewInstance());

  std::map<unsigned int, NodePtr> s;
  std::vector<std::string> out;
  std::vector<std::vector<unsigned int> > results;

  for (ConstraintPtr c : constr_pttn_) {
    if (!c->isSoft() && !c->isCover()) {
      s.insert(std::make_pair(s.size(), c->expr()));
      out.push_back(c->name());
    }
  }
  results = solver->analyseContradiction(s);

  for (std::vector<unsigned int> result : results) {
    std::vector<std::string> vec;

    for (unsigned int i : result) {
      vec.push_back(out[i]);
    }
    contradictions_.push_back(vec);
  }
}

void VariableDefaultSolver::analyseSofts() {
  std::vector<unsigned int> result = solver_->analyseSofts();
  std::vector<unsigned int>::iterator ite = result.begin();
  unsigned cnt = 0;

  for (ConstraintPtr c : constr_pttn_) {
    if (ite == result.end()) break;
    if (c->isSoft()) {
      if (*ite == cnt) {
        ite++;
        inactive_softs_.push_back(c->name());
      }
      cnt++;
    }
  }
}
}  // namespace crave

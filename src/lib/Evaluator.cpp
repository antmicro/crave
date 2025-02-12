#include "../crave/utils/Evaluator.hpp"

namespace crave {

template <>
bool get_result(EvalVisitor const& v) {
  return v.result().value() != 0;
}

void Evaluator::assign(unsigned id, Constant c) { assignments_[id] = c; }

}  // namespace crave

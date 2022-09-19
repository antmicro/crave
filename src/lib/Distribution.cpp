#include "../crave/frontend/Distribution.hpp"

#include <random>

namespace crave {
distribution<bool>::distribution(const double prob) : prob_(prob) {}

distribution_tag<bool> distribution<bool>::create(const double prob) {
  return distribution_tag<bool>(distribution<bool>(prob));
}

bool distribution<bool>::nextValue() const {
  std::bernoulli_distribution dist(prob_);
  return dist(*rng.get());
}

std::vector<weighted_range<bool> > const& distribution<bool>::ranges() const {
  static std::vector<weighted_range<bool> > v;
  return v;
}
}  // namespace crave

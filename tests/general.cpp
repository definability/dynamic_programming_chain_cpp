#include "doctest.h"

#include <tuple>

import dynamic_programming_problems;
import operators;
import simple_semiring;

using OrAndSemiring = SimpleSemiring<bool, LogicalOr, LogicalAnd>;
using MaxMinSemiring = SimpleSemiring<float, Max<float>, Min<float>>;
using MaxProdSemiring = SimpleSemiring<short, Max<short>, Product<short>>;

#define ENUMERATE_SEMIRINGS(DO) \
  DO(OrAndSemiring, bool),      \
  DO(MaxMinSemiring, float),    \
  DO(MaxProdSemiring, short)

#define APPEND_ALGORITHMS(...) \
  std::tuple<__VA_ARGS__, AlgorithmTypes::Compact>, \
  std::tuple<__VA_ARGS__, AlgorithmTypes::Fast>

#define TYPES ENUMERATE_SEMIRINGS(APPEND_ALGORITHMS)

TEST_CASE_TEMPLATE("or-and: two nodes and two labels", TypesTuple, TYPES)
{
  using CurrentSemiring = std::tuple_element_t<0, TypesTuple>;
  using R = std::tuple_element_t<1, TypesTuple>;
  using Algorithm = std::tuple_element_t<2, TypesTuple>;
  DynamicProgrammingProblem<Algorithm, int, int, R, CurrentSemiring, 2, 2> problem;
  problem.get_vertices()[0u, 0u] = false;
  problem.get_vertices()[0u, 1u] = true;
  problem.get_vertices()[1u, 0u] = true;
  problem.get_vertices()[1u, 1u] = true;
  problem.get_edges()[0u, 0u, 0u] = true;
  problem.get_edges()[0u, 0u, 1u] = true;
  problem.get_edges()[0u, 1u, 0u] = true;
  problem.get_edges()[0u, 1u, 1u] = false;
  problem.solve();

  CHECK_EQ(problem.cost, true);
  CHECK_EQ(problem.get_vertices()[0u, 0u], false);
  CHECK_EQ(problem.get_vertices()[0u, 1u], true);
  CHECK_EQ(problem.get_vertices()[1u, 0u], true);
  CHECK_EQ(problem.get_vertices()[1u, 1u], true);
  CHECK_EQ(problem.get_labelling()[0u], 1);
  CHECK_EQ(problem.get_labelling()[1u], 0);
}
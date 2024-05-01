#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <tuple>

import dynamic_programming_problems;
import operators;
import simple_semiring;

template<typename R> using MinPlusSemiring = SimpleSemiring<R, Min<R>, Sum<R>>;

#define ENUMERATE_PENALTY_TYPES(DO) \
  DO(signed char),                  \
  DO(signed short),                 \
  DO(signed int),                   \
  DO(signed long),                  \
  DO(float),                        \
  DO(double)

#define APPEND_INDEX_TYPES(...) \
  std::tuple<__VA_ARGS__, unsigned int, signed int>, \
  std::tuple<__VA_ARGS__, signed int, signed int>,   \
  std::tuple<__VA_ARGS__, unsigned int, unsigned int>, \
  std::tuple<__VA_ARGS__, signed int, unsigned int>

#define ENUMERATE_ALGORITHMS(T) \
  APPEND_INDEX_TYPES(T, AlgorithmTypes::Compact), \
  APPEND_INDEX_TYPES(T, AlgorithmTypes::Fast)

#define TYPES ENUMERATE_PENALTY_TYPES(ENUMERATE_ALGORITHMS)

template<typename TypesTuple, std::size_t nodes, std::size_t labels>
struct ProblemFromTupleHelper
{
  using R = std::tuple_element_t<0, TypesTuple>;
  using Algorithm = std::tuple_element_t<1, TypesTuple>;
  using NodeIndex = std::tuple_element_t<2, TypesTuple>;
  using LabelIndex = std::tuple_element_t<3, TypesTuple>;

  static_assert(
    std::numeric_limits<R>::is_iec559 or not std::is_floating_point_v<R>);

  using Problem = DynamicProgrammingProblem<
    Algorithm,
    NodeIndex,
    LabelIndex,
    R,
    MinPlusSemiring<R>,
    nodes,
    labels
  >;
};

template<typename TypesTuple, std::size_t nodes, std::size_t labels>
using ProblemFromTuple =
  ProblemFromTupleHelper<TypesTuple, nodes, labels>::Problem;

TEST_CASE_TEMPLATE("one node and one label", TypesTuple, TYPES)
{
  ProblemFromTuple<TypesTuple, 1, 1> problem;
  SUBCASE("zero cost")
  {
    problem.get_vertices()[0u, 0u] = 0;
    problem.solve();

    CHECK_EQ(problem.cost, 0);
    CHECK_EQ(problem.get_vertices()[0u, 0u], 0);
    CHECK_EQ(problem.get_labelling()[0u], 0);
  }
  SUBCASE("positive cost")
  {
    problem.get_vertices()[0u, 0u] = 1;
    problem.solve();

    CHECK_EQ(problem.cost, 1);
    CHECK_EQ(problem.get_vertices()[0u, 0u], 1);
    CHECK_EQ(problem.get_labelling()[0u], 0);
  }
  SUBCASE("negative cost")
  {
    problem.get_vertices()[0u, 0u] = -1;
    problem.solve();

    CHECK_EQ(problem.cost, -1);
    CHECK_EQ(problem.get_vertices()[0u, 0u], -1);
    CHECK_EQ(problem.get_labelling()[0u], 0);
  }
}

TEST_CASE_TEMPLATE("one node and two labels", TypesTuple, TYPES)
{
  ProblemFromTuple<TypesTuple, 1, 2> problem;
  SUBCASE("the first label is the best")
  {
    problem.get_vertices()[0u, 0u] = 0;
    problem.get_vertices()[0u, 1u] = 1;
    problem.solve();

    CHECK_EQ(problem.cost, 0);
    CHECK_EQ(problem.get_vertices()[0u, 0u], 0);
    CHECK_EQ(problem.get_vertices()[0u, 1u], 1);
    CHECK_EQ(problem.get_labelling()[0u], 0);
  }
  SUBCASE("the second label is the best")
  {
    problem.get_vertices()[0u, 0u] = 2;
    problem.get_vertices()[0u, 1u] = 1;
    problem.solve();

    CHECK_EQ(problem.cost, 1);
    CHECK_EQ(problem.get_vertices()[0u, 0u], 2);
    CHECK_EQ(problem.get_vertices()[0u, 1u], 1);
    CHECK_EQ(problem.get_labelling()[0u], 1);
  }
}

TEST_CASE_TEMPLATE("two nodes and one label", TypesTuple, TYPES)
{
  ProblemFromTuple<TypesTuple, 2, 1> problem;
  problem.get_vertices()[0u, 0u] = 0;
  problem.get_vertices()[1u, 0u] = 0;
  problem.get_edges()[0u, 0u, 0u] = 0;
  problem.solve();

  CHECK_EQ(problem.cost, 0);
  CHECK_EQ(problem.get_vertices()[0u, 0u], 0);
  CHECK_EQ(problem.get_vertices()[1u, 0u], 0);
  CHECK_EQ(problem.get_labelling()[0u], 0);
  CHECK_EQ(problem.get_labelling()[1u], 0);
}

TEST_CASE_TEMPLATE("two nodes and two labels", TypesTuple, TYPES)
{
  ProblemFromTuple<TypesTuple, 2, 2> problem;
  SUBCASE("zero labels are processed right")
  {
    problem.get_vertices()[0u, 0u] = 0;
    problem.get_vertices()[0u, 1u] = 1;
    problem.get_vertices()[1u, 0u] = 0;
    problem.get_vertices()[1u, 1u] = 1;
    problem.get_edges()[0u, 0u, 0u] = 0;
    problem.get_edges()[0u, 0u, 1u] = 0;
    problem.get_edges()[0u, 1u, 0u] = 0;
    problem.get_edges()[0u, 1u, 1u] = 0;
    problem.solve();

    CHECK_EQ(problem.cost, 0);
    CHECK_EQ(problem.get_vertices()[0u, 0u], 0);
    CHECK_EQ(problem.get_vertices()[0u, 1u], 1);
    CHECK_EQ(problem.get_vertices()[1u, 0u], 0);
    CHECK_EQ(problem.get_vertices()[1u, 1u], 1);
    CHECK_EQ(problem.get_labelling()[0u], 0);
    CHECK_EQ(problem.get_labelling()[1u], 0);
  }
  SUBCASE("costs of problem.get_edges() are equal")
  {
    problem.get_vertices()[0u, 0u] = 1;
    problem.get_vertices()[0u, 1u] = 2;
    problem.get_vertices()[1u, 0u] = 20;
    problem.get_vertices()[1u, 1u] = 10;
    problem.get_edges()[0u, 0u, 0u] = -10;
    problem.get_edges()[0u, 0u, 1u] = -10;
    problem.get_edges()[0u, 1u, 0u] = -10;
    problem.get_edges()[0u, 1u, 1u] = -10;
    problem.solve();

    CHECK_EQ(problem.cost, 1);
    CHECK_EQ(problem.get_vertices()[0u, 0u], 1);
    CHECK_EQ(problem.get_vertices()[0u, 1u], 2);
    CHECK_EQ(problem.get_vertices()[1u, 0u], 20);
    CHECK_EQ(problem.get_vertices()[1u, 1u], 10);
    CHECK_EQ(problem.get_labelling()[0u], 0);
    CHECK_EQ(problem.get_labelling()[1u], 1);
  }
  SUBCASE("costs in the first node are equal")
  {
    problem.get_vertices()[0u, 0u] = 1;
    problem.get_vertices()[0u, 1u] = 1;
    problem.get_vertices()[1u, 0u] = 10;
    problem.get_vertices()[1u, 1u] = 20;
    problem.get_edges()[0u, 0u, 0u] = 0;
    problem.get_edges()[0u, 0u, 1u] = 0;
    problem.get_edges()[0u, 1u, 0u] = -10;
    problem.get_edges()[0u, 1u, 1u] = -10;
    problem.solve();

    CHECK_EQ(problem.cost, 1);
    CHECK_EQ(problem.get_vertices()[0u, 0u], 11);
    CHECK_EQ(problem.get_vertices()[0u, 1u], 1);
    CHECK_EQ(problem.get_vertices()[1u, 0u], 10);
    CHECK_EQ(problem.get_vertices()[1u, 1u], 20);
    CHECK_EQ(problem.get_labelling()[0u], 1);
    CHECK_EQ(problem.get_labelling()[1u], 0);
  }
  SUBCASE("problem.get_edges() and nodes affect the resulting problem.get_labelling()")
  {
    problem.get_vertices()[0u, 0u] = 1;
    problem.get_vertices()[0u, 1u] = 2;
    problem.get_vertices()[1u, 0u] = 20;
    problem.get_vertices()[1u, 1u] = 10;
    problem.get_edges()[0u, 0u, 0u] = -10;
    problem.get_edges()[0u, 0u, 1u] = 0;
    problem.get_edges()[0u, 1u, 0u] = -10;
    problem.get_edges()[0u, 1u, 1u] = -10;
    problem.solve();

    CHECK_EQ(problem.cost, 2);
    CHECK_EQ(problem.get_vertices()[0u, 0u], 11);
    CHECK_EQ(problem.get_vertices()[0u, 1u], 2);
    CHECK_EQ(problem.get_vertices()[1u, 0u], 20);
    CHECK_EQ(problem.get_vertices()[1u, 1u], 10);
    CHECK_EQ(problem.get_labelling()[0u], 1);
    CHECK_EQ(problem.get_labelling()[1u], 1);
  }
}

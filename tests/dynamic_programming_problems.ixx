module;

#include <array>
#include <concepts>
#include <mdspan>

export module dynamic_programming_problems;

import dynamic_programming;

#define ENUMERATE_TEMPLATE_PARAMETERS(DO) \
  DO(std::integral, NodeIndex),           \
  DO(std::integral, LabelIndex),          \
  DO(std::equality_comparable, R),        \
  DO(Semiring<R>, ProblemSemiring),       \
  DO(std::size_t, nodes),                 \
  DO(std::size_t, labels),                \
  DO(std::size_t, edges_rank)

#define TEMPLATE_PARAMETER(Type, value) Type value
#define TEMPLATE_ARGUMENT(Type, value) value

#define TEMPLATE_PARAMETER_LIST ENUMERATE_TEMPLATE_PARAMETERS(TEMPLATE_PARAMETER)
#define TEMPLATE_ARGUMENTS ENUMERATE_TEMPLATE_PARAMETERS(TEMPLATE_ARGUMENT)

template<TEMPLATE_PARAMETER_LIST>
requires(nodes > 0 and labels > 0 and (edges_rank == 2 or edges_rank == 3))
class DynamicProgrammingProblemBase;

export template<TEMPLATE_PARAMETER_LIST>
class DynamicProgrammingProblemFast;

export template<TEMPLATE_PARAMETER_LIST>
class DynamicProgrammingProblemCompact;

template<typename Type>
struct ProblemChooser
{
  static_assert(false, "Unsupported algorithm type");
};

export struct AlgorithmTypes
{
  struct Fast {};
  struct Compact {};
};

export template<typename Type, TEMPLATE_PARAMETER_LIST>
using DynamicProgrammingProblem =
  typename ProblemChooser<Type>::template type<TEMPLATE_ARGUMENTS>;

template<TEMPLATE_PARAMETER_LIST>
class DynamicProgrammingProblemCompact
  : public DynamicProgrammingProblemBase<TEMPLATE_ARGUMENTS>
{
public:
  void solve()
  {
    dynamic_programming_compact(
      ProblemSemiring{}, this->get_vertices(), this->get_edges(),
      this->get_labelling(),
      this->cost
    );
  }
};

template<TEMPLATE_PARAMETER_LIST>
class DynamicProgrammingProblemFast
  : public DynamicProgrammingProblemBase<TEMPLATE_ARGUMENTS>
{
private:
  using LocalBestLabelsExtents = std::extents<NodeIndex, nodes - 1, labels>;
  using LocalBestLabelsArray = std::array<LabelIndex, (nodes - 1) * labels>;
  using ProblemLocalBestLabels =
    LocalBestLabels<LabelIndex, LocalBestLabelsExtents>;
public:
  void solve()
  {
    dynamic_programming_fast(
      ProblemSemiring{}, this->get_vertices(), this->get_edges(),
      this->get_labelling(),
      this->cost, this->get_local_best_labels()
    );
  }

  ProblemLocalBestLabels get_local_best_labels()
  {
    return ProblemLocalBestLabels{local_best_labels_array.data()};
  }
private:
  LocalBestLabelsArray local_best_labels_array;
};

template<TEMPLATE_PARAMETER_LIST>
requires(nodes > 0 and labels > 0 and (edges_rank == 2 or edges_rank == 3))
class DynamicProgrammingProblemBase
{
private:
  using VerticesExtents = std::extents<NodeIndex, nodes, labels>;
  using EdgesExtents = std::conditional_t<
    edges_rank == 2,
    std::extents<NodeIndex, labels, labels>,
    std::extents<NodeIndex, nodes - 1, labels, labels>
  >;
public:
  Vertices<R, VerticesExtents> get_vertices()
  {
    return Vertices<R, VerticesExtents>{vertices_array.data()};
  }
  Edges<R, EdgesExtents> get_edges()
  {
    return Edges<R, EdgesExtents>{edges_array.data()};
  }
  Labelling<LabelIndex, nodes> get_labelling()
  {
    return Labelling<LabelIndex, nodes>{labelling_array.data(), nodes};
  }
  R cost;
protected:
  DynamicProgrammingProblemBase() = default;
  ~DynamicProgrammingProblemBase() = default;

  std::array<R, nodes * labels> vertices_array;
  std::array<R, (nodes - 1) * labels * labels> edges_array;
  std::array<LabelIndex, nodes> labelling_array;
};

template<>
struct ProblemChooser<AlgorithmTypes::Fast>
{
  template<TEMPLATE_PARAMETER_LIST>
  using type = DynamicProgrammingProblemFast<TEMPLATE_ARGUMENTS>;
};

template<>
struct ProblemChooser<AlgorithmTypes::Compact>
{
  template<TEMPLATE_PARAMETER_LIST>
  using type = DynamicProgrammingProblemCompact<TEMPLATE_ARGUMENTS>;
};
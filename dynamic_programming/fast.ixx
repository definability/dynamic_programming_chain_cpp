/// @brief The module contains an implementation of
/// a time-efficient dynamic programming algorithm on an arbitrary semiring.
module;

#include <concepts>
#include <mdspan>
#include <numeric>
#include <span>

export module dynamic_programming:fast;

import :traits;
import :helpers;

export template<typename Label, typename Extents>
  requires(Extents::rank() == 2) using LocalBestLabels = std::mdspan<Label, Extents>;

void calculate_labelling(auto&& labelling, auto&& local_best_labels)
{
  for (auto node_index = 0zu; node_index + 1 < labelling.size(); ++node_index)
  {
    labelling[node_index +1] = local_best_labels[node_index, labelling[node_index]];
  }
}

export template<
  std::equality_comparable R, Semiring <R> SpecificSemiring,
  typename VerticesExtents,
  typename EdgesExtents,
  typename Label, std::size_t labelling_extent,
  typename LocalBestLabelsExtents
>
void dynamic_programming_fast(
  SpecificSemiring semiring,
  const Vertices <R, VerticesExtents>& vertices,
  const Edges <R, EdgesExtents>& edges,
  const Labelling <Label, labelling_extent>& labelling,
  R& cost,
  LocalBestLabels<Label, LocalBestLabelsExtents>& local_best_labels
)
{
  validate_input(vertices, edges, labelling);
  if (local_best_labels.extent(0) + 1 != vertices.extent(0))
  {
    throw std::invalid_argument{
      "Local best labels can be defined only for N-1 nodes"};
  }
  if (local_best_labels.extent(1) != vertices.extent(1))
  {
    throw std::invalid_argument{
      "Vertices and local best labels must have the same number of labels"};
  }
  cost = calculate_cost<Label>(semiring, vertices, edges, local_best_labels, labelling);
  calculate_labelling(labelling, local_best_labels);
}

template<typename Label>
auto calculate_cost(
  auto&& semiring,
  auto&& vertices,
  auto&& edges,
  auto&& local_best_labels,
  auto&& labelling)
{
  for (auto next_node{vertices.extent(0) - 1}; next_node > 0; --next_node)
  {
    auto&& next_node_vertices = get_slice(vertices, next_node);

    auto&& current_node = next_node - 1;
    auto&& current_node_vertices = get_slice(vertices, current_node);

    for (auto current_node_label = 0zu;
         current_node_label < vertices.extent(1); ++current_node_label)
    {
      auto&& current_label_edges = get_slice(
        edges, current_node, current_node_label
      );
      Label next_node_label{0};
      [[assume(current_label_edges.begin() != current_label_edges.end())]]
      auto&& best_result = semiring.multiplexer(
        current_label_edges[next_node_label],
        next_node_vertices[next_node_label]
      );
      local_best_labels[current_node, current_node_label] = next_node_label;
      while (++next_node_label < vertices.extent(1))
      {
        auto&& value = semiring.adder(
          best_result, semiring.multiplexer(
            current_label_edges[next_node_label],
            next_node_vertices[next_node_label]
          ));
        if (value != best_result)
        {
          local_best_labels[current_node, current_node_label] = next_node_label;
          best_result = std::move(value);
        }
      }
      semiring.multiplexer.inplace(
        current_node_vertices[current_node_label], std::move(best_result));
    }
  }

  auto&& next_node_vertices = get_slice(vertices, 0);
  Label label{0};
  [[assume(next_node_vertices.begin() != next_node_vertices.end())]]
  auto best_result = next_node_vertices[label];
  labelling[0] = label;
  while (++label < vertices.extent(1))
  {
    auto&& value = semiring.adder(best_result, next_node_vertices[label]);
    if (value != best_result)
    {
      labelling[0] = label;
      best_result = std::move(value);
    }
  }
  return best_result;
}

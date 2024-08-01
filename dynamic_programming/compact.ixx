/// @brief The module contains an implementation of
/// a memory-efficient dynamic programming algorithm on an arbitrary semiring.
module;

#include <numeric>
#include <span>

export module dynamic_programming:compact;

import :traits;
import :helpers;

export template<
  std::equality_comparable R,
  Semiring<R> SpecificSemiring,
  typename VerticesExtents,
  typename EdgesExtents,
  typename Label,
  std::size_t labelling_extent
>
void dynamic_programming_compact(
  SpecificSemiring semiring,
  const Vertices<R, VerticesExtents>& vertices,
  const Edges<R, EdgesExtents>& edges,
  const Labelling<Label, labelling_extent>& labelling,
  R& labelling_cost)
{
  validate_input(vertices, edges, labelling);
  labelling_cost = calculate_cost(semiring, vertices, edges);
  calculate_labelling(semiring, vertices, edges, labelling, labelling_cost);
}

auto calculate_cost(auto&& semiring, auto&& vertices, auto&& edges)
{
  using Size = typename std::decay_t<decltype(vertices)>::extents_type::size_type;

  [[assume(vertices.extent(0) >= 1)]]
  auto next_node = static_cast<Size>(vertices.extent(0)) - Size{1};
  auto&& next_node_vertices = get_slice(vertices, next_node);
  for (; next_node > 0; --next_node)
  {
    [[assume(next_node >= 1)]]
    const auto current_node = next_node - Size{1};
    auto&& current_node_vertices = get_slice(vertices, current_node);

    for (auto label = Size{0}; label < static_cast<Size>(vertices.extent(1)); ++label)
    {
      auto&& current_label_edges = get_edge_slice(edges, current_node, label);
      auto&& inner_product_result = inner_product(
        semiring,
        current_label_edges,
        next_node_vertices
      );
      semiring.multiplexer.inplace(
        current_node_vertices[label],
        std::move(inner_product_result));
    }
    next_node_vertices = current_node_vertices;
  }

  [[assume(next_node_vertices.begin() != next_node_vertices.end())]]
  return std::reduce(
    ++next_node_vertices.begin(),
    next_node_vertices.end(),
    *next_node_vertices.begin(),
    semiring.adder
  );
}

void calculate_labelling(
  auto&& semiring,
  auto&& vertices,
  auto&& edges,
  auto&& labelling,
  auto&& cost)
{
  using Size = typename std::decay_t<decltype(vertices)>::extents_type::size_type;

  {
    auto current_node = Size{};
    auto&& next_node_vertices = get_slice(vertices, current_node);
    for (Size current_node_label{};
         current_node_label < static_cast<Size>(vertices.extent(1));
         ++current_node_label)
    {
      if (next_node_vertices[current_node_label] == cost)
      {
        labelling[current_node] = current_node_label;
        break;
      }
    }
  }

  for (Size next_node{1};
       next_node < static_cast<Size>(vertices.extent(0)); ++next_node)
  {
    auto&& next_node_vertices = get_slice(vertices, next_node);

    [[assume(next_node >= 1)]]
    auto&& current_node = next_node - 1;

    auto&& current_node_label = labelling[current_node];
    auto&& current_label_edges = get_edge_slice(
      edges,
      current_node,
      current_node_label
    );
    auto&& inner_product_result = inner_product(
      semiring, current_label_edges, next_node_vertices);
    for (Size next_node_label{};
         next_node_label < static_cast<Size>(vertices.extent(1)); ++next_node_label)
    {
      auto&& multiplication_result = semiring.multiplexer(
        current_label_edges[next_node_label],
        vertices[next_node, next_node_label]
      );
      if (multiplication_result == inner_product_result)
      {
        labelling[next_node] = next_node_label;
        break;
      }
    }
  }
}

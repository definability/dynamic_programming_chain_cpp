/// @brief The module contains private helping functions
/// for different implementations of dynamic programming.
module;

#include <mdspan>
#include <numeric>

export module dynamic_programming:helpers;

export void validate_input(auto&& vertices, auto&& edges, auto&& labelling)
{
  if (vertices.extent(0) == 0)
  {
    throw std::invalid_argument{"The number of nodes must not be zero"};
  }
  if (vertices.extent(1) == 0)
  {
    throw std::invalid_argument{"The number of labels must not be zero"};
  }
  if (edges.extent(0) + 1 != vertices.extent(0))
  {
    throw std::invalid_argument{"Edges must come from exactly N-1 nodes"};
  }
  if (labelling.size() != vertices.extent(0))
  {
    throw std::invalid_argument{
      "Labelling must be of the same length as the chain"};
  }
  if (edges.extent(1) != vertices.extent(1))
  {
    throw std::invalid_argument{
      "Vertices and edges must have the same number of labels"};
  }
  if (edges.extent(1) != edges.extent(2))
  {
    throw std::invalid_argument{
      "Edges must have the same number of input and output labels"};
  }
}

export auto get_slice(auto&& input_span, auto&&... indices)
{
  auto&& begin = input_span.data_handle();
  auto&& offset = input_span.mapping()(indices..., 0);
  auto&& accessor = input_span.accessor();

  auto&& slice_begin = accessor.offset(begin, offset);

  auto&& slice_size = input_span.extent(sizeof...(indices));
  return std::span{slice_begin, slice_size};
}

export auto inner_product(
  auto&& semiring,
  auto&& current_label_edges,
  auto&& next_node_vertices
)
{
  [[assume(next_node_vertices.begin() != next_node_vertices.end()]]
  [[assume(current_label_edges.begin() != current_label_edges.end()]]
  return std::transform_reduce(
    ++next_node_vertices.begin(),
    next_node_vertices.end(),
    ++current_label_edges.begin(),
    semiring.multiplexer(next_node_vertices[0], current_label_edges[0]),
    semiring.adder,
    semiring.multiplexer
  );
}

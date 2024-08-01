/// @brief The module contains private helping functions
/// for different implementations of dynamic programming.
module;

#include <mdspan>
#include <numeric>

export module dynamic_programming:helpers;

export template<typename Vertices, typename Edges>
void validate_input(Vertices&& vertices, Edges&& edges, auto&& labelling)
{
  static constexpr auto edges_rank = std::decay_t<Edges>::extents_type::rank();

  if (vertices.extent(0) == 0)
  {
    throw std::invalid_argument{"The number of nodes must not be zero"};
  }
  if (vertices.extent(1) == 0)
  {
    throw std::invalid_argument{"The number of labels must not be zero"};
  }
  if constexpr (edges_rank == 3)
  {
    if (edges.extent(0) + 1 != vertices.extent(0))
    {
      throw std::invalid_argument{"Edges must come from exactly N-1 nodes"};
    }
  }
  if (edges.extent(edges_rank - 2) != vertices.extent(1))
  {
    throw std::invalid_argument{
      "Vertices and get_edges must have the same number of labels"};
  }
  if (edges.extent(edges_rank - 2) != edges.extent(edges_rank - 1))
  {
    throw std::invalid_argument{
      "Edges must have the same number of input and output labels"};
  }

  using Size = typename std::decay_t<Vertices>::extents_type::size_type;
  if (labelling.size() != static_cast<Size>(vertices.extent(0)))
  {
    throw std::invalid_argument{
      "Labelling must be of the same length as the chain"};
  }
}

export auto get_slice(auto&& input_span, auto&&... indices)
{
  auto&& begin = input_span.data_handle();
  auto&& offset = input_span.mapping()(indices..., 0);
  auto&& accessor = input_span.accessor();

  auto&& slice_begin = accessor.offset(begin, offset);

  auto&& slice_size = input_span.extent(sizeof...(indices));
  return std::span{slice_begin, static_cast<std::size_t>(slice_size)};
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

export template<typename Edges>
auto get_edge_slice(Edges&& edges, auto&& current_node, auto&& label)
{
  static constexpr auto edges_rank = std::decay_t<Edges>::extents_type::rank();
  if constexpr (edges_rank == 2)
  {
    return get_slice(edges, label);
  }
  else if constexpr (edges_rank == 3)
  {
    return get_slice(edges, current_node, label);
  }
  else
  {
    static_assert(false, "Only 2D and 3D spans are allowed for edges");
  }
}

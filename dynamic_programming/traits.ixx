/// @brief The module contains handy concepts and template aliases
/// to be used for dynamic programming.
module;

#include <concepts>
#include <mdspan>
#include <span>

export module dynamic_programming:traits;

export template<class SpecificSemiring, typename R> concept Semiring =
requires(SpecificSemiring semiring, R& lhs, R& rhs)
{
  { semiring.adder(lhs, rhs) } -> std::same_as<R>;
  { semiring.multiplexer(lhs, rhs) } -> std::same_as<R>;
}
and requires(SpecificSemiring semiring, R& result, R&& multiplier)
{
  {
  semiring.multiplexer.inplace(result, std::move(multiplier))
  } -> std::same_as<void>;
};

export template<typename R, typename Extents> requires(Extents::rank() == 2)
  using Vertices = std::mdspan<R, Extents>;
export template<typename R, typename Extents> requires(Extents::rank() == 3)
  using Edges = std::mdspan<R, Extents>;
export template<typename Label, std::size_t extent>
  using Labelling = std::span<Label, extent>;

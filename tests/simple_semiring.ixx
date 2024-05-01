module;

#include <concepts>
#include <numeric>

export module simple_semiring;

export template<
  typename R,
  std::invocable<const R&, const R&> Adder = std::plus<R>,
  std::invocable<const R&, const R&> Multiplexer = std::multiplies<R>>
struct SimpleSemiring
{
  Adder adder;
  Multiplexer multiplexer;
};

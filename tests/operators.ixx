module;

#include <functional>

export module operators;

template<typename R> struct FakeInplaceOperation
{
  void inplace(this auto&& self, R& result, R&& operand)
  {
    result = self(result, std::move(operand));
  }
};

export
{
  template<typename R> struct Sum : std::plus<R>
  {
    static void inplace(R& result, R&& operand)
    {
      result += std::move(operand);
    }
  };
  template<typename R> struct Product : std::multiplies<R>
  {
    static void inplace(R& result, R&& operand)
    {
      result *= std::move(operand);
    }
  };
  struct LogicalOr : std::logical_or<bool>
  {
    static void inplace(bool& result, bool&& operand)
    {
      result |= operand;
    }
  };
  struct LogicalAnd : std::logical_and<bool>
  {
    static void inplace(bool& result, bool&& operand)
    {
      result &= operand;
    }
  };

  template<typename R>
  struct Min : FakeInplaceOperation<R>
  {
    static R operator()(const R& lhs, const R& rhs)
    {
      return std::min(lhs, rhs);
    }
  };

  template<typename R>
  struct Max : FakeInplaceOperation<R>
  {
    static R operator()(const R& lhs, const R& rhs)
    {
      return std::max(lhs, rhs);
    }
  };
}
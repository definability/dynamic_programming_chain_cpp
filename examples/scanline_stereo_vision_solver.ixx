module;

#include <algorithm>
#include <limits>
#include <cstddef>
#include <mdspan>
#include <vector>

export module scanline_stereo_vision_solver;

import dynamic_programming;
import operators;
import simple_semiring;

template<typename R> using MinPlusSemiring = SimpleSemiring<R, Min<R>, Sum<R>>;

template<typename R, typename Label> struct SolverBase;
export template<typename R, typename Label> struct SolverFast;
export template<typename R, typename Label> struct SolverCompact;

template<typename R, typename Label> struct SolverBase
{
  SolverBase(const std::size_t image_height, const std::size_t image_width, const std::size_t output_maximum_disparity, const R smoothness_penalty_weight)
  : vertices_data(image_width * (output_maximum_disparity + 1))
  , edges_data((output_maximum_disparity + 1) * (output_maximum_disparity + 1))
  , labelling_data(image_width)
  , height{image_height}
  , width{image_width}
  , maximum_disparity{output_maximum_disparity}
  {
    std::ranges::fill(vertices_data, std::numeric_limits<R>::infinity());
    std::ranges::fill(edges_data, smoothness_penalty_weight);

    auto&& disparity_levels = maximum_disparity + 1;
    const std::mdspan edges{edges_data.data(), disparity_levels, disparity_levels};
    for (auto disparity = 0zu; disparity < disparity_levels; ++disparity) {
      edges[disparity, disparity] = static_cast<R>(0);
    }
  }

  void solve(this auto&& self, auto&& left_image, auto&& right_image, auto& disparity_map)
  {
    auto&& disparity_levels = self.maximum_disparity + 1;
    const std::mdspan vertices{self.vertices_data.data(), self.width, disparity_levels};
    const std::mdspan edges{self.edges_data.data(), disparity_levels, disparity_levels};
    const std::span labelling{self.labelling_data.data(), self.width};

    for (auto row = 0zu; row < self.height; ++row)
    {
      for (auto column = 0zu; column < self.width; ++column)
      {
        const auto right_pixel = std::min(self.width, column + disparity_levels);
        for (auto disparity = 0zu; column + disparity < right_pixel; ++disparity)
        {
          vertices[column, disparity] = std::abs(
            static_cast<R>(left_image(column + disparity, row))
            - static_cast<R>(right_image(column, row)));
        }
      }

      self.dynamic_programming();

      for (auto column = 0zu; column < self.width; ++column)
      {
        disparity_map(column, row) = static_cast<uint8_t>(
          static_cast<uint32_t>(labelling[column]) *
          static_cast<uint32_t>(std::numeric_limits<uint8_t>::max()) /
          static_cast<uint32_t>(self.maximum_disparity));
      }
    }
  }
protected:
  ~SolverBase() = default;

  std::vector<R> vertices_data;
  std::vector<R> edges_data;
  std::vector<Label> labelling_data;

  const std::size_t height;
  const std::size_t width;
  const std::size_t maximum_disparity;
};

template<typename R, typename Label> struct SolverCompact : SolverBase<R, Label>
{
  SolverCompact(const std::size_t image_height, const std::size_t image_width, const std::size_t output_maximum_disparity, const R smoothness_penalty_weight)
    : SolverBase<R, Label>{image_height, image_width, output_maximum_disparity, smoothness_penalty_weight}
  {
  }
private:
  friend SolverBase<R, Label>;
  auto dynamic_programming()
  {
    auto&& disparity_levels = this->maximum_disparity + 1;
    const std::mdspan vertices{this->vertices_data.data(), this->width, disparity_levels};
    const std::mdspan edges{this->edges_data.data(), disparity_levels, disparity_levels};
    const std::span labelling{this->labelling_data.data(), this->width};
    R labelling_cost;

    dynamic_programming_compact(MinPlusSemiring<R>{}, vertices, edges, labelling, labelling_cost);
  }
};

template<typename R, typename Label> struct SolverFast : SolverBase<R, Label>
{
  SolverFast(const std::size_t image_height, const std::size_t image_width, const std::size_t output_maximum_disparity, const R smoothness_penalty_weight)
    : SolverBase<R, Label>{image_height, image_width, output_maximum_disparity, smoothness_penalty_weight}
    , local_best_labels_data((image_width - 1) * (output_maximum_disparity + 1))
  {
  }
private:
  friend SolverBase<R, Label>;
  auto dynamic_programming()
  {
    auto&& disparity_levels = this->maximum_disparity + 1;
    const std::mdspan vertices{this->vertices_data.data(), this->width, disparity_levels};
    const std::mdspan edges{this->edges_data.data(), disparity_levels, disparity_levels};
    const std::span labelling{this->labelling_data.data(), this->width};
    std::mdspan local_best_labels{local_best_labels_data.data(), this->width - 1, disparity_levels};
    R labelling_cost;

    dynamic_programming_fast(MinPlusSemiring<R>{}, vertices, edges, labelling, labelling_cost, local_best_labels);
  }
  std::vector<Label> local_best_labels_data;
};

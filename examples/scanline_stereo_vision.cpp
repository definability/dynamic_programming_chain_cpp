/// This file is part of MyProject, which is licensed under the MIT License.
/// This file also uses the CImg library,
/// which is provided "as is" and licensed under the CeCILL license.
#define cimg_display 0
#include "CImg/CImg.h"

#include <cstring>
#include <exception>
#include <filesystem>
#include <iostream>
#include <limits>

import scanline_stereo_vision_solver;

template<typename R, typename Label, template<typename, typename> class Solver>
void stereo(
  auto&& left_image_file_name,
  auto&& right_image_file_name,
  auto&& output_image_file_name,
  auto&& maximum_disparity,
  R smoothness_penalty_weight)
{
  cimg_library::CImg<uint8_t> left_image(left_image_file_name);
  cimg_library::CImg<uint8_t> right_image(right_image_file_name);

  if (left_image.is_empty() || right_image.is_empty())
  {

    std::cerr << "Could not open or find the images" << std::endl;
    return;
  }
  if (left_image.width() != right_image.width())
  {
    std::cerr << "The images must have the same width" << std::endl;
    return;
  }
  if (left_image.height() != right_image.height())
  {
    std::cerr << "The images must have the same height" << std::endl;
    return;
  }

  if (left_image.spectrum() == 3)
  {
    left_image.RGBtoYCbCr().channel(0);
  }
  if (right_image.spectrum() == 3)
  {
    right_image.RGBtoYCbCr().channel(0);
  }

  auto&& height = static_cast<std::size_t>(left_image.height());
  auto&& width = static_cast<std::size_t>(left_image.width());
  Solver<R, Label> solver{height, width, maximum_disparity, smoothness_penalty_weight};
  cimg_library::CImg<uint8_t> disparity_map(width, height, 1, 1, 0);
  solver.solve(left_image, right_image, disparity_map);
  disparity_map.save(output_image_file_name);
}

template<typename Label> void print_disparity_error(auto&& value)
{
  std::cerr
    << "Maximum disparity must be a valid unsigned integer in ranges from 0 to "
    << std::to_string(std::numeric_limits<Label>::max())
    << " but got: '" << value << '\'' << std::endl;
}

void print_smoothness_error(auto&& value)
{
  std::cerr
    << "Smoothness penalty weight must be a valid floating-point value "
    << " in ranges from 0 to "
    << std::to_string(std::numeric_limits<float>::max())
    << " but got: '" << value << '\'' << std::endl;
}

int main(const int argc, const char* const argv[]) try
{
  using R = float;
  using Label = uint16_t;

  if (argc != 7) {
    std::cerr << "Usage: " << argv[0]
              << " <left_image> <right_image> <output_image>"
              << " <maximum_disparity> <smoothness_penalty_weight>"
              << " <algorithm:(fast|compact)>"
              << std::endl;
    return EXIT_FAILURE;
  }

  auto&& left_image_path = std::filesystem::absolute(argv[1]);
  if (!std::filesystem::exists(left_image_path))
  {
    std::cerr << "The left image file does not exist: "
              << left_image_path << std::endl;
    return EXIT_FAILURE;
  }

  auto&& right_image_path = std::filesystem::absolute(argv[2]);
  if (!std::filesystem::exists(right_image_path))
  {
    std::cerr << "The right image file does not exist: "
              << right_image_path << std::endl;
    return EXIT_FAILURE;
  }

  const auto& output_image_path = std::filesystem::absolute(argv[3]);
  if (!std::filesystem::exists(output_image_path.parent_path()))
  {
    std::cerr << "Output folder does not exist: "
              << output_image_path.parent_path() << std::endl;
    return EXIT_FAILURE;
  }

  unsigned long maximum_disparity;
  try
  {
    maximum_disparity = std::stoul(argv[4]);
  }
  catch (const std::invalid_argument& exception)
  {
    print_disparity_error<Label>(argv[4]);
    return EXIT_FAILURE;
    std::cerr << "Error message: " << exception.what();
  }
  catch (const std::out_of_range& exception)
  {
    print_disparity_error<Label>(argv[4]);
    std::cerr << "Error message: " << exception.what();
    return EXIT_FAILURE;
  }

  R smoothness_penalty_weight;
  try
  {
    smoothness_penalty_weight = std::stof(argv[5]);
  }
  catch (const std::invalid_argument& exception)
  {
    print_smoothness_error(argv[5]);
    std::cerr << "Error message: " << exception.what();
    return EXIT_FAILURE;
  }
  catch (const std::out_of_range& exception)
  {
    print_smoothness_error(argv[5]);
    std::cerr << "Error message: " << exception.what();
    return EXIT_FAILURE;
  }

  if (maximum_disparity <= 0 or maximum_disparity > std::numeric_limits<Label>::max())
  {
    print_disparity_error<Label>(argv[4]);
    return EXIT_FAILURE;
  }
  if (smoothness_penalty_weight < 0.0F)
  {
    print_smoothness_error(argv[5]);
    return EXIT_FAILURE;
  }

  try
  {
    if (std::strcmp(argv[6], "compact") == 0)
    {
      stereo<R, Label, SolverCompact>(
        left_image_path.c_str(),
        right_image_path.c_str(),
        output_image_path.c_str(),
        maximum_disparity,
        smoothness_penalty_weight
      );
    }
    else if (std::strcmp(argv[6], "fast") == 0)
    {
      stereo<R, Label, SolverFast>(
        left_image_path.c_str(),
        right_image_path.c_str(),
        output_image_path.c_str(),
        maximum_disparity,
        smoothness_penalty_weight
      );
    }
    else
    {
      std::cerr << "The algorithm must be one of 'compact' or 'fast' "
                << "(without quotes) but '" << argv[6] << "' provided"
                << std::endl;
      return EXIT_FAILURE;
    }
  }
  catch (const cimg_library::CImgException& exception)
  {
    std::cerr << "Image IO error: " << exception.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
catch (const std::exception& exception)
{
  std::cerr << "Unhandled exception: " << exception.what() << std::endl;
  return EXIT_FAILURE;
}
catch (...)
{
  std::cerr << "Unknown exception." << std::endl;
  return EXIT_FAILURE;
}

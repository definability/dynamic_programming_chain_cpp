# Dynamic Programming on Chain Graphs

Implementation of dynamic programming on chain graphs on an arbitrary semiring
using C++23.

Check out the following Medium stories to know more about this project:
- [Dynamic Programming on Chain Graphs. Part 1: The Problem][medium-part1];
- [Dynamic Programming on Chain Graphs. Part 2: The Solution][medium-part2];
- [Dynamic Programming on Chain Graphs. Part 3: Preparing for Implementation][medium-part3];
- [Dynamic Programming on Chain Graphs. Part 4: The Implementation][medium-part4].
- [Dynamic Programming on Chain Graphs. Part 5: Testing][medium-part5].

A writing about the binocular stereo vision example:
- [Scanline Stereo Vision][medium-stereo-vision].

## Build Requirements

The project contains a [Dockerfile][clang18-dockerfile]
with all needed dependencies included.
You can build it in Linux terminal by executing the following command
from the root directory of this project:
```shell
docker build -t ubuntu-clang:18 .
```
It is based on [Ubuntu 24.04 LTS][ubuntu-24.04].
Below is the explanation of the contents of the Docker image.

The project requires a compiler and a standard library
compatible with some [C++20][compatibility-cpp20]
and [C++23][compatibility-cpp23] features,
including but not limited to the following:
- [C++20 modules][cpp-modules];
- [deducing this][deducing-this];
- [multidimensional operator[]][md-subscript-operator];
- [std::mdspan][mdspan].

As of April 2024, it is [clang 18][clang-18]
and [libc++ 18][libcpp-18].
Also, it needs a compatible build system:
[CMake 3.28][cmake-3.28] or newer
and [Ninja 1.10][ninja-1.10] or newer.

## Building the Project

### Shell

You can set up the project using
```shell
cmake \
  -S . \
  -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=clang++-18 \
  -DCMAKE_CXX_FLAGS='-stdlib=libc++ -Wall -Wextra -pedantic'
```
Then, build it with
```shell
cmake --build build
```
You may see warnings about [assume][attribute-assume] attribute
not being supported, which is okay at the moment.
Let us wait for [Clang 19][clang-cpp-status] to be included
in an Ubuntu repository.

### Docker

To set up and build the project in Docker container,
I recommend you using your current user id and directory
to avoid root-owned files and keep you absolute paths right:
```shell
docker run --rm -u ${UID}:${UID} -v $(pwd):$(pwd) ubuntu-clang:18 bash -c " \
  cd $(pwd) && \
  cmake \
    -S . \
    -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=clang++-18 \
    -DCMAKE_CXX_FLAGS='-stdlib=libc++ -Wall -Wextra -pedantic' && \
  cmake --build build \
"
```

### IDE

Those using [JetBrains CLion][clion]
can set up a [Docker toolchain][jetbrains-docker-toolchain] by specifying:
- CMake: Docker CMake;
- Build Tool: `/usr/bin/ninja`;
- C Compiler: `clang-18`;
- C++ Compiler `clang++-18`;
- Debugger: `lldb-18`.

## Running Tests

The project uses [doctest] &mdash; a lightweight testing framework compatible with C++23.

Configure the project
```shell
cmake \
  -S . \
  -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=clang++-18 \
  -DCMAKE_CXX_FLAGS='-stdlib=libc++ -Wall -Wextra -pedantic'
```
Build tests
```shell
cmake --build build --target tests
```
Run the tests
```shell
./build/tests/tests
```

You can do this in Docker container:
```shell
docker run --rm -u ${UID}:${UID} -v $(pwd):$(pwd) ubuntu-clang:18 bash -c " \
  cd $(pwd) && \
  cmake \
    -S . \
    -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=clang++-18 \
    -DCMAKE_CXX_FLAGS='-stdlib=libc++ -Wall -Wextra -pedantic' && \
  cmake --build build --target tests && \
  build/tests/tests \
"
```

## Using the Project

Configure the project
```shell
cmake \
  -S . \
  -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=clang++-18 \
  -DCMAKE_CXX_FLAGS='-stdlib=libc++ -Wall -Wextra -pedantic'
```

### Scanline Stereo Vision Example

You can get the example inputs from the [Middlebury Stereo Datasets][middlebury].

Build the example
```shell
cmake --build build --target scanline_stereo_vision
```
Run the example using `view0.png` as the left image and
`view1.png` as the right image.
Use the maximum disparity `50` and smoothness weight `25`.
Compute the disparity map using the "compact" algorithm
and write the resulting disparity map into `out.png`.
```shell
./build/examples/scanline_stereo_vision view0.png view1.png out.png 50 25 compact
```

You can do this in Docker container:
```shell
docker run --rm -u ${UID}:${UID} -v $(pwd):$(pwd) ubuntu-clang:18 bash -c " \
  cd $(pwd) && \
  cmake \
    -S . \
    -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=clang++-18 \
    -DCMAKE_CXX_FLAGS='-stdlib=libc++ -Wall -Wextra -pedantic' && \
  cmake --build build --target scanline_stereo_vision && \
  build/examples/scanline_stereo_vision view0.png view1.png out.png 50 25 compact \
"
```

## Licensing

This project includes the [CImg] library.
It is provided "as is" under the [CeCILL] license.
You can find more information about the [CImg] library
and its licensing terms in the [third_party/CImg] directory.
Please refer to the [third_party/CImg/Licence_CeCILL_V2-en.txt]
for the full license text.

The other project parts (the files outside the [third_party] directory)
are licensed under the [MIT license],
so you can freely use the source code in pet projects,
commercial projects, and home assignments.
You can also modify it, distribute it, and sell it.
Note that the project is provided "as is,"
so the author is not responsible for your usage of it.
See the [LICENSE] file for more details.

[attribute-assume]: https://en.cppreference.com/w/cpp/language/attributes/assume
[CeCILL]: http://www.cecill.info/licences/Licence_CeCILL_V2-en.html
[CImg]: https://cimg.eu
[clang-18]: https://releases.llvm.org/18.1.0/tools/clang/docs/ReleaseNotes.html
[clang-cpp-status]: https://clang.llvm.org/cxx_status.html
[clang18-dockerfile]: ./Dockerfile
[clion]: https://www.jetbrains.com/clion/
[cmake-3.28]: https://cmake.org/cmake/help/latest/release/3.28.html
[compatibility-cpp20]: https://en.cppreference.com/w/cpp/compiler_support/20
[compatibility-cpp23]: https://en.cppreference.com/w/cpp/compiler_support/23
[cpp-modules]: https://en.cppreference.com/w/cpp/language/modules
[deducing-this]: https://en.cppreference.com/w/cpp/language/member_functions#Explicit_object_member_functions
[doctest]: https://github.com/doctest/doctest
[jetbrains-docker-toolchain]: https://www.jetbrains.com/help/clion/clion-toolchains-in-docker.html
[libcpp-18]: https://releases.llvm.org/18.1.0/projects/libcxx/docs/ReleaseNotes.html
[LICENSE]: https://github.com/definability/dynamic_programming_chain_cpp/blob/master/LICENSE
[md-subscript-operator]: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2128r5.pdf
[mdspan]: https://en.cppreference.com/w/cpp/container/mdspan
[medium-part1]: https://medium.com/@valeriy.krygin/dynamic-programming-on-chain-graphs-part-1-the-problem-78bcf0250257
[medium-part2]: https://medium.com/@valeriy.krygin/dynamic-programming-on-chain-graphs-part-2-the-solution-37c1bad8570e
[medium-part3]: https://medium.com/@valeriy.krygin/dynamic-programming-on-chain-graphs-part-3-preparing-for-implementation-bdceb5bf1345
[medium-part4]: https://medium.com/@valeriy.krygin/dynamic-programming-on-chain-graphs-part-4-the-implementation-35b55a528afb
[medium-part5]: https://medium.com/@valeriy.krygin/dynamic-programming-on-chain-graphs-part-5-testing-38e0aa01f18b
[medium-stereo-vision]: https://medium.com/@valeriy.krygin/scanline-stereo-vision-85ff252ec521
[middlebury]: https://vision.middlebury.edu/stereo/data/
[MIT License]: https://choosealicense.com/licenses/mit
[ninja-1.10]: https://github.com/ninja-build/ninja/releases/tag/v1.10.0
[third_party]: https://github.com/definability/dynamic_programming_chain_cpp/tree/master/third_party
[third_party/CImg]: https://github.com/definability/dynamic_programming_chain_cpp/tree/master/third_party/CImg
[third_party/CImg/Licence_CeCILL_V2-en.txt]: https://github.com/definability/dynamic_programming_chain_cpp/tree/master/third_party/CImg/Licence_CeCILL_V2-en.txt
[ubuntu-24.04]: https://ubuntu.com/blog/tag/ubuntu-24-04-lts

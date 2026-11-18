cmake_minimum_required(VERSION 4.4.5)
project(SCMDR CXX)

add_library(scmdr
	src/canvas.cpp
	src/stb_image.h
	src/texture.cpp
	src/integer_rect.hpp
	src/color.hpp
	include/scmdr/canvas.hpp
	include/scmdr/texture.hpp
	src/integer_rect.cpp
	src/color.cpp
)
target_include_directories(scmdr PUBLIC include)
target_compile_definitions(scmdr PRIVATE STB_IMAGE_IMPLEMENTATION)
target_compile_features(scmdr PUBLIC cxx_std_20)

include(FetchContent)
FetchContent_Declare(
	fmt
	GIT_REPOSITORY
	https://github.com/fmtlib/fmt.git
	GIT_TAG
	12.2.1
	SYSTEM
)
FetchContent_MakeAvailable(fmt)
target_link_libraries(scmdr PRIVATE fmt::fmt)
add_library(scmdr::scmdr ALIAS scmdr)

# example
add_executable(example examples/main.cpp)
target_link_libraries(example PRIVATE scmdr::scmdr)
target_link_options(example PRIVATE -static)
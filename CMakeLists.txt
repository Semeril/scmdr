cmake_minimum_required(VERSION 4.4.5)
project(SCMDR VERSION 1 LANGUAGES CXX)

add_library(scmdr)
add_library(scmdr::scmdr ALIAS scmdr)

target_sources(scmdr
	PRIVATE
		./src/scmdr.cpp
		./src/stb_image.h
)
target_sources(scmdr
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS ./include
	FILES ./include/scmdr/scmdr.hpp
)
target_compile_definitions(scmdr PRIVATE STB_IMAGE_IMPLEMENTATION)
target_compile_features(scmdr PRIVATE cxx_std_23)

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

# example
add_executable(example)
target_sources(example PRIVATE ./examples/main.cpp)
target_link_libraries(example PRIVATE scmdr::scmdr)
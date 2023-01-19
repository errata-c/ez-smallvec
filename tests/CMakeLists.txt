cmake_minimum_required(VERSION 3.24)

project(${CMAKE_PROJECT_NAME}_TESTS
VERSION 1.0.0
LANGUAGES C CXX)

FetchContent_Declare(
	Catch2
	GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
	GIT_TAG "v3.2.1"
	FIND_PACKAGE_ARGS CONFIG
)
FetchContent_MakeAvailable(Catch2)


add_executable(basic_test 
	"basic/basic.cpp"
)
target_link_libraries("basic_test" 
PRIVATE 
	ez::smallvec
	Catch2::Catch2WithMain
)

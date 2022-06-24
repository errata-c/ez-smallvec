cmake_minimum_required(VERSION 3.20)

project(${CMAKE_PROJECT_NAME}-tests 
VERSION 1.0.0
LANGUAGES C CXX)

find_package(Catch2 CONFIG REQUIRED)

add_executable(basic_test 
	"basic/basic.cpp"
)
target_link_libraries("basic_test" 
PRIVATE 
	ez::smallvec
	Catch2::Catch2WithMain
)

cmake_minimum_required(VERSION 3.0)
project(kvasir_abcd)

# create the kvasir_abcd library target
add_library(kvasir_abcd INTERFACE)
target_include_directories(kvasir_abcd INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
# abcd requires a number of features to compile, this will automatically set the c++ standard version
# to support those features. The c++ version is not explicitly set, so that cmake can figure
# things out by itself
target_compile_features(kvasir_abcd INTERFACE
		cxx_generic_lambdas
		cxx_variable_templates
		cxx_variadic_templates
		cxx_constexpr
		cxx_alias_templates
		cxx_auto_type
		cxx_long_long_type
		cxx_right_angle_brackets
		cxx_static_assert
		cxx_uniform_initialization
		cxx_user_literals
		${CPP_EXTRA_FEATURES})

add_subdirectory(mp11)
target_link_libraries(kvasir_abcd INTERFACE Boost::mp11)

add_executable(kvasir_abcd_test test/test.cpp)
target_link_libraries(kvasir_abcd_test kvasir_abcd)
target_compile_options(kvasir_abcd_test PUBLIC -ftemplate-depth=2048 -Wall) # -std=gnu++1z)

option(MAKE_INCLUDE_TESTS OFF)
if (MAKE_INCLUDE_TESTS)
	function(convert_name out filename)
		string(REGEX REPLACE "[\\./]" "_" result "${filename}")
		set(${out} ${result} PARENT_SCOPE)
	endfunction()
	function(make_test_includes_file file)
		convert_name(basename "${file}")
		file(WRITE ${CMAKE_BINARY_DIR}/include_test/${basename}.cpp
				"#include <${file}>\nint main() {}")
	endfunction()
	file(GLOB_RECURSE headers RELATIVE "${CMAKE_CURRENT_LIST_DIR}/src/"
			"${CMAKE_CURRENT_LIST_DIR}/src/*.hpp")
	foreach(file IN LISTS headers)
		make_test_includes_file(${file})
		convert_name(basename "${file}")
		add_executable(${basename} include_test/${basename}.cpp)
		target_link_libraries(${basename} kvasir_abcd)
		target_compile_options(${basename} PUBLIC -Wall)
		add_dependencies(kvasir_abcd_test ${basename})
	endforeach()
	file(GLOB_RECURSE tests RELATIVE "${CMAKE_CURRENT_LIST_DIR}/"
			"${CMAKE_CURRENT_LIST_DIR}/test/*.hpp")
	foreach (file IN LISTS tests)
		make_test_includes_file(${file})
		convert_name(basename "${file}")
		add_executable(${basename} include_test/${basename}.cpp)
		target_link_libraries(${basename} kvasir_abcd)
		target_include_directories(${basename} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
		target_compile_options(${basename} PUBLIC -Wall)
		add_dependencies(kvasir_abcd_test ${basename})
	endforeach ()
endif ()

# Add Standardese target
find_program(STANDARDESE standardese HINTS ${STANDARDESE_ROOT})

if (STANDARDESE)
	set(KVASIR_ABCD_DOCS_DIR ${CMAKE_BINARY_DIR}/doc/standardese)
	message(STATUS "Found standardese for documentation generation")
	message(STATUS "Documentation dir is: ${KVASIR_ABCD_DOCS_DIR}")
	file(MAKE_DIRECTORY ${KVASIR_ABCD_DOCS_DIR})
	add_custom_target(
			standardese
			COMMAND ${STANDARDESE}
			-I${CMAKE_SOURCE_DIR}/src/kvasir/abcd
			--input.blacklist_namespace=detail
			--compilation.comments_in_macro=false
			--output.require_comment_for_full_synopsis=false
			--output.format=html
			${CMAKE_SOURCE_DIR}/src/kvasir/abcd
			WORKING_DIRECTORY ${KVASIR_ABCD_DOCS_DIR}
	)
endif (STANDARDESE)

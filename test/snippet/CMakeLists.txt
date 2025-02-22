# -----------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
# -----------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.10)
project (bio_test_snippet CXX)

find_package (BIO REQUIRED HINTS ${CMAKE_SOURCE_DIR}/../../build_system)

include (${CMAKE_SOURCE_DIR}/../../submodules/seqan3/test/seqan3-test.cmake)
include (${CMAKE_SOURCE_DIR}/../../submodules/seqan3/test/cmake/diagnostics/list_unused_snippets.cmake)

add_library (snippet_main snippet_main.cpp)
target_link_libraries (snippet_main PUBLIC seqan3::test gtest)

macro (bio_snippet test_name_prefix snippet snippet_base_path)
    seqan3_test_component (snippet_target_name "${snippet}" TARGET_NAME)
    seqan3_test_component (snippet_test_name "${snippet}" TEST_NAME)
    seqan3_test_component (snippet_target_path "${snippet}" TARGET_PATH)
    set (target "${snippet_target_name}_snippet")

    add_executable (${target} "${snippet_base_path}/${snippet}")
    target_link_libraries (${target} PUBLIC snippet_main bio::bio)
    target_compile_options (${target} PUBLIC "-Wno-unused")
    set_target_properties(${target}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${snippet_target_path}"
    )
    collect_used_snippet (${target})

    set (snippet_test_target "${test_name_prefix}/${snippet_test_name}_snippet")
    add_test (NAME "${snippet_test_target}" COMMAND ${target})

    # disable version checker, as it interferes with comparing the snippet output
    set_tests_properties ("${snippet_test_target}" PROPERTIES ENVIRONMENT SEQAN3_NO_VERSION_CHECK=0)

    set (snippet_compare_test_target "${test_name_prefix}/${snippet_test_name}_snippet_cmp_output")

    add_test (NAME "${snippet_compare_test_target}"
              COMMAND ${CMAKE_COMMAND}
                      -DTARGET_FILE=$<TARGET_FILE:${target}>
                      -DSOURCE_FILE=${snippet_base_path}/${snippet}
                      -P "${CMAKE_SOURCE_DIR}/../../submodules/seqan3/test/snippet/compare_snippet_output.cmake")

    # disable version checker, as it interferes with comparing the snippet output
    set_tests_properties ("${snippet_compare_test_target}" PROPERTIES ENVIRONMENT SEQAN3_NO_VERSION_CHECK=0)

    unset (snippet_target_name)
    unset (snippet_test_name)
    unset (snippet_target_path)
    unset (target)
endmacro ()

macro (bio_snippets test_name_prefix snippet_base_path)
    seqan3_test_files (snippet_files "${snippet_base_path}" "*.cpp")
    foreach (snippet ${snippet_files})
        bio_snippet("${test_name_prefix}" "${snippet}" "${snippet_base_path}")
    endforeach ()
endmacro ()

seqan3_require_ccache ()
seqan3_require_test ()

bio_snippets ("snippet" "${CMAKE_CURRENT_SOURCE_DIR}")
#bio_snippets ("doc/snippet" "${CMAKE_CURRENT_SOURCE_DIR}/../../doc")

# Skip tests, e.g. because of non-deterministic output
# set_tests_properties ("example/test" PROPERTIES SKIP_RETURN_CODE 1)

list_unused_snippets ("${CMAKE_CURRENT_SOURCE_DIR}")
#list_unused_snippets ("${CMAKE_CURRENT_SOURCE_DIR}/../../doc")


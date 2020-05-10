file (GLOB_RECURSE TEST_STRING_SOURCE_FILES string.c)
add_executable (test_string ${TEST_STRING_SOURCE_FILES})
target_link_libraries (test_string ppe cunit)

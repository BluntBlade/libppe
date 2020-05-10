file (GLOB_RECURSE TEST_STRING_SOURCE_FILES string.c)
target_link_libraries (ppe cunit)
add_executable (test_string ${TEST_STRING_SOURCE_FILES})

cmake_minimum_required (VERSION 3.3.1)
project (libppe)

include (CheckSymbolExists)

set (PPE_LARGE_FILE_SUPPORT_AWARE OFF CACHE BOOL "Set to ON to detect Large File Support dynamically (default: OFF)")
set (PPE_SHARED_FD_FOR_SECTIONS_SUPPORT ON CACHE BOOL "Set to ON to enable shared fd for sections (default: ON)")

include_directories (/usr/include /usr/local/include SYSTEM)
include_directories (inc)

link_directories (/usr/lib /usr/local/lib)

set (CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE")
check_symbol_exists (lseek64 "unistd.h" HAVE_LSEEK64)
check_symbol_exists (pread "unistd.h" HAVE_PREAD)
set (CMAKE_REQUIRED_DEFINITIONS "")

if (DEFINED HAVE_LSEEK64)
    add_compile_options (-DPPE_CFG_LARGE_FILE_SUPPORT)
    if (DEFINED PPE_LARGE_FILE_SUPPORT_AWARE AND ${PPE_LARGE_FILE_SUPPORT_AWARE})
        add_compile_options (-DPPE_CFG_LARGE_FILE_SUPPORT_AWARE)
    endif (DEFINED PPE_LARGE_FILE_SUPPORT_AWARE AND ${PPE_LARGE_FILE_SUPPORT_AWARE})
endif (DEFINED HAVE_LSEEK64)

if (DEFINED HAVE_PREAD AND DEFINED PPE_SHARED_FD_FOR_SECTIONS_SUPPORT AND ${PPE_SHARED_FD_FOR_SECTIONS_SUPPORT})
    add_compile_options (-DPPE_CFG_SHARED_FD_FOR_SECTIONS)
endif (DEFINED HAVE_PREAD AND DEFINED PPE_SHARED_FD_FOR_SECTIONS_SUPPORT AND ${PPE_SHARED_FD_FOR_SECTIONS_SUPPORT})

add_compile_options (-D_GNU_SOURCE --std=c99 -Wall)

file (GLOB_RECURSE SOURCE_FILES src/*.c)
include_directories (src)
add_library (ppe SHARED ${SOURCE_FILES})

if (DEFINED HAVE_LSEEK64 AND DEFINED PPE_LARGE_FILE_SUPPORT_AWARE AND ${PPE_LARGE_FILE_SUPPORT_AWARE})
    target_link_libraries (ppe dl)
endif (DEFINED HAVE_LSEEK64 AND DEFINED PPE_LARGE_FILE_SUPPORT_AWARE AND ${PPE_LARGE_FILE_SUPPORT_AWARE})

#target_link_libraries (ppe curl ssl crypto)

#add_subdirectory (test)
#add_subdirectory (demo)
#add_test (NAME test_json COMMAND test/test_json)

# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any
# means.
#
# In jurisdictions that recognize copyright laws, the author or authors
# of this software dedicate any and all copyright interest in the
# software to the public domain. We make this dedication for the benefit
# of the public at large and to the detriment of our heirs and
# successors. We intend this dedication to be an overt act of
# relinquishment in perpetuity of all present and future rights to this
# software under copyright law.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# For more information, please refer to <http://unlicense.org/>

cmake_minimum_required(VERSION 3.15)
project(hashmap)

include(CTest)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

# Base warning flags and whether -std= flags are supported.
# MSVC and clang-cl use /std: and don't accept -std=, so we skip
# the standard flag for those compilers.
if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  set(HASHMAP_C_WARNINGS "-Wall -Wextra -Werror")
  set(HASHMAP_C_USE_STD_FLAG TRUE)
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")
  if(CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(HASHMAP_C_WARNINGS "/Wall /WX /wd4514 /wd5045")
    set(HASHMAP_C_USE_STD_FLAG FALSE)
  else()
    set(HASHMAP_C_WARNINGS "-Wall -Wextra -Weverything -Werror -Wno-poison-system-directories")
    set(HASHMAP_C_USE_STD_FLAG TRUE)
  endif()
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  set(HASHMAP_C_WARNINGS "/Wall /WX /wd4514 /wd5045")
  set(HASHMAP_C_USE_STD_FLAG FALSE)
else()
  message(WARNING "Unknown C compiler '${CMAKE_C_COMPILER_ID}'!")
  set(HASHMAP_C_WARNINGS "")
  set(HASHMAP_C_USE_STD_FLAG FALSE)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(HASHMAP_CXX_WARNINGS "-Wall -Wextra -Werror")
  set(HASHMAP_CXX_USE_STD_FLAG TRUE)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(HASHMAP_CXX_WARNINGS "/Wall /WX /wd4514 /wd5045")
    set(HASHMAP_CXX_USE_STD_FLAG FALSE)
  else()
    set(HASHMAP_CXX_WARNINGS "-Wall -Wextra -Weverything -Werror -Wno-poison-system-directories")
    set(HASHMAP_CXX_USE_STD_FLAG TRUE)
  endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(HASHMAP_CXX_WARNINGS "/Wall /WX /wd4514 /wd5045")
  set(HASHMAP_CXX_USE_STD_FLAG FALSE)
else()
  message(WARNING "Unknown C++ compiler '${CMAKE_CXX_COMPILER_ID}'!")
  set(HASHMAP_CXX_WARNINGS "")
  set(HASHMAP_CXX_USE_STD_FLAG FALSE)
endif()

# Helper: set compile flags for a C source file (warnings + standard + extras).
function(hashmap_set_c_flags FILE STANDARD)
  set(EXTRA_FLAGS ${ARGN})
  if(HASHMAP_C_USE_STD_FLAG)
    set_source_files_properties(${FILE} PROPERTIES
      COMPILE_FLAGS "${HASHMAP_C_WARNINGS} ${STANDARD} ${EXTRA_FLAGS}")
  else()
    set_source_files_properties(${FILE} PROPERTIES
      COMPILE_FLAGS "${HASHMAP_C_WARNINGS} ${EXTRA_FLAGS}")
  endif()
endfunction()

# Helper: set compile flags for a C++ source file (warnings + standard + extras).
function(hashmap_set_cxx_flags FILE STANDARD)
  set(EXTRA_FLAGS ${ARGN})
  if(HASHMAP_CXX_USE_STD_FLAG)
    set_source_files_properties(${FILE} PROPERTIES
      COMPILE_FLAGS "${HASHMAP_CXX_WARNINGS} ${STANDARD} ${EXTRA_FLAGS}")
  else()
    set_source_files_properties(${FILE} PROPERTIES
      COMPILE_FLAGS "${HASHMAP_CXX_WARNINGS} ${EXTRA_FLAGS}")
  endif()
endfunction()

# C files — each compiled under a specific C standard.
hashmap_set_c_flags(main.c  "-std=gnu89")
hashmap_set_c_flags(test.c  "-std=gnu89")
hashmap_set_c_flags(test99.c "-std=c99")

list(FIND CMAKE_C_COMPILE_FEATURES c_std_11 IDX)
if(${IDX} GREATER -1)
  hashmap_set_c_flags(test11.c "-std=c11")
endif()

# Exercise the SSE4.2 hashing path when the target supports it.
if(NOT "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
  if(MSVC OR CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    hashmap_set_c_flags(test_sse42.c "-std=gnu89" "/arch:AVX")
  else()
    hashmap_set_c_flags(test_sse42.c "-std=gnu89" "-msse4.2")
  endif()
endif()

# C++ files — each compiled under a specific C++ standard.
hashmap_set_cxx_flags(test.cpp   "" "-Wno-c++98-compat")
hashmap_set_cxx_flags(test98.cpp "-std=gnu++98")
hashmap_set_cxx_flags(test11.cpp "-std=c++11" "-Wno-c++98-compat")
hashmap_set_cxx_flags(test14.cpp "-std=c++14" "-Wno-c++98-compat")
hashmap_set_cxx_flags(test17.cpp "-std=c++17" "-Wno-c++98-compat")
hashmap_set_cxx_flags(test20.cpp "-std=c++20" "-Wno-c++98-compat")

set(HASHMAP_TEST_SOURCES
  ../hashmap.h
  utest.h
  test_shared.h
  main.c
  test.c
  test99.c
  test11.c
  test_sse42.c
  test.cpp
  test98.cpp
  test11.cpp
  test14.cpp
  test17.cpp
  test20.cpp
)

# Helper: add a test executable and register it with CTest.
function(hashmap_add_test NAME)
  add_executable(${NAME} ${HASHMAP_TEST_SOURCES})
  add_test(NAME ${NAME} COMMAND $<TARGET_FILE:${NAME}>)
endfunction()

hashmap_add_test(hashmap_test)

if(MSVC)
  # Build a second test binary linked against the static CRT (/MT)
  # to ensure we work with both static and dynamic libc linkage.
  set_property(TARGET hashmap_test PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  hashmap_add_test(hashmap_mt_test)
  set_property(TARGET hashmap_mt_test PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Some sanitizer runtimes can't initialise on every host even when
# the toolchain links them fine — e.g. clang's TSan and MSan on
# aarch64 only ship shadow-memory layouts for 39-, 42- and 48-bit
# user VMAs, but Raspberry Pi OS' kernel is configured with
# CONFIG_ARM64_VA_BITS=47. The binary builds, but aborts before
# main with "FATAL: ThreadSanitizer: unsupported VMA range". Probe
# each candidate sanitizer with try_run and skip the ones whose
# runtime can't start. Result is cached so reconfigure is cheap;
# delete CMakeCache.txt to re-probe (e.g. after changing kernels).
function(hashmap_check_sanitizer_runs SAN OUT_VAR)
  set(CACHE_VAR "HASHMAP_SAN_${SAN}_RUNS")
  if(DEFINED ${CACHE_VAR})
    set(${OUT_VAR} ${${CACHE_VAR}} PARENT_SCOPE)
    return()
  endif()

  set(PROBE_SRC "${CMAKE_BINARY_DIR}/hashmap_san_probe_${SAN}.c")
  file(WRITE "${PROBE_SRC}" "int main(void) { return 0; }\n")

  try_run(RUN_RESULT COMPILE_RESULT
    "${CMAKE_BINARY_DIR}/hashmap_san_probe_${SAN}"
    "${PROBE_SRC}"
    COMPILE_DEFINITIONS "-fsanitize=${SAN}" "-fno-omit-frame-pointer"
    LINK_OPTIONS "-fsanitize=${SAN}"
  )

  if(COMPILE_RESULT AND "${RUN_RESULT}" STREQUAL "0")
    set(RESULT TRUE)
    message(STATUS "Sanitizer ${SAN}: usable")
  else()
    set(RESULT FALSE)
    message(STATUS "Sanitizer ${SAN}: skipping "
                   "(compile=${COMPILE_RESULT}, run=${RUN_RESULT})")
  endif()

  set(${CACHE_VAR} ${RESULT} CACHE INTERNAL
      "Whether the ${SAN} sanitizer runtime starts on this host")
  set(${OUT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()

if(("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang") AND NOT "${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" STREQUAL "MSVC")
  set(HASHMAP_SANITIZER_CANDIDATES
    address
    undefined
    thread
  )

  if(NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")
    set(HASHMAP_SANITIZER_CANDIDATES ${HASHMAP_SANITIZER_CANDIDATES} memory)
  endif()

  set(HASHMAP_SANITIZERS)
  foreach(HASHMAP_SANITIZER ${HASHMAP_SANITIZER_CANDIDATES})
    if(CMAKE_CROSSCOMPILING)
      list(APPEND HASHMAP_SANITIZERS ${HASHMAP_SANITIZER})
    else()
      hashmap_check_sanitizer_runs(${HASHMAP_SANITIZER} HASHMAP_SAN_RUNS)
      if(HASHMAP_SAN_RUNS)
        list(APPEND HASHMAP_SANITIZERS ${HASHMAP_SANITIZER})
      endif()
    endif()
  endforeach()

  foreach(HASHMAP_SANITIZER ${HASHMAP_SANITIZERS})
    hashmap_add_test(hashmap_${HASHMAP_SANITIZER}_test)
    target_compile_options(hashmap_${HASHMAP_SANITIZER}_test PRIVATE -fsanitize=${HASHMAP_SANITIZER} -fno-omit-frame-pointer)
    target_link_options(hashmap_${HASHMAP_SANITIZER}_test PRIVATE -fsanitize=${HASHMAP_SANITIZER})
  endforeach()
endif()

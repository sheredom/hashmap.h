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

project(hashmap)
cmake_minimum_required(VERSION 3.15)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  set_source_files_properties(main.c test.c PROPERTIES
    COMPILE_FLAGS "-Wall -Wextra -Werror -std=gnu89"
  )
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set_source_files_properties(main.c test.c PROPERTIES
      COMPILE_FLAGS "/Wall /WX /wd4514 /wd5045"
    )
  else()
    set_source_files_properties(main.c test.c PROPERTIES
      COMPILE_FLAGS "-Wall -Wextra -Weverything -Werror -std=gnu89"
    )
  endif()
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  set_source_files_properties(main.c test.c PROPERTIES
    COMPILE_FLAGS "/Wall /WX /wd4514 /wd5045"
  )
else()
  message(WARNING "Unknown compiler '${CMAKE_C_COMPILER_ID}'!")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set_source_files_properties(test.cpp PROPERTIES
    COMPILE_FLAGS "-Wall -Wextra -Werror -std=gnu++98"
  )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set_source_files_properties(test.cpp PROPERTIES
      COMPILE_FLAGS "/Wall /WX /wd4514 /wd5045"
    )
  else()
    set_source_files_properties(test.cpp PROPERTIES
      COMPILE_FLAGS "-Wall -Wextra -Weverything -Werror -std=gnu++98"
    )
  endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set_source_files_properties(test.cpp PROPERTIES
    COMPILE_FLAGS "/Wall /WX /wd4514 /wd5045"
  )
else()
  message(WARNING "Unknown compiler '${CMAKE_C_COMPILER_ID}'!")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set_source_files_properties(test11.cpp PROPERTIES
    COMPILE_FLAGS "-Wall -Wextra -Werror -std=c++11"
  )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set_source_files_properties(test11.cpp PROPERTIES
      COMPILE_FLAGS "/Wall /WX /wd4514 /wd5045"
    )
  else()
    set_source_files_properties(test11.cpp PROPERTIES
      COMPILE_FLAGS "-Wall -Wextra -Weverything -Werror -std=c++11"
    )
  endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set_source_files_properties(test11.cpp PROPERTIES
    COMPILE_FLAGS "/Wall /WX /wd4514 /wd5045"
  )
else()
  message(WARNING "Unknown compiler '${CMAKE_C_COMPILER_ID}'!")
endif()

if (NOT "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
  if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set_source_files_properties(test_sse42.c PROPERTIES
      COMPILE_FLAGS "-Wall -Wextra -Werror -std=gnu89 -msse4.2"
    )
  elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      set_source_files_properties(test_sse42.c PROPERTIES
        COMPILE_FLAGS "/Wall /WX /wd4514 /wd5045 /arch:AVX"
      )
    else()
      set_source_files_properties(test_sse42.c PROPERTIES
        COMPILE_FLAGS "-Wall -Wextra -Weverything -Werror -std=gnu89 -msse4.2"
      )
    endif()
  elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    set_source_files_properties(test_sse42.c PROPERTIES
      COMPILE_FLAGS "/Wall /WX /wd4514 /wd5045 /arch:AVX"
    )
  else()
    message(WARNING "Unknown compiler '${CMAKE_C_COMPILER_ID}'!")
  endif()
endif()

add_executable(hashmap_test
  ../hashmap.h
  main.c
  test.c
  test_sse42.c
  test.cpp
  test11.cpp
)

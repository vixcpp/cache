#  @file CMakeLists.txt
#  @author Gaspard Kirira
#
#  Copyright 2025, Gaspard Kirira. All rights reserved.
#  https://github.com/vixcpp/vix
#  Use of this source code is governed by a MIT license
#  that can be found in the License file.

cmake_minimum_required(VERSION 3.20)

include(CTest)

if (BUILD_TESTING)
  enable_testing()
endif()

# --------------------------------------------------------------------
# Target resolution
# --------------------------------------------------------------------
set(VIX_CACHE_TEST_TARGET vix::cache)

if (NOT TARGET ${VIX_CACHE_TEST_TARGET} AND TARGET vix_cache)
  set(VIX_CACHE_TEST_TARGET vix_cache)
endif()

if (NOT TARGET ${VIX_CACHE_TEST_TARGET})
  message(FATAL_ERROR "[cache/tests] Missing cache target (expected vix::cache or vix_cache).")
endif()

# --------------------------------------------------------------------
# Helpers
# --------------------------------------------------------------------
function(vix_cache_add_test test_name test_source)
  add_executable(${test_name}
    ${test_source}
  )

  target_compile_features(${test_name}
    PRIVATE
      cxx_std_20
  )

  target_link_libraries(${test_name}
    PRIVATE
      ${VIX_CACHE_TEST_TARGET}
  )

  if (TARGET vix_warnings)
    target_link_libraries(${test_name}
      PRIVATE
        vix_warnings
    )
  endif()

  if (VIX_ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(${test_name}
      PRIVATE
        -fno-omit-frame-pointer
        -fsanitize=address,undefined
    )

    target_link_options(${test_name}
      PRIVATE
        -fsanitize=address,undefined
    )
  endif()

  if (BUILD_TESTING)
    add_test(
      NAME ${test_name}
      COMMAND ${test_name}
    )
  endif()
endfunction()

# --------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------
vix_cache_add_test(cache_smoke_test
  http/cache_smoke_test.cpp
)

vix_cache_add_test(cache_context_mapper_smoke_test
  http/cache_context_mapper_smoke_test.cpp
)

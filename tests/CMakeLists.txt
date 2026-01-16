#  @file CMakeLists.cpp
#  @author Gaspard Kirira
#
#  Copyright 2025, Gaspard Kirira.  All rights reserved.
#  https://github.com/vixcpp/vix
#  Use of this source code is governed by a MIT license
#  that can be found in the License file.
cmake_minimum_required(VERSION 3.20)

# Enable CTest
include(CTest)

if (BUILD_TESTING)
  enable_testing()
endif()

# Common test settings
set(VIX_CACHE_TEST_TARGET vix::cache)
if (NOT TARGET ${VIX_CACHE_TEST_TARGET} AND TARGET vix_cache)
  set(VIX_CACHE_TEST_TARGET vix_cache)
endif()

if (NOT TARGET ${VIX_CACHE_TEST_TARGET})
  message(FATAL_ERROR "[cache/tests] Missing cache target (expected vix::cache or vix_cache).")
endif()

# Cache smoke test
add_executable(cache_smoke_test
  http/cache_smoke_test.cpp
)

target_link_libraries(cache_smoke_test PRIVATE
  ${VIX_CACHE_TEST_TARGET}
)

target_compile_features(cache_smoke_test PRIVATE cxx_std_20)

if (BUILD_TESTING)
  add_test(
    NAME cache_smoke_test
    COMMAND cache_smoke_test
  )
endif()

# CacheContextMapper smoke test
add_executable(cache_context_mapper_smoke_test
  http/cache_context_mapper_smoke_test.cpp
)

target_link_libraries(cache_context_mapper_smoke_test PRIVATE
  ${VIX_CACHE_TEST_TARGET}
)

target_compile_features(cache_context_mapper_smoke_test PRIVATE cxx_std_20)

if (BUILD_TESTING)
  add_test(
    NAME cache_context_mapper_smoke_test
    COMMAND cache_context_mapper_smoke_test
  )
endif()

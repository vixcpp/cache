#  @file CMakeLists.cpp
#  @author Gaspard Kirira
#
#  Copyright 2025, Gaspard Kirira.  All rights reserved.
#  https://github.com/vixcpp/vix
#  Use of this source code is governed by a MIT license
#  that can be found in the License file.
#
# ====================================================================
# Vix.cpp — Cache Module
# ====================================================================
# Purpose:
#   HTTP caching engine for Vix. Provides pluggable cache stores,
#   cache policies, and keying strategies. Builds as STATIC when
#   sources exist, otherwise as a header-only INTERFACE target.
#
# Public Targets:
#   - vix_cache   : The actual library target (STATIC or INTERFACE)
#   - vix::cache  : Namespaced alias for consumers
#
# Public Dependencies:
#   - vix::net
#
# Installation/Export:
#   Installs into the umbrella export-set `VixTargets`.
# ====================================================================

cmake_minimum_required(VERSION 3.20)
project(vix_cache VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

# Global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Sources discovery
file(GLOB_RECURSE CACHE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Net dependency
option(VIX_CACHE_FETCH_NET "Auto-fetch vix::net if missing" ON)

if (NOT TARGET vix::net)
  if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../net/CMakeLists.txt")
    message(STATUS "[cache] Adding net from umbrella: ../net")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../net" "net")
  elseif (VIX_CACHE_FETCH_NET)
    include(FetchContent)
    message(STATUS "[cache] Fetching vix::net via FetchContent")
    FetchContent_Declare(vix_net
      GIT_REPOSITORY https://github.com/vixcpp/net.git
      GIT_TAG        v0.1.0
    )
    FetchContent_MakeAvailable(vix_net)
  else()
    message(FATAL_ERROR "vix::net not found. Enable VIX_CACHE_FETCH_NET=ON or provide the target before cache.")
  endif()
endif()

set(VIX_NET_TARGET vix::net)
if (NOT TARGET ${VIX_NET_TARGET} AND TARGET vix::net)
  set(VIX_NET_TARGET vix::net)
endif()

# STATIC
if (CACHE_SOURCES)
  message(STATUS "[cache] Building STATIC library with detected sources.")

  add_library(vix_cache STATIC ${CACHE_SOURCES})
  add_library(vix::cache ALIAS vix_cache)

  target_compile_features(vix_cache PUBLIC cxx_std_20)

  target_include_directories(vix_cache
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  target_link_libraries(vix_cache
    PUBLIC
      ${VIX_NET_TARGET}
  )

  if (TARGET vix_warnings)
    target_link_libraries(vix_cache PUBLIC vix_warnings)
    if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
      target_link_libraries(vix_cache INTERFACE vix_sanitizers)
    endif()
  endif()

  set_target_properties(vix_cache PROPERTIES
    OUTPUT_NAME vix_cache
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME cache
  )

  install(TARGETS vix_cache
    EXPORT VixTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
          FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

# HEADER-ONLY
else()
  message(STATUS "[cache] Building HEADER-ONLY library (no sources).")

  add_library(vix_cache INTERFACE)
  add_library(vix::cache ALIAS vix_cache)

  target_compile_features(vix_cache INTERFACE cxx_std_20)

  target_include_directories(vix_cache INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  target_link_libraries(vix_cache INTERFACE
    ${VIX_NET_TARGET}
  )

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_cache INTERFACE vix_sanitizers)
  endif()

  install(TARGETS vix_cache
    EXPORT VixTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
          FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
endif()

# Tests
option(VIX_CACHE_BUILD_TESTS "Build cache module tests" OFF)

if (VIX_CACHE_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()

# Summary
message(STATUS "------------------------------------------------------")
message(STATUS "vix::cache configured (${PROJECT_VERSION})")
if (CACHE_SOURCES)
  message(STATUS "Mode: STATIC / sources found")
else()
  message(STATUS "Mode: HEADER-ONLY / no sources")
endif()
message(STATUS "Net target:      ${VIX_NET_TARGET}")
message(STATUS "Include dir: ${CMAKE_CURRENT_SOURCE_DIR}/include (for <vix/cache/...>)")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")

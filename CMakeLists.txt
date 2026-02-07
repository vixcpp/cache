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

# --------------------------------------------------------------------
# Umbrella build policy:
# When building inside the Vix umbrella, dependencies are managed by the
# umbrella (add_subdirectory order + options). Cache must not FetchContent
# nor add sibling subdirectories.
# --------------------------------------------------------------------
if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  set(VIX_CACHE_FETCH_NET OFF CACHE BOOL "Auto-fetch vix::net if missing" FORCE)
endif()

# Sources discovery
file(GLOB_RECURSE CACHE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Net dependency
option(VIX_CACHE_FETCH_NET "Auto-fetch vix::net if missing" ON)

if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  # Umbrella owns dependency graph: never add_subdirectory/FetchContent here.
  if (NOT TARGET vix::net)
    message(FATAL_ERROR "[cache] Umbrella build: vix::net must be provided by umbrella before cache.")
  endif()
else()
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
endif()

set(VIX_NET_TARGET vix::net)

# --------------------------------------------------------------------
# JSON dependency (required by src/FileStore.cpp -> <nlohmann/json.hpp>)
# Policy:
#   - Umbrella: must provide nlohmann_json::nlohmann_json already (or via system).
#   - Standalone: try CONFIG package, optionally FetchContent if allowed.
# --------------------------------------------------------------------
option(VIX_CACHE_FETCH_NLOHMANN "Fetch nlohmann/json if missing (standalone only)" ON)

if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  set(VIX_CACHE_FETCH_NLOHMANN OFF CACHE BOOL "Fetch nlohmann/json if missing (standalone only)" FORCE)
endif()

set(VIX_NLOHMANN_TARGET "")

# First try CONFIG package
find_package(nlohmann_json CONFIG QUIET)
if (TARGET nlohmann_json::nlohmann_json)
  set(VIX_NLOHMANN_TARGET nlohmann_json::nlohmann_json)
endif()

# Standalone fallback: FetchContent
if (NOT VIX_NLOHMANN_TARGET)
  if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD) AND VIX_CACHE_FETCH_NLOHMANN)
    include(FetchContent)
    message(STATUS "[cache] Fetching nlohmann/json via FetchContent")
    FetchContent_Declare(nlohmann_json
      GIT_REPOSITORY https://github.com/nlohmann/json.git
      GIT_TAG        v3.11.3
    )
    FetchContent_MakeAvailable(nlohmann_json)

    if (TARGET nlohmann_json::nlohmann_json)
      set(VIX_NLOHMANN_TARGET nlohmann_json::nlohmann_json)
    endif()
  endif()
endif()

if (NOT VIX_NLOHMANN_TARGET)
  message(FATAL_ERROR
    "[cache] Missing dependency: nlohmann_json::nlohmann_json.\n"
    "Reason: src/FileStore.cpp includes <nlohmann/json.hpp>.\n"
    "Fix:\n"
    "  - Umbrella: ensure nlohmann_json is available before adding cache (system package or umbrella fallback).\n"
    "  - Standalone: install nlohmann_json or set VIX_CACHE_FETCH_NLOHMANN=ON.\n"
  )
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
      ${VIX_NLOHMANN_TARGET}
  )


  if (TARGET vix_warnings)
    target_link_libraries(vix_cache PUBLIC vix_warnings)
  endif()

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_cache PUBLIC vix_sanitizers)
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

  if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
  endif()


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
    ${VIX_NLOHMANN_TARGET}
  )

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_cache INTERFACE vix_sanitizers)
  endif()

  install(TARGETS vix_cache
    EXPORT VixTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
  endif()

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
message(STATUS "JSON target:     ${VIX_NLOHMANN_TARGET}")
message(STATUS "Net target:      ${VIX_NET_TARGET}")
message(STATUS "Include dir: ${CMAKE_CURRENT_SOURCE_DIR}/include (for <vix/cache/...>)")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")

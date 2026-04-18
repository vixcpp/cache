# ====================================================================
#  @file CMakeLists.txt
#  @author Gaspard Kirira
#
#  Copyright 2025, Gaspard Kirira.  All rights reserved.
#  https://github.com/vixcpp/vix
#  Use of this source code is governed by a MIT license
#  that can be found in the License file.
#
# ====================================================================
# Vix.cpp - Cache Module
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
#   - vix::json
#
# Installation/Export:
#   - Umbrella build exports into: VixTargets
#   - Standalone build exports into: vix_cacheTargets
# ====================================================================

cmake_minimum_required(VERSION 3.20)
project(vix_cache VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# --------------------------------------------------------------------
# Global settings
# --------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --------------------------------------------------------------------
# Export set selection
# --------------------------------------------------------------------
if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  set(VIX_CACHE_EXPORT_SET VixTargets)
else()
  set(VIX_CACHE_EXPORT_SET vix_cacheTargets)
endif()

# --------------------------------------------------------------------
# Umbrella build policy:
# When building inside the Vix umbrella, dependencies are managed by the
# umbrella (add_subdirectory order + options). Cache must not FetchContent
# nor add sibling subdirectories.
# --------------------------------------------------------------------
option(VIX_CACHE_FETCH_NET "Auto-fetch vix::net if missing" ON)

if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  set(VIX_CACHE_FETCH_NET OFF CACHE BOOL "Auto-fetch vix::net if missing" FORCE)
endif()

# --------------------------------------------------------------------
# Sources discovery
# --------------------------------------------------------------------
file(GLOB_RECURSE CACHE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# --------------------------------------------------------------------
# Net dependency
# Resolution policy:
#   1) Reuse already available targets
#   2) Try local sibling module
#   3) Try installed packages
#   4) Optional FetchContent as last resort
# --------------------------------------------------------------------
set(VIX_NET_TARGET "")

if (TARGET vix::net)
  message(STATUS "[cache] Using existing target vix::net")
  set(VIX_NET_TARGET vix::net)

elseif (TARGET vix_net)
  message(STATUS "[cache] Using existing target vix_net")
  set(VIX_NET_TARGET vix_net)

elseif (TARGET net)
  message(STATUS "[cache] Using existing target net")
  set(VIX_NET_TARGET net)

else()
  get_filename_component(_VIX_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
  set(_VIX_NET_DIR "${_VIX_MODULES_DIR}/net")

  if (EXISTS "${_VIX_NET_DIR}/CMakeLists.txt")
    message(STATUS "[cache] Adding local sibling net: ${_VIX_NET_DIR}")
    add_subdirectory("${_VIX_NET_DIR}" "${CMAKE_BINARY_DIR}/_vix_net")
  endif()

  if (TARGET vix::net)
    set(VIX_NET_TARGET vix::net)
  elseif (TARGET vix_net)
    set(VIX_NET_TARGET vix_net)
  elseif (TARGET net)
    set(VIX_NET_TARGET net)
  else()
    find_package(vix_net CONFIG QUIET)
    find_package(net CONFIG QUIET)

    if (TARGET vix::net)
      set(VIX_NET_TARGET vix::net)
    elseif (TARGET vix_net)
      set(VIX_NET_TARGET vix_net)
    elseif (TARGET net)
      set(VIX_NET_TARGET net)
    endif()
  endif()

  if (NOT VIX_NET_TARGET AND VIX_CACHE_FETCH_NET)
    include(FetchContent)
    message(STATUS "[cache] Fetching vix::net via FetchContent")
    FetchContent_Declare(vix_net
      GIT_REPOSITORY https://github.com/vixcpp/net.git
      GIT_TAG        v0.1.0
    )
    FetchContent_MakeAvailable(vix_net)

    if (TARGET vix::net)
      set(VIX_NET_TARGET vix::net)
    elseif (TARGET vix_net)
      set(VIX_NET_TARGET vix_net)
    elseif (TARGET net)
      set(VIX_NET_TARGET net)
    endif()
  endif()
endif()

if (NOT VIX_NET_TARGET)
  message(FATAL_ERROR
    "[cache] vix::net not found.\n"
    "Expected one of:\n"
    "  - existing target: vix::net / vix_net / net\n"
    "  - local sibling module in ../net\n"
    "  - installed package via find_package(vix_net CONFIG) or find_package(net CONFIG)\n"
  )
endif()

# --------------------------------------------------------------------
# JSON dependency
# Resolution policy:
#   1) Reuse already available targets
#   2) Try local sibling module
#   3) Try installed packages
# Notes:
#   - Cache depends on the Vix json module, not directly on nlohmann_json.
# --------------------------------------------------------------------
set(VIX_JSON_TARGET "")

if (TARGET vix::json)
  message(STATUS "[cache] Using existing target vix::json")
  set(VIX_JSON_TARGET vix::json)

elseif (TARGET vix_json)
  message(STATUS "[cache] Using existing target vix_json")
  set(VIX_JSON_TARGET vix_json)

elseif (TARGET json)
  message(STATUS "[cache] Using existing target json")
  set(VIX_JSON_TARGET json)

else()
  get_filename_component(_VIX_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
  set(_VIX_JSON_DIR "${_VIX_MODULES_DIR}/json")

  if (EXISTS "${_VIX_JSON_DIR}/CMakeLists.txt")
    message(STATUS "[cache] Adding local sibling json: ${_VIX_JSON_DIR}")
    add_subdirectory("${_VIX_JSON_DIR}" "${CMAKE_BINARY_DIR}/_vix_json")
  endif()

  if (TARGET vix::json)
    set(VIX_JSON_TARGET vix::json)
  elseif (TARGET vix_json)
    set(VIX_JSON_TARGET vix_json)
  elseif (TARGET json)
    set(VIX_JSON_TARGET json)
  else()
    find_package(vix_json CONFIG QUIET)
    find_package(json CONFIG QUIET)

    if (TARGET vix::json)
      set(VIX_JSON_TARGET vix::json)
    elseif (TARGET vix_json)
      set(VIX_JSON_TARGET vix_json)
    elseif (TARGET json)
      set(VIX_JSON_TARGET json)
    endif()
  endif()
endif()

if (NOT VIX_JSON_TARGET)
  message(FATAL_ERROR
    "[cache] vix::json not found.\n"
    "Expected one of:\n"
    "  - existing target: vix::json / vix_json / json\n"
    "  - local sibling module in ../json\n"
    "  - installed package via find_package(vix_json CONFIG) or find_package(json CONFIG)\n"
  )
endif()

# --------------------------------------------------------------------
# Library target
# --------------------------------------------------------------------
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
      ${VIX_JSON_TARGET}
  )

  # Build-only helpers must not leak into exported interfaces.
  if (TARGET vix_warnings)
    target_link_libraries(vix_cache PRIVATE
      $<BUILD_INTERFACE:vix_warnings>
    )
  endif()

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_cache PRIVATE
      $<BUILD_INTERFACE:vix_sanitizers>
    )
  endif()

  set_target_properties(vix_cache PROPERTIES
    OUTPUT_NAME vix_cache
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME cache
  )

  install(TARGETS vix_cache
    EXPORT ${VIX_CACHE_EXPORT_SET}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

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
    ${VIX_JSON_TARGET}
  )

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    # Do not export build-only sanitizer helpers from header-only package targets.
  endif()

  set_target_properties(vix_cache PROPERTIES
    EXPORT_NAME cache
  )

  install(TARGETS vix_cache
    EXPORT ${VIX_CACHE_EXPORT_SET}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
endif()

# --------------------------------------------------------------------
# Install headers
# --------------------------------------------------------------------
if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
      PATTERN "*.hpp"
      PATTERN "*.h"
  )
endif()

# --------------------------------------------------------------------
# Standalone package config/export
# Only for non-umbrella builds
# --------------------------------------------------------------------
if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))

  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/vix_cacheConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/vix_cacheConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_cache"
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/vix_cacheConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
  )

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/vix_cacheConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/vix_cacheConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_cache"
  )

  install(EXPORT vix_cacheTargets
    FILE vix_cacheTargets.cmake
    NAMESPACE vix::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_cache"
  )

endif()

# --------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------
option(VIX_CACHE_BUILD_TESTS "Build cache module tests" OFF)

if (VIX_CACHE_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()

# --------------------------------------------------------------------
# Summary
# --------------------------------------------------------------------
message(STATUS "------------------------------------------------------")
message(STATUS "vix::cache configured (${PROJECT_VERSION})")
if (CACHE_SOURCES)
  message(STATUS "Mode: STATIC / sources found")
else()
  message(STATUS "Mode: HEADER-ONLY / no sources")
endif()
message(STATUS "Export set:       ${VIX_CACHE_EXPORT_SET}")
message(STATUS "JSON target:      ${VIX_JSON_TARGET}")
message(STATUS "Net target:       ${VIX_NET_TARGET}")
message(STATUS "Include dir:      ${CMAKE_CURRENT_SOURCE_DIR}/include (for <vix/cache/...>)")
message(STATUS "Sanitizers:       ${VIX_ENABLE_SANITIZERS}")
message(STATUS "Build type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")

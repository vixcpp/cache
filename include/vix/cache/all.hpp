/**
 *
 *  @file cache/cache.hpp
 *  @author Gaspard Kirira
 *
 *  @brief Internal aggregation header for the Vix cache module.
 *
 *  This file includes all core caching components such as cache entries,
 *  policies, stores, context mapping, and utilities.
 *
 *  For most use cases, prefer:
 *    #include <vix/cache.hpp>
 *
 *  Copyright 2025, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/vix
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Vix.cpp
 */
#ifndef VIX_CACHE_CACHE_HPP
#define VIX_CACHE_CACHE_HPP

// Core
#include <vix/cache/Cache.hpp>
#include <vix/cache/CacheEntry.hpp>
#include <vix/cache/CacheKey.hpp>
#include <vix/cache/CachePolicy.hpp>

// Context
#include <vix/cache/CacheContext.hpp>
#include <vix/cache/CacheContextMapper.hpp>

// Stores
#include <vix/cache/CacheStore.hpp>
#include <vix/cache/MemoryStore.hpp>
#include <vix/cache/LruMemoryStore.hpp>
#include <vix/cache/FileStore.hpp>

// Utilities
#include <vix/cache/HeaderUtil.hpp>

#endif // VIX_CACHE_CACHE_HPP

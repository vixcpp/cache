/**
 *
 *  @file CacheStore.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2025, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/vix
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Vix.cpp
 *
 */
#ifndef VIX_CACHE_STORE_HPP
#define VIX_CACHE_STORE_HPP

#include <optional>
#include <string>

#include <vix/cache/CacheEntry.hpp>

namespace vix::cache
{
  /**
   * @brief Abstract storage backend for cache entries.
   *
   * CacheStore defines the minimal persistence interface required by
   * the Cache facade. Implementations may store entries in memory,
   * on disk, or in any other local storage.
   *
   * The interface is intentionally simple and synchronous to keep
   * cache behavior explicit and predictable.
   */
  class CacheStore
  {
  public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~CacheStore() = default;

    /**
     * @brief Insert or update a cache entry.
     *
     * @param key Cache key.
     * @param entry Cache entry to store.
     */
    virtual void put(const std::string &key, const CacheEntry &entry) = 0;

    /**
     * @brief Retrieve a cache entry by key.
     *
     * @param key Cache key.
     * @return Optional cache entry if found.
     */
    virtual std::optional<CacheEntry> get(const std::string &key) = 0;

    /**
     * @brief Remove a cache entry by key.
     *
     * @param key Cache key.
     */
    virtual void erase(const std::string &key) = 0;

    /**
     * @brief Clear all cache entries.
     */
    virtual void clear() = 0;
  };

} // namespace vix::cache

#endif // VIX_CACHE_STORE_HPP

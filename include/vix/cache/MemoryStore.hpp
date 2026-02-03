/**
 *
 *  @file MemoryStore.hpp
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
#ifndef VIX_MEMORY_STORE_HPP
#define VIX_MEMORY_STORE_HPP

#include <mutex>
#include <unordered_map>

#include <vix/cache/CacheStore.hpp>

namespace vix::cache
{
  /**
   * @brief Simple in-memory cache store.
   *
   * MemoryStore is the most minimal CacheStore implementation:
   * - stores entries in an in-memory hash map
   * - provides basic thread-safety via a mutex
   * - performs no eviction or persistence
   *
   * It is primarily intended for:
   * - testing and prototyping
   * - small caches with explicit lifecycle control
   * - scenarios where persistence and eviction are handled elsewhere
   */
  class MemoryStore final : public CacheStore
  {
  public:
    /**
     * @brief Construct an empty in-memory cache store.
     */
    MemoryStore() : CacheStore{}, mu_{}, map_{} {}

    /**
     * @brief Insert or update a cache entry.
     *
     * @param key Cache key.
     * @param entry Cache entry to store.
     */
    void put(const std::string &key, const CacheEntry &entry) override;

    /**
     * @brief Retrieve a cache entry by key.
     *
     * @param key Cache key.
     * @return Optional cache entry if found.
     */
    std::optional<CacheEntry> get(const std::string &key) override;

    /**
     * @brief Remove a cache entry by key.
     *
     * @param key Cache key.
     */
    void erase(const std::string &key) override;

    /**
     * @brief Clear all cache entries.
     */
    void clear() override;

  private:
    /**
     * @brief Mutex protecting the internal map.
     */
    std::mutex mu_;

    /**
     * @brief In-memory map of cache entries.
     */
    std::unordered_map<std::string, CacheEntry> map_;
  };

} // namespace vix::cache

#endif // VIX_MEMORY_STORE_HPP

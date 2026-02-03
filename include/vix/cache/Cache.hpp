/**
 *
 *  @file Cache.hpp
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
#ifndef VIX_CACHE_HPP
#define VIX_CACHE_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include <vix/cache/CacheContext.hpp>
#include <vix/cache/CachePolicy.hpp>
#include <vix/cache/CacheStore.hpp>

namespace vix::cache
{
  /**
   * @brief High-level cache facade with policy-driven behavior.
   *
   * Cache coordinates:
   * - a CacheStore responsible for persistence (memory, file, etc.)
   * - a CachePolicy defining expiration, validation and reuse rules
   * - a CacheContext carrying request-specific semantics
   *
   * This class is intentionally minimal and deterministic: it does not
   * perform I/O by itself and does not hide state transitions. All cache
   * decisions are explicit and reproducible.
   *
   * Typical use cases include HTTP GET caching, offline-first response
   * reuse, and edge/local cache layers.
   */
  class Cache
  {
  public:
    /**
     * @brief Construct a cache instance.
     *
     * @param policy Cache policy controlling expiration and reuse rules.
     * @param store Backing cache store implementation.
     */
    Cache(
        CachePolicy policy,
        std::shared_ptr<CacheStore> store);

    /**
     * @brief Retrieve an entry from the cache.
     *
     * The policy is consulted to determine whether the cached entry is
     * valid, stale-but-usable, or expired for the given context.
     *
     * @param key Cache key.
     * @param now_ms Current time in milliseconds.
     * @param ctx Cache context (request semantics).
     * @return Optional cache entry if usable.
     */
    std::optional<CacheEntry> get(
        const std::string &key,
        std::int64_t now_ms,
        CacheContext ctx);

    /**
     * @brief Insert or update a cache entry.
     *
     * @param key Cache key.
     * @param entry Cache entry to store.
     */
    void put(const std::string &key, const CacheEntry &entry);

    /**
     * @brief Prune expired cache entries.
     *
     * @param now_ms Current time in milliseconds.
     * @return Number of entries removed.
     */
    std::size_t prune(std::int64_t now_ms);

  private:
    /**
     * @brief Cache policy controlling validation and expiration.
     */
    CachePolicy policy_;

    /**
     * @brief Underlying cache store.
     */
    std::shared_ptr<CacheStore> store_;
  };

} // namespace vix::cache

#endif // VIX_CACHE_HPP

/**
 *
 *  @file CachePolicy.hpp
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
#ifndef VIX_CACHE_POLICY_HPP
#define VIX_CACHE_POLICY_HPP

#include <cstdint>

namespace vix::cache
{
  /**
   * @brief Cache freshness and staleness policy.
   *
   * CachePolicy defines the rules used to decide whether a cached entry
   * can be served, reused, or considered expired.
   *
   * The policy supports:
   * - a strict freshness window (TTL)
   * - controlled reuse of stale entries on network error
   * - controlled reuse of stale entries when offline
   *
   * All decisions are based on entry age (now - created_at).
   */
  struct CachePolicy
  {
    /**
     * @brief Time-to-live in milliseconds.
     *
     * Entries younger than this value are considered fresh.
     */
    std::int64_t ttl_ms{60'000}; // 1 min

    /**
     * @brief Maximum age for serving stale entries on network error.
     */
    std::int64_t stale_if_error_ms{5 * 60'000}; // 5 min

    /**
     * @brief Maximum age for serving stale entries when offline.
     */
    std::int64_t stale_if_offline_ms{10 * 60'000};

    /**
     * @brief Whether stale entries may be served on network error.
     */
    bool allow_stale_if_error{true};

    /**
     * @brief Whether stale entries may be served when offline.
     */
    bool allow_stale_if_offline{true};

    /**
     * @brief Check whether an entry is still fresh.
     *
     * @param age_ms Age of the cache entry in milliseconds.
     * @return true if the entry is fresh.
     */
    bool is_fresh(std::int64_t age_ms) const noexcept
    {
      return age_ms <= ttl_ms;
    }

    /**
     * @brief Check whether a stale entry may be served after a network error.
     *
     * @param age_ms Age of the cache entry in milliseconds.
     * @return true if serving stale-on-error is allowed.
     */
    bool allow_stale_error(std::int64_t age_ms) const noexcept
    {
      return allow_stale_if_error && age_ms <= stale_if_error_ms;
    }

    /**
     * @brief Check whether a stale entry may be served while offline.
     *
     * @param age_ms Age of the cache entry in milliseconds.
     * @return true if serving stale-while-offline is allowed.
     */
    bool allow_stale_offline(std::int64_t age_ms) const noexcept
    {
      return allow_stale_if_offline && age_ms <= stale_if_offline_ms;
    }
  };

} // namespace vix::cache

#endif // VIX_CACHE_POLICY_HPP

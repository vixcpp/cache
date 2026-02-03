/**
 *
 *  @file CacheContext.hpp
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
#ifndef VIX_CACHE_CONTEXT_HPP
#define VIX_CACHE_CONTEXT_HPP

namespace vix::cache
{
  /**
   * @brief Context describing runtime conditions for cache decisions.
   *
   * CacheContext captures transient environmental information that influences
   * how cache entries are interpreted and reused.
   *
   * Typical signals include:
   * - offline: no network connectivity is available
   * - network_error: a network request was attempted but failed
   *
   * CachePolicy implementations may use this context to allow serving stale
   * entries, bypass cache, or adjust validation rules.
   */
  struct CacheContext
  {
    /**
     * @brief True when no network connectivity is available.
     */
    bool offline{false};

    /**
     * @brief True when a request failed due to network issues.
     */
    bool network_error{false};

    /**
     * @brief Context for normal online operation.
     */
    static CacheContext Online() noexcept { return {}; }

    /**
     * @brief Context indicating offline operation.
     */
    static CacheContext Offline() noexcept { return {.offline = true}; }

    /**
     * @brief Context indicating a network error occurred.
     */
    static CacheContext NetworkError() noexcept { return {.network_error = true}; }
  };

} // namespace vix::cache

#endif // VIX_CACHE_CONTEXT_HPP

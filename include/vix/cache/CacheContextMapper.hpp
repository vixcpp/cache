/**
 *
 *  @file CacheContextMapper.hpp
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
#ifndef VIX_CACHE_CONTEXT_MAPPER_HPP
#define VIX_CACHE_CONTEXT_MAPPER_HPP

#include <cstdint>

#include <vix/cache/CacheContext.hpp>
#include <vix/net/NetworkProbe.hpp>

namespace vix::cache
{
  /**
   * @brief Outcome of a network-backed request.
   *
   * This enum is used to enrich cache context with information
   * about how a request terminated.
   */
  enum class RequestOutcome
  {
    /**
     * @brief Request completed successfully.
     */
    Ok,

    /**
     * @brief Request failed due to a network error.
     */
    NetworkError
  };

  /**
   * @brief Build a CacheContext from a NetworkProbe.
   *
   * The resulting context reflects whether the system is currently
   * considered offline according to the probe.
   *
   * @param probe Network probe instance.
   * @param now_ms Current time in milliseconds.
   * @return CacheContext derived from network state.
   */
  inline CacheContext contextFromProbe(
      const vix::net::NetworkProbe &probe,
      std::int64_t now_ms)
  {
    CacheContext ctx{};
    if (!probe.isOnline(now_ms))
    {
      ctx.offline = true;
    }
    return ctx;
  }

  /**
   * @brief Build a CacheContext from a NetworkProbe and request outcome.
   *
   * Extends contextFromProbe() by marking network_error when the
   * request explicitly failed due to network issues.
   *
   * @param probe Network probe instance.
   * @param now_ms Current time in milliseconds.
   * @param outcome Result of the network request.
   * @return CacheContext derived from network state and outcome.
   */
  inline CacheContext contextFromProbeAndOutcome(
      const vix::net::NetworkProbe &probe,
      std::int64_t now_ms,
      RequestOutcome outcome)
  {
    CacheContext ctx = contextFromProbe(probe, now_ms);
    if (outcome == RequestOutcome::NetworkError)
    {
      ctx.network_error = true;
    }
    return ctx;
  }

  /**
   * @brief Convenience helper for an offline cache context.
   */
  inline CacheContext contextOffline() noexcept { return CacheContext::Offline(); }

  /**
   * @brief Convenience helper for an online cache context.
   */
  inline CacheContext contextOnline() noexcept { return CacheContext::Online(); }

  /**
   * @brief Convenience helper for a network-error cache context.
   */
  inline CacheContext contextNetworkError() noexcept { return CacheContext::NetworkError(); }

} // namespace vix::cache

#endif // VIX_CACHE_CONTEXT_MAPPER_HPP

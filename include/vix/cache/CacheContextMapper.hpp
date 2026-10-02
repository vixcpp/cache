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

#include <vix/cache/CacheContext.hpp>

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
   * @brief Build a CacheContext from current connectivity state.
   *
   * @param online True when the caller considers connectivity available.
   * @return CacheContext derived from the supplied connectivity state.
   */
  inline CacheContext contextFromConnectivity(bool online) noexcept
  {
    CacheContext ctx{};
    if (!online)
    {
      ctx.offline = true;
    }
    return ctx;
  }

  /**
   * @brief Build a CacheContext from connectivity state and request outcome.
   *
   * Extends contextFromConnectivity() by marking network_error when the
   * request explicitly failed due to network issues.
   *
   * @param online True when the caller considers connectivity available.
   * @param outcome Result of the network request.
   * @return CacheContext derived from network state and outcome.
   */
  inline CacheContext contextFromConnectivityAndOutcome(
      bool online,
      RequestOutcome outcome) noexcept
  {
    CacheContext ctx = contextFromConnectivity(online);

    if (outcome == RequestOutcome::NetworkError)
    {
      ctx.network_error = true;
    }

    return ctx;
  }

  /**
   * @brief Convenience helper for an offline cache context.
   */
  inline CacheContext contextOffline() noexcept
  {
    return CacheContext::Offline();
  }

  /**
   * @brief Convenience helper for an online cache context.
   */
  inline CacheContext contextOnline() noexcept
  {
    return CacheContext::Online();
  }

  /**
   * @brief Convenience helper for a network-error cache context.
   */
  inline CacheContext contextNetworkError() noexcept
  {
    return CacheContext::NetworkError();
  }

} // namespace vix::cache

#endif // VIX_CACHE_CONTEXT_MAPPER_HPP

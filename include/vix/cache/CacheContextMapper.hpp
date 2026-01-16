/**
 *
 *  @file CacheContextMapper.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2025, Gaspard Kirira.  All rights reserved.
 *  https://github.com/vixcpp/vix
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
  enum class RequestOutcome
  {
    Ok,
    NetworkError
  };

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

  inline CacheContext contextOffline() noexcept { return CacheContext::Offline(); }
  inline CacheContext contextOnline() noexcept { return CacheContext::Online(); }
  inline CacheContext contextNetworkError() noexcept { return CacheContext::NetworkError(); }

} // namespace vix::cache

#endif

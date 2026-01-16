/**
 *
 *  @file CacheContext.hpp
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
#ifndef VIX_CACHE_CONTEXT_HPP
#define VIX_CACHE_CONTEXT_HPP

namespace vix::cache
{

  struct CacheContext
  {
    bool offline{false};       // no network
    bool network_error{false}; // request failed due to network issues

    static CacheContext Online() noexcept { return {}; }
    static CacheContext Offline() noexcept { return {.offline = true}; }
    static CacheContext NetworkError() noexcept { return {.network_error = true}; }
  };

} // namespace vix::cache

#endif

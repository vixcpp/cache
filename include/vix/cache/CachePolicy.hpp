/**
 *
 *  @file CachePolicy.hpp
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
#ifndef VIX_CACHE_POLICY_HPP
#define VIX_CACHE_POLICY_HPP

#include <cstdint>

namespace vix::cache
{
  struct CachePolicy
  {
    std::int64_t ttl_ms{60'000};                // 1 min
    std::int64_t stale_if_error_ms{5 * 60'000}; // 5 min
    std::int64_t stale_if_offline_ms{10 * 60'000};
    bool allow_stale_if_error{true};
    bool allow_stale_if_offline{true};

    bool is_fresh(std::int64_t age_ms) const noexcept
    {
      return age_ms <= ttl_ms;
    }

    bool allow_stale_error(std::int64_t age_ms) const noexcept
    {
      return allow_stale_if_error && age_ms <= stale_if_error_ms;
    }

    bool allow_stale_offline(std::int64_t age_ms) const noexcept
    {
      return allow_stale_if_offline && age_ms <= stale_if_offline_ms;
    }
  };

} // namespace vix::cache

#endif

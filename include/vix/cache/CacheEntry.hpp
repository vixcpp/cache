/**
 *
 *  @file CacheEntry.hpp
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
#ifndef VIX_CACHE_ENTRY_HPP
#define VIX_CACHE_ENTRY_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

namespace vix::cache
{

  struct CacheEntry
  {
    int status{200};
    std::string body;
    std::unordered_map<std::string, std::string> headers;

    std::int64_t created_at_ms{0};
  };

} // namespace vix::cache

#endif

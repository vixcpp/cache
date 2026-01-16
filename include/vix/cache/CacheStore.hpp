/**
 *
 *  @file CacheStore.hpp
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
#ifndef VIX_CACHE_STORE_HPP
#define VIX_CACHE_STORE_HPP

#include <optional>
#include <string>

#include <vix/cache/CacheEntry.hpp>

namespace vix::cache
{

  class CacheStore
  {
  public:
    virtual ~CacheStore() = default;
    virtual void put(const std::string &key, const CacheEntry &entry) = 0;
    virtual std::optional<CacheEntry> get(const std::string &key) = 0;
    virtual void erase(const std::string &key) = 0;
    virtual void clear() = 0;
  };

} // namespace vix::cache

#endif

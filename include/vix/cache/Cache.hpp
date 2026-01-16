/**
 *
 *  @file Cache.hpp
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
#ifndef VIX_CACHE_HPP
#define VIX_CACHE_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <cstddef>

#include <vix/cache/CachePolicy.hpp>
#include <vix/cache/CacheStore.hpp>
#include <vix/cache/CacheContext.hpp>

namespace vix::cache
{

  class Cache
  {
  public:
    Cache(CachePolicy policy,
          std::shared_ptr<CacheStore> store);
    std::optional<CacheEntry> get(
        const std::string &key,
        std::int64_t now_ms,
        CacheContext ctx);
    void put(const std::string &key, const CacheEntry &entry);
    std::size_t prune(std::int64_t now_ms);

  private:
    CachePolicy policy_;
    std::shared_ptr<CacheStore> store_;
  };

} // namespace vix::cache

#endif

/**
 *
 *  @file MemoryStore.hpp
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
#ifndef VIX_MEMORY_STORE_HPP
#define VIX_MEMORY_STORE_HPP

#include <mutex>
#include <unordered_map>

#include <vix/cache/CacheStore.hpp>

namespace vix::cache
{

  class MemoryStore final : public CacheStore
  {
  public:
    MemoryStore() : CacheStore{}, mu_{}, map_{} {}

    void put(const std::string &key, const CacheEntry &entry) override;
    std::optional<CacheEntry> get(const std::string &key) override;
    void erase(const std::string &key) override;
    void clear() override;

  private:
    std::mutex mu_;
    std::unordered_map<std::string, CacheEntry> map_;
  };
} // namespace vix::cache

#endif

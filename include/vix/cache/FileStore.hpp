/**
 *
 *  @file FileStore.hpp
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
#ifndef VIX_FILE_STORE_HPP
#define VIX_FILE_STORE_HPP

#include <filesystem>
#include <mutex>
#include <unordered_map>

#include <vix/cache/CacheStore.hpp>

namespace vix::cache
{

  class FileStore final : public CacheStore
  {
  public:
    struct Config
    {
      std::filesystem::path file_path{"./.vix/cache_http.json"};
      bool pretty_json{false};
    };

    explicit FileStore(Config cfg);
    void put(const std::string &key, const CacheEntry &entry) override;
    std::optional<CacheEntry> get(const std::string &key) override;
    void erase(const std::string &key) override;
    void clear() override;

    template <typename Pred>
    std::size_t eraseIf(Pred pred)
    {
      std::lock_guard<std::mutex> lk(mu_);
      load_();

      std::size_t removed = 0;
      for (auto it = map_.begin(); it != map_.end();)
      {
        if (pred(it->second))
        {
          it = map_.erase(it);
          ++removed;
        }
        else
        {
          ++it;
        }
      }

      if (removed > 0)
      {
        flush_();
      }

      return removed;
    }

  private:
    void load_();
    void flush_();

  private:
    Config cfg_;
    bool loaded_{false};
    std::mutex mu_;
    std::unordered_map<std::string, CacheEntry> map_;
  };

} // namespace vix::cache

#endif

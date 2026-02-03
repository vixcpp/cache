/**
 *
 *  @file LruMemoryStore.hpp
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
#ifndef VIX_LRU_MEMORY_STORE_HPP
#define VIX_LRU_MEMORY_STORE_HPP

#include <cstdint>
#include <list>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include <vix/cache/CacheEntry.hpp>
#include <vix/cache/CacheStore.hpp>

namespace vix::cache
{
  /**
   * @brief In-memory LRU (Least Recently Used) cache store.
   *
   * LruMemoryStore is a CacheStore implementation that keeps cache entries
   * in memory with an eviction policy based on recency of access.
   *
   * Characteristics:
   * - O(1) average put/get/erase
   * - fixed maximum number of entries (max_entries)
   * - thread-safe via a single mutex
   *
   * This store is ideal for fast hot-path caching where persistence is not
   * required.
   */
  class LruMemoryStore final : public CacheStore
  {
  public:
    /**
     * @brief Configuration for the LRU memory store.
     */
    struct Config
    {
      /**
       * @brief Maximum number of entries to keep in memory.
       */
      std::size_t max_entries{1024};
    };

    /**
     * @brief Construct a LruMemoryStore.
     *
     * @param cfg Store configuration.
     */
    explicit LruMemoryStore(Config cfg) : cfg_(cfg) {}

    /**
     * @brief Insert or update a cache entry.
     *
     * Updates the entry if it already exists and marks it as most recently used.
     *
     * @param key Cache key.
     * @param entry Cache entry to store.
     */
    void put(const std::string &key, const CacheEntry &entry) override
    {
      std::lock_guard<std::mutex> lk(mu_);

      auto it = map_.find(key);
      if (it != map_.end())
      {
        it->second.entry = entry;
        touch_(it);
        return;
      }

      lru_.push_front(key);
      Node n;
      n.entry = entry;
      n.it = lru_.begin();
      map_.emplace(key, std::move(n));

      evictIfNeeded_();
    }

    /**
     * @brief Retrieve a cache entry by key.
     *
     * Marks the entry as most recently used if found.
     *
     * @param key Cache key.
     * @return Optional cache entry if found.
     */
    std::optional<CacheEntry> get(const std::string &key) override
    {
      std::lock_guard<std::mutex> lk(mu_);
      auto it = map_.find(key);
      if (it == map_.end())
        return std::nullopt;

      touch_(it);
      return it->second.entry;
    }

    /**
     * @brief Remove a cache entry by key.
     *
     * @param key Cache key.
     */
    void erase(const std::string &key) override
    {
      std::lock_guard<std::mutex> lk(mu_);
      auto it = map_.find(key);
      if (it == map_.end())
        return;

      lru_.erase(it->second.it);
      map_.erase(it);
    }

    /**
     * @brief Clear all entries.
     */
    void clear() override
    {
      std::lock_guard<std::mutex> lk(mu_);
      map_.clear();
      lru_.clear();
    }

    /**
     * @brief Erase entries matching a predicate.
     *
     * @tparam Pred Unary predicate taking a const CacheEntry&.
     * @param pred Predicate used to decide which entries to remove.
     * @return Number of entries removed.
     */
    template <typename Pred>
    std::size_t eraseIf(Pred pred)
    {
      std::lock_guard<std::mutex> lk(mu_);
      std::size_t removed = 0;

      for (auto it = map_.begin(); it != map_.end();)
      {
        if (pred(it->second.entry))
        {
          lru_.erase(it->second.it);
          it = map_.erase(it);
          ++removed;
        }
        else
        {
          ++it;
        }
      }

      return removed;
    }

  private:
    /**
     * @brief Node stored in the hash map.
     *
     * Stores the entry and an iterator pointing to its position in the LRU list.
     */
    struct Node
    {
      CacheEntry entry{};
      std::list<std::string>::iterator it{};
    };

    /**
     * @brief Mark an entry as most recently used.
     */
    void touch_(typename std::unordered_map<std::string, Node>::iterator it)
    {
      lru_.splice(lru_.begin(), lru_, it->second.it);
      it->second.it = lru_.begin();
    }

    /**
     * @brief Evict least recently used entries until within limit.
     */
    void evictIfNeeded_()
    {
      while (map_.size() > cfg_.max_entries && !lru_.empty())
      {
        const std::string &victim = lru_.back();
        auto it = map_.find(victim);
        if (it != map_.end())
          map_.erase(it);
        lru_.pop_back();
      }
    }

  private:
    /**
     * @brief Stored configuration.
     */
    Config cfg_;

    /**
     * @brief Mutex protecting internal state.
     */
    std::mutex mu_;

    /**
     * @brief LRU list storing keys, front = most recently used.
     */
    std::list<std::string> lru_;

    /**
     * @brief Hash map from key to node data.
     */
    std::unordered_map<std::string, Node> map_;
  };

} // namespace vix::cache

#endif // VIX_LRU_MEMORY_STORE_HPP

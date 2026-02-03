/**
 *
 *  @file FileStore.hpp
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
#ifndef VIX_FILE_STORE_HPP
#define VIX_FILE_STORE_HPP

#include <filesystem>
#include <mutex>
#include <unordered_map>

#include <vix/cache/CacheStore.hpp>

namespace vix::cache
{
  /**
   * @brief File-backed cache store implementation.
   *
   * FileStore persists cache entries into a single JSON file on disk.
   * It is intended as a simple, durable cache backend suitable for:
   * - offline-first HTTP caching
   * - crash-safe local reuse of responses
   * - environments without external storage dependencies
   *
   * The store loads its content lazily on first access and keeps an
   * in-memory map protected by a mutex. Any mutation triggers a flush
   * back to disk.
   *
   * @note This implementation prioritizes correctness and simplicity
   * over write throughput. For large caches or high concurrency,
   * alternative backends may be preferable.
   */
  class FileStore final : public CacheStore
  {
  public:
    /**
     * @brief Configuration for FileStore.
     */
    struct Config
    {
      /**
       * @brief Path to the JSON file used for persistence.
       */
      std::filesystem::path file_path{"./.vix/cache_http.json"};

      /**
       * @brief Whether to pretty-print the JSON output.
       *
       * Useful for debugging and inspection, but increases file size.
       */
      bool pretty_json{false};
    };

    /**
     * @brief Construct a file-backed cache store.
     *
     * The underlying file is not read immediately; loading occurs lazily
     * on first access.
     *
     * @param cfg Store configuration.
     */
    explicit FileStore(Config cfg);

    /**
     * @brief Insert or update a cache entry.
     *
     * @param key Cache key.
     * @param entry Cache entry to store.
     */
    void put(const std::string &key, const CacheEntry &entry) override;

    /**
     * @brief Retrieve a cache entry by key.
     *
     * @param key Cache key.
     * @return Optional cache entry if found.
     */
    std::optional<CacheEntry> get(const std::string &key) override;

    /**
     * @brief Remove a cache entry by key.
     *
     * @param key Cache key.
     */
    void erase(const std::string &key) override;

    /**
     * @brief Clear all cache entries.
     *
     * Removes all entries from memory and persists the empty state.
     */
    void clear() override;

    /**
     * @brief Erase cache entries matching a predicate.
     *
     * This helper allows bulk removal based on custom logic
     * (e.g. expiration, size constraints).
     *
     * @tparam Pred Unary predicate taking a const CacheEntry&.
     * @param pred Predicate used to decide which entries to remove.
     * @return Number of entries removed.
     */
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
    /**
     * @brief Load the cache file into memory if not already loaded.
     */
    void load_();

    /**
     * @brief Flush the in-memory cache state back to disk.
     */
    void flush_();

  private:
    /**
     * @brief Stored configuration.
     */
    Config cfg_;

    /**
     * @brief Whether the cache file has been loaded.
     */
    bool loaded_{false};

    /**
     * @brief Mutex protecting internal state.
     */
    std::mutex mu_;

    /**
     * @brief In-memory map of cache entries.
     */
    std::unordered_map<std::string, CacheEntry> map_;
  };

} // namespace vix::cache

#endif // VIX_FILE_STORE_HPP

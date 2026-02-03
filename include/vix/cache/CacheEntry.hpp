/**
 *
 *  @file CacheEntry.hpp
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
#ifndef VIX_CACHE_ENTRY_HPP
#define VIX_CACHE_ENTRY_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

namespace vix::cache
{
  /**
   * @brief Single cached response entry.
   *
   * CacheEntry represents the stored result of a request, typically
   * an HTTP GET. It contains both the payload and minimal metadata
   * required by cache policies.
   */
  struct CacheEntry
  {
    /**
     * @brief Response status code.
     */
    int status{200};

    /**
     * @brief Response body.
     */
    std::string body;

    /**
     * @brief Response headers.
     */
    std::unordered_map<std::string, std::string> headers;

    /**
     * @brief Creation timestamp in milliseconds.
     */
    std::int64_t created_at_ms{0};
  };

} // namespace vix::cache

#endif // VIX_CACHE_ENTRY_HPP

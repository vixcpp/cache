/**
 *
 *  @file HeaderUtil.hpp
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
#ifndef VIX_HEADER_UTIL_HPP
#define VIX_HEADER_UTIL_HPP

#include <cctype>
#include <string>
#include <unordered_map>

namespace vix::cache
{
  /**
   * @brief Utilities for normalizing and handling HTTP headers.
   *
   * HeaderUtil provides small helper functions used by the cache layer
   * to normalize header names in a deterministic way.
   *
   * The main goal is to avoid cache key inconsistencies caused by
   * case differences in HTTP header names.
   */
  struct HeaderUtil
  {
    /**
     * @brief Convert a string to lowercase.
     *
     * This helper is typically used to normalize HTTP header names.
     *
     * @param s Input string.
     * @return Lowercased version of the input.
     */
    static std::string toLower(const std::string &s)
    {
      std::string out;
      out.reserve(s.size());
      for (char c : s)
        out.push_back(static_cast<char>(
            std::tolower(static_cast<unsigned char>(c))));
      return out;
    }

    /**
     * @brief Normalize header names in-place.
     *
     * All header names are converted to lowercase. If multiple headers
     * differ only by case, the last one wins.
     *
     * This function is useful before building cache keys or applying
     * cache policies that rely on header lookup.
     *
     * @param headers Map of header name to value, modified in-place.
     */
    static void normalizeInPlace(
        std::unordered_map<std::string, std::string> &headers)
    {
      std::unordered_map<std::string, std::string> norm;
      norm.reserve(headers.size());

      for (const auto &kv : headers)
      {
        norm[toLower(kv.first)] = kv.second; // last-wins
      }
      headers.swap(norm);
    }
  };

} // namespace vix::cache

#endif // VIX_HEADER_UTIL_HPP

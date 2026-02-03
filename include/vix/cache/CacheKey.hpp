/**
 *
 *  @file CacheKey.hpp
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
#ifndef VIX_CACHE_KEY_HPP
#define VIX_CACHE_KEY_HPP

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace vix::cache
{
  /**
   * @brief Deterministic cache key builder for request-based caching.
   *
   * CacheKey generates a stable string key from request components
   * such as method, path, query parameters, and selected headers.
   *
   * Design goals:
   * - Deterministic: same logical request yields the same key
   * - Order-insensitive for query parameters
   * - Case-normalized for HTTP method and header names
   * - Explicit control over which headers influence the cache key
   *
   * This is primarily intended for HTTP GET caching and offline-first
   * cache reuse.
   */
  class CacheKey
  {
  public:
    /**
     * @brief Build a cache key from request components.
     *
     * The resulting key has the following normalized form:
     *
     *   METHOD path?sorted_query |h:header=value;header=value;
     *
     * - METHOD is uppercased
     * - Query parameters are sorted lexicographically
     * - Header names are lowercased and trimmed
     *
     * @param method HTTP method (e.g. "GET").
     * @param path Request path.
     * @param query Raw query string (without '?').
     * @param headers Request headers.
     * @param include_headers List of header names to include in the key.
     * @return Deterministic cache key string.
     */
    static std::string fromRequest(
        std::string_view method,
        std::string_view path,
        std::string_view query,
        const std::unordered_map<std::string, std::string> &headers,
        const std::vector<std::string> &include_headers = {})
    {
      std::string m = upper_(method);
      std::string p(path);
      std::string q = normalizeQuery_(query);

      std::string key;
      key.reserve(64 + p.size() + q.size());

      key += m;
      key += " ";
      key += p;

      if (!q.empty())
      {
        key += "?";
        key += q;
      }

      if (!include_headers.empty())
      {
        key += " |h:";
        for (const auto &name : include_headers)
        {
          const std::string hn = lower_(name);
          auto it = headers.find(name);
          if (it == headers.end())
          {
            auto it2 = headers.find(hn);
            if (it2 != headers.end())
            {
              key += hn;
              key += "=";
              key += trim_(it2->second);
              key += ";";
            }
            continue;
          }

          key += hn;
          key += "=";
          key += trim_(it->second);
          key += ";";
        }
      }

      return key;
    }

  private:
    /**
     * @brief Convert a string to lowercase.
     */
    static std::string lower_(std::string_view s)
    {
      std::string out;
      out.reserve(s.size());
      for (char c : s)
        out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
      return out;
    }

    /**
     * @brief Convert a string to uppercase.
     */
    static std::string upper_(std::string_view s)
    {
      std::string out;
      out.reserve(s.size());
      for (char c : s)
        out.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
      return out;
    }

    /**
     * @brief Trim leading and trailing whitespace from a string.
     */
    static std::string trim_(const std::string &v)
    {
      std::size_t b = 0;
      while (b < v.size() && std::isspace(static_cast<unsigned char>(v[b])))
        ++b;

      std::size_t e = v.size();
      while (e > b && std::isspace(static_cast<unsigned char>(v[e - 1])))
        --e;

      return v.substr(b, e - b);
    }

    /**
     * @brief Normalize a query string into a deterministic form.
     *
     * Query parameters are split, sorted by key/value, and
     * reassembled to ensure order-independent equivalence.
     */
    static std::string normalizeQuery_(std::string_view query)
    {
      if (query.empty())
        return {};

      std::vector<std::pair<std::string, std::string>> items;
      items.reserve(8);

      std::size_t i = 0;
      while (i < query.size())
      {
        std::size_t amp = query.find('&', i);
        if (amp == std::string_view::npos)
          amp = query.size();

        std::string_view part = query.substr(i, amp - i);
        std::size_t eq = part.find('=');

        std::string k;
        std::string v;

        if (eq == std::string_view::npos)
        {
          k = std::string(part);
          v = "";
        }
        else
        {
          k = std::string(part.substr(0, eq));
          v = std::string(part.substr(eq + 1));
        }

        items.emplace_back(std::move(k), std::move(v));
        i = amp + 1;
      }

      std::sort(items.begin(), items.end(), [](const auto &a, const auto &b)
                {
                  if (a.first != b.first)
                    return a.first < b.first;
                  return a.second < b.second; });

      std::ostringstream oss;
      for (std::size_t idx = 0; idx < items.size(); ++idx)
      {
        if (idx)
          oss << "&";
        oss << items[idx].first;
        if (!items[idx].second.empty())
          oss << "=" << items[idx].second;
      }
      return oss.str();
    }
  };

} // namespace vix::cache

#endif // VIX_CACHE_KEY_HPP

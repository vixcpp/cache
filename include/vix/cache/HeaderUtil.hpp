/**
 *
 *  @file HeaderUtil.hpp
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
#ifndef VIX_HEADER_UTIL_HPP
#define VIX_HEADER_UTIL_HPP

#include <cctype>
#include <string>
#include <unordered_map>

namespace vix::cache
{
  struct HeaderUtil
  {
    static std::string toLower(const std::string &s)
    {
      std::string out;
      out.reserve(s.size());
      for (char c : s)
        out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
      return out;
    }

    static void normalizeInPlace(std::unordered_map<std::string, std::string> &headers)
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

#endif

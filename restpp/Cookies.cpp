/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "Cookies.h"

namespace librestpp {
  std::map<std::string, std::string> getCookies(const std::string& cookieHeader) {
		std::map<std::string, std::string> result;
    // Not the most elegant thing in the world
    // TODO: confirm cookie parsing is correct (I did it from memory)
    size_t start = 0;
    while (start < cookieHeader.size()) {
      size_t semicolon = cookieHeader.find_first_of(';', start);
      size_t equals = cookieHeader.find_first_of('=', start);
      if (equals > semicolon) {
        break;
      }
      std::string key = cookieHeader.substr(start, equals - start);
      size_t valueStart = equals + 1;
      std::string value = cookieHeader.substr(valueStart, semicolon - valueStart);
      result[key] = value;
      size_t oldStart = start;
      start = semicolon + 1;
      if (start <= oldStart) {
        // We've looped over the size of size_t (probably because semicolon wasn't found)
        start = cookieHeader.size();
      }
      while (start < cookieHeader.size() && cookieHeader[start] == ' ') {
        start++;
      }
    }
    return result;
  }
}

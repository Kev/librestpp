/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <string>
#include <boost/optional.hpp>
#include <map>

namespace librestpp {
  std::map<std::string, std::string> getCookies(const std::string& cookieHeader);
}

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

namespace librestpp {
  template <class T>
	class SessionCollection {
		public:
			SessionCollection();
			virtual ~SessionCollection();

			virtual boost:shared_ptr<T> getSession(const std::string& sessionKey) = 0;
	};
}

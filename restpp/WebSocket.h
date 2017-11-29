/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include "JSON.h"

namespace librestpp {
	class WebSocket {
		public:
			typedef std::shared_ptr<WebSocket> ref;
			WebSocket();

			virtual void send(const std::string& message) = 0;
			void send(std::shared_ptr<JSONObject> json);
			virtual boost::optional<std::string> getHeader(const std::string& header) = 0;
			boost::optional<std::string> getCookie(const std::string& key);
		public:
			boost::signals2::signal<void(std::shared_ptr<JSONObject>)> onMessage;
			boost::signals2::signal<void()> onClosed;
		protected:
			virtual void handleMessage(const std::string& message);
	};
}

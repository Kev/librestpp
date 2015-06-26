/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include "JSON.h"

namespace librestpp {
	class WebSocket {
		public:
			typedef boost::shared_ptr<WebSocket> ref;
			WebSocket();

			virtual void send(const std::string& message) = 0;
			void send(boost::shared_ptr<JSONObject> json);
		public:
			boost::signals2::signal<void(boost::shared_ptr<JSONObject>)> onMessage;
		protected:
			virtual void handleMessage(const std::string& message);
	};
}

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

namespace librestpp {
  class RESTRequest;
  class WebSocket;
	class ServerDriver {
		public:
			boost::signals2::signal<void(std::shared_ptr<WebSocket>)> onWebSocketConnection;
      boost::signals2::signal<void(std::shared_ptr<RESTRequest>)> onRESTRequest;
	};
}

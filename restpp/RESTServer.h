/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include "PathVerb.h"
#include "WebSocket.h"

namespace librestpp {
	class JSONRESTHandler;

	class RESTServer {
		struct Private;
		public:
			RESTServer(int port);
			virtual ~RESTServer();

			void addDefaultGetEndpoint(boost::shared_ptr<JSONRESTHandler> handler);
			void addJSONEndpoint(const PathVerb& pathVerb, boost::shared_ptr<JSONRESTHandler> handler);
			void poll();
			void run();
		public:
			boost::signals2::signal<void(boost::shared_ptr<WebSocket>)> onWebSocketConnection;

		private:
			//TODO: Get rid of PIMPL and expose enough websocketpp for eventloop integration
			boost::shared_ptr<Private> private_;
	};
}

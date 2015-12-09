/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include "PathVerb.h"
#include "WebSocket.h"

namespace librestpp {
	class RESTHandler;
	class RESTRequest;
	class ServerDriver;

	class RESTServer {
		public:
			RESTServer(std::shared_ptr<ServerDriver> driver);

			~RESTServer();

			void addDefaultGetEndpoint(std::shared_ptr<RESTHandler> handler);

			void addEndpoint(const PathVerb& pathVerb, std::shared_ptr<RESTHandler> handler);
		public:
			boost::signals2::signal<void(std::shared_ptr<WebSocket>)> onWebSocketConnection;
		private:
			void handleRequest(std::shared_ptr<RESTRequest> request);
		private:
			std::map<PathVerb, std::shared_ptr<RESTHandler>> fullPathHandlers_;
			std::map<PathVerb, std::shared_ptr<RESTHandler>> wildcardHandlers_;
			std::shared_ptr<RESTHandler> defaultHandler_;
			std::shared_ptr<ServerDriver> driver_;
	};
}

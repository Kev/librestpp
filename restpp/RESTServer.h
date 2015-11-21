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

namespace boost {
	namespace asio {
		class io_service;
	}
}

namespace librestpp {
	class JSONRESTHandler;

	class RESTServer {
		class Private;
		public:
			RESTServer(std::shared_ptr<boost::asio::io_service> ioService = std::shared_ptr<boost::asio::io_service>());
			virtual ~RESTServer();

			void addDefaultGetEndpoint(std::shared_ptr<JSONRESTHandler> handler);
			void addJSONEndpoint(const PathVerb& pathVerb, std::shared_ptr<JSONRESTHandler> handler);
			/**
			 * Start the server listening on the port. The io_service still needs to run underneath, by polling poll(),
			 * by calling run() or by using an external io_service and running it outside the RESTServer.
			 */
			bool listen(int port);
			/**
			 * Set SO_REUSEADDR. Must be called before listen().
			 */
			void setReuseAddr(bool b);
			/**
			 * Poll the io_service for activity. Not generally useful (run the io_service instead).
			 * Must not be called before listen().
			 */
			void poll();
			/**
			 * Run the io_service. Not needed where an external io_service is passed in.
			 * Must not be called before listen().
			 */
			void run();
		public:
			boost::signals2::signal<void(std::shared_ptr<WebSocket>)> onWebSocketConnection;

		private:
			//TODO: Get rid of PIMPL and expose enough websocketpp for eventloop integration
			std::shared_ptr<Private> private_;
	};
}

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>

#include "PathVerb.h"

namespace librestpp {
	class JSONRESTHandler;

	class RESTServer {
		struct Private;
		public:
			RESTServer(int port);
			virtual ~RESTServer();

			void addJSONEndpoint(const PathVerb& pathVerb, boost::shared_ptr<JSONRESTHandler> handler);
			void poll();

		private:
			//TODO: Get rid of PIMPL and expose enough websocketpp for eventloop integration
			boost::shared_ptr<Private> private_;
	};
}

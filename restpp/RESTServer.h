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
	class JSONRESTHandler;
	class RESTRequest;

	template<class /*ServerDriver*/ T>
	class RESTServer {
		public:
			RESTServer(std::shared_ptr<T> driver) : driver_(driver) {
				driver_->onRESTRequest.connect(boost::bind(&RESTServer::handleRequest, this, _1));
				driver_->onWebSocketConnection.connect(onWebSocketConnection);
			}

			~RESTServer() {
				driver_->onRESTRequest.disconnect(boost::bind(&RESTServer::handleRequest, this, _1));
			}

			void addDefaultGetEndpoint(std::shared_ptr<JSONRESTHandler> handler)  {
				defaultHandler_ = handler;
			}

			void addJSONEndpoint(const PathVerb& pathVerb, std::shared_ptr<JSONRESTHandler> handler) {
				handlers_[pathVerb] = handler;
			}
		public:
			boost::signals2::signal<void(std::shared_ptr<WebSocket>)> onWebSocketConnection;
		protected:
			void handleRequest(std::shared_ptr<RESTRequest> request)  {
				const PathVerb& pathVerb = request->getPathVerb();
				std::shared_ptr<JSONRESTHandler> handler = handlers_[pathVerb];
				if (pathVerb.verb != PathVerb::INVALID && (!!handler || !!defaultHandler_)) {
					if (!handler) {
						handler = defaultHandler_;
					}
					handler->handleRequest(request);
				}
				else {
					request->setReplyHeader(RESTRequest::HTTP_NOT_FOUND);
					request->setContentType("text/plain");
					request->addReplyContent("Not Found");
					request->sendReply();
				}

			}
		private:
			std::map<PathVerb, std::shared_ptr<JSONRESTHandler>> handlers_;
			std::shared_ptr<JSONRESTHandler> defaultHandler_;
			std::shared_ptr<T> driver_;
	};
}

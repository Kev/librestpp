/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "RESTServer.h"

#include "drivers/ServerDriver.h"
#include "RESTHandler.h"
#include "RESTRequest.h"

namespace librestpp {
RESTServer::RESTServer(std::shared_ptr<ServerDriver> driver) : driver_(driver) {
	driver_->onRESTRequest.connect(boost::bind(&RESTServer::handleRequest, this, _1));
	driver_->onWebSocketConnection.connect(onWebSocketConnection);
}

RESTServer::~RESTServer() {
	driver_->onRESTRequest.disconnect(boost::bind(&RESTServer::handleRequest, this, _1));
}

void RESTServer::addDefaultGetEndpoint(std::shared_ptr<RESTHandler> handler)  {
	defaultHandler_ = handler;
}

void RESTServer::addEndpoint(const PathVerb& pathVerb, std::shared_ptr<RESTHandler> handler) {
	if (pathVerb.hasWildcard()) {
		wildcardHandlers_[pathVerb] = handler;
	}
	else {
		fullPathHandlers_[pathVerb] = handler;
	}
}

void RESTServer::handleRequest(std::shared_ptr<RESTRequest> request)  {
	PathVerb pathVerb = request->getPathVerb().withoutQuery();
	// First check the (easy) exact match handlers
	std::shared_ptr<RESTHandler> handler = fullPathHandlers_[pathVerb];
	if (!handler) {
		//If that's not found, look for a wildcard matcher
		for (auto it : wildcardHandlers_) {
			auto parameters = it.first.getParameters(pathVerb.path);
			if (parameters) {
				request->setParameters(parameters.get());
				auto wildcardHandler = it.second;
				wildcardHandler->handleRequest(request);
				return;
			}
		}
	}
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
}

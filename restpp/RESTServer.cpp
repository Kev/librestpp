/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "RESTServer.h"

#include "drivers/ServerDriver.h"
#include "JSONRestHandler.h"
#include "RESTRequest.h"

namespace librestpp {
RESTServer::RESTServer(std::shared_ptr<ServerDriver> driver) : driver_(driver) {
	driver_->onRESTRequest.connect(boost::bind(&RESTServer::handleRequest, this, _1));
	driver_->onWebSocketConnection.connect(onWebSocketConnection);
}

RESTServer::~RESTServer() {
	driver_->onRESTRequest.disconnect(boost::bind(&RESTServer::handleRequest, this, _1));
}

void RESTServer::addDefaultGetEndpoint(std::shared_ptr<JSONRESTHandler> handler)  {
	defaultHandler_ = handler;
}

void RESTServer::addJSONEndpoint(const PathVerb& pathVerb, std::shared_ptr<JSONRESTHandler> handler) {
	handlers_[pathVerb] = handler;
}

void RESTServer::handleRequest(std::shared_ptr<RESTRequest> request)  {
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
}

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "RESTRequest.h"

namespace librestpp {

RESTRequest::RESTRequest(const PathVerb& pathVerb, const std::string& body) : pathVerb_(pathVerb), body_(body) {

}

RESTRequest::~RESTRequest() {

}

void RESTRequest::addReplyContent(boost::shared_ptr<JSONObject> jsonContent) {
	addReplyContent(jsonContent->serialize());
}

const std::string& RESTRequest::getBody() {
	return body_;
}

boost::shared_ptr<JSONObject> RESTRequest::getJSON() {
	return JSONObject::parse(body_);
}

}

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "RESTRequest.h"

#include <iostream>
#include <sstream>
#include "Cookies.h"

namespace librestpp {

RESTRequest::RESTRequest(const PathVerb& pathVerb, const std::string& body) : pathVerb_(pathVerb), body_(body) {

}

RESTRequest::~RESTRequest() {

}

void RESTRequest::addReplyContent(std::shared_ptr<JSONObject> jsonContent) {
	addReplyContent(jsonContent->serialize());
}

void RESTRequest::addReplyContent(std::shared_ptr<JSONArray> jsonContent) {
	addReplyContent(jsonContent->serialize());
}

const std::string& RESTRequest::getBody() {
	return body_;
}

std::shared_ptr<JSONObject> RESTRequest::getJSON() {
	return JSONObject::parse(body_);
}

boost::optional<std::string> RESTRequest::getCookie(const std::string& key) {
	if (!cookies_) {
		boost::optional<std::string> cookieHeader = getHeader("Cookie");
		if (cookieHeader) {
			cookies_ = getCookies(*cookieHeader);
		}
		else {
			cookies_ = std::map<std::string, std::string>();
		}
	}
	auto it = cookies_->find(key);
	boost::optional<std::string> cookie;
	if (it != cookies_->end()) {
		cookie = it->second;
	}
	return cookie;
}

const PathVerb& RESTRequest::getPathVerb() const {
	return pathVerb_;
}

const std::vector<std::string>& RESTRequest::getParameters() const {
	return parameters_;
}

void RESTRequest::setParameters(const std::vector<std::string>& parameters) {
	parameters_ = parameters;
}

void RESTRequest::setContentType(const std::string& contentType) {
	setReplyHeader("Content-Type", contentType);
}

void RESTRequest::setLibrestppCookie(const std::string& cookieValue, size_t maxAgeSeconds) {
	std::stringstream value;
	value << "librestpp_session=" << cookieValue << ";max-age=" << maxAgeSeconds;
	setReplyHeader("Set-Cookie", value.str());
}

}

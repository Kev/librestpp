/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "RESTRequest.h"

#include <iostream>

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

boost::optional<std::string> RESTRequest::getCookie(const std::string& key) {
	if (!cookies_) {
		cookies_ = std::map<std::string, std::string>();
		boost::optional<std::string> cookieHeader = getHeader("Cookie");
		if (cookieHeader) {
			// Not the most elegant thing in the world
			// TODO: confirm cookie parsing is correct (I did it from memory)
			size_t start = 0;
			while (start < cookieHeader->size()) {
				size_t semicolon = cookieHeader->find_first_of(';', start);
				size_t equals = cookieHeader->find_first_of('=', start);
				if (equals > semicolon) {
					break;
				}
				std::string key = cookieHeader->substr(start, equals - start);
				size_t valueStart = equals + 1;
				std::string value = cookieHeader->substr(valueStart, semicolon - valueStart);
				std::cerr << "Got key '" << key << "' and value '" << value << "'" << std::endl;
				(*cookies_)[key] = value;
				start = semicolon + 1;
				while (start < cookieHeader->size() && (*cookieHeader)[start] == ' ') {
					start++;
				}
			}
		}
	}
	std::map<std::string, std::string>::iterator it = cookies_->find(key);
	boost::optional<std::string> cookie;
	if (it != cookies_->end()) {
		cookie = it->second;
	}
	return cookie;
}

}

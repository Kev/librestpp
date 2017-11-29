/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "WebSocket.h"

#include <memory>
#include <string>

namespace librestpp {

WebSocket::WebSocket() {

}

void WebSocket::handleMessage(const std::string& message) {
	std::shared_ptr<JSONObject> obj = JSONObject::parse(message);
	if (obj) {
		onMessage(obj);
	}
}

void WebSocket::send(std::shared_ptr<JSONObject> json) {
	send(json->serialize());
}

// FIXME: Copy pasted from RESTRequest
boost::optional<std::string> WebSocket::getCookie(const std::string& cookieKey) {
	boost::optional<std::string> cookieHeader = getHeader("Cookie");
	if (!cookieHeader) {
		return boost::optional<std::string>();
	}

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
		if (key == cookieKey) {
			return value;
		}
		size_t oldStart = start;
		start = semicolon + 1;
		if (start <= oldStart) {
			// We've looped over the size of size_t (probably because semicolon wasn't found)
			start = cookieHeader->size();
		}
		while (start < cookieHeader->size() && (*cookieHeader)[start] == ' ') {
			start++;
		}
	}
	return boost::optional<std::string>();
}

}

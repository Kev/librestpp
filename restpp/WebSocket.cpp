/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "WebSocket.h"

#include <memory>
#include <string>
#include "Cookies.h"

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

boost::optional<std::string> WebSocket::getCookie(const std::string& key) {
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
	return it != cookies_->end() ? it->second : boost::optional<std::string>();
}

}

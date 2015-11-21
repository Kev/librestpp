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

}

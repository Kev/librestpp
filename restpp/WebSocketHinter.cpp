/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "WebSocketHinter.h"

#include <memory>
#include <string>

namespace librestpp {

WebSocketHinter::WebSocketHinter(WebSocket::ref webSocket) : webSocket_(std::move(webSocket)) {

}

void WebSocketHinter::send(std::shared_ptr<JSONObject> json) {
	webSocket_->send(json);
}

void WebSocketHinter::sendModelHint(const std::string& uri) {
	std::shared_ptr<JSONObject> json = std::make_shared<JSONObject>();
	json->set("type", std::make_shared<JSONString>("ampersand-websocket-hint"));
	json->set("urlRoot", std::make_shared<JSONString>(uri));
	send(json);
}

WebSocket::ref WebSocketHinter::getWebSocket() {
	return webSocket_;
}

}

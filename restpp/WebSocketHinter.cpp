/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "WebSocketHinter.h"

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

namespace librestpp {

WebSocketHinter::WebSocketHinter(WebSocket::ref webSocket) : webSocket_(webSocket) {

}

void WebSocketHinter::send(boost::shared_ptr<JSONObject> json) {
	webSocket_->send(json);
}

void WebSocketHinter::sendModelHint(const std::string& uri) {
	boost::shared_ptr<JSONObject> json = boost::make_shared<JSONObject>();
	json->set("type", boost::make_shared<JSONString>("ampersand-websocket-hint"));
	json->set("urlRoot", boost::make_shared<JSONString>(uri));
	send(json);
}

}

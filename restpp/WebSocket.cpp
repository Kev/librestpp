/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "WebSocket.h"

namespace librestpp {

WebSocket::WebSocket() {

}

void WebSocket::handleMessage(const std::string& message) {
  boost::shared_ptr<JSONObject> obj = JSONObject::parse(message);
  if (obj) {
    onMessage(obj);
  }
}

void WebSocket::send(boost::shared_ptr<JSONObject> json) {
  send(json->serialize());
}

}

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include "WebSocket.h"

namespace librestpp {
	/**
	 * WebSocket wrapper with convenience functions for sending hints
	 * to be consumed by ampersond-websocket-hint for triggering
	 * model updates.
	 */
	class WebSocketHinter {
		public:
			typedef std::shared_ptr<WebSocketHinter> ref;
			WebSocketHinter(WebSocket::ref webSocket);
			void send(std::shared_ptr<JSONObject> json);
			void sendModelHint(const std::string& uri);
			WebSocket::ref getWebSocket();
		private:
			WebSocket::ref webSocket_;
	};
}

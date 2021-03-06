/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <functional>
#include <memory>

//#include <boost/asio/io_service.hpp>

namespace boost {
  namespace asio {
    namespace ssl {
      class context;
    }
    class io_service;
  }
}

#include "ServerDriver.h"

namespace librestpp {
	class WebSocketPPASIOTLSServerDriver : public ServerDriver {
    public:
      WebSocketPPASIOTLSServerDriver(std::function<void (boost::asio::ssl::context*)> initTLS, std::shared_ptr<boost::asio::io_service> ioService = std::shared_ptr<boost::asio::io_service>());
      /**
			 * See WebSocketPPServerDriverBase#listen
			 */
			bool listen(int port);
      /**
			 * See WebSocketPPServerDriverBase#setReuseAddr
			 */
			void setReuseAddr(bool b);
      /**
			 * See WebSocketPPServerDriverBase#poll
			 */
			void poll();
      /**
			 * See WebSocketPPServerDriverBase#run
			 */
			void run();
    private:
      class Private;
      std::shared_ptr<Private> private_; // Should really be unique_ptr, but shared_ptr lets us PIMPL it and keep the websocketpp includes out of the public API
	};
}

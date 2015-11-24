/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "WebSocketPPASIOServerDriver.h"

#include "WebSocketPPServerDriverBase.h"

namespace librestpp {

class WebSocketPPASIOServerDriver::Private {
  public:
    Private(std::shared_ptr<boost::asio::io_service> ioService, ServerDriver* driver) : driverImplementation(driver) {

    }
    WebSocketPPServerDriverBase<websocketpp::config::asio> driverImplementation;
};

WebSocketPPASIOServerDriver::WebSocketPPASIOServerDriver(std::shared_ptr<boost::asio::io_service> ioService) {
  private_ = std::make_shared<Private>(ioService, this);
}

bool WebSocketPPASIOServerDriver::listen(int port) {
  return private_->driverImplementation.listen(port);
}

void WebSocketPPASIOServerDriver::setReuseAddr(bool b) {
  private_->driverImplementation.setReuseAddr(b);
}

void WebSocketPPASIOServerDriver::poll() {
  private_->driverImplementation.poll();
}

void WebSocketPPASIOServerDriver::run() {
  private_->driverImplementation.run();
}

}

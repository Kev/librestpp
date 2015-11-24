/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "WebSocketPPASIOTLSServerDriver.h"

#include <boost/bind.hpp>

#include <websocketpp/config/asio.hpp>

#include "WebSocketPPServerDriverBase.h"

namespace librestpp {

class WebSocketPPASIOTLSServerDriver::Private {
  public:
    Private(std::function<void (boost::asio::ssl::context*)> initTLS, std::shared_ptr<boost::asio::io_service> ioService, ServerDriver* driver) : initTLS_(initTLS), driverImplementation(driver) {
      driverImplementation.getServer().set_tls_init_handler(boost::bind(&WebSocketPPASIOTLSServerDriver::Private::handleTLSInit, this, _1));

    }
    WebSocketPPServerDriverBase<websocketpp::config::asio_tls> driverImplementation;
  private:
    websocketpp::lib::shared_ptr<boost::asio::ssl::context> handleTLSInit(websocketpp::connection_hdl /*hdl*/) {
      auto ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
      initTLS_(ctx.get());
      return ctx;
  }
  std::function<void (boost::asio::ssl::context*)> initTLS_;
};

WebSocketPPASIOTLSServerDriver::WebSocketPPASIOTLSServerDriver(std::function<void (boost::asio::ssl::context*)> initTLS, std::shared_ptr<boost::asio::io_service> ioService) {
  private_ = std::make_shared<Private>(initTLS, ioService, this);
}

bool WebSocketPPASIOTLSServerDriver::listen(int port) {
  return private_->driverImplementation.listen(port);
}

void WebSocketPPASIOTLSServerDriver::setReuseAddr(bool b) {
  private_->driverImplementation.setReuseAddr(b);
}

void WebSocketPPASIOTLSServerDriver::poll() {
  private_->driverImplementation.poll();
}

void WebSocketPPASIOTLSServerDriver::run() {
  private_->driverImplementation.run();
}

}

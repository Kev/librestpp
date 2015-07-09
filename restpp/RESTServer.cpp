/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "RESTServer.h"

#include <sstream>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/http/constants.hpp>


#include "JSONRESTHandler.h"

namespace librestpp {

class RESTRequestInt : public RESTRequest {
	public:
		RESTRequestInt(const PathVerb& pathVerb, const std::string& body, websocketpp::server<websocketpp::config::asio>::connection_ptr connection) : RESTRequest(pathVerb, body), connection_(connection), contentType_("application/octet-stream") {

		}

		void setReplyHeader(RESTRequest::ResultCode code) {
			connection_->set_status(ourCodeToTheirCode(code));
		}

		void addReplyContent(const std::string& content) {
			//TODO: When websocketpp supports partial responses, support them
			//TODO: Binary responses
			reply_ << content;
		}

		void sendReply() {
			connection_->replace_header("Content-Type", contentType_);
			connection_->set_body(reply_.str());
#ifndef RESTPP_NO_DEFER
			/*websocketpp::lib::error_code ec = */connection_->send_http_response();
#endif
		}

		void setContentType(const std::string& contentType) {
			contentType_ = contentType;
		}

		boost::optional<std::string> getHeader(const std::string& header) {
			boost::optional<std::string> result;
			std::string value = connection_->get_request_header(header);
			if (!value.empty()) {
				result = value;
			}
			return result;
		}

	private:

		websocketpp::http::status_code::value ourCodeToTheirCode(RESTRequest::ResultCode code) {
			switch (code) {
				//FIXME: More codes here
				case RESTRequest::HTTP_OK: return websocketpp::http::status_code::ok;
				case RESTRequest::HTTP_UNAUTHORIZED: return websocketpp::http::status_code::unauthorized;
				default: return websocketpp::http::status_code::not_found;
			}
		}

		websocketpp::server<websocketpp::config::asio>::connection_ptr connection_;
		std::stringstream reply_;
		std::string contentType_;
};

class WebSocketInt : public WebSocket {
	public:
		WebSocketInt(websocketpp::server<websocketpp::config::asio>::connection_ptr connection, websocketpp::server<websocketpp::config::asio>* server) : connection_(connection), server_(server) {
			connection->set_message_handler(boost::bind(&WebSocketInt::handleMessageInt, this, _2));
		}

		void send(const std::string& message) {
			server_->send(connection_, message, websocketpp::frame::opcode::text);
		}

	private:
		void handleMessageInt(websocketpp::server<websocketpp::config::asio>::message_ptr message) {
			handleMessage(message->get_payload());
		}
	private:
		websocketpp::server<websocketpp::config::asio>::connection_ptr connection_;
		websocketpp::server<websocketpp::config::asio>* server_;
};

class RESTServer::Private {
	public:
		Private() {
			server_.set_http_handler(boost::bind(&RESTServer::Private::handleHTTPRequest, this, _1));
			server_.set_open_handler(boost::bind(&RESTServer::Private::handleNewWebSocket,this, _1));
		}

		bool start(int port) {
			try {
				server_.init_asio();
				server_.listen(port);
				server_.start_accept();
			}
			catch (const websocketpp::exception& /*e*/) {
				return false;
			}
			return true;
		}

		void addJSONEndpoint(const PathVerb& pathVerb, boost::shared_ptr<JSONRESTHandler> handler) {
			handlers_[pathVerb] = handler;
		}

		void addDefaultGetEndpoint(boost::shared_ptr<JSONRESTHandler> handler) {
			defaultHandler_ = handler;
		}

		void poll() {
			server_.poll();
		}

		void run() {
			server_.run();
		}

		boost::signals2::signal<void(boost::shared_ptr<WebSocket>)> onWebSocketConnection;

	private:
		void handleNewWebSocket(websocketpp::connection_hdl handle) {
			websocketpp::server<websocketpp::config::asio>::connection_ptr connection = server_.get_con_from_hdl(handle);
			WebSocket::ref webSocket = boost::make_shared<WebSocketInt>(connection, &server_);
			onWebSocketConnection(webSocket);
		}

		void handleHTTPRequest(websocketpp::connection_hdl handle) {
			websocketpp::server<websocketpp::config::asio>::connection_ptr connection = server_.get_con_from_hdl(handle);
			std::string path = connection->get_uri()->get_resource();
			std::string method = connection->get_request().get_method();
			PathVerb::RESTVerb verb = PathVerb::INVALID;
			if (method == "GET") {
				verb = PathVerb::GET;
			}
			else if (method == "POST") {
				verb = PathVerb::POST;
			}
			//FIXME: Other verbs

			//TODO: Check resource exhaustion
			std::string body = connection->get_request_body();
			PathVerb pathVerb(path, verb);
			boost::shared_ptr<RESTRequest> request = boost::make_shared<RESTRequestInt>(pathVerb, body, connection);

			boost::shared_ptr<JSONRESTHandler> handler = handlers_[pathVerb];
			if (verb != PathVerb::INVALID && (!!handler || !!defaultHandler_)) {
				if (!handler) {
					handler = defaultHandler_;
				}
#ifndef RESTPP_NO_DEFER
				/*websocketpp::lib::error_code ec =*/connection->defer_http_response();
#endif
				handler->handleRequest(request);
			}
			else {
				connection->set_body("Not found");
				connection->set_status(websocketpp::http::status_code::not_found);
			}
		}

		websocketpp::server<websocketpp::config::asio> server_;
		std::map<PathVerb, boost::shared_ptr<JSONRESTHandler> > handlers_;
		boost::shared_ptr<JSONRESTHandler> defaultHandler_;
};

RESTServer::RESTServer(int port) {
	private_ = boost::make_shared<RESTServer::Private>();
	private_->start(port);
	private_->onWebSocketConnection.connect(onWebSocketConnection);
}

RESTServer::~RESTServer() {

}

void RESTServer::addDefaultGetEndpoint(boost::shared_ptr<JSONRESTHandler> handler) {
	private_->addDefaultGetEndpoint(handler);
}

void RESTServer::addJSONEndpoint(const PathVerb& pathVerb, boost::shared_ptr<JSONRESTHandler> handler) {
	private_->addJSONEndpoint(pathVerb, handler);
}

void RESTServer::poll() {
	private_->poll();
}

void RESTServer::run() {
	private_->run();
}

}

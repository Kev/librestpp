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
		RESTRequestInt(const PathVerb& pathVerb, const std::string& body, websocketpp::server<websocketpp::config::asio>::connection_ptr connection) : RESTRequest(pathVerb, body), connection_(connection) {

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
			connection_->set_body(reply_.str());
			/*websocketpp::lib::error_code ec = */connection_->send_http_response();
		}

	private:

		websocketpp::http::status_code::value ourCodeToTheirCode(RESTRequest::ResultCode code) {
			switch (code) {
				//FIXME: More codes here
				case RESTRequest::HTTP_OK: return websocketpp::http::status_code::ok;
				default: return websocketpp::http::status_code::not_found;
			}
		}

		websocketpp::server<websocketpp::config::asio>::connection_ptr connection_;
		std::stringstream reply_;
};

class RESTServer::Private {
	public:
		Private() {
			server_.set_http_handler(boost::bind(&RESTServer::Private::handleRequest, this, _1));
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

	private:
		void handleRequest(websocketpp::connection_hdl handle) {
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
			if (verb != PathVerb::INVALID && !!handler) {
				/*websocketpp::lib::error_code ec =*/connection->defer_http_response();
				handler->handleRequest(request);
			}
			else if (!!defaultHandler_) {
				/*websocketpp::lib::error_code ec =*/connection->defer_http_response();
				defaultHandler_->handleRequest(request);
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

}

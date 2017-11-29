/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include <websocketpp/server.hpp>

#include "../PathVerb.h"
#include "../RESTRequest.h"
#include "../WebSocket.h"

namespace librestpp {

	template<typename T>
	class WebSocketPPServerDriverBase {
		public:
			WebSocketPPServerDriverBase(ServerDriver* driver, std::shared_ptr<boost::asio::io_service> ioService) : driver_(driver), ioService_(ioService) {
				server_.set_http_handler(boost::bind(&WebSocketPPServerDriverBase::handleHTTPRequest, this, _1));
				server_.set_open_handler(boost::bind(&WebSocketPPServerDriverBase::handleNewWebSocket,this, _1));

			}
			~WebSocketPPServerDriverBase() {}

			/**
			 * Start the server listening on the port. The io_service still needs to run underneath, by polling poll(),
			 * by calling run() or by using an external io_service and running it outside the RESTServer.
			 */
			bool listen(int port) {
				try {
					if (!!ioService_) {
						server_.init_asio(ioService_.get());
					}
					else {
						server_.init_asio();
					}
					server_.listen(port);
					server_.start_accept();
				}
				catch (const websocketpp::exception& /*e*/) {
					return false;
				}
				return true;
			}
			/**
			 * Set SO_REUSEADDR. Must be called before listen().
			 */
			void setReuseAddr(bool b) {
				server_.set_reuse_addr(b);
			}
			/**
			 * Poll the io_service for activity. Not generally useful (run the io_service instead).
			 * Must not be called before listen().
			 */
			void poll() {
				server_.poll();
			}
			/**
			 * Run the io_service. Not needed where an external io_service is passed in.
			 * Must not be called before listen().
			 */
			void run() {
				server_.run();
			}

			websocketpp::server<T>& getServer() {
				return server_;
			}

		private:
			class WebSocketPPRestRequest : public RESTRequest {
				public:
					WebSocketPPRestRequest(const PathVerb& pathVerb, const std::string& body, typename websocketpp::server<T>::connection_ptr connection) : RESTRequest(pathVerb, body), connection_(std::move(connection)), contentTypeSet_(false) {

					}

					void setReplyHeader(RESTRequest::ResultCode code) override {
						connection_->set_status(ourCodeToTheirCode(code));
					}

					void addReplyContent(const std::string& content) override {
						//TODO: When websocketpp supports partial responses, support them
						//TODO: Binary responses
						reply_ << content;
					}

					void sendReply() override {
						if (!contentTypeSet_) {
							connection_->replace_header("Content-Type", "application/octet-stream");
						}
						connection_->set_body(reply_.str());
						/*websocketpp::lib::error_code ec = */connection_->send_http_response();
					}

					void setReplyHeader(const std::string& header, const std::string& value) override {
						if (header == "Content-Type") {
							contentTypeSet_ = true;
						}
						connection_->replace_header(header, value);
					}

					boost::optional<std::string> getHeader(const std::string& header) override {
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
							case RESTRequest::HTTP_NOT_FOUND: return websocketpp::http::status_code::not_found;
 							case RESTRequest::HTTP_BAD_REQUEST: return websocketpp::http::status_code::bad_request;
 							case RESTRequest::HTTP_FORBIDDEN: return websocketpp::http::status_code::forbidden;
						}
					}

					typename websocketpp::server<T>::connection_ptr connection_;
					std::stringstream reply_;
					bool contentTypeSet_;
 					boost::optional<std::string> cookie_;
			};

			class WebSocketPPWebSocket : public WebSocket {
				public:
					WebSocketPPWebSocket(typename websocketpp::server<T>::connection_ptr connection, websocketpp::server<T>* server) : connection_(connection), server_(server) {
						connection->set_message_handler(boost::bind(&WebSocketPPWebSocket::handleMessageInt, this, _2));
						connection->set_close_handler(boost::bind(&WebSocketPPWebSocket::handleClosedInt, this));
						connection->set_fail_handler(boost::bind(&WebSocketPPWebSocket::handleClosedInt, this));
					}

					void send(const std::string& message) override {
						server_->send(connection_, message, websocketpp::frame::opcode::text);
					}

				private:
					void handleMessageInt(typename websocketpp::server<T>::message_ptr message) {
						handleMessage(message->get_payload());
					}

					void handleClosedInt() {
						onClosed();
					}
				private:
					typename websocketpp::server<T>::connection_ptr connection_;
					websocketpp::server<T>* server_;
			};


			void handleNewWebSocket(websocketpp::connection_hdl handle) {
				typename websocketpp::server<T>::connection_ptr connection = server_.get_con_from_hdl(handle);
				WebSocket::ref webSocket = std::make_shared<WebSocketPPWebSocket>(connection, &server_);
				driver_->onWebSocketConnection(webSocket);
			}

			void handleHTTPRequest(websocketpp::connection_hdl handle) {
				typename websocketpp::server<T>::connection_ptr connection = server_.get_con_from_hdl(handle);
				std::string path = connection->get_uri()->get_resource();
				std::string method = connection->get_request().get_method();
				PathVerb::RESTVerb verb = PathVerb::INVALID;
				if (method == "GET") {
					verb = PathVerb::GET;
				}
				else if (method == "POST") {
					verb = PathVerb::POST;
				}
				else if (method == "PUT") {
					verb = PathVerb::PUT;
				}
				else if (method == "PATCH") {
					verb = PathVerb::PATCH;
				}
				//FIXME: Other verbs

				//TODO: Check resource exhaustion
				std::string body = connection->get_request_body();
				PathVerb pathVerb(path, verb);
				std::shared_ptr<RESTRequest> request = std::make_shared<WebSocketPPRestRequest>(pathVerb, body, connection);

				/*websocketpp::lib::error_code ec =*/connection->defer_http_response();
				driver_->onRESTRequest(request);
			}
		private:
			std::shared_ptr<boost::asio::io_service> ioService_;
			websocketpp::server<T> server_;
			ServerDriver* driver_;
	};
}

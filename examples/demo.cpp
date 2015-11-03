/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <restpp/JSONRESTHandler.h>
#include <restpp/RESTServer.h>

using namespace librestpp;

class StringHandler : public JSONRESTHandler {
	public:
		StringHandler(const std::string& result) : result_(result) {}
		virtual void handleRequest(boost::shared_ptr<RESTRequest> request) {
			request->setReplyHeader(RESTRequest::HTTP_OK);
			request->setContentType("text/plain");
			request->addReplyContent(result_);
			request->sendReply();
		}

	private:
		std::string result_;
};

class TextEchoHandler : public JSONRESTHandler {
	public:
		TextEchoHandler() {}
		virtual void handleRequest(boost::shared_ptr<RESTRequest> request) {
			request->setReplyHeader(RESTRequest::HTTP_OK);
			request->addReplyContent(request->getBody());
			request->setContentType("text/plain");
			request->sendReply();
		}

};

class JSONEchoHandler : public JSONRESTHandler {
	public:
		JSONEchoHandler() {}
		virtual void handleRequest(boost::shared_ptr<RESTRequest> request) {
			boost::shared_ptr<JSONObject> json = request->getJSON();

			request->setReplyHeader(RESTRequest::HTTP_OK);
			if (!!json) {
				request->addReplyContent(json);
				request->setContentType("application/json");
			}
			else {
				request->setContentType("text/plain");
				request->addReplyContent("Invalid JSON sent");
			}
			request->sendReply();
		}

};

class JSONChefHandler : public JSONRESTHandler {
	public:
		JSONChefHandler() {}
		virtual void handleRequest(boost::shared_ptr<RESTRequest> request) {
			boost::shared_ptr<JSONObject> json = request->getJSON();

			request->setReplyHeader(RESTRequest::HTTP_OK);
			if (!!json) {
				request->addReplyContent(boost::dynamic_pointer_cast<JSONObject>(chefify(json)));
				request->setContentType("application/json");
			}
			else {
				request->setContentType("text/plain");
				request->addReplyContent("Invalid JSON sent");
			}
			request->sendReply();
		}

	private:
		JSONValue::ref chefify(JSONValue::ref value) {
			boost::shared_ptr<JSONObject> jsonObject;
			boost::shared_ptr<JSONString> jsonString;
			if ((jsonObject = boost::dynamic_pointer_cast<JSONObject>(value))) {
				boost::shared_ptr<JSONObject> result = boost::make_shared<JSONObject>();
				auto values = jsonObject->getValues();
				for (auto it = values.begin(); it != values.end(); it++) {
					result->set(it->first, chefify(it->second));
				}
				return result;
			}
			else if ((jsonString = boost::dynamic_pointer_cast<JSONString>(value))) {
				return boost::make_shared<JSONString>("bork bork");
			}
			return value;
		}
};

int main(int argc, const char* argv[])
{
	librestpp::RESTServer server;

	std::stringstream longString;
	for (int i = 0; i < 70000; i++) {
		longString << "Arr ";
	}

	boost::shared_ptr<JSONRESTHandler> demoHandler = boost::make_shared<StringHandler>("hi");
	server.addJSONEndpoint(PathVerb("/demo", PathVerb::GET), demoHandler);

	boost::shared_ptr<JSONRESTHandler> longHandler = boost::make_shared<StringHandler>(longString.str());
	server.addJSONEndpoint(PathVerb("/long", PathVerb::GET), longHandler);

	boost::shared_ptr<JSONRESTHandler> echoGetHandler = boost::make_shared<StringHandler>("Echos must be POST");
	server.addJSONEndpoint(PathVerb("/echo", PathVerb::GET), echoGetHandler);

	boost::shared_ptr<JSONRESTHandler> echoHandler = boost::make_shared<JSONEchoHandler>();
	server.addJSONEndpoint(PathVerb("/echo", PathVerb::POST), echoHandler);

	boost::shared_ptr<JSONRESTHandler> textEchoHandler = boost::make_shared<TextEchoHandler>();
	server.addJSONEndpoint(PathVerb("/echotext", PathVerb::POST), textEchoHandler);

	boost::shared_ptr<JSONRESTHandler> chefHandler = boost::make_shared<JSONChefHandler>();
	server.addJSONEndpoint(PathVerb("/chef", PathVerb::POST), chefHandler);

	std::cout << "Starting demo server on port 1080, at paths /demo, /long and /echo (GET) and /echo, /echotext and /chef (POST)" << std::endl;

	server.listen(1080);
	server.run();
}

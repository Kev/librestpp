/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include "JSON.h"
#include "PathVerb.h"

namespace librestpp {
	class RESTRequest {
		public:
			enum ResultCode {HTTP_OK};
			RESTRequest(const PathVerb& pathVerb, const std::string& body);
			virtual ~RESTRequest();

			virtual void setReplyHeader(RESTRequest::ResultCode code) = 0;
			virtual void addReplyContent(const std::string& content) = 0; //TODO: Switch to ByteArrayish things instead of strings
			virtual void addReplyContent(boost::shared_ptr<JSONObject> jsonContent);
			virtual void setContentType(const std::string& contentType) = 0;
			virtual void sendReply() = 0;
			const std::string& getBody(); //TODO: Switch to ByteArrayish things instead of strings
			boost::shared_ptr<JSONObject> getJSON();

		private:
			PathVerb pathVerb_;
			std::string body_;
	};
}

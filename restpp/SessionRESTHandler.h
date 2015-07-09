/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <string>


#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "JSONRESTHandler.h"
#include "SessionCollection.h"


namespace librestpp {
	template<class T>
	class SessionObjectRESTHandler : public JSONRESTHandler {
		public:
			SessionObjectRESTHandler(boost::shared_ptr<SessionCollection<T> > sessions, boost::function<void(boost::shared_ptr<T>, boost::shared_ptr<RESTRequest>)> handler) : sessions_(sessions), handler_(handler) {}
			virtual ~SessionObjectRESTHandler() {}

			virtual void handleRequest(boost::shared_ptr<RESTRequest> request) {
				boost::optional<std::string> sessionKey = request->getCookie("librestpp_session");
				boost::shared_ptr<T> session;
				if (sessionKey) {
					session = sessions_->getSession(*sessionKey);
				}
				if (session) {
					handler_(session, request);
				}
				else {
					request->setReplyHeader(RESTRequest::HTTP_UNAUTHORIZED);
					request->setContentType("text/plain");
					request->addReplyContent("Not authorized");
					request->sendReply();
				}
			}
		private:
			boost::shared_ptr<SessionCollection<T> > sessions_;
			boost::function<void(boost::shared_ptr<T>, boost::shared_ptr<RESTRequest>)> handler_;
	};
}

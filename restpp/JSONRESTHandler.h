/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <memory>

#include "RESTRequest.h"


namespace librestpp {
	class JSONRESTHandler {
		public:
			JSONRESTHandler();
			virtual ~JSONRESTHandler();

			virtual void handleRequest(std::shared_ptr<RESTRequest> request) = 0;
	};
}

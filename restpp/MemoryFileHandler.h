/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "RESTHandler.h"

namespace librestpp {

	/**
	 * Cache a file contents in memory and serve.
	 * N.B. Stores in a string, so text files only.
	 */
	class MemoryFileHandler : public RESTHandler {
		public:
			MemoryFileHandler(const std::string& path);
			virtual void handleRequest(std::shared_ptr<RESTRequest> request);

		private:
			std::vector<unsigned char> content_;
			std::string contentType_;
	};

}

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#pragma once

#include <string>


#include <boost/shared_ptr.hpp>

#include "JSONRESTHandler.h"

namespace librestpp {

  /**
   * Cache a file contents in memory and serve.
   * N.B. Stores in a string, so text files only.
   */
  class MemoryFileHandler : public JSONRESTHandler {
  	public:
  		MemoryFileHandler(const std::string& path);
  		virtual void handleRequest(boost::shared_ptr<RESTRequest> request);

  	private:
  		std::vector<unsigned char> content_;
  };

}

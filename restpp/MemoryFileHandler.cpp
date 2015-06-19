/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the LICENSE file for more information.
 */

#include "MemoryFileHandler.h"

#include <iostream>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

namespace librestpp {

#define BUFFER_SIZE 4096

std::string byteArrayToString(const std::vector<unsigned char>& b) {
  size_t i;
  for (i = b.size(); i > 0; --i) {
    if (b[i - 1] != 0) {
      break;
    }
  }
  return i > 0 ? std::string(reinterpret_cast<const char*>(&b[0]), i) : "";
}

MemoryFileHandler::MemoryFileHandler(const std::string& pathString) {
  std::cout << "Loading for path " << pathString << std::endl;
  boost::filesystem::path file(pathString);
  boost::filesystem::ifstream input(file, std::ios_base::in|std::ios_base::binary);
  while (input.good()) {
    size_t oldSize = content_.size();
    content_.resize(oldSize + BUFFER_SIZE);
    input.read(reinterpret_cast<char*>(&content_[oldSize]), BUFFER_SIZE);
    content_.resize(oldSize + boost::numeric_cast<size_t>(input.gcount()));
  }
  input.close();
  // std::string str = byteArrayToString(content_);
  // std::cout << "Storing contents for path " << pathString << ": " << std::endl << str << std::endl;
}

void MemoryFileHandler::handleRequest(boost::shared_ptr<RESTRequest> request) {
	request->setReplyHeader(RESTRequest::HTTP_OK);
	request->addReplyContent(byteArrayToString(content_));
	request->sendReply();
}

}

# Copyright (c) 2015 Isode Limited.
# All rights reserved.
# See the LICENSE file for more information.

BOOST = /usr/local/brew/Cellar/boost/1.57.0
RAPIDJSON = rapidjson

TARGET = librestpp.so
SOURCES = RESTServer.cpp JSONRESTHandler.cpp RESTRequest.cpp JSON.cpp
OBJECTS = $(SOURCES:.cpp=.o)
CXX = clang++
LDFLAGS =-L $(BOOST)/lib -lboost_chrono -lboost_system
CXXFLAGS=-g -isystem websocketpp/ -isystem $(BOOST)/include/ -isystem $(RAPIDJSON)/include -std=c++11

all: .clang_complete demo

$(TARGET): websocketpp/.git $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) -shared $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(TARGET)

websocketpp/.git:
	git submodule init && git submodule update

demo: $(TARGET)
	$(CXX) $(CXXFLAGS) -o demo demo.cpp -L . -lrestpp -lboost_thread-mt $(LDFLAGS)

# Not a phony, but this ensures rebuild each time
.PHONY: .clang_complete
.clang_complete:
	echo $(CXXFLAGS) > .clang_complete

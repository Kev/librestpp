cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(librestpp VERSION 0.1 LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(Websocketpp REQUIRED)

add_library(restpp
    restpp/Cookies.cpp
    restpp/JSON.cpp
    restpp/MemoryFileHandler.cpp
    restpp/RESTHandler.cpp
    restpp/RESTRequest.cpp
    restpp/RESTServer.cpp
    restpp/WebSocket.cpp
    restpp/WebSocketHinter.cpp
    restpp/drivers/WebSocketPPASIOServerDriver.cpp
    restpp/drivers/WebSocketPPASIOTLSServerDriver.cpp
    )

include_directories(
    ${BOOST_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${RAPIDJSON_INCLUDE_DIR}
    ${WEBSOCKETPP_INCLUDE_DIR}
    )

set(HEADER_FILES 
    restpp/Cookies.h
    restpp/JSON.h
    restpp/MemoryFileHandler.h
    restpp/PathVerb.h
    restpp/RESTHandler.h
    restpp/RESTRequest.h
    restpp/RESTServer.h
    restpp/SessionCollection.h
    restpp/SessionRESTHandler.h
    restpp/WebSocket.h
    restpp/WebSocketHinter.h
    )

set(DRIVERS_HEADER_FILES
    restpp/drivers/ServerDriver.h
    restpp/drivers/WebSocketPPASIOServerDriver.h
    restpp/drivers/WebSocketPPASIOTLSServerDriver.h
    restpp/drivers/WebSocketPPServerDriverBase.h
    )

install(TARGETS restpp DESTINATION lib)
install(FILES ${HEADER_FILES} DESTINATION include/restpp)
install(FILES ${DRIVERS_HEADER_FILES} DESTINATION include/restpp/drivers)

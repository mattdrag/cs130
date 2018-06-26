#include <iostream>
#include "gtest/gtest.h"
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"
#include "request.hpp"

class ConnectionTest : public ::testing::Test {
protected:
	bool isSocketOpen() {
        conn = new http::server::Connection(io_service, manager_, handlers_, default_handler_, status_handler_);
        return conn->socket().is_open();
    }
    void startConn() {
        conn = new http::server::Connection(io_service, manager_, handlers_, default_handler_, status_handler_);
        conn->start();
    }
    void stopConn() {
        conn = new http::server::Connection(io_service, manager_, handlers_, default_handler_, status_handler_);
        conn->stop();
    }
    http::server::Connection* conn;
    boost::asio::io_service io_service;
    http::server::ConnectionManager manager_; 
    std::map<std::string, http::server::RequestHandler*> handlers_;
    http::server::RequestHandler* default_handler_;
    http::server::RequestHandler* status_handler_;
};

TEST_F(ConnectionTest, SocketTest) {
	// When we get the socket, we expect it to be closed
	// since we never bound it.
    EXPECT_FALSE(isSocketOpen());
}

TEST_F(ConnectionTest, StartTest) {
	// Starting connection should throw an exception.
	// This is designed to expect that exception to be thrown.
    EXPECT_ANY_THROW(startConn());
    // This is expected to throw an exception because it is unable to 
    // start a connection with an uninitialized io_service
}


TEST_F(ConnectionTest, StopTest) {
    // Stopping connection should not throw an exception.
	// It merely stops all connections, if there is none to stop it returns silently.
    EXPECT_NO_THROW(stopConn());
}

#include <iostream>
#include "gtest/gtest.h"
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"

class ConnectionManagerTest : public ::testing::Test {
protected:
    void makeConn() {
        conn = new http::server::Connection(io_service, manager_, handlers_, default_handler_);
    }
    void startConnMan() {
        manager_.start(conn->shared_from_this());
    }
    void stopConnMan() {
        manager_.stop(conn->shared_from_this());
    }
    void stopAllConnMan() {
        manager_.stopAll();
    }
    http::server::Connection* conn;
    boost::asio::io_service io_service;
    http::server::ConnectionManager manager_; 
    std::map<std::string, RequestHandler*> handlers_;
    RequestHandler* default_handler_;
};

TEST_F(ConnectionManagerTest, SimpleConnection) {
    // Make sure we can successfully form a connection
	EXPECT_NO_THROW(makeConn());
}

TEST_F(ConnectionManagerTest, StartManager) {
    makeConn();
    // We expect bad_weak_ptr exception just like start() in connection,
    // since the manager tries to start the connection.
    EXPECT_ANY_THROW(startConnMan());
}

TEST_F(ConnectionManagerTest, StopManager) {
    makeConn();
    // We accept a throw because conn is currently not running, theres nothing to stop.
    EXPECT_ANY_THROW(stopConnMan());
}

TEST_F(ConnectionManagerTest, StopAllManager) {
    // There should be nothing thrown. 
    // With no connections, the manager will stop all (none), and then return.
    EXPECT_NO_THROW(stopAllConnMan());
}

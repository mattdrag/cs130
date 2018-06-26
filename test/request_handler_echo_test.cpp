#include <iostream>
#include <string>
#include "gtest/gtest.h"
#include "request_handler_echo.hpp"
#include "response.hpp"
#include "request.hpp"

class EchoHandlerTest : public ::testing::Test {
protected:
	void HandleEchoRequest(std::string request_string) {
		req = http::server::Request::Parse(request_string);
		handler.HandleRequest(*req, &resp);
	}
	http::server::EchoHandler handler;
	std::unique_ptr<http::server::Request> req;
	http::server::Response resp;
};

TEST_F(EchoHandlerTest, SimpleRequest) {
	std::string simple_request = "Testing";
	HandleEchoRequest(simple_request);
	std::string expected_response = "HTTP/1.1 200 OK\r\n"
									"Content-Length: 7\r\n"
									"Content-Type: text/plain\r\n\r\n" +
									simple_request;
	EXPECT_EQ(resp.ToString(), expected_response);
}

TEST_F(EchoHandlerTest, EmptyRequest) {
	HandleEchoRequest("");
	std::string expected_response = "HTTP/1.1 200 OK\r\n"
									"Content-Length: 0\r\n"
									"Content-Type: text/plain\r\n\r\n";
	EXPECT_EQ(resp.ToString(), expected_response);
}

TEST_F(EchoHandlerTest, BigRequest) {
	std::string big_request = "";
	for (int i = 0; i < 50; i++) {
		big_request += "EchoEchoEchoEchoEchoEchoEchoEchoEchoEchoEchoEcho";
	}
	HandleEchoRequest(big_request);
	std::string expected_response = "HTTP/1.1 200 OK\r\n"
									"Content-Length: " + std::to_string(big_request.length()) + "\r\n"
									"Content-Type: text/plain\r\n\r\n" + big_request;
	EXPECT_EQ(resp.ToString(), expected_response);
}
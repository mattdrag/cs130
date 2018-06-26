#include <iostream>
#include <string>
#include "gtest/gtest.h"
#include "request_handler_notfound.hpp"
#include "response.hpp"
#include "request.hpp"

TEST(NotFoundHandlerTest, SimpleTest) {
	http::server::NotFoundHandler handler;
	std::unique_ptr<http::server::Request> req;
	http::server::Response resp;
	handler.HandleRequest(*req, &resp);

	std::string expected_html = "<html><head><title>Not Found</title></head>"
								"<body><h1>404 Not Found</h1></body></html>";
	std::string expected_response = "HTTP/1.1 404 Not Found\r\n"
									"Content-Length: 85\r\n"
									"Content-Type: text/html\r\n\r\n" +
									expected_html;
	EXPECT_EQ(resp.ToString(), expected_response);
}
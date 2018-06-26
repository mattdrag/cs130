#include <string>
#include <iostream>

#include "gtest/gtest.h"
#include "response.hpp"

TEST(ResponseTest, OkResponse){
	std::string content = "Testing";
	http::server::Response response;
	response.SetStatus(http::server::Response::OK);
	response.AddHeader("Content-Length", "7");
	response.AddHeader("Content-Type", "text/plain");
	response.SetBody(content);

	std::string expected_response = "HTTP/1.1 200 OK\r\n"
									"Content-Length: 7\r\n"
									"Content-Type: text/plain\r\n\r\n" +
									content;
	EXPECT_EQ(response.ToString(), expected_response);
}

TEST(ResponseTest, BadRequestResponse){
	http::server::Response response;
	response.SetStatus(http::server::Response::BAD_REQUEST);
	std::string expected_response = "HTTP/1.1 400 Bad Request\r\n\r\n";
	EXPECT_EQ(response.ToString(), expected_response);
}

TEST(ResponseTest, NotFoundResponse){
	http::server::Response response;
	response.SetStatus(http::server::Response::NOT_FOUND);
	std::string expected_response = "HTTP/1.1 404 Not Found\r\n\r\n";
	EXPECT_EQ(response.ToString(), expected_response);
}

TEST(ResponseTest, InternalServerErrorResponse){
	http::server::Response response;
	response.SetStatus(http::server::Response::INTERNAL_SERVER_ERROR);
	std::string expected_response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
	EXPECT_EQ(response.ToString(), expected_response);
}

//Test parsing and new function GetRedirectHostAndPath
TEST(ResponseTest, ValidParsing){
	http::server::Response* response = http::server::Response::Parse("HTTP/1.1 200 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\n\r\n");
	ASSERT_TRUE(response);
	response->AddHeader("Location", "http://www.ucla.edu/path");
	std::string protocol;
	std::string host;
	std::string path;
	response->GetRedirectHostAndPath(host, path, protocol);
	EXPECT_EQ(protocol, "http");
	EXPECT_EQ(host, "www.ucla.edu");
	EXPECT_EQ(path, "/path");
	delete response;
}

//test invalid responses caught
TEST(ResponseTest, InvalidParse){
	//no response code
	http::server::Response* response = http::server::Response::Parse("HTTP/1.1 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\n\r\n");
	ASSERT_FALSE(response);
	delete response;
	//missing semicolon
	response = http::server::Response::Parse("HTTP/1.1 200 OK\r\nDate Mon, 27 Jul 2009 12:28:53 GMT\r\n\r\n");
	ASSERT_FALSE(response);
}

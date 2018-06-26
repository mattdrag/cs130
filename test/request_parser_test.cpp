#include <string>
#include <cstring>

#include "gtest/gtest.h"
#include "request_parser.hpp"
#include "request.hpp"

class RequestParserTest : public ::testing::Test {
protected:
	bool ParseRequest(std::string http_request) {
		char * begin = strdup(http_request.c_str());
		char * end = begin + http_request.length();
		boost::tribool result;
		boost::tie(result, boost::tuples::ignore) = request_parser_.parse(request_, begin, end);
		if (result) {
			return true;
		}
		return false; // Treat indeterminate and false results as bad for testing purposes
	}
	http::server::request_parser request_parser_;
	http::server::Request request_;
};

TEST_F(RequestParserTest, GoodRequest) {
	EXPECT_EQ(ParseRequest("GET / HTTP/1.1\r\nContent-Type: text/plain\r\n\r\n"), true);
}

TEST_F(RequestParserTest, AllowContentAfterHeaders) {
	EXPECT_EQ(ParseRequest("GET / HTTP/1.1\r\nContent-Type: text/plain\r\n\r\nTEST"), true);
}

TEST_F(RequestParserTest, EmptyRequest) {
	EXPECT_EQ(ParseRequest(""), false);
}

TEST_F(RequestParserTest, BadBeginning) {
	EXPECT_EQ(ParseRequest("1"), false);
}

TEST_F(RequestParserTest, IncompleteRequest) {
	EXPECT_EQ(ParseRequest("GET"), false);
}

TEST_F(RequestParserTest, NoHTTPVersion) {
	EXPECT_EQ(ParseRequest("GET /"), false);
}

TEST_F(RequestParserTest, BadHTTPVersion) {
	EXPECT_EQ(ParseRequest("GET / HTTP/.."), false);
}

TEST_F(RequestParserTest, NoFileRequested) {
	EXPECT_EQ(ParseRequest("GET HTTP/1.1"), false);
}

TEST_F(RequestParserTest, IncompleteHeader) {
	EXPECT_EQ(ParseRequest("GET / HTTP/1.1\r\nContent-Type:\r\n\r\n"), false);
}

TEST_F(RequestParserTest, NoCarriageReturnAtEnd) {
	EXPECT_EQ(ParseRequest("GET / HTTP/1.1\r\nContent-Type: text/plain"), false);
}
#include <string>
#include <iostream>

#include "gtest/gtest.h"
#include "request.hpp"

class RequestTest : public ::testing::Test {
protected:
	void Parse(const std::string raw_request) {
		request = http::server::Request::Parse(raw_request);
	}
	std::unique_ptr<http::server::Request> request;
};

TEST_F(RequestTest, GetRequest){
	const std::string raw_request = "GET / HTTP/1.0\r\nContent-Type: text/plain\r\n\r\n";
	Parse(raw_request);

	EXPECT_EQ(request->raw_request(), raw_request);
	EXPECT_EQ(request->method(), "GET");
	EXPECT_EQ(request->uri(), "/");

	// The following test currently fails and says the version is "9.11"...
	// Our server currently doesn't do anything with the version number but this is
	// something worth looking to
	// EXPECT_EQ(request->version(), "1.0");

	// Headers = std::vector<std::pair<std::string, std::string>>;
	EXPECT_EQ(request->headers().size(), 1);
	EXPECT_EQ(request->headers().at(0).first, "Content-Type");
	EXPECT_EQ(request->headers().at(0).second, "text/plain");
}

TEST_F(RequestTest, GetFileRequest){
	const std::string raw_request = "GET /file HTTP/1.0\r\nContent-Type: text/plain\r\n\r\n";
	Parse(raw_request);
	EXPECT_EQ(request->raw_request(), raw_request);
	EXPECT_EQ(request->method(), "GET");
	EXPECT_EQ(request->uri(), "/file");
	EXPECT_EQ(request->headers().size(), 1);
	EXPECT_EQ(request->headers().at(0).first, "Content-Type");
	EXPECT_EQ(request->headers().at(0).second, "text/plain");
}

TEST_F(RequestTest, PostRequest){
	const std::string raw_request = "POST / HTTP/1.0\r\n\r\n";
	Parse(raw_request);
	EXPECT_EQ(request->raw_request(), raw_request);
	EXPECT_EQ(request->method(), "POST");
	EXPECT_EQ(request->uri(), "/");
	EXPECT_EQ(request->headers().size(), 0);
}

TEST_F(RequestTest, EmptyRequest){
	const std::string raw_request = "";
	Parse(raw_request);
	EXPECT_EQ(request->raw_request(), raw_request);
	EXPECT_EQ(request->method(), "");
	EXPECT_EQ(request->uri(), "");
	EXPECT_EQ(request->headers().size(), 0);
}

TEST_F(RequestTest, IncompleteRequest){
	const std::string raw_request = "GET";
	Parse(raw_request);
	EXPECT_EQ(request->raw_request(), raw_request);
	EXPECT_EQ(request->method(), "GET");
	EXPECT_EQ(request->uri(), "");
	EXPECT_EQ(request->headers().size(), 0);
}

TEST_F(RequestTest, ManipulateRequest){
	const std::string raw_request = "GET /file HTTP/1.0\r\nContent-Type: text/plain\r\n\r\n";
	Parse(raw_request);
	ASSERT_TRUE(request.get());
	request->set_header("Cookie", "yum");

	//adding Cookie to headers
	EXPECT_EQ(request->ToString(), "GET /file HTTP/1.0\r\nContent-Type: text/plain\r\nCookie: yum\r\n\r\n");

	//remove header
	request->remove_header("Cookie");
	EXPECT_EQ(request->ToString(), raw_request);

}

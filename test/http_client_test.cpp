#include <string>

#include "gtest/gtest.h"
#include "http_client.hpp"
#include "request.hpp"
#include "response.hpp"

//basic tests to establish connections to a host and with a certain protocol
TEST(HttpClientTest, EstablishConnectionTest) {
	http::server::HttpClient c;
	EXPECT_TRUE(c.EstablishConnection("www.google.com", "http"));
	EXPECT_ANY_THROW(c.EstablishConnection("www....google.com", "http"));
	EXPECT_ANY_THROW(c.EstablishConnection("www.google.com", "f"));
}

//basic request send
TEST(HttpClientTest, SendRequestTest) {
	http::server::HttpClient c;
	auto req= http::server::Request::Parse("GET / HTTP/1.1\r\nHost: www.example.com\r\nConnection: close\r\n\r\n");
	ASSERT_NE(req.get(), nullptr);
	ASSERT_TRUE(c.EstablishConnection("www.example.com", "http"));
	auto res = c.SendRequest(*req.get());
	EXPECT_NE(res, nullptr);
}

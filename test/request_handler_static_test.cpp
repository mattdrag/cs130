#include <iostream>
#include <string>
#include <memory>
#include "gtest/gtest.h"
#include "request_handler_static.hpp"
#include "response.hpp"
#include "request.hpp"

class StaticHandlerTest : public ::testing::Test {
protected:
	http::server::RequestHandler::Status HandleStaticRequest(const std::string& uri_prefix, const std::string root, const std::string requested_file) {
		
		NginxConfig config;
		NginxConfigStatement statement;
		statement.tokens_.push_back("root");
		statement.tokens_.push_back(root);
		config.statements_.push_back(std::make_shared<NginxConfigStatement>(statement));

		std::string request_string = "GET /" + uri_prefix + "/" + requested_file + " HTTP/1.1\r\n"
									 "User-Agent: curl/7.35.0\r\n"
									 "Host: localhost:8080\r\n"
									 "Accept: */*\r\n\r\n";

		req = http::server::Request::Parse(request_string);
		handler.Init(uri_prefix, config);
		return handler.HandleRequest(*req, &resp);
	}

	http::server::StaticHandler handler;
	std::unique_ptr<http::server::Request> req;
	http::server::Response resp;
	const std::string not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
	const std::string bad_request = "HTTP/1.1 400 Bad Request\r\n\r\n";
};

TEST_F(StaticHandlerTest, SimpleRequest) {
	HandleStaticRequest("static1", "deploy/www", "index.html");

	std::string expected_html = "<!DOCTYPE html>\n"
		"<html>\n<body>\n\n<h1>This is default.html</h1>\n\n"
		"<p>The webserver successfully served this static file YAY</p>\n\n"
		"</body>\n</html>\n";

	std::string expected_response = "HTTP/1.1 200 OK\r\n"
									"Content-Length: 141\r\n"
									"Content-Type: text/html\r\n\r\n" +
									expected_html;

	EXPECT_EQ(resp.ToString(), expected_response);
}

TEST_F(StaticHandlerTest, MarkdownRequest) {
	HandleStaticRequest("static1", "deploy/www", "markdown_test.md");

	std::string expected_html = "<h1>This is a Markdown file!</h1>\n"
	"<p>This is a <a href=\"http://www.google.com\">link</a>! "
	"Words can be <em>italic</em>, <strong>bold</strong>, or <code>code</code>.</p>\n\n"
	"<h2>This an h2 header</h2>\n"
	"<p>Markdown also supports bulleted lists:</p>\n\n\n"
	"<ul>\n"
	"<li>This is something interesting</li>\n"
	"<li>This is something else that is interesting</li>\n"
	"</ul>\n\n"
	"<hr/>"
	"<h3>This is an h3 header</h3>\n"
	"<p>Lorem ipsum</p>\n\n";

	std::string expected_response = "HTTP/1.1 200 OK\r\n"
									"Content-Length: 402\r\n"
									"Content-Type: text/html\r\n\r\n" +
									expected_html;

	EXPECT_EQ(resp.ToString(), expected_response);
}

TEST_F(StaticHandlerTest, EmptyRequest) {
	EXPECT_EQ(HandleStaticRequest("", "", ""), http::server::RequestHandler::NOT_FOUND);
	EXPECT_EQ(resp.ToString(), not_found);
}

TEST_F(StaticHandlerTest, URLNotFound) {
	EXPECT_EQ(HandleStaticRequest("static1", "", "thisWontWork.jpg"), http::server::RequestHandler::NOT_FOUND);
	EXPECT_EQ(resp.ToString(), not_found);
}

TEST_F(StaticHandlerTest, BadRequest) {
	EXPECT_EQ(HandleStaticRequest("static", "", "%"), http::server::RequestHandler::BAD_REQUEST);
	EXPECT_EQ(resp.ToString(), bad_request);
}

TEST_F(StaticHandlerTest, EscapeSpaceCharacter) {
	EXPECT_EQ(HandleStaticRequest("static", "", "%20"), http::server::RequestHandler::NOT_FOUND);
	EXPECT_EQ(resp.ToString(), not_found);

	EXPECT_EQ(HandleStaticRequest("static", "", "+"), http::server::RequestHandler::NOT_FOUND);
	EXPECT_EQ(resp.ToString(), not_found);
}

TEST_F(StaticHandlerTest,NoFileExtension) {
	EXPECT_EQ(HandleStaticRequest("static", "", "a"), http::server::RequestHandler::NOT_FOUND);
	EXPECT_EQ(resp.ToString(), not_found);
}



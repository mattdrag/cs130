#include <iostream>
#include <string>
#include "gtest/gtest.h"
#include "request_handler_sql.hpp"
#include "request.hpp"
#include "response.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"


class RequestHandlerSqlTest : public ::testing::Test {
protected:
    http::server::RequestHandler::Status HandleQuery(std::string query_string) {
        std::string request_string = "GET /sql/query?q=" + query_string + " HTTP/1.1\r\n"
                                     "User-Agent: curl/7.35.0\r\n"
                                     "Host: localhost:8080\r\n"
                                     "Accept: */*\r\n\r\n";
        req = http::server::Request::Parse(request_string);
        return handler.HandleRequest(*req, &resp);
    }

    http::server::RequestHandler::Status HandleUpdate(std::string update_string) {
        std::string request_string = "GET /sql/update?q=" + update_string + " HTTP/1.1\r\n"
                                     "User-Agent: curl/7.35.0\r\n"
                                     "Host: localhost:8080\r\n"
                                     "Accept: */*\r\n\r\n";
        req = http::server::Request::Parse(request_string);
        return handler.HandleRequest(*req, &resp);
    }

    http::server::SqlHandler handler;
    std::unique_ptr<http::server::Request> req;
    http::server::Response resp;
};

TEST_F(RequestHandlerSqlTest, QueryTest) {
    std::string queryString = "SHOW+Tables%3B";
    EXPECT_NO_THROW(HandleQuery(queryString));
}

TEST_F(RequestHandlerSqlTest, UpdateTest) {
    std::string updateString = "INSERT+INTO+Team+VALUES+%28\"Matt\"%29";
    EXPECT_NO_THROW(HandleQuery(updateString));
}
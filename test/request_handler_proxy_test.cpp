#include <string>
#include "gtest/gtest.h"
#include "request_handler_proxy.hpp"
#include "request_handler.hpp"
#include "request.hpp"
#include "request_parser.hpp"
#include "http_client.hpp"
#include "response.hpp"
#include "../nginx-configparser/config_parser.h"

class ProxyHandlerTest : public ::testing::Test {
protected:
	bool parseString(const std::string config_string) {
		std::stringstream config_stream(config_string);
		return parser.Parse(&config_stream, &out_config);
	}

	http::server::RequestHandler::Status initProxy(std::string prefix){
		return proxy_handler.Init(prefix, out_config);
	}

    void clearConfig() {
        out_config = NginxConfig();
    }

	NginxConfigParser parser;
	NginxConfig out_config;
	http::server::ProxyHandler proxy_handler;
};

//test init of ProxyHandler with various configs
TEST_F(ProxyHandlerTest, InitTest) {
    parseString("proxy_pass http://www.ucla.edu/static;");
    auto init_status = initProxy("/proxy");
    EXPECT_EQ(init_status, http::server::RequestHandler::OK);
    clearConfig();

    //default to port 80 http
    parseString("proxy_pass www.ucla.edu/static;");
    init_status = initProxy("/proxy");
    EXPECT_EQ(init_status, http::server::RequestHandler::OK);
    clearConfig();

    parseString("proxy_pass www.ucla.edu/static extra_token;");
    init_status = initProxy("/proxy");
    EXPECT_EQ(init_status, http::server::RequestHandler::BAD_CONFIG);
}

//testing HandleRequest is done by integration test, functionality test covered by request, response, http_client classes
//since it mostly just outsources the work to those classes

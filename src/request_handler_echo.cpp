//
// request_handler_echo.cpp
// ~~~~~~~~~~~~~~~~~~~
//

#include "request_handler_echo.hpp"
#include <string>
#include <boost/lexical_cast.hpp>
#include "response.hpp"
#include "request.hpp"
#include "request_handler.hpp"

namespace http {
namespace server {

RequestHandler::Status EchoHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
	uri_prefix_ = uri_prefix;
	return RequestHandler::OK;
}

RequestHandler::Status EchoHandler::HandleRequest(const Request& request, Response* response) {
	response->SetStatus(Response::OK);
	response->AddHeader("Content-Length", std::to_string(request.raw_request().size()));
	response->AddHeader("Content-Type", "text/plain");
	response->SetBody(request.raw_request());

	return RequestHandler::OK;
}

REGISTER_REQUEST_HANDLER(EchoHandler);

} // namespace server
} // namespace http

//
// request_handler_notfound.cpp
// ~~~~~~~~~~~~~~~~~~~
//

#include "request_handler_notfound.hpp"
#include <string>
#include <boost/lexical_cast.hpp>
#include "response.hpp"
#include "request.hpp"

namespace http {
namespace server {

RequestHandler::Status NotFoundHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
	uri_prefix_ = uri_prefix;
	return RequestHandler::OK;
}

RequestHandler::Status NotFoundHandler::HandleRequest(const Request& request, Response* response) {
	const std::string NOT_FOUND_HTML = "<html><head><title>Not Found</title></head><body><h1>404 Not Found</h1></body></html>";

	response->SetStatus(Response::NOT_FOUND);
	response->AddHeader("Content-Length", std::to_string(NOT_FOUND_HTML.size()));
	response->AddHeader("Content-Type", "text/html");
	response->SetBody(NOT_FOUND_HTML);

	return RequestHandler::NOT_FOUND;
}

REGISTER_REQUEST_HANDLER(NotFoundHandler);

} // namespace server
} // namespace http

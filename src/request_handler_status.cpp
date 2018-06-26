//
// request_handler_status.cpp
// ~~~~~~~~~~~~~~~~~~~
//

#include "request_handler_status.hpp"
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include "response.hpp"
#include "request.hpp"

namespace http {
namespace server {

RequestHandler::Status StatusHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
	total_requests_ = 0;
	uri_prefix_ = uri_prefix;
	return RequestHandler::OK;
}

bool StatusHandler::SetHandlers(const std::vector<std::pair<std::string, std::string>> all_handlers) {
	handlers_ = all_handlers;
	return true;
}

RequestHandler::Status StatusHandler::HandleRequest(const Request& request, Response* response) {
	std::string status_html = "<html><head>"
							  "<title>Server Status</title>"
							  "<style>"
							  "body {background:#eee; font-family:Helvetica; color:#333;}"
							  "b {color:#2980b9;}"
							  "h1 {margin-top:0;}"
							  "</style>"
							  "</head><body>"
							  "<div style=\"padding:20px; margin:20px auto;"
							  "width:70%; background:#fff; border:1px solid #e0e0e0;\">";
	std::string status_line;
	status_html.append("<h1>Server status</h1>");

	// Handler info.
	status_html.append("Active handlers:<ul>");
	for (std::vector<int>::size_type i = 0; i != handlers_.size(); i++) {
		status_line = "<li>Handler: <b>" + handlers_[i].first + "</b>, path: <b>" + handlers_[i].second + "</b></li>";
		status_html.append(status_line);
	}
	status_html.append("</ul>");
 	
 	// Completed request info.
 	status_line = "<b>" + std::to_string(total_requests_) + "</b> total requests to the server:</h1><ul>";
	status_html.append(status_line);
	for (std::vector<int>::size_type i = 0; i != url_and_response_.size(); i++) {
		status_line = "<li>Requested URL: <b>" + url_and_response_[i].first + "</b>, response code: <b>" + url_and_response_[i].second + "</b></li>";
		status_html.append(status_line);
	}
	status_html.append("</ul></div></body></html>");

	// HTTP response.
	response->SetStatus(Response::OK);
	response->AddHeader("Content-Length", std::to_string(status_html.size()));
	response->AddHeader("Content-Type", "text/html");
	response->SetBody(status_html);

	return RequestHandler::OK;
}

void StatusHandler::update(std::string uri, int respCode){
	total_requests_++;
	url_and_response_.push_back(std::make_pair(uri, std::to_string(respCode)));
}


REGISTER_REQUEST_HANDLER(StatusHandler);

} // namespace server
} // namespace http

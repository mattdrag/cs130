//
// request_handler_sql.hpp
// ~~~~~~~~~~~~~~~~~~~
//

#ifndef HTTP_REQUEST_HANDLER_SQL_HPP
#define HTTP_REQUEST_HANDLER_SQL_HPP

#include "request_handler.hpp"

namespace http {
namespace server {

// Handler for echo requests.
class SqlHandler : public http::server::RequestHandler {
public:
  	Status Init(const std::string& uri_prefix, const NginxConfig& config) override;

  	Status HandleRequest(const Request& request, Response* response) override;
private:
	std::string root_;

	bool url_decode(const std::string& in, std::string& out);
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_SQL_HPP
//
// request_handler_echo.hpp
// ~~~~~~~~~~~~~~~~~~~
//

#ifndef HTTP_REQUEST_HANDLER_ECHO_HPP
#define HTTP_REQUEST_HANDLER_ECHO_HPP

#include "request_handler.hpp"

namespace http {
namespace server {

// Handler for echo requests.
class EchoHandler : public http::server::RequestHandler {
public:
  Status Init(const std::string& uri_prefix, const NginxConfig& config) override;

  Status HandleRequest(const Request& request, Response* response) override;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_ECHO_HPP
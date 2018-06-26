//
// request_handler_static.hpp
// ~~~~~~~~~~~~~~~~~~~
//

#ifndef HTTP_REQUEST_HANDLER_STATIC_HPP
#define HTTP_REQUEST_HANDLER_STATIC_HPP

#include "request_handler.hpp"

namespace http {
namespace server {

// Handler for static file requests.
class StaticHandler : public http::server::RequestHandler {
public:
  Status Init(const std::string& uri_prefix, const NginxConfig& config) override;

  Status HandleRequest(const Request& request, Response* response) override;

private:
  // Perform URL-decoding on a string. Returns false if the encoding is invalid.
  static bool url_decode(const std::string& in, std::string& out);
  std::string root_;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_STATIC_HPP
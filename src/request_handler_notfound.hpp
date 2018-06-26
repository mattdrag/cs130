//
// request_handler_notfound.hpp
// ~~~~~~~~~~~~~~~~~~~
//

#ifndef HTTP_REQUEST_HANDLER_NOTFOUND_HPP
#define HTTP_REQUEST_HANDLER_NOTFOUND_HPP

#include "request_handler.hpp"
#include <string>

namespace http {
namespace server {

/// Handler for 404 not found requests.
class NotFoundHandler : public http::server::RequestHandler {
public:
  Status Init(const std::string& uri_prefix, const NginxConfig& config) override;

  Status HandleRequest(const Request& request, Response* response) override;
};


} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_NOTFOUND_HPP
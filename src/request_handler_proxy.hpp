//
// request_handler_proxy.hpp
// ~~~~~~~~~~~~~~~~~~~
//

#ifndef HTTP_REQUEST_HANDLER_PROXY_HPP
#define HTTP_REQUEST_HANDLER_PROXY_HPP

#include "request_handler.hpp"
#include "request.hpp"
#include "response.hpp"

namespace http {
namespace server {

// Handler for echo requests.
class ProxyHandler : public http::server::RequestHandler {
public:
  Status Init(const std::string& uri_prefix, const NginxConfig& config) override;

  Status HandleRequest(const Request& request, Response* response) override;

private:
  Request TransformRequest(const Request& request) const;
  Request TransformRedirect(const Request& request) const;

  Response* RunOutsideRequest(const Request& request) const;

  std::string host_;
  std::string path_;
  std::string port_;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_PROXY_HPP

//
// request_handler_status.hpp
// ~~~~~~~~~~~~~~~~~~~
//

#ifndef HTTP_REQUEST_HANDLER_STATUS_HPP
#define HTTP_REQUEST_HANDLER_STATUS_HPP

#include "request_handler.hpp"
#include <string>

namespace http {
namespace server {

/// Handler that retunrs server status.
class StatusHandler : public http::server::RequestHandler {
public:
  Status Init(const std::string& uri_prefix, const NginxConfig& config) override;

  Status HandleRequest(const Request& request, Response* response) override;

  bool SetHandlers(const std::vector<std::pair<std::string, std::string>> all_handlers);

  void update(std::string s, int respCode);

private:

  int total_requests_; // How many requests has the server received? 
  std::vector<std::pair<std::string, std::string>> url_and_response_; //Break this down by URL requested and the response code returned (200, 400, etc.).
  std::vector<std::pair<std::string, std::string>> handlers_; // What request handlers exist, and for what URL prefixes?

};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_STATUS_HPP
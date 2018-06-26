//
// request_handler_proxy.cpp
// ~~~~~~~~~~~~~~~~~~~
//

#include "request_handler_proxy.hpp"
#include <string>
#include <boost/lexical_cast.hpp>
#include "http_client.hpp"
#include "response.hpp"
#include "request.hpp"
#include "request_handler.hpp"

namespace http {
namespace server {

RequestHandler::Status ProxyHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
	uri_prefix_ = uri_prefix;

	std::vector<std::shared_ptr<NginxConfigStatement>> statements = config.statements_;

  // Get root directory from config.
  if (statements.size() == 1) {
    std::vector<std::string> statement_tokens = statements.at(0)->tokens_;
    if (statement_tokens.size() == 2 && statement_tokens.at(0) == "proxy_pass") {
      std::string full_url = statement_tokens.at(1);

      port_ = "80"; // default port to 80

      std::string::size_type colon_pos = full_url.find(':');
      // Found colon, so set host with contents before colon
      if(colon_pos != std::string::npos) {
        host_ = full_url.substr(0, colon_pos);
        
        std::string::size_type slash_pos = full_url.find('/');
        // Found slash, so set port between colon and slash
        // Also set path with everything "/" and after
        if(slash_pos != std::string::npos) {
          port_ = full_url.substr(colon_pos + 1, slash_pos - colon_pos - 1);
          path_ = full_url.substr(slash_pos);
        }
        // No slash, so set port until end and path with default "/"
        else {
          port_ = full_url.substr(colon_pos + 1);
          path_ = "/";
        }
      }
      // No colon found, so keep default port
      else {
        std::cout << "ProxyHandler " << uri_prefix << " didn't specify port. Using default 80" << std::endl;

        std::string::size_type slash_pos = full_url.find('/');
        // Found slash, so set host from beginning to slash
        // Also set path with everything "/" and after
        if(slash_pos != std::string::npos) {
          host_ = full_url.substr(0, slash_pos);
          path_ = full_url.substr(slash_pos);
        }
        // No slash, so set host until end and path with default "/"
        else {
          host_ = full_url;
          path_ = "/";
        }
      }

      if(host_ == "localhost") {
        host_ = "";
      }
  
      return RequestHandler::OK;
    }
  }

  return RequestHandler::BAD_CONFIG;
}

RequestHandler::Status ProxyHandler::HandleRequest(const Request& request, Response* response) {
  Request new_request = TransformRequest(request);
  Response* resp = RunOutsideRequest(new_request);
  if(resp == nullptr) {
    return RequestHandler::INTERNAL_SERVER_ERROR;
  }

  if(resp->GetStatus() == Response::FOUND) {
	  std::cout << "redirecting" << std::endl;
	if(!resp->GetRedirectHostAndPath(host_, path_, port_)) {
	  return RequestHandler::INTERNAL_SERVER_ERROR;
	}
    Request redirect_request = TransformRedirect(new_request);
    resp = RunOutsideRequest(redirect_request);
    if(resp == nullptr) {
      return RequestHandler::INTERNAL_SERVER_ERROR;
    }
  }

  (*response) = (*resp);

  return RequestHandler::OK;
}

Request ProxyHandler::TransformRequest(const Request& request) const {
  Request transformed_request(request);
  transformed_request.remove_header("Cookie");
  transformed_request.set_header("Host", host_);
  transformed_request.set_header("Connection", "close");
  transformed_request.set_uri(path_ + request.uri().substr(uri_prefix_.length()));
  return transformed_request;
}

Request ProxyHandler::TransformRedirect(const Request& request) const {
  Request transformed_request(request);
  transformed_request.remove_header("Cookie");
  transformed_request.set_header("Host", host_);
  transformed_request.set_header("Connection", "close");
  transformed_request.set_uri(path_);

  return transformed_request;
}

Response* ProxyHandler::RunOutsideRequest(const Request& request) const {
  HttpClient c;
  c.EstablishConnection(host_, port_);
  auto resp = c.SendRequest(request);
  return resp;
}

REGISTER_REQUEST_HANDLER(ProxyHandler);

} // namespace server
} // namespace http

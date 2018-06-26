//
// request.hpp
// ~~~~~~~~~~~
//
// Represents an HTTP Request.
//
// Usage:
//   auto request = Request::Parse(raw_request);

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace http {
namespace server {

class Request {
 public:
  static std::unique_ptr<Request> Parse(const std::string& raw_request);

  std::string raw_request() const;
  std::string method() const;
  std::string uri() const;
  std::string version() const;
  std::string GetContentLength() const {
    for (auto const& header : headers_) {
      if (header.first == "Content-Length") return header.second;
    } 
    return "";
}
  void appendBody(const std::string remainingReq) {
    body_.append(remainingReq);
    raw_request_.append(remainingReq);
  }

  using Headers = std::vector<std::pair<std::string, std::string>>;
  Headers headers() const;

  std::string body() const;

  std::string ToString() const;

  // Setters
  void set_header(std::string first, std::string second);
  void remove_header(std::string key);
  void set_uri(std::string uri);

private:
  std::string raw_request_;
  std::string method_;
  std::string uri_;
  std::string version_;
  Headers headers_;
  std::string body_;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HPP
